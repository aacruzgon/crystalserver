////////////////////////////////////////////////////////////////////////
// Crystal Server - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////

#include "game/cyclopedia_map/cyclopedia_map.hpp"

#include "config/configmanager.hpp"
#include "lib/di/container.hpp"
#include "utils/tools.hpp"

SoftSingleton CyclopediaMap::instanceTracker("CyclopediaMap");

CyclopediaMap &CyclopediaMap::getInstance() {
	return inject<CyclopediaMap>();
}

bool CyclopediaMap::loadFromXml() {
	m_areas.clear();
	m_subAreas.clear();
	m_subAreaOrder.clear();

	pugi::xml_document doc;
	const auto fileName = g_configManager().getString(CORE_DIRECTORY) + "/XML/cyclopedia_map.xml";
	const auto result = doc.load_file(fileName.c_str());
	if (!result) {
		printXMLError(__FUNCTION__, fileName, result);
		return false;
	}

	const auto rootNode = doc.child("cyclopediamap");
	m_defaultDonationGoal = rootNode.attribute("donationGoal").as_ullong();

	for (const auto &areaNode : rootNode.children("area")) {
		CyclopediaMapArea area;
		area.id = static_cast<uint16_t>(areaNode.attribute("id").as_uint());
		area.name = areaNode.attribute("name").as_string();
		const auto goalAttr = areaNode.attribute("goal");
		area.donationGoal = goalAttr ? goalAttr.as_ullong() : m_defaultDonationGoal;
		if (area.id == 0) {
			g_logger().warn("[{}] skipping area with no id", __FUNCTION__);
			continue;
		}

		for (const auto &subAreaNode : areaNode.children("subarea")) {
			CyclopediaMapSubArea subArea;
			subArea.id = static_cast<uint16_t>(subAreaNode.attribute("id").as_uint());
			subArea.areaId = area.id;
			subArea.name = subAreaNode.attribute("name").as_string();
			if (subArea.id == 0) {
				g_logger().warn("[{}] skipping subarea with no id in area {}", __FUNCTION__, area.id);
				continue;
			}

			for (const auto &rectNode : subAreaNode.children("rect")) {
				const auto x = static_cast<uint16_t>(rectNode.attribute("x").as_uint());
				const auto y = static_cast<uint16_t>(rectNode.attribute("y").as_uint());
				const auto width = rectNode.attribute("width").as_uint();
				const auto height = rectNode.attribute("height").as_uint();
				if (width == 0 || height == 0) {
					continue;
				}

				CyclopediaMapRect rect;
				rect.fromX = x;
				rect.fromY = y;
				// width/height are tile counts, so the last tile is from + size - 1.
				rect.toX = static_cast<uint16_t>(x + width - 1);
				rect.toY = static_cast<uint16_t>(y + height - 1);
				subArea.rects.emplace_back(rect);
			}

			if (subArea.rects.empty()) {
				g_logger().warn("[{}] subarea {} has no usable rect", __FUNCTION__, subArea.id);
				continue;
			}

			if (m_subAreas.contains(subArea.id)) {
				g_logger().warn("[{}] duplicate subarea id {}", __FUNCTION__, subArea.id);
				continue;
			}

			area.subAreas.emplace_back(subArea.id);
			m_subAreaOrder.emplace_back(subArea.id);
			m_subAreas.emplace(subArea.id, std::move(subArea));
		}

		if (area.subAreas.empty()) {
			g_logger().warn("[{}] area {} has no subareas", __FUNCTION__, area.id);
			continue;
		}

		m_areas.emplace_back(std::move(area));
	}

	if (m_areas.empty()) {
		g_logger().warn("[{}] no cyclopedia map areas loaded", __FUNCTION__);
		return false;
	}

	g_logger().info("Loaded {} cyclopedia map areas with {} subareas", m_areas.size(), m_subAreas.size());
	return true;
}

uint16_t CyclopediaMap::getSubAreaIdAt(const Position &position) const {
	// Subarea rectangles overlap - "Thais City" sits inside "Thais Surroundings" - so a
	// first-hit scan would pick whichever happened to be listed first. The client resolves
	// this by taking the SMALLEST matching rectangle (see cyclopediaAreaIdAtTile in the
	// client's map.lua), so match that rule here or the two sides disagree about where the
	// player is standing.
	//
	// The client can refine further using a per-subarea shape mask keyed by the subarea id;
	// that mask is a client asset and is not available here, so a position inside the
	// bounding box but outside the true shape resolves to the smallest box rather than to
	// the mask's answer. Worth knowing if a subarea ever looks off by a border tile.
	uint16_t bestSubAreaId = 0;
	uint64_t bestArea = std::numeric_limits<uint64_t>::max();

	for (const auto subAreaId : m_subAreaOrder) {
		const auto it = m_subAreas.find(subAreaId);
		if (it == m_subAreas.end()) {
			continue;
		}

		for (const auto &rect : it->second.rects) {
			if (!rect.contains(position)) {
				continue;
			}

			const auto area = static_cast<uint64_t>(rect.toX - rect.fromX + 1) * static_cast<uint64_t>(rect.toY - rect.fromY + 1);
			if (area < bestArea) {
				bestArea = area;
				bestSubAreaId = subAreaId;
			}
		}
	}

	return bestSubAreaId;
}

const CyclopediaMapSubArea *CyclopediaMap::getSubArea(uint16_t subAreaId) const {
	const auto it = m_subAreas.find(subAreaId);
	return it != m_subAreas.end() ? &it->second : nullptr;
}

const CyclopediaMapArea *CyclopediaMap::getArea(uint16_t areaId) const {
	const auto it = std::ranges::find_if(m_areas, [areaId](const auto &area) { return area.id == areaId; });
	return it != m_areas.end() ? &*it : nullptr;
}

uint16_t CyclopediaMap::getAreaIdOfSubArea(uint16_t subAreaId) const {
	const auto *subArea = getSubArea(subAreaId);
	return subArea ? subArea->areaId : 0;
}
