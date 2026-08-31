#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

#include "creatures/creatures_definitions.hpp"
#include "game/movement/position.hpp"
#include "utils/utils_definitions.hpp"

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 7 (map). Constants only - this suite runs after protobuf's static
// teardown. CrystalOTC mirrors these in tests/protocol/map_schema_test.cpp because the two
// repositories compile separate verbatim copies of the official protocol.proto.
suite<"networkmessage"> mapProtocolTest = [] {
	test("map envelope numbers match the official schema") = [] {
		expect(eq(100, proto::GameserverMessageExtensions::kFullMapFieldNumber));
		expect(eq(101, proto::GameserverMessageExtensions::kTopRowFieldNumber));
		expect(eq(102, proto::GameserverMessageExtensions::kRightColumnFieldNumber));
		expect(eq(103, proto::GameserverMessageExtensions::kBottomRowFieldNumber));
		expect(eq(104, proto::GameserverMessageExtensions::kLeftColumnFieldNumber));
		expect(eq(105, proto::GameserverMessageExtensions::kFieldDataFieldNumber));
		expect(eq(106, proto::GameserverMessageExtensions::kCreateOnMapFieldNumber));
		expect(eq(107, proto::GameserverMessageExtensions::kChangeOnMapFieldNumber));
		expect(eq(108, proto::GameserverMessageExtensions::kDeleteOnMapFieldNumber));
		expect(eq(190, proto::GameserverMessageExtensions::kTopFloorFieldNumber));
		expect(eq(191, proto::GameserverMessageExtensions::kBottomFloorFieldNumber));

		// The type enum still spells the columns ROW.
		expect(eq(100, proto::GAMESERVER_MESSAGE_TYPE_FULLMAP));
		expect(eq(101, proto::GAMESERVER_MESSAGE_TYPE_TOPROW));
		expect(eq(102, proto::GAMESERVER_MESSAGE_TYPE_RIGHTROW));
		expect(eq(103, proto::GAMESERVER_MESSAGE_TYPE_BOTTOMROW));
		expect(eq(104, proto::GAMESERVER_MESSAGE_TYPE_LEFTROW));
		expect(eq(105, proto::GAMESERVER_MESSAGE_TYPE_FIELDDATA));
		expect(eq(106, proto::GAMESERVER_MESSAGE_TYPE_CREATEONMAP));
		expect(eq(107, proto::GAMESERVER_MESSAGE_TYPE_CHANGEONMAP));
		expect(eq(108, proto::GAMESERVER_MESSAGE_TYPE_DELETEONMAP));
		expect(eq(190, proto::GAMESERVER_MESSAGE_TYPE_TOPFLOOR));
		expect(eq(191, proto::GAMESERVER_MESSAGE_TYPE_BOTTOMFLOOR));
	};

	test("map framing fields match the official schema") = [] {
		// MapArea: origin and extent exist but never travel; the fields chain does.
		expect(eq(1, proto::MapArea::kOriginFieldNumber));
		expect(eq(2, proto::MapArea::kExtentFieldNumber));
		expect(eq(3, proto::MapArea::kFieldsFieldNumber));

		expect(eq(1, proto::MapFieldData::kFieldsFieldNumber));
		expect(eq(3, proto::MapFields::kFieldsFieldNumber));

		// One field: its objects, the count of empty fields following it, and its own
		// absolute coordinate as extension 100 - every populated field in the tutorial
		// dump carries it.
		expect(eq(1, proto::MapField::kObjectsFieldNumber));
		expect(eq(2, proto::MapField::kEmptyFieldsFollowingFieldNumber));
		expect(eq(100, proto::MapFieldExtensions::kExtraDataFieldNumber));
		expect(eq(1, proto::MapFieldExtraData::kPositionFieldNumber));

		expect(eq(1, proto::GameserverMessageFullMap::kAreaFieldNumber));
		expect(eq(2, proto::GameserverMessageFullMap::kPlayerPositionFieldNumber));
		expect(eq(1, proto::GameserverMessageTopRow::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageRightColumn::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageBottomRow::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageLeftColumn::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageTopFloor::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageBottomFloor::kAreaFieldNumber));
		expect(eq(1, proto::GameserverMessageFieldData::kAreaFieldNumber));
		expect(eq(2, proto::GameserverMessageFieldData::kPositionFieldNumber));

		expect(eq(1, proto::GameserverMessageCreateOnMap::kPositionFieldNumber));
		expect(eq(2, proto::GameserverMessageCreateOnMap::kStackPositionFieldNumber));
		expect(eq(3, proto::GameserverMessageCreateOnMap::kObjectFieldNumber));
		expect(eq(1, proto::GameserverMessageChangeOnMap::kPositionFieldNumber));
		expect(eq(2, proto::GameserverMessageChangeOnMap::kStackPositionFieldNumber));
		expect(eq(3, proto::GameserverMessageChangeOnMap::kObjectFieldNumber));

		// DeleteOnMap's oneof is the legacy 0xFFFF discriminator.
		expect(eq(1, proto::GameserverMessageDeleteOnMap::kObjectPositionFieldNumber));
		expect(eq(2, proto::GameserverMessageDeleteOnMap::kCreatureIdFieldNumber));
		expect(eq(1, proto::WorldmapObjectPosition::kPositionFieldNumber));
		expect(eq(2, proto::WorldmapObjectPosition::kStackPositionFieldNumber));
	};

	test("creature data fields match the official schema") = [] {
		// CreatureData rides as extension 100 of an AppearanceInstance whose appearance id
		// is the legacy creature opcode: 97 unknown, 98 known, 99 turn.
		expect(eq(100, proto::AppearanceInstanceExtensions::kCreatureFieldNumber));

		expect(eq(1, proto::CreatureData::kField1FieldNumber));
		expect(eq(2, proto::CreatureData::kCreatureIdFieldNumber));
		expect(eq(3, proto::CreatureData::kCreatureTypeFieldNumber));
		expect(eq(4, proto::CreatureData::kMasterIdFieldNumber));
		expect(eq(5, proto::CreatureData::kVocationFieldNumber));
		expect(eq(6, proto::CreatureData::kNameFieldNumber));
		expect(eq(7, proto::CreatureData::kHealthPercentFieldNumber));
		expect(eq(8, proto::CreatureData::kDirectionFieldNumber));
		expect(eq(9, proto::CreatureData::kOutfitFieldNumber));
		expect(eq(10, proto::CreatureData::kMountFieldNumber));
		expect(eq(11, proto::CreatureData::kLightIntensityFieldNumber));
		expect(eq(12, proto::CreatureData::kLightColorFieldNumber));
		expect(eq(13, proto::CreatureData::kSpeedFieldNumber));
		expect(eq(14, proto::CreatureData::kSkullFieldNumber));
		expect(eq(15, proto::CreatureData::kPartyFlagFieldNumber));
		expect(eq(16, proto::CreatureData::kGuildFlagFieldNumber));
		expect(eq(17, proto::CreatureData::kNpcSpeechFlagFieldNumber));
		// mark is the slice 7 reading: 255 on all 21 full descriptions in the tutorial
		// dump - the legacy static-square byte, whose 0xFF means unmarked.
		expect(eq(18, proto::CreatureData::kMarkFieldNumber));
		expect(eq(19, proto::CreatureData::kUnpassableFieldNumber));
		expect(eq(24, proto::CreatureData::kIconsFieldNumber));

		expect(eq(1, proto::CreatureOtherIcons::kPlayerIconFieldNumber));
		expect(eq(2, proto::CreatureOtherIcons::kCreatureIconFieldNumber));
		expect(eq(3, proto::CreatureOtherIcons::kCountFieldNumber));

		expect(eq(1, proto::Outfit::kLookTypeFieldNumber));
		expect(eq(2, proto::Outfit::kColorsFieldNumber));
		expect(eq(3, proto::Outfit::kAddonsFieldNumber));
		expect(eq(4, proto::Outfit::kLookItemFieldNumber));
		expect(eq(1, proto::OutfitColor::kHeadFieldNumber));
		expect(eq(2, proto::OutfitColor::kBodyFieldNumber));
		expect(eq(3, proto::OutfitColor::kLegsFieldNumber));
		expect(eq(4, proto::OutfitColor::kFeetFieldNumber));
	};

	test("creature enums pass through the bridge unchanged") = [] {
		// CREATURE_TYPE matches CreatureType_t on 0-3; 4 (summon-other) is a client-side
		// reclassification and 5 (hidden) is this stack's private value, downgraded to
		// MONSTER on the wire and restored over the legacy 0x95 opcode.
		expect(eq(static_cast<int>(proto::CREATURE_TYPE_PLAYER), static_cast<int>(CREATURETYPE_PLAYER)));
		expect(eq(static_cast<int>(proto::CREATURE_TYPE_MONSTER), static_cast<int>(CREATURETYPE_MONSTER)));
		expect(eq(static_cast<int>(proto::CREATURE_TYPE_NPC), static_cast<int>(CREATURETYPE_NPC)));
		expect(eq(static_cast<int>(proto::CREATURE_TYPE_PLAYERSUMMON), static_cast<int>(CREATURETYPE_SUMMON_PLAYER)));

		expect(eq(static_cast<int>(proto::CREATURE_DIRECTION_NORTH), static_cast<int>(DIRECTION_NORTH)));
		expect(eq(static_cast<int>(proto::CREATURE_DIRECTION_EAST), static_cast<int>(DIRECTION_EAST)));
		expect(eq(static_cast<int>(proto::CREATURE_DIRECTION_SOUTH), static_cast<int>(DIRECTION_SOUTH)));
		expect(eq(static_cast<int>(proto::CREATURE_DIRECTION_WEST), static_cast<int>(DIRECTION_WEST)));

		// the client names skulls by picture, CipSoft by meaning: yellow is ATTACKER,
		// white AGGRESSOR, orange REVENGE
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_NONE), static_cast<int>(SKULL_NONE)));
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_ATTACKER), static_cast<int>(SKULL_YELLOW)));
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_AGGRESSOR), static_cast<int>(SKULL_WHITE)));
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_PLAYERKILLER), static_cast<int>(SKULL_RED)));
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_EXCESSIVE_PLAYERKILLER), static_cast<int>(SKULL_BLACK)));
		expect(eq(static_cast<int>(proto::PLAYER_KILLER_FLAG_REVENGE), static_cast<int>(SKULL_ORANGE)));

		expect(eq(static_cast<int>(proto::PARTY_FLAG_NONE), static_cast<int>(SHIELD_NONE)));
		expect(eq(static_cast<int>(proto::PARTY_FLAG_OTHER), static_cast<int>(SHIELD_GRAY)));

		expect(eq(static_cast<int>(proto::GUILD_FLAG_NONE), static_cast<int>(GUILDEMBLEM_NONE)));
		expect(eq(static_cast<int>(proto::GUILD_FLAG_WAR_ALLY), static_cast<int>(GUILDEMBLEM_ALLY)));
		expect(eq(static_cast<int>(proto::GUILD_FLAG_OTHER), static_cast<int>(GUILDEMBLEM_OTHER)));

		expect(eq(static_cast<int>(proto::NPC_SPEECH_FLAG_NONE), static_cast<int>(SPEECHBUBBLE_NONE)));
		expect(eq(static_cast<int>(proto::NPC_SPEECH_FLAG_HIRELING), static_cast<int>(SPEECHBUBBLE_HIRELING)));

		// The icon sheets: the quests sheet passes through PLAYERICON, the modifications
		// sheet through CREATUREICON, with the same numbering the legacy category byte
		// selected.
		expect(eq(static_cast<int>(proto::CREATUREICON_NONE), static_cast<int>(CreatureIconModifications_t::None)));
		expect(eq(static_cast<int>(proto::CREATUREICON_BOUNTY_TASK_MONSTER), static_cast<int>(CreatureIconModifications_t::BountyTask)));
		expect(eq(static_cast<int>(proto::PLAYERICON_NONE), static_cast<int>(CreatureIconQuests_t::None)));
	};
};
