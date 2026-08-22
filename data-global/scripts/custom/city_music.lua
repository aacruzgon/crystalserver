--[[
	City themes.

	The client ships the full CipSoft soundtrack inside its soundbank, but nothing
	on the server ever asked for it: sendMusicSoundEffect had exactly one caller,
	the /musicsound gamemaster talkaction. This script supplies the missing half.

	Towns come from the loaded map (Game.getTowns), so the areas below are matched
	by town NAME rather than by hardcoded coordinates - a map change moves the
	music with the temple instead of stranding it.

	Music ids are soundbank MusicTemplate ids. The named track for each is in the
	comment; ids not listed here simply play no theme.
]]

local CITY_MUSIC = {
	["thais"] = 21, -- Thais City
	["carlin"] = 11, -- Carlin
	["ab'dendriel"] = 10, -- Ab'Dendriel
	["kazordoon"] = 17, -- Kazordoon
	["venore"] = 23, -- Venore
	["edron"] = 13, -- Edron
	["darashia"] = 12, -- Darama
	["ankrahmun"] = 12, -- Darama
	["port hope"] = 8, -- Tiquanda
	["liberty bay"] = 8, -- Tiquanda
	["svargrond"] = 19, -- Hrodmir (Svargrond)
	["yalahar"] = 24, -- Yalahar
	["farmine"] = 14, -- Zao
	["rathleton"] = 18, -- Oramond (Rathleton)
	["issavi"] = 26, -- Kilmaresh
	["rookgaard"] = 27, -- Dawnport / Rookgaard
	["dawnport"] = 27, -- Dawnport / Rookgaard
	["marapur"] = 29, -- Marapur
	["silvertides"] = 29, -- Marapur
	["gray beach"] = 16, -- Quirefang and More
	["feyrist"] = 15, -- Feyrist
}

-- How far from a temple still counts as "in the city", in tiles.
local CITY_RADIUS = 50

-- The client dedupes repeated ids, so this only has to be fast enough to feel
-- immediate on foot; it is not a per-step hook.
local CHECK_INTERVAL = 2000

local MUSIC_NONE = 0

-- guid -> music id last sent, so we only emit on an actual change
local lastSent = {}

local musicTowns = nil

local function getMusicTowns()
	if musicTowns then
		return musicTowns
	end

	musicTowns = {}

	local matched, skipped = {}, {}
	for _, town in ipairs(Game.getTowns()) do
		local name = town:getName()
		local musicId = CITY_MUSIC[name:lower()]
		if musicId then
			musicTowns[#musicTowns + 1] = {
				name = name,
				musicId = musicId,
				position = town:getTemplePosition(),
			}
			matched[#matched + 1] = name
		else
			skipped[#skipped + 1] = name
		end
	end

	logger.info("[city_music] themed towns: {}", #matched > 0 and table.concat(matched, ", ") or "none")
	if #skipped > 0 then
		logger.debug("[city_music] towns with no theme mapped: {}", table.concat(skipped, ", "))
	end

	return musicTowns
end

local function musicForPosition(position)
	for _, town in ipairs(getMusicTowns()) do
		local temple = town.position
		if math.abs(position.x - temple.x) <= CITY_RADIUS and math.abs(position.y - temple.y) <= CITY_RADIUS then
			return town.musicId
		end
	end

	return MUSIC_NONE
end

local cityMusicTick = GlobalEvent("CityMusicTick")

function cityMusicTick.onThink(interval)
	-- resolve (and log) the town table on the first tick, with or without players
	getMusicTowns()

	for _, player in ipairs(Game.getPlayers()) do
		local guid = player:getGuid()
		local wanted = musicForPosition(player:getPosition())

		if lastSent[guid] ~= wanted then
			lastSent[guid] = wanted
			player:sendMusicSoundEffect(wanted)
		end
	end

	return true
end

cityMusicTick:interval(CHECK_INTERVAL)
cityMusicTick:register()

-- Drop the memo on logout, otherwise a returning character whose theme has not
-- changed would never be sent it again.
local cityMusicLogout = CreatureEvent("CityMusicLogout")

function cityMusicLogout.onLogout(player)
	lastSent[player:getGuid()] = nil
	return true
end

cityMusicLogout:register()
