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

#include "utils/const.hpp"

// Enums
// Connection and networkmessage.
enum { FORCE_CLOSE = true };
enum { HEADER_LENGTH = 2 };
enum { CHECKSUM_LENGTH = 4 };
enum { XTEA_MULTIPLE = 8 };
enum { MAX_BODY_LENGTH = NETWORKMESSAGE_MAXSIZE - HEADER_LENGTH - CHECKSUM_LENGTH - XTEA_MULTIPLE };
enum { MAX_PROTOCOL_BODY_LENGTH = MAX_BODY_LENGTH - 10 };

enum ConnectionState_t : uint8_t {
	CONNECTION_STATE_OPEN,
	CONNECTION_STATE_IDENTIFYING,
	CONNECTION_STATE_READINGS,
	CONNECTION_STATE_CLOSED
};
// Connection and networkmessage.

// Protocol.
enum RequestedInfo_t : uint16_t {
	REQUEST_BASIC_SERVER_INFO = 1 << 0,
	REQUEST_OWNER_SERVER_INFO = 1 << 1,
	REQUEST_MISC_SERVER_INFO = 1 << 2,
	REQUEST_PLAYERS_INFO = 1 << 3,
	REQUEST_MAP_INFO = 1 << 4,
	REQUEST_EXT_PLAYERS_INFO = 1 << 5,
	REQUEST_PLAYER_STATUS_INFO = 1 << 6,
	REQUEST_SERVER_SOFTWARE_INFO = 1 << 7,
};

enum ChecksumMethods_t : uint8_t {
	CHECKSUM_METHOD_NONE,
	CHECKSUM_METHOD_ADLER32,
	CHECKSUM_METHOD_SEQUENCE
};

// CipSoft's SESSION_END_INFORMATION. 2 specifically means another client took over the
// character - it is not a generic force-close; a forced logout is OTHER (99). 100 is
// the official refuse-a-quit-request arm, which this server expresses as a cancel
// message instead.
enum SessionEndInformations : uint8_t {
	SESSION_END_CONFIRM_QUIT_GAME = 0,
	SESSION_END_OFFLINE_TRAINING = 1,
	SESSION_END_TAKEOVER = 2,
	SESSION_END_CHARACTER_DEAD_TIMEOUT = 3,
	SESSION_END_CHARACTER_TRADE = 4,
	SESSION_END_OTHER = 99,
	SESSION_END_REJECT_QUIT_GAME = 100,
};

enum Resource_t : uint8_t {
	RESOURCE_BANK = 0x00,
	RESOURCE_INVENTORY_MONEY = 0x01,
	RESOURCE_INVENTORY_CURRENCY_CUSTOM = 0x02,
	RESOURCE_PREY_CARDS = 0x0A,
	RESOURCE_TASK_HUNTING = 0x32,
	RESOURCE_FORGE_DUST = 0x46,
	RESOURCE_FORGE_SLIVER = 0x47,
	RESOURCE_FORGE_CORES = 0x48,
	RESOURCE_LUNAR_ASCENSION_ORBS = 0x49,
	RESOURCE_LESSER_GEMS = 0x51,
	RESOURCE_REGULAR_GEMS = 0x52,
	RESOURCE_GREATER_GEMS = 0x53,
	RESOURCE_LESSER_FRAGMENT = 0x54,
	RESOURCE_GREATER_FRAGMENT = 0x55,
	RESOURCE_BOUNTY_POINTS = 0x56,
	RESOURCE_SOULSEALS_POINTS = 0x57,
	RESOURCE_COIN_NORMAL = 0x5A,
	RESOURCE_COIN_TRANSFERRABLE = 0x5B
};

enum CharmResource_t : uint8_t {
	RESOURCE_CHARM = 0x1E,
	RESOURCE_MINOR_CHARM = 0x1F,
	RESOURCE_MAX_CHARM = 0x20,
	RESOURCE_MAX_MINOR_CHARM = 0x21
};

enum InspectObjectTypes : uint8_t {
	INSPECT_NORMALOBJECT = 0,
	INSPECT_NPCTRADE = 1,
	INSPECT_PLAYERTRADE = 2,
	INSPECT_CYCLOPEDIA = 3,
	INSPECT_PROFICIENCY = 4,
};

// Inspection relationship state broadcast to a client over 0x77. Mirrors InspectionFlags in
// the client's modules/gamelib/const.lua, which reads the byte as two halves: bit 2 (value 4)
// means "you may inspect them", and the 0/1/8 part describes your own allowance toward them
// (Invite / Revoke / Allow). Per-pair allowances are not implemented, so only the inspect
// half ever varies here; see Player::getInspectionState.
enum InspectCharacterState : uint8_t {
	INSPECT_STATE_ASK_AND_INVITE = 0,
	INSPECT_STATE_ASK_AND_REVOKE = 1,
	INSPECT_STATE_INSPECT_AND_INVITE = 4,
	INSPECT_STATE_INSPECT_AND_REVOKE = 5,
	INSPECT_STATE_ASK_AND_ALLOW = 8,
	INSPECT_STATE_INSPECT_AND_ALLOW = 12,
};

// Actions of the inspect-character packet (0xCE). Mirrors InspectionParseFlags in the
// client's modules/gamelib/const.lua; keep the two in step.
enum InspectCharacterAction : uint8_t {
	INSPECT_CHARACTER_INVITE = 1,
	INSPECT_CHARACTER_ASK = 2,
	INSPECT_CHARACTER_ALLOW = 3,
	INSPECT_CHARACTER_INSPECT = 4,
	INSPECT_CHARACTER_REVOKE = 5,
	INSPECT_CHARACTER_ALLOW_ALL = 6,
	INSPECT_CHARACTER_DISMISS_ALL = 7,
};

enum CyclopediaCharacterInfo_OutfitType_t : uint8_t {
	CYCLOPEDIA_CHARACTERINFO_OUTFITTYPE_NONE = 0,
	CYCLOPEDIA_CHARACTERINFO_OUTFITTYPE_QUEST = 1,
	CYCLOPEDIA_CHARACTERINFO_OUTFITTYPE_STORE = 2
};

enum MagicEffectsType_t : uint8_t {
	// ends magic effect loop
	MAGIC_EFFECTS_END_LOOP = 0,
	// needs uint8_t delta after type to adjust position
	MAGIC_EFFECTS_DELTA = 1,
	// needs uint16_t delay after type to delay in miliseconds effect display
	MAGIC_EFFECTS_DELAY = 2,
	// needs uint16_t effectid and uint8_t actor after type
	MAGIC_EFFECTS_CREATE_EFFECT = 3,
	// needs uint16_t effectid, deltaX(int8_t), deltaY(int8_t) and uint8_t actor after type
	MAGIC_EFFECTS_CREATE_DISTANCEEFFECT = 4,
	// needs uint16_t effectid, deltaX(int8_t), deltaY(int8_t) and uint8_t actor after type
	MAGIC_EFFECTS_CREATE_DISTANCEEFFECT_REVERSED = 5,
	// needs uint8_t soundSourceType and uint16_t soundId after type
	MAGIC_EFFECTS_CREATE_SOUND_MAIN_EFFECT = 6,
	// needs uint8_t soundSourceType, uint16_t soundId after type
	MAGIC_EFFECTS_CREATE_SOUND_SECONDARY_EFFECT = 7,
};

// MAGIC_EFFECTS_DELTA is a running total the client decodes as (total % 18, total / 18) tiles
// from the position the packet opened with, so a batch can only address the 18-wide band at or
// south-east of its anchor.
static constexpr uint32_t MAGIC_EFFECTS_DELTA_ROW_WIDTH = 18;

enum ImpactAnalyzerAndTracker_t : uint8_t {
	ANALYZER_HEAL = 0,
	ANALYZER_DAMAGE_DEALT = 1,
	ANALYZER_DAMAGE_RECEIVED = 2
};

// CipSoft's STASH_ACTION. Values are theirs; the names are ours - STOW,
// STOWCONTAINERCONTENT, STOWALLOFTYPE, RETRIEVE in their spelling.
enum Stash_Actions_t : uint8_t {
	STASH_ACTION_STOW_ITEM = 0,
	STASH_ACTION_STOW_CONTAINER = 1,
	STASH_ACTION_STOW_STACK = 2,
	STASH_ACTION_WITHDRAW = 3
};

// CipSoft's STASH_RETRIEVE_SOURCE, the byte that follows STASH_ACTION_WITHDRAW. The client
// sends it; playerStashWithdraw currently discards it and always retrieves from the stash.
// Named here so the field is not mistaken for a stack position, which is what it was called.
enum StashRetrieveSource_t : uint8_t {
	STASH_RETRIEVE_SOURCE_STASH = 0,
	STASH_RETRIEVE_SOURCE_DEPOT_SEARCH = 1
};

// CipSoft's MESSAGE: the kind field of GameserverMessageShowMessageDialog, which says which
// canned dialog the text belongs to. The numbering is by decade, one decade per subsystem,
// and the gaps between them are real. There is deliberately no generic arm - see the note on
// sendMessageDialog, which has 47 callers and no value that fits them.
enum MessageDialog_t : uint8_t {
	MESSAGEDIALOG_IMBUEMENT_SUCCESS = 0,
	MESSAGEDIALOG_IMBUEMENT_ERROR = 1,
	MESSAGEDIALOG_IMBUEMENT_CHANCE_FAILED = 2,
	MESSAGEDIALOG_IMBUEMENT_STATION_GONE = 3,
	MESSAGEDIALOG_CLEARING_SUCCESS = 10,
	MESSAGEDIALOG_CLEARING_ERROR = 11,
	MESSAGEDIALOG_PREY_MESSAGE = 20,
	MESSAGEDIALOG_PREY_ERROR = 21,
	MESSAGEDIALOG_RETRIEVE_ERROR = 30,
	MESSAGEDIALOG_DAILY_REWARDS_DISABLED = 40,
	MESSAGEDIALOG_EXALTATION_ERROR = 50,
	MESSAGEDIALOG_BOSSTIARY_PODIUMRECEIVED = 60,
	MESSAGEDIALOG_CAPACITY_WARNING = 70,
	MESSAGEDIALOG_SHOW_SPELLBOOK = 80,
	MESSAGEDIALOG_NAGGING_MESSAGE = 90
};

// CipSoft's DEPOT_SEARCH_RETRIEVE_SOURCE, the byte that ends a depot-search retrieve
// request. Note it starts at 1, not 0: there is no "none" arm, and a zero-based reading
// would send every depot retrieval to the inbox.
enum DepotSearchRetrieveSource_t : uint8_t {
	DEPOT_SEARCH_RETRIEVE_SOURCE_DEPOT = 1,
	DEPOT_SEARCH_RETRIEVE_SOURCE_INBOX = 2
};

// CipSoft's GETOUTFIT - who the outfit dialog is requested for. Note the asymmetry with
// SetOutfitType_t: value 1 means try-on here and hireling there.
enum GetOutfitType_t : uint8_t {
	GETOUTFIT_SELECT_FOR_PLAYER = 0,
	GETOUTFIT_TRY_PLAYER_OUTFIT_MOUNT = 1,
	GETOUTFIT_SELECT_FOR_HIRELING = 2,
	GETOUTFIT_TRY_HIRELING_OUTFIT = 3
};

// CipSoft's SETOUTFIT - whose outfit an outfit-window confirm applies to. The hireling
// arm carries a uint32 hireling id after the outfit.
enum SetOutfitType_t : uint8_t {
	SETOUTFIT_PLAYER = 0,
	SETOUTFIT_HIRELING = 1,
	SETOUTFIT_SHOW_OFF_SOCKET = 2
};

// CipSoft's STEP_DIRECTION, the wire encoding of one auto-walk step in
// GameclientMessageGoPath. A different enum from Direction, which is the four-value
// facing: here east is 1, the values run counter-clockwise, and there is no zero arm.
// It also disagrees with Direction's own diagonal numbering (SOUTHWEST is 4 there and
// 6 here), so neither enum can stand in for the other.
enum StepDirection_t : uint8_t {
	STEP_DIRECTION_EAST = 1,
	STEP_DIRECTION_NORTHEAST = 2,
	STEP_DIRECTION_NORTH = 3,
	STEP_DIRECTION_NORTHWEST = 4,
	STEP_DIRECTION_WEST = 5,
	STEP_DIRECTION_SOUTHWEST = 6,
	STEP_DIRECTION_SOUTH = 7,
	STEP_DIRECTION_SOUTHEAST = 8
};

// CipSoft's QUICK_LOOT_MODE - which corpses a quick-loot request covers.
enum QuickLootMode_t : uint8_t {
	QUICK_LOOT_MODE_SINGLE_CORPSE = 0,
	QUICK_LOOT_MODE_AREA_AT_CORPSE = 1,
	QUICK_LOOT_MODE_AREA_AT_PLAYER = 2
};

// CipSoft's MANAGED_CONTAINER_ACTION. One enum covers both halves of the system: the loot
// containers at 0..3 and the obtain containers at 4..7.
enum ManagedContainerAction_t : uint8_t {
	LOOT_CONTAINER_SELECT = 0,
	LOOT_CONTAINER_CLEAR = 1,
	LOOT_CONTAINER_OPEN = 2,
	LOOT_CONTAINER_FALLBACK = 3,
	OBTAIN_CONTAINER_SELECT = 4,
	OBTAIN_CONTAINER_CLEAR = 5,
	OBTAIN_CONTAINER_OPEN = 6,
	OBTAIN_CONTAINER_FALLBACK = 7
};

struct HighscoreCharacter {
	HighscoreCharacter(std::string name, uint64_t points, uint32_t id, uint32_t rank, uint16_t level, uint8_t vocation, std::string worldName, std::string loyaltyTitle) :
		name(std::move(name)),
		points(points),
		id(id),
		rank(rank),
		level(level),
		vocation(vocation),
		worldName(std::move(worldName)),
		loyaltyTitle(std::move(loyaltyTitle)) { }

	std::string name;
	uint64_t points;
	uint32_t id;
	uint32_t rank;
	uint16_t level;
	uint8_t vocation;
	std::string worldName;
	std::string loyaltyTitle;
};

enum Imbuement_Window_t : uint8_t {
	IMBUEMENT_WINDOW_CHOICE = 0,
	IMBUEMENT_WINDOW_SELECT_ITEM = 1,
	IMBUEMENT_WINDOW_SCROLL = 2
};
// The weapon proficiency command set now lives in src/protobuf/proficiency.proto as
// WEAPON_PROFICIENCY_COMMAND, transcribed from the official client along with the messages
// it travels in - the values here were the same ten, duplicated.

