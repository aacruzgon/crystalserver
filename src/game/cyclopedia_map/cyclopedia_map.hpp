////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////

#pragma once

#include "lib/di/soft_singleton.hpp"

#include "game/movement/position.hpp"

class SoftSingleton;
class SoftSingletonGuard;

// One rectangle of a subarea. Floor-independent: a subarea covers its rectangle
// on every z, which is how the client's zone table is shaped too.
struct CyclopediaMapRect {
	uint16_t fromX = 0;
	uint16_t fromY = 0;
	uint16_t toX = 0;
	uint16_t toY = 0;

	[[nodiscard]] bool contains(const Position &position) const {
		return position.x >= fromX && position.x <= toX && position.y >= fromY && position.y <= toY;
	}
};

struct CyclopediaMapSubArea {
	uint16_t id = 0;
	uint16_t areaId = 0;
	std::string name;
	std::vector<CyclopediaMapRect> rects;

	[[nodiscard]] bool contains(const Position &position) const {
		return std::ranges::any_of(rects, [&position](const auto &rect) { return rect.contains(position); });
	}
};

struct CyclopediaMapArea {
	uint16_t id = 0;
	std::string name;
	std::vector<uint16_t> subAreas;
	// Gold the world must donate before this area's improved respawn switches on.
	uint64_t donationGoal = 0;
};

// The Cyclopedia Map's static geography: 22 main areas made of 207 subareas.
//
// Deliberately NOT built on the Zone system. Zone::addArea() expands a rectangle
// into one entry per tile, which is right for a quest room and ruinous here - these
// rectangles cover whole continents, so expanding them would be hundreds of millions
// of positions. A subarea lookup is instead a point-in-rect test, cheap enough to run
// on every step once the caller caches the subarea it was in last (see
// Player::updateCyclopediaMapArea).
class CyclopediaMap {
public:
	CyclopediaMap() = default;

	// Singletons should not be cloneable nor assignable.
	CyclopediaMap(const CyclopediaMap &) = delete;
	CyclopediaMap &operator=(const CyclopediaMap &) = delete;

	static CyclopediaMap &getInstance();

	bool loadFromXml();

	// Returns 0 when the position is not inside any known subarea, which is normal -
	// the table only covers the discoverable world, not every tile on the map.
	[[nodiscard]] uint16_t getSubAreaIdAt(const Position &position) const;

	[[nodiscard]] const CyclopediaMapSubArea *getSubArea(uint16_t subAreaId) const;
	[[nodiscard]] const CyclopediaMapArea *getArea(uint16_t areaId) const;
	[[nodiscard]] uint16_t getAreaIdOfSubArea(uint16_t subAreaId) const;

	[[nodiscard]] const std::vector<CyclopediaMapArea> &getAreas() const {
		return m_areas;
	}

	[[nodiscard]] uint64_t getDefaultDonationGoal() const {
		return m_defaultDonationGoal;
	}

	[[nodiscard]] size_t getSubAreaCount() const {
		return m_subAreas.size();
	}

private:
	std::vector<CyclopediaMapArea> m_areas;
	std::unordered_map<uint16_t, CyclopediaMapSubArea> m_subAreas;
	// Iterated linearly by getSubAreaIdAt; 207 rect tests is nothing next to the
	// work a single step already does, and callers only reach here on a cache miss.
	std::vector<uint16_t> m_subAreaOrder;
	uint64_t m_defaultDonationGoal = 0;

	static SoftSingleton instanceTracker;
	SoftSingletonGuard guard { instanceTracker };
};

constexpr auto g_cyclopediaMap = CyclopediaMap::getInstance;
