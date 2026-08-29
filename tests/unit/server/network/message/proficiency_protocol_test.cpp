#include "pch.hpp"

#include <boost/ut.hpp>

#include "lib/logging/in_memory_logger.hpp"

#include "server/network/message/networkmessage.hpp"

#include <proficiency.pb.h>

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

suite<"networkmessage"> proficiencyProtocolTest = [] {
	// NetworkMessage logs through the DI container on its refusal paths, and boost::ut runs
	// suites outside any application bootstrap, so the container has to be stood up here or
	// the first refused read segfaults.
	di::extension::injector<> injector {};
	DI::setTestContainer(&InMemoryLogger::install(injector));

	// These numbers are not ours to choose: they were transcribed from the official client's
	// parse tables so the schema stays diffable against a future client version, and the
	// CrystalOTC copy of proficiency.proto has to agree with this one field for field.
	// Renumbering anything here silently breaks the pair, so pin it.
	test("field numbers match the official schema") = [] {
		expect(eq(1, proto::WeaponProficiencyPerkPick::kLevelFieldNumber));
		expect(eq(2, proto::WeaponProficiencyPerkPick::kPositionFieldNumber));

		expect(eq(1, proto::WeaponProficiencyPerkShaping::kLevelFieldNumber));
		expect(eq(2, proto::WeaponProficiencyPerkShaping::kPositionFieldNumber));
		expect(eq(3, proto::WeaponProficiencyPerkShaping::kModifierFieldNumber));
		expect(eq(4, proto::WeaponProficiencyPerkShaping::kRankFieldNumber));

		expect(eq(1, proto::GameserverMessageWeaponProficiency::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::GameserverMessageWeaponProficiency::kExperienceFieldNumber));
		expect(eq(3, proto::GameserverMessageWeaponProficiency::kPicksFieldNumber));
		expect(eq(4, proto::GameserverMessageWeaponProficiency::kShapedFieldNumber));

		expect(eq(1, proto::GameserverMessageWeaponProficiencyNotification::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::GameserverMessageWeaponProficiencyNotification::kExperienceFieldNumber));
		expect(eq(3, proto::GameserverMessageWeaponProficiencyNotification::kHasUnusedPerkFieldNumber));

		expect(eq(1, proto::GameserverMessageShapedPerkReshapeOffers::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::GameserverMessageShapedPerkReshapeOffers::kSlotFieldNumber));
		expect(eq(3, proto::GameserverMessageShapedPerkReshapeOffers::kOffersFieldNumber));

		expect(eq(1, proto::GameclientMessageWeaponProficiencyCommand::kCommandFieldNumber));
		expect(eq(2, proto::GameclientMessageWeaponProficiencyCommand::kObjectTypeIdFieldNumber));
		expect(eq(3, proto::GameclientMessageWeaponProficiencyCommand::kPicksFieldNumber));
		expect(eq(4, proto::GameclientMessageWeaponProficiencyCommand::kArgumentFieldNumber));
	};

	test("command values match the official enum") = [] {
		expect(eq(0, proto::WEAPON_PROFICIENCY_COMMAND_GET_PROFICIENCY));
		expect(eq(1, proto::WEAPON_PROFICIENCY_COMMAND_GET_ALL_PROFICIENCIES));
		expect(eq(2, proto::WEAPON_PROFICIENCY_COMMAND_RESET_PROFICIENCY));
		expect(eq(3, proto::WEAPON_PROFICIENCY_COMMAND_PICK_PERK));
		expect(eq(4, proto::WEAPON_PROFICIENCY_COMMAND_SHAPE_PERK));
		expect(eq(5, proto::WEAPON_PROFICIENCY_COMMAND_REFINE_SHAPED_PERK));
		expect(eq(6, proto::WEAPON_PROFICIENCY_COMMAND_MAXIMIZE_SHAPED_PERK));
		expect(eq(7, proto::WEAPON_PROFICIENCY_COMMAND_RESHAPE_SHAPED_PERK));
		expect(eq(8, proto::WEAPON_PROFICIENCY_COMMAND_SELECT_RESHAPE_OPTION));
		expect(eq(9, proto::WEAPON_PROFICIENCY_COMMAND_CLEAR_SHAPED_PERK));
	};


	// The framing the two ends agree on is { opcode, uint16 length, raw bytes }. getString()
	// transcodes ISO-8859-1 <-> UTF-8 and would corrupt a serialised message, which is the
	// whole reason getBytes() exists - so pin that a payload survives byte for byte,
	// embedded NULs and high bytes included.
	//
	// The protobuf round-trip itself is deliberately not tested here: boost::ut runs its
	// suites from the runner's destructor, after main returns, by which point protobuf's own
	// static teardown may already have happened. Constructing a message at that point
	// segfaults. What needs guarding is our framing and the field numbers above; protobuf's
	// serialisation is upstream's problem.
	test("a binary payload survives NetworkMessage framing byte for byte") = [] {
		std::string payload;
		for (int i = 0; i < 256; ++i) {
			payload.push_back(static_cast<char>(i));
		}
		expect(payload.find('\0') != std::string::npos);

		NetworkMessage msg;
		msg.addByte(0xC4);
		msg.add<uint16_t>(static_cast<uint16_t>(payload.size()));
		msg.addBytes(payload.data(), payload.size());

		msg.setBufferPosition(NetworkMessage::INITIAL_BUFFER_POSITION);
		expect(eq(0xC4, static_cast<int>(msg.getByte())));

		const uint16_t size = msg.get<uint16_t>();
		expect(eq(payload.size(), static_cast<size_t>(size)));
		expect(eq(payload, msg.getBytes(size)));
	};

	test("getBytes refuses to read past the end") = [] {
		NetworkMessage msg;
		msg.addByte(0x01);
		msg.setBufferPosition(NetworkMessage::INITIAL_BUFFER_POSITION);
		// One byte written, ten asked for: the read is refused rather than served garbage.
		expect(msg.getBytes(10).empty());
	};

	test("an empty payload is legal") = [] {
		NetworkMessage msg;
		msg.add<uint16_t>(0);
		msg.setBufferPosition(NetworkMessage::INITIAL_BUFFER_POSITION);
		expect(eq(0, static_cast<int>(msg.get<uint16_t>())));
		expect(msg.getBytes(0).empty());
	};
};
