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

#include "creatures/players/proficiencies/proficiencies_definitions.hpp"

// The pool a shaped perk is drawn from, addressed by the same "modifier enum" the client
// decodes for display. Both sides must agree on this numbering exactly - the wire only
// carries the id and the rank, and the client rebuilds the perk text from its own copy of
// the table. The layout is:
//
//   1..250    spell augments, in five 50-wide vocation blocks (1 knight, 51 paladin,
//             101 sorcerer, 151 druid, 201 monk). Within a block, offset = id - blockStart,
//             the augment group is offset / 10 (0..4) and the spell is offset % 10 (0..5);
//             offsets whose remainder is 6..9 are holes.
//   251..271  percentage damage against one bestiary class (id - 250).
//   281..323  direct perks, sparsely numbered.
//
// A perk's value scales with its refine rank over 0..PROFICIENCY_MAX_REFINE_RANK: rank 0
// gives rawMin, rank 10 gives rawMax, and the steps between are
// rawMin + floor(((rawMax - rawMin) / 10) * rank).
struct ProficiencyModifier {
	WeaponProficiencyPerkType_t perkType = PROFICIENCY_PERK_ATTACK_DAMAGE;
	uint16_t spellId = 0;
	uint8_t augmentType = 0;
	uint8_t bestiaryId = 0;
	int32_t rawMin = 0;
	int32_t rawMax = 0;
	// Life/mana on hit/kill are stored as plain integers; every other perk's raw value is
	// a ten-thousandth, matching the fractions in proficiencies.json.
	bool integerValue = false;
};

namespace ProficiencyModifiers {

	// Resolves a pool id. Returns false for holes and out-of-range ids.
	bool get(uint16_t modifierEnum, ProficiencyModifier &out);
	// The perk value at a given refine rank, in the same units proficiencies.json uses.
	float valueAtRank(const ProficiencyModifier &modifier, uint8_t rank);
	// Every id a player of this base vocation may roll. Vocation-specific spell augments are
	// restricted to that vocation's block; bestiary and direct perks are shared.
	const std::vector<uint16_t> &rollablePool(uint8_t baseVocationId);
	// The spell-augment block a base vocation rolls from, or 0 when it has none.
	uint16_t vocationBlockStart(uint8_t baseVocationId);

} // namespace ProficiencyModifiers
