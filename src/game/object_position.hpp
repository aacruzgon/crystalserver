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

#include "game/movement/position.hpp"

// Which arm of CipSoft's ObjectPosition an object sits in.
enum class ObjectPositionKind : uint8_t {
	Worldmap,
	Inventory,
	Container,
	DepotSearchDepot,
	DepotSearchInbox,
	Any
};

// CipSoft models "where an object is" as ObjectPosition, a six-arm choice. The legacy wire
// has no such message - it packs all six arms into a single Position triple, and every
// reader rediscovers the encoding from magic numbers:
//
//   Worldmap          x != 0xFFFF                        real coordinates
//   Inventory         x == 0xFFFF, y = slot, z = 0       y is a Slots_t
//   Container         x == 0xFFFF, y = 0x40 | id, z = i  id is an open container, i the row
//   DepotSearchDepot  x == 0xFFFF, y == 0x20
//   DepotSearchInbox  x == 0xFFFF, y == 0x21
//   Any               x == 0xFFFF, y == 0, z == 0        "any object of this type"
//
// This is the one place that encoding is written down. It names the shape; it does not
// change the protocol, and the client carries the same six arms in src/client/objectposition.h.
struct ObjectPosition {
	static constexpr uint16_t LEGACY_NON_MAP_X = 0xFFFF;
	static constexpr uint8_t LEGACY_CONTAINER_FLAG = 0x40;
	static constexpr uint8_t LEGACY_CONTAINER_ID_MASK = 0x0F;
	static constexpr uint8_t LEGACY_DEPOT_SEARCH_DEPOT_Y = 0x20;
	static constexpr uint8_t LEGACY_DEPOT_SEARCH_INBOX_Y = 0x21;

	ObjectPositionKind kind = ObjectPositionKind::Any;

	// Worldmap only.
	Position coordinate;
	// Container: the open container's id. Inventory: the Slots_t.
	uint8_t slot = 0;
	// Container: the row within the container.
	uint8_t row = 0;

	// Recover the arm from the legacy triple. Reads the same discriminants, in the same
	// order, that internalGetThing used to test inline.
	static ObjectPosition fromLegacy(const Position &position) {
		ObjectPosition result;
		if (position.x != LEGACY_NON_MAP_X) {
			result.kind = ObjectPositionKind::Worldmap;
			result.coordinate = position;
			return result;
		}

		const auto y = static_cast<uint8_t>(position.y);
		if (y & LEGACY_CONTAINER_FLAG) {
			result.kind = ObjectPositionKind::Container;
			result.slot = y & LEGACY_CONTAINER_ID_MASK;
			result.row = position.z;
			return result;
		}

		if (y == LEGACY_DEPOT_SEARCH_DEPOT_Y) {
			result.kind = ObjectPositionKind::DepotSearchDepot;
			return result;
		}

		if (y == LEGACY_DEPOT_SEARCH_INBOX_Y) {
			result.kind = ObjectPositionKind::DepotSearchInbox;
			return result;
		}

		if (y == 0 && position.z == 0) {
			result.kind = ObjectPositionKind::Any;
			return result;
		}

		result.kind = ObjectPositionKind::Inventory;
		result.slot = y;
		return result;
	}
};
