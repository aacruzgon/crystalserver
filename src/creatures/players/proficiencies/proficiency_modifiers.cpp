////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#include "creatures/players/proficiencies/proficiency_modifiers.hpp"

namespace {

constexpr uint16_t kSpellAugmentLast = 250;
constexpr uint16_t kBestiaryFirst = 251;
constexpr uint16_t kBestiaryLast = 271;
constexpr uint8_t kBlockWidth = 50;
constexpr uint8_t kSpellsPerGroup = 6; // offsets 6..9 of each group of ten are holes
constexpr uint8_t kGroupsPerBlock = 5;

// Block start -> the six spells that block's augments attach to. The blocks are ordered
// knight, paladin, sorcerer, druid, monk; the spell ids identify them unambiguously
// (Berserk/Fierce Berserk, Divine Caldera/Barrage, Energy Wave/Hell's Core, Terra Wave/
// Ice Burst, Flurry of Blows/Chained Penance).
struct VocationBlock {
	uint16_t blockStart;
	uint8_t baseVocationId;
	uint16_t spells[kSpellsPerGroup];
};

constexpr VocationBlock kVocationBlocks[] = {
	{ 1, 4 /* knight   */, { 80, 105, 106, 59, 316, 261 } },
	{ 51, 3 /* paladin  */, { 124, 302, 303, 258, 57, 122 } },
	{ 101, 1 /* sorcerer */, { 13, 24, 240, 260, 310, 23 } },
	{ 151, 2 /* druid    */, { 43, 120, 262, 263, 317, 318 } },
	{ 201, 5 /* monk     */, { 289, 288, 294, 287, 301, 290 } },
};

struct AugmentGroup {
	uint8_t augmentType;
	int32_t rawMin;
	int32_t rawMax;
};

// Group index within a block is offset / 10.
constexpr AugmentGroup kAugmentGroups[kGroupsPerBlock] = {
	{ PROFICIENCY_AUGMENTTYPE_CRITICAL_HIT_CHANCE, 100, 300 },
	{ PROFICIENCY_AUGMENTTYPE_CRITICAL_EXTRA_DAMAGE, 500, 2000 },
	{ PROFICIENCY_AUGMENTTYPE_BASE_DAMAGE, 100, 300 },
	{ PROFICIENCY_AUGMENTTYPE_MANA_LEECH, 100, 600 },
	{ PROFICIENCY_AUGMENTTYPE_LIFE_LEECH, 100, 1200 },
};

constexpr int32_t kBestiaryRawMin = 50;
constexpr int32_t kBestiaryRawMax = 250;

struct DirectPerk {
	uint16_t modifierEnum;
	WeaponProficiencyPerkType_t perkType;
	int32_t rawMin;
	int32_t rawMax;
	bool integerValue;
};

constexpr DirectPerk kDirectPerks[] = {
	{ 281, PROFICIENCY_PERK_CRITICAL_HIT_CHANCE_FOR_OFFENSIVE_RUNES, 50, 150, false },
	{ 282, PROFICIENCY_PERK_CRITICAL_HIT_CHANCE_FOR_AUTOATTACK, 50, 250, false },
	{ 283, PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE_FOR_OFFENSIVE_RUNES, 200, 1500, false },
	{ 284, PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE_FOR_AUTOATTACK, 300, 2000, false },
	{ 285, PROFICIENCY_PERK_LIFE_GAIN_ONHIT, 2, 12, true },
	{ 286, PROFICIENCY_PERK_MANA_GAIN_ONKILL, 4, 24, true },
	{ 287, PROFICIENCY_PERK_LIFE_GAIN_ONKILL, 10, 50, true },
	{ 291, PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_DAMAGE_FOR_AUTOATTACK, 200, 1000, false },
	{ 292, PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_DAMAGE_FOR_SPELLS, 100, 800, false },
	{ 293, PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_HEALING_FOR_SPELLS, 200, 1000, false },
	{ 321, PROFICIENCY_PERK_ALPHA_STRIKE_EXTRA_DAMAGE, 200, 1000, false },
	{ 322, PROFICIENCY_PERK_OMEGA_STRIKE_EXTRA_DAMAGE, 50, 250, false },
	{ 323, PROFICIENCY_PERK_ARMOR_PENETRATION, 400, 1000, false },
};

const VocationBlock* blockForVocation(uint8_t baseVocationId) {
	for (const auto &block : kVocationBlocks) {
		if (block.baseVocationId == baseVocationId) {
			return &block;
		}
	}
	return nullptr;
}

const VocationBlock* blockContaining(uint16_t modifierEnum) {
	for (const auto &block : kVocationBlocks) {
		if (modifierEnum >= block.blockStart && modifierEnum < block.blockStart + kBlockWidth) {
			return &block;
		}
	}
	return nullptr;
}

} // namespace

namespace ProficiencyModifiers {

	bool get(uint16_t modifierEnum, ProficiencyModifier &out) {
		if (modifierEnum == 0) {
			return false;
		}

		if (modifierEnum <= kSpellAugmentLast) {
			const VocationBlock* block = blockContaining(modifierEnum);
			if (!block) {
				return false;
			}

			const uint16_t offset = modifierEnum - block->blockStart;
			const uint8_t spellIndex = static_cast<uint8_t>(offset % 10);
			const uint8_t groupIndex = static_cast<uint8_t>(offset / 10);
			if (spellIndex >= kSpellsPerGroup || groupIndex >= kGroupsPerBlock) {
				return false;
			}

			const AugmentGroup &group = kAugmentGroups[groupIndex];
			out = {};
			out.perkType = PROFICIENCY_PERK_AUGMENT_TYPE;
			out.spellId = block->spells[spellIndex];
			out.augmentType = group.augmentType;
			out.rawMin = group.rawMin;
			out.rawMax = group.rawMax;
			return true;
		}

		if (modifierEnum >= kBestiaryFirst && modifierEnum <= kBestiaryLast) {
			out = {};
			out.perkType = PROFICIENCY_PERK_BESTIARY_DAMAGE;
			out.bestiaryId = static_cast<uint8_t>(modifierEnum - (kBestiaryFirst - 1));
			out.rawMin = kBestiaryRawMin;
			out.rawMax = kBestiaryRawMax;
			return true;
		}

		for (const auto &direct : kDirectPerks) {
			if (direct.modifierEnum == modifierEnum) {
				out = {};
				out.perkType = direct.perkType;
				out.rawMin = direct.rawMin;
				out.rawMax = direct.rawMax;
				out.integerValue = direct.integerValue;
				return true;
			}
		}

		return false;
	}

	float valueAtRank(const ProficiencyModifier &modifier, uint8_t rank) {
		int32_t raw;
		if (rank == 0) {
			raw = modifier.rawMin;
		} else if (rank >= PROFICIENCY_MAX_REFINE_RANK) {
			raw = modifier.rawMax;
		} else {
			const double step = static_cast<double>(modifier.rawMax - modifier.rawMin) / PROFICIENCY_MAX_REFINE_RANK;
			raw = modifier.rawMin + static_cast<int32_t>(std::floor(step * rank));
		}

		return modifier.integerValue ? static_cast<float>(raw) : static_cast<float>(raw) / 10000.0f;
	}

	uint16_t vocationBlockStart(uint8_t baseVocationId) {
		const VocationBlock* block = blockForVocation(baseVocationId);
		return block ? block->blockStart : 0;
	}

	const std::vector<uint16_t> &rollablePool(uint8_t baseVocationId) {
		// Built once per vocation and handed out by reference; the table is static data.
		static std::map<uint8_t, std::vector<uint16_t>> pools;
		static std::mutex poolsMutex;

		std::scoped_lock lock(poolsMutex);
		auto it = pools.find(baseVocationId);
		if (it != pools.end()) {
			return it->second;
		}

		std::vector<uint16_t> pool;

		if (const VocationBlock* block = blockForVocation(baseVocationId)) {
			for (uint8_t group = 0; group < kGroupsPerBlock; ++group) {
				for (uint8_t spell = 0; spell < kSpellsPerGroup; ++spell) {
					pool.push_back(static_cast<uint16_t>(block->blockStart + group * 10 + spell));
				}
			}
		}

		for (uint16_t bestiary = kBestiaryFirst; bestiary <= kBestiaryLast; ++bestiary) {
			pool.push_back(bestiary);
		}

		for (const auto &direct : kDirectPerks) {
			pool.push_back(direct.modifierEnum);
		}

		return pools.emplace(baseVocationId, std::move(pool)).first->second;
	}

} // namespace ProficiencyModifiers
