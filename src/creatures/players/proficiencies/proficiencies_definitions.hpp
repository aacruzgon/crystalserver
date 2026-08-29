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

#pragma once

enum WeaponProficiencyPerkType_t : uint16_t {
	PROFICIENCY_PERK_ATTACK_DAMAGE = 0,
	PROFICIENCY_PERK_DEFENSE = 1,
	PROFICIENCY_PERK_WEAPON_SHIELD_MOD = 2,
	PROFICIENCY_PERK_SKILLID_BONUS = 3,
	PROFICIENCY_PERK_SPECIAL_MAGIC_LEVEL = 4,
	PROFICIENCY_PERK_AUGMENT_TYPE = 5,
	PROFICIENCY_PERK_BESTIARY_DAMAGE = 6,
	PROFICIENCY_PERK_DAMAGE_GAIN_BOSS_AND_SINISTER_EMBRACED = 7,
	PROFICIENCY_PERK_CRITICAL_HIT_CHANCE = 8,
	PROFICIENCY_PERK_CRITICAL_HIT_CHANCE_FOR_ELEMENT_ID_SPELLS_AND_RUNES = 9,
	PROFICIENCY_PERK_CRITICAL_HIT_CHANCE_FOR_OFFENSIVE_RUNES = 10,
	PROFICIENCY_PERK_CRITICAL_HIT_CHANCE_FOR_AUTOATTACK = 11,
	PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE = 12,
	PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE_FOR_ELEMENT_ID_SPELLS_AND_RUNES = 13,
	PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE_FOR_OFFENSIVE_RUNES = 14,
	PROFICIENCY_PERK_CRITICAL_EXTRA_DAMAGE_FOR_AUTOATTACK = 15,
	PROFICIENCY_PERK_MANA_LEECH = 16,
	PROFICIENCY_PERK_LIFE_LEECH = 17,
	PROFICIENCY_PERK_MANA_GAIN_ONHIT = 18,
	PROFICIENCY_PERK_LIFE_GAIN_ONHIT = 19,
	PROFICIENCY_PERK_MANA_GAIN_ONKILL = 20,
	PROFICIENCY_PERK_LIFE_GAIN_ONKILL = 21,
	PROFICIENCY_PERK_GAIN_DAMAGE_AT_RANGE = 22,
	PROFICIENCY_PERK_RANGED_HIT_CHANCE = 23,
	PROFICIENCY_PERK_ATTACK_RANGE = 24,
	PROFICIENCY_PERK_SKILLID_PERCENTAGE_AS_EXTRA_DAMAGE_FOR_AUTOATTACK = 25,
	PROFICIENCY_PERK_SKILLID_PERCENTAGE_AS_EXTRA_DAMAGE_FOR_SPELLS = 26,
	PROFICIENCY_PERK_SKILLID_PERCENTAGE_AS_EXTRA_HEALING_FOR_SPELLS = 27,
	PROFICIENCY_PERK_ALPHA_STRIKE_EXTRA_DAMAGE = 28,
	PROFICIENCY_PERK_OMEGA_STRIKE_EXTRA_DAMAGE = 29,
	PROFICIENCY_PERK_ARMOR_PENETRATION = 30,
	PROFICIENCY_PERK_ELEMENTAL_PIERCE = 31,
	PROFICIENCY_PERK_HOMING_MISSILE = 32,
	// Defined by the official client but unused by the 15.32 proficiency table. They are
	// reachable in play all the same: the 15.30 shaping pool can roll them onto a slot.
	PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_DAMAGE_FOR_AUTOATTACK = 33,
	PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_DAMAGE_FOR_SPELLS = 34,
	PROFICIENCY_PERK_HIGHEST_COMBAT_SKILL_AS_EXTRA_HEALING_FOR_SPELLS = 35,
};

enum WeaponProficiencyPerkSkills_t : int8_t {
	PROFICIENCY_SKILL_MAGIC = 1,
	PROFICIENCY_SKILL_SHIELD = 6,
	PROFICIENCY_SKILL_DISTANCE = 7,
	PROFICIENCY_SKILL_SWORD = 8,
	PROFICIENCY_SKILL_CLUB = 9,
	PROFICIENCY_SKILL_AXE = 10,
	PROFICIENCY_SKILL_FIST = 11,
	PROFICIENCY_SKILL_FISHING = 13,
};

// Recovered from the official 15.32 client: the augment description dispatcher computes
// `augmentType - 2`, bounds-checks against 15 and jumps through a 16-entry table. Only the
// seven members below resolve to a description string; 1, 4 and 7-13 all fall through to
// the client's "dummy_unknown" branch, so they play no part in weapon proficiency and are
// left unnamed rather than guessed at.
enum WeaponProficiencyPerkAugmentType_t : uint8_t {
	PROFICIENCY_AUGMENTTYPE_NONE = 0,
	// "%1 base damage for %2" - value is a 0..1 fraction.
	PROFICIENCY_AUGMENTTYPE_BASE_DAMAGE = 2,
	// "%1 healing for %2" - value is a 0..1 fraction.
	PROFICIENCY_AUGMENTTYPE_HEALING = 3,
	// "%1 chain length for %2" - value is a target count. Unused by the 15.32 data.
	PROFICIENCY_AUGMENTTYPE_CHAIN_LENGTH = 5,
	// "%1 cooldown for %2" - the one augment whose value is signed SECONDS rather than a
	// fraction, and the only source of negative values anywhere in the proficiency table.
	PROFICIENCY_AUGMENTTYPE_COOLDOWN = 6,
	// The remaining four are 0..1 fractions.
	PROFICIENCY_AUGMENTTYPE_LIFE_LEECH = 14,
	PROFICIENCY_AUGMENTTYPE_MANA_LEECH = 15,
	PROFICIENCY_AUGMENTTYPE_CRITICAL_EXTRA_DAMAGE = 16,
	PROFICIENCY_AUGMENTTYPE_CRITICAL_HIT_CHANCE = 17,
};

enum WeaponProficiencyPerkDamageType_t : int32_t {
	PROFICIENCY_DAMAGETYPE_NONE = 0,
	PROFICIENCY_DAMAGETYPE_PHYSICAL = 1, // COMBAT_PHYSICALDAMAGE
	PROFICIENCY_DAMAGETYPE_FIRE = 8, // COMBAT_FIREDAMAGE
	PROFICIENCY_DAMAGETYPE_EARTH = 16, // COMBAT_EARTHDAMAGE
	PROFICIENCY_DAMAGETYPE_ENERGY = 32, // COMBAT_ENERGYDAMAGE
	PROFICIENCY_DAMAGETYPE_ICE = 64, // COMBAT_ICEDAMAGE
	PROFICIENCY_DAMAGETYPE_HOLY = 128, // COMBAT_HOLYDAMAGE
	PROFICIENCY_DAMAGETYPE_DEATH = 256, // COMBAT_DEATHDAMAGE
	PROFICIENCY_DAMAGETYPE_HEALING = 1048576, // COMBAT_HEALING
};
