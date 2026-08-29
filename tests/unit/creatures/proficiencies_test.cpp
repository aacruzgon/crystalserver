#include "pch.hpp"

#include <boost/ut.hpp>

#include "creatures/players/proficiencies/proficiencies.hpp"
#include "creatures/players/proficiencies/proficiency_modifiers.hpp"

using namespace boost::ut;

namespace {
	// Both sides of the wire decode a shaped perk from nothing but its pool id and rank, so
	// the server's table has to agree with the client's (mods/game_proficiency/const.lua and
	// proficiency_data.lua) value for value. These helpers mirror the client's arithmetic.
	float clientValue(int32_t rawMin, int32_t rawMax, uint8_t rank, bool integerValue) {
		const double step = static_cast<double>(rawMax - rawMin) / 10.0;
		int32_t raw;
		if (rank == 0) {
			raw = rawMin;
		} else if (rank >= 10) {
			raw = rawMax;
		} else {
			raw = rawMin + static_cast<int32_t>(std::floor(step * rank));
		}
		return integerValue ? static_cast<float>(raw) : static_cast<float>(raw) / 10000.0f;
	}

	ProficiencyModifier resolve(uint16_t modifierEnum) {
		ProficiencyModifier modifier {};
		ProficiencyModifiers::get(modifierEnum, modifier);
		return modifier;
	}
}

suite<"creatures"> proficiencyExperienceTest = [] {
	// The official progression table. Levels 8 and 9 grant no perks and exist only for
	// Mastery. Getting level 8 wrong is exactly the bug the client's copy had.
	test("experience thresholds match the official table") = [] {
		struct Row {
			uint8_t level;
			uint32_t standard;
			uint32_t knight;
			uint32_t crossbow;
		};
		constexpr Row rows[] = {
			{ 1, 1750, 1250, 600 },
			{ 2, 25000, 20000, 8000 },
			{ 3, 100000, 80000, 30000 },
			{ 4, 400000, 300000, 150000 },
			{ 5, 2000000, 1500000, 650000 },
			{ 6, 8000000, 6000000, 2500000 },
			{ 7, 30000000, 20000000, 10000000 },
			{ 8, 60000000, 40000000, 20000000 },
			{ 9, 90000000, 60000000, 30000000 },
		};

		for (const auto &row : rows) {
			expect(eq(row.standard, Proficiencies::getExperienceForLevel(row.level, ProficiencyLane_t::Standard)));
			expect(eq(row.knight, Proficiencies::getExperienceForLevel(row.level, ProficiencyLane_t::Knight)));
			expect(eq(row.crossbow, Proficiencies::getExperienceForLevel(row.level, ProficiencyLane_t::Crossbow)));
		}
	};

	test("rejects levels outside 1..9") = [] {
		expect(eq(0u, Proficiencies::getExperienceForLevel(0, ProficiencyLane_t::Standard)));
		expect(eq(0u, Proficiencies::getExperienceForLevel(PROFICIENCY_MAX_LEVEL + 1, ProficiencyLane_t::Standard)));
	};
};

suite<"creatures"> proficiencyModifierTest = [] {
	test("spell augment blocks are keyed by vocation") = [] {
		// Block starts, and the first spell of each: knight Berserk, paladin Divine Caldera,
		// sorcerer Energy Wave, druid Strong Ice Wave, monk Greater Flurry of Blows.
		expect(eq(uint16_t { 80 }, resolve(1).spellId));
		expect(eq(uint16_t { 124 }, resolve(51).spellId));
		expect(eq(uint16_t { 13 }, resolve(101).spellId));
		expect(eq(uint16_t { 43 }, resolve(151).spellId));
		expect(eq(uint16_t { 289 }, resolve(201).spellId));

		for (const uint16_t blockStart : { 1, 51, 101, 151, 201 }) {
			expect(eq(static_cast<uint16_t>(PROFICIENCY_PERK_AUGMENT_TYPE), static_cast<uint16_t>(resolve(blockStart).perkType)));
		}
	};

	test("within a block, group is offset/10 and spell is offset%10") = [] {
		// Offsets 0..5 walk the six spells of group 0 (critical hit chance).
		expect(eq(uint16_t { 105 }, resolve(2).spellId));
		expect(eq(uint8_t { PROFICIENCY_AUGMENTTYPE_CRITICAL_HIT_CHANCE }, resolve(2).augmentType));
		// Offset 10 is the same first spell in group 1 (critical extra damage).
		expect(eq(uint16_t { 80 }, resolve(11).spellId));
		expect(eq(uint8_t { PROFICIENCY_AUGMENTTYPE_CRITICAL_EXTRA_DAMAGE }, resolve(11).augmentType));
		// Groups 2, 3 and 4 are base damage, mana leech and life leech.
		expect(eq(uint8_t { PROFICIENCY_AUGMENTTYPE_BASE_DAMAGE }, resolve(21).augmentType));
		expect(eq(uint8_t { PROFICIENCY_AUGMENTTYPE_MANA_LEECH }, resolve(31).augmentType));
		expect(eq(uint8_t { PROFICIENCY_AUGMENTTYPE_LIFE_LEECH }, resolve(41).augmentType));
	};

	test("offsets 6..9 of every group are holes") = [] {
		ProficiencyModifier modifier {};
		for (const uint16_t hole : { 7, 8, 9, 10, 17, 50, 57, 100 }) {
			expect(!ProficiencyModifiers::get(hole, modifier)) << "id " << hole << " should not resolve";
		}
		// So is anything past the last block and before the bestiary range.
		expect(!ProficiencyModifiers::get(0, modifier));
		expect(!ProficiencyModifiers::get(280, modifier));
		expect(!ProficiencyModifiers::get(324, modifier));
	};

	test("bestiary ids run 1..21 over 251..271") = [] {
		expect(eq(uint8_t { 1 }, resolve(251).bestiaryId));
		expect(eq(uint8_t { 21 }, resolve(271).bestiaryId));
		expect(eq(static_cast<uint16_t>(PROFICIENCY_PERK_BESTIARY_DAMAGE), static_cast<uint16_t>(resolve(251).perkType)));

		ProficiencyModifier modifier {};
		expect(!ProficiencyModifiers::get(272, modifier));
	};

	test("values track the client's rank curve") = [] {
		struct Case {
			uint16_t modifierEnum;
			int32_t rawMin;
			int32_t rawMax;
			bool integerValue;
		};
		// One per shape of entry: augment groups, bestiary, a fractional direct perk and an
		// integer one.
		constexpr Case cases[] = {
			{ 1, 100, 300, false }, // critical hit chance augment
			{ 11, 500, 2000, false }, // critical extra damage augment
			{ 41, 100, 1200, false }, // life leech augment
			{ 251, 50, 250, false }, // bestiary damage
			{ 285, 2, 12, true }, // life gain on hit - raw integer
			{ 287, 10, 50, true }, // life gain on kill - raw integer
			{ 323, 400, 1000, false }, // armor penetration
		};

		for (const auto &entry : cases) {
			const auto modifier = resolve(entry.modifierEnum);
			for (uint8_t rank = 0; rank <= PROFICIENCY_MAX_REFINE_RANK; ++rank) {
				const float expected = clientValue(entry.rawMin, entry.rawMax, rank, entry.integerValue);
				expect(eq(expected, ProficiencyModifiers::valueAtRank(modifier, rank)))
					<< "id " << entry.modifierEnum << " rank " << rank;
			}
		}
	};

	test("rank clamps at the maximum") = [] {
		const auto modifier = resolve(1);
		expect(eq(ProficiencyModifiers::valueAtRank(modifier, PROFICIENCY_MAX_REFINE_RANK),
		          ProficiencyModifiers::valueAtRank(modifier, PROFICIENCY_MAX_REFINE_RANK + 5)));
	};

	test("a vocation rolls only its own augments") = [] {
		// 30 augments (5 groups x 6 spells) + 21 bestiary classes + 13 direct perks.
		constexpr size_t expectedSize = 30 + 21 + 13;

		for (const uint8_t baseVocation : { 1, 2, 3, 4, 5 }) {
			const auto &pool = ProficiencyModifiers::rollablePool(baseVocation);
			expect(eq(expectedSize, pool.size())) << "vocation " << baseVocation;

			const uint16_t blockStart = ProficiencyModifiers::vocationBlockStart(baseVocation);
			expect(blockStart != 0_u);

			ProficiencyModifier modifier {};
			for (const uint16_t modifierEnum : pool) {
				// Every id in a pool must resolve - no holes may leak into a roll.
				expect(ProficiencyModifiers::get(modifierEnum, modifier)) << "id " << modifierEnum;
				// And any augment in it must come from this vocation's own block.
				if (modifierEnum <= 250) {
					expect(modifierEnum >= blockStart && modifierEnum < blockStart + 50)
						<< "id " << modifierEnum << " outside vocation " << baseVocation << "'s block";
				}
			}
		}
	};

	test("a vocation with no augment block still rolls the shared perks") = [] {
		expect(eq(uint16_t { 0 }, ProficiencyModifiers::vocationBlockStart(0)));
		expect(eq(size_t { 21 + 13 }, ProficiencyModifiers::rollablePool(0).size()));
	};
};
