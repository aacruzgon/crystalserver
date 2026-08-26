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

class Player;
class KV;

struct Summary {
	uint16_t m_preyWildcards = 0;
	uint16_t m_instantRewards = 0;
	uint16_t m_hirelings = 0;

	[[maybe_unused]] Summary(uint16_t mPreyWildcards, uint16_t mInstantRewards, uint16_t mHirelings) :
		m_preyWildcards(mPreyWildcards), m_instantRewards(mInstantRewards), m_hirelings(mHirelings) { }
};

class PlayerCyclopedia {
public:
	explicit PlayerCyclopedia(Player &player);

	Summary getSummary() const;

	void loadSummaryData() const;
	void loadDeathHistory(uint16_t page, uint16_t entriesPerPage) const;
	void loadRecentKills(uint16_t page, uint16_t entriesPerPage) const;

	void updateStoreSummary(uint8_t type, uint16_t amount = 1, const std::string &id = "") const;
	uint16_t getAmount(uint8_t type) const;
	void updateAmount(uint8_t type, uint16_t amount = 1) const;

	[[nodiscard]] std::map<uint16_t, uint16_t> getResult(uint8_t type) const;
	void insertValue(uint8_t type, uint16_t amount = 1, const std::string &id = "") const;

	// --- Cyclopedia Map discovery ---------------------------------------------
	// A subarea is discovered by standing in it. The set is the player's progress
	// through the world and is what drives the per-area percentage on the Map tab.
	void loadMapDiscovery();
	[[nodiscard]] bool isSubAreaDiscovered(uint16_t subAreaId) const;
	// True only the first time, so callers can push an update without diffing the set.
	bool discoverSubArea(uint16_t subAreaId);
	[[nodiscard]] const std::unordered_set<uint16_t> &getDiscoveredSubAreas() const;
	// Percentage of an area's subareas the player has discovered, 0-100.
	[[nodiscard]] uint8_t getAreaProgress(uint16_t areaId) const;

	// Where the player is right now. Transient: recomputed on move and on login.
	[[nodiscard]] uint16_t getCurrentAreaId() const {
		return m_currentAreaId;
	}
	[[nodiscard]] uint16_t getCurrentSubAreaId() const {
		return m_currentSubAreaId;
	}
	void setCurrentArea(uint16_t areaId, uint16_t subAreaId) {
		m_currentAreaId = areaId;
		m_currentSubAreaId = subAreaId;
	}

private:
	const std::shared_ptr<KV> &getMapDiscoveryKV();

	Player &m_player;

	std::unordered_set<uint16_t> m_discoveredSubAreas;
	std::shared_ptr<KV> m_mapDiscoveryKV;
	uint16_t m_currentAreaId = 0;
	uint16_t m_currentSubAreaId = 0;
};
