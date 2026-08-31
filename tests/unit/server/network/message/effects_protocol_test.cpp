#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

#include "creatures/creatures_definitions.hpp"

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 6 (effects). Constants only - this suite runs after protobuf's static
// teardown. CrystalOTC mirrors these in tests/protocol/effects_schema_test.cpp because the
// two repositories compile separate verbatim copies of the official protocol.proto.
suite<"networkmessage"> effectsProtocolTest = [] {
	test("effects envelope numbers match the official schema") = [] {
		expect(eq(130, proto::GameserverMessageExtensions::kAmbientLightFieldNumber));
		expect(eq(131, proto::GameserverMessageExtensions::kGraphicalEffectsFieldNumber));
		expect(eq(132, proto::GameserverMessageExtensions::kRemoveGraphicalEffectFieldNumber));
		expect(eq(133, proto::GameserverMessageExtensions::kSoundTriggerFieldNumber));

		// The type enum spells AmbientLight's entry AMBIENTE.
		expect(eq(130, proto::GAMESERVER_MESSAGE_TYPE_AMBIENTE));
		expect(eq(131, proto::GAMESERVER_MESSAGE_TYPE_GRAPHICALEFFECTS));
		expect(eq(132, proto::GAMESERVER_MESSAGE_TYPE_REMOVEGRAPHICALEFFECT));
		expect(eq(133, proto::GAMESERVER_MESSAGE_TYPE_SOUNDTRIGGER));

		// The three effect kinds are AppearanceInstance extensions.
		expect(eq(104, proto::AppearanceInstanceExtensions::kGraphicalEffectFieldNumber));
		expect(eq(105, proto::AppearanceInstanceExtensions::kMissileEffectFieldNumber));
		expect(eq(111, proto::AppearanceInstanceExtensions::kSoundEffectFieldNumber));
	};

	test("effects message fields match the official schema") = [] {
		expect(eq(1, proto::GameserverMessageAmbientLight::kIntensityFieldNumber));
		expect(eq(2, proto::GameserverMessageAmbientLight::kColorFieldNumber));

		expect(eq(1, proto::GameserverMessageGraphicalEffects::kEffectsFieldNumber));
		expect(eq(1, proto::GameserverMessageRemoveGraphicalEffect::kEffectFieldNumber));
		expect(eq(1, proto::AppearanceInstance::kAppearanceIdFieldNumber));

		// delay_ms is the slice 6 reading: the tutorial dump falsified effect_id (the id
		// rides in appearance_id and the field was always 0), and the legacy
		// MAGIC_EFFECTS_DELAY is the only per-effect datum with no other home.
		expect(eq(1, proto::GraphicalEffectAppearanceInstance::kPositionFieldNumber));
		expect(eq(2, proto::GraphicalEffectAppearanceInstance::kDelayMsFieldNumber));
		expect(eq(3, proto::GraphicalEffectAppearanceInstance::kSourceFieldNumber));

		expect(eq(1, proto::MissileEffectAppearanceInstance::kFromFieldNumber));
		expect(eq(2, proto::MissileEffectAppearanceInstance::kToFieldNumber));
		expect(eq(3, proto::MissileEffectAppearanceInstance::kSourceFieldNumber));

		expect(eq(1, proto::SoundEffectAppearanceInstance::kPositionFieldNumber));
		expect(eq(2, proto::SoundEffectAppearanceInstance::kSoundIdFieldNumber));
		expect(eq(3, proto::SoundEffectAppearanceInstance::kKindFieldNumber));
		expect(eq(4, proto::SoundEffectAppearanceInstance::kSourceFieldNumber));

		expect(eq(1, proto::Coordinate::kXFieldNumber));
		expect(eq(2, proto::Coordinate::kYFieldNumber));
		expect(eq(3, proto::Coordinate::kZFieldNumber));

		expect(eq(1, proto::GameserverMessageSoundTrigger::kTriggerTypeFieldNumber));
		expect(eq(2, proto::GameserverMessageSoundTrigger::kAmbienceStreamIdFieldNumber));
		expect(eq(3, proto::GameserverMessageSoundTrigger::kMusicTemplateIdFieldNumber));
		expect(eq(4, proto::GameserverMessageSoundTrigger::kNumericSoundEffectIdFieldNumber));
	};

	test("effects enums match the official schema on both layers") = [] {
		// EFFECT_SOURCE carries the same numbering as SourceEffect_t, so the value passes
		// through every layer unchanged.
		expect(eq(static_cast<int>(proto::UNSPECIFIED), static_cast<int>(SourceEffect_t::GLOBAL)));
		expect(eq(static_cast<int>(proto::OWN), static_cast<int>(SourceEffect_t::OWN)));
		expect(eq(static_cast<int>(proto::OTHER_PLAYER), static_cast<int>(SourceEffect_t::OTHERS)));
		expect(eq(static_cast<int>(proto::OTHER_CREATURE), static_cast<int>(SourceEffect_t::CREATURES)));
		expect(eq(static_cast<int>(proto::OTHER_CREATURE_BOSS_AREA), static_cast<int>(SourceEffect_t::BOSS)));

		// The sound kind: NONE for a standalone main sound, MISSILE_HIT for the secondary
		// sound of a pair - the value the legacy framing wrote as its leading enum byte.
		expect(eq(0, proto::NONE));
		expect(eq(1, proto::MISSILE_HIT));
		expect(eq(2, proto::ADDITIONAL_MISSILE_EFFECT));

		// The legacy anthem type byte was these values verbatim.
		expect(eq(0, proto::SOUND_TRIGGER_TYPE_AMBIENCE_STREAM));
		expect(eq(1, proto::SOUND_TRIGGER_TYPE_MUSIC_TEMPLATE));
		expect(eq(2, proto::SOUND_TRIGGER_TYPE_NUMERIC_SOUND));
	};
};
