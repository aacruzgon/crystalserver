-- City ambience follows the towns embedded in the loaded map. The previous
-- implementation subscribed to named zones such as "thais-city", but those
-- zones are not present in world-zones.xml or in the OTBM, so no player could
-- ever enter them.

local TOWN_AMBIENCE = {
	["thais"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_SWAMP_INSECTS_BIRDS_NOISES_CITY,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_CREATURES_INSECTS_NIGHT,
	},
	["carlin"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_SWAMP_INSECTS_BIRDS_NOISES_CITY,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_CREATURES_INSECTS_NIGHT,
	},
	["ab'dendriel"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_BIRDS_MAGIC_SPARKS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_SPARKS_INSECTS_CREATURES_NIGHT,
	},
	["kazordoon"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_CAVE_WIND_NOISES,
		night = SOUND_EFFECT_TYPE_AMBIENT_CAVE_HAMMERING_NOISES_HAMMERING,
	},
	["venore"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_SWAMP_INSECTS_BIRDS_NOISES,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_CREATURES_INSECTS_NIGHT,
	},
	["edron"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_INSECTS_CREATURES,
		night = SOUND_EFFECT_TYPE_AMBIENT_INSECTS_CREATURES_NIGHT,
	},
	["darashia"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM_2,
	},
	["ankrahmun"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM_2,
	},
	["port hope"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_CITY_NATURE_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["liberty bay"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WATER_PORT_SEA_BIRDS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WATER_PORT_SEA_BIRDS_2,
	},
	["svargrond"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_STORM,
		night = SOUND_EFFECT_TYPE_AMBIENT_CREATURES_STORM_NOISES_NIGHT,
	},
	["yalahar"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NOISES_MAGIC_SPARKS_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_NOISES_MAGIC_SPARKS_STORM,
	},
	["farmine"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_BIRDS_HAMMERING_CREATURE_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_NOISES_CREATURES_INSECTS_NIGHT,
	},
	["rathleton"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_HUMANS_HAMMERING,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_CREATURES_HAMMERING,
	},
	["issavi"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM_2,
	},
	["rookgaard"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_DAY,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["dawnport"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_DAY,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["dawnport tutorial"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_DAY,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["island of destiny"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_DAY,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["marapur"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_BIRDS_NOISES,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_CREATURES_INSECTS_NOISES_NIGHT,
	},
	["silvertides"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WATER_SEA,
		night = SOUND_EFFECT_TYPE_AMBIENT_WATER_SEA,
	},
	["moonfall"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WATER_SEA,
		night = SOUND_EFFECT_TYPE_AMBIENT_WATER_SEA,
	},
	["gray beach"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_CREATURES_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_INSECTS_CREATURES,
	},
	["krailos"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_CREATURES_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_INSECTS_CREATURES,
	},
	["roshamuul"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_CREATURES_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_CREATURES_NIGHT,
	},
	["cobra bastion"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_NOISES_STORM_2,
	},
	["bounac"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_CITY_NATURE_HUMANS,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["feyrist"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_BIRDS_MAGIC_SPARKS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WIND_MAGIC_SPARKS_INSECTS_CREATURES_NIGHT,
	},
	["gnomprona"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_BUBBLING_NOISES,
		night = SOUND_EFFECT_TYPE_AMBIENT_BUBBLING_NOISES,
	},
	["candia"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_NATURE_DAY,
		night = SOUND_EFFECT_TYPE_AMBIENT_NATURE_NIGHT,
	},
	["blue valley"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_STREAM_91,
		night = SOUND_EFFECT_TYPE_AMBIENT_STREAM_92,
	},
	["newhaven"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_WATER_PORT_SEA_BIRDS,
		night = SOUND_EFFECT_TYPE_AMBIENT_WATER_PORT_SEA_BIRDS_2,
	},
	["targuna"] = {
		day = SOUND_EFFECT_TYPE_AMBIENT_STREAM_100,
		night = SOUND_EFFECT_TYPE_AMBIENT_STREAM_100,
	},
}

local CITY_RADIUS = 60
local CITY_KEEP_RADIUS = 110
local MAX_FLOOR_DISTANCE = 2
local CHECK_INTERVAL = 2000

local CITY_RADIUS_OVERRIDE = {
	["thais"] = 90,
	["venore"] = 80,
	["yalahar"] = 90,
	["kazordoon"] = 80,
}

-- guid -> { ambienceId, townName }
local lastSent = {}
local ambienceTowns = nil

local function getEffectivePeriod()
	if forcePeriod then
		return forcePeriod == "night" and "night" or "day"
	end
	return getTibiaTimerDayOrNight() == "night" and "night" or "day"
end

local function getAmbienceTowns()
	if ambienceTowns then
		return ambienceTowns
	end

	ambienceTowns = {}
	local matched, skipped = {}, {}
	for _, town in ipairs(Game.getTowns()) do
		local name = town:getName()
		local key = name:lower()
		local ambience = TOWN_AMBIENCE[key]
		if ambience then
			ambienceTowns[#ambienceTowns + 1] = {
				name = name,
				position = town:getTemplePosition(),
				radius = CITY_RADIUS_OVERRIDE[key] or CITY_RADIUS,
				day = ambience.day,
				night = ambience.night,
			}
			matched[#matched + 1] = name
		else
			skipped[#skipped + 1] = name
		end
	end

	logger.info("[ambient_sounds] themed towns: {}", #matched > 0 and table.concat(matched, ", ") or "none")
	if #skipped > 0 then
		logger.debug("[ambient_sounds] towns with no ambience mapped: {}", table.concat(skipped, ", "))
	end

	return ambienceTowns
end

local function ambienceForPosition(position, currentTownName)
	for _, town in ipairs(getAmbienceTowns()) do
		local temple = town.position
		local radius = town.name == currentTownName and math.max(town.radius, CITY_KEEP_RADIUS) or town.radius
		if math.abs(position.z - temple.z) <= MAX_FLOOR_DISTANCE
			and math.abs(position.x - temple.x) <= radius
			and math.abs(position.y - temple.y) <= radius
		then
			return town[getEffectivePeriod()], town.name
		end
	end

	return SOUND_EFFECT_TYPE_AMBIENT_SILENCE, nil
end

local cityAmbienceTick = GlobalEvent("CityAmbienceTick")

function cityAmbienceTick.onThink(interval)
	getAmbienceTowns()

	for _, player in ipairs(Game.getPlayers()) do
		local guid = player:getGuid()
		local current = lastSent[guid]
		local wanted, townName = ambienceForPosition(player:getPosition(), current and current.townName)

		if not current or current.ambienceId ~= wanted then
			lastSent[guid] = { ambienceId = wanted, townName = townName }
			player:sendAmbientSoundEffect(wanted)
		end
	end

	return true
end

cityAmbienceTick:interval(CHECK_INTERVAL)
cityAmbienceTick:register()

local cityAmbienceLogout = CreatureEvent("CityAmbienceLogout")

function cityAmbienceLogout.onLogout(player)
	lastSent[player:getGuid()] = nil
	return true
end

cityAmbienceLogout:register()
