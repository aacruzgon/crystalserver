#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 2 (containers). Same contract as proficiency_protocol_test.cpp: the two
// repos each carry a verbatim copy of protocol.proto and nothing links them, so each side
// pins its own copy - CrystalOTC mirrors these assertions in
// tests/protocol/containers_schema_test.cpp. Constants only: boost::ut runs after main,
// past protobuf's static teardown, where constructing a message segfaults; the wire-level
// decode tests live on the client side for that reason.
suite<"networkmessage"> containersProtocolTest = [] {
	test("containers envelope numbers match the official schema") = [] {
		expect(eq(41, proto::GameserverMessageExtensions::kStashFieldNumber));
		expect(eq(42, proto::GameserverMessageExtensions::kSpecialContainersAvailableFieldNumber));
		expect(eq(110, proto::GameserverMessageExtensions::kContainerFieldNumber));
		expect(eq(111, proto::GameserverMessageExtensions::kCloseContainerFieldNumber));
		expect(eq(112, proto::GameserverMessageExtensions::kCreateInContainerFieldNumber));
		expect(eq(113, proto::GameserverMessageExtensions::kChangeInContainerFieldNumber));
		expect(eq(114, proto::GameserverMessageExtensions::kDeleteInContainerFieldNumber));
		expect(eq(120, proto::GameserverMessageExtensions::kSetInventoryFieldNumber));
		expect(eq(121, proto::GameserverMessageExtensions::kDeleteInventoryFieldNumber));
		expect(eq(192, proto::GameserverMessageExtensions::kUpdateManagedContainersFieldNumber));
		expect(eq(245, proto::GameserverMessageExtensions::kPlayerInventoryFieldNumber));

		expect(eq(41, proto::GAMESERVER_MESSAGE_TYPE_STASH));
		expect(eq(42, proto::GAMESERVER_MESSAGE_TYPE_SPECIALCONTAINERSAVAILABLE));
		expect(eq(110, proto::GAMESERVER_MESSAGE_TYPE_CONTAINER));
		expect(eq(111, proto::GAMESERVER_MESSAGE_TYPE_CLOSECONTAINER));
		expect(eq(112, proto::GAMESERVER_MESSAGE_TYPE_CREATEINCONTAINER));
		expect(eq(113, proto::GAMESERVER_MESSAGE_TYPE_CHANGEINCONTAINER));
		expect(eq(114, proto::GAMESERVER_MESSAGE_TYPE_DELETEINCONTAINER));
		expect(eq(120, proto::GAMESERVER_MESSAGE_TYPE_SETINVENTORY));
		expect(eq(121, proto::GAMESERVER_MESSAGE_TYPE_DELETEINVENTORY));
		expect(eq(192, proto::GAMESERVER_MESSAGE_TYPE_UPDATE_MANAGED_CONTAINERS));
		expect(eq(245, proto::GAMESERVER_MESSAGE_TYPE_PLAYERINVENTORY));

		expect(eq(40, proto::GameclientMessageExtensions::kStashActionFieldNumber));
		expect(eq(117, proto::GameclientMessageExtensions::kContainerActionFieldNumber));
		expect(eq(119, proto::GameclientMessageExtensions::kEquipObjectFieldNumber));
		expect(eq(120, proto::GameclientMessageExtensions::kMoveObjectFieldNumber));
		expect(eq(130, proto::GameclientMessageExtensions::kUseObjectFieldNumber));
		expect(eq(131, proto::GameclientMessageExtensions::kUseTwoObjectsFieldNumber));
		expect(eq(132, proto::GameclientMessageExtensions::kUseOnCreatureFieldNumber));
		expect(eq(133, proto::GameclientMessageExtensions::kTurnObjectFieldNumber));
		expect(eq(135, proto::GameclientMessageExtensions::kCloseContainerFieldNumber));
		expect(eq(136, proto::GameclientMessageExtensions::kUpContainerFieldNumber));
		expect(eq(139, proto::GameclientMessageExtensions::kToggleWrapStateFieldNumber));
		expect(eq(140, proto::GameclientMessageExtensions::kLookFieldNumber));
		expect(eq(143, proto::GameclientMessageExtensions::kQuickLootFieldNumber));
		expect(eq(144, proto::GameclientMessageExtensions::kManagedContainerFieldNumber));
		expect(eq(145, proto::GameclientMessageExtensions::kQuickLootBlackWhiteListFieldNumber));
		expect(eq(203, proto::GameclientMessageExtensions::kBrowseFieldFieldNumber));
		expect(eq(204, proto::GameclientMessageExtensions::kSeekInContainerFieldNumber));

		expect(eq(40, proto::GAMECLIENT_MESSAGE_TYPE_STASHACTION));
		expect(eq(119, proto::GAMECLIENT_MESSAGE_TYPE_EQUIPOBJECT));
		expect(eq(120, proto::GAMECLIENT_MESSAGE_TYPE_MOVEOBJECT));
		expect(eq(130, proto::GAMECLIENT_MESSAGE_TYPE_USEOBJECT));
		expect(eq(131, proto::GAMECLIENT_MESSAGE_TYPE_USETWOOBJECTS));
		expect(eq(132, proto::GAMECLIENT_MESSAGE_TYPE_USEONCREATURE));
		expect(eq(133, proto::GAMECLIENT_MESSAGE_TYPE_TURNOBJECT));
		expect(eq(135, proto::GAMECLIENT_MESSAGE_TYPE_CLOSECONTAINER));
		expect(eq(136, proto::GAMECLIENT_MESSAGE_TYPE_UPCONTAINER));
		expect(eq(139, proto::GAMECLIENT_MESSAGE_TYPE_TOGGLEWRAPSTATE));
		expect(eq(140, proto::GAMECLIENT_MESSAGE_TYPE_LOOK));
		expect(eq(143, proto::GAMECLIENT_MESSAGE_TYPE_QUICKLOOT));
		expect(eq(144, proto::GAMECLIENT_MESSAGE_TYPE_MANAGED_CONTAINER));
		expect(eq(145, proto::GAMECLIENT_MESSAGE_TYPE_QUICKLOOTBLACKWHITELIST));
		expect(eq(203, proto::GAMECLIENT_MESSAGE_TYPE_BROWSEFIELD));
		expect(eq(204, proto::GAMECLIENT_MESSAGE_TYPE_SEEKINCONTAINER));
	};

	test("appearance instance extension numbers match the official schema") = [] {
		expect(eq(1, proto::AppearanceInstance::kAppearanceIdFieldNumber));
		expect(eq(101, proto::AppearanceInstanceExtensions::kCumulativeFieldNumber));
		expect(eq(102, proto::AppearanceInstanceExtensions::kLiquidFieldNumber));
		expect(eq(106, proto::AppearanceInstanceExtensions::kContainerFieldNumber));
		expect(eq(107, proto::AppearanceInstanceExtensions::kShowOffSocketFieldNumber));
		expect(eq(108, proto::AppearanceInstanceExtensions::kUpgradeableFieldNumber));
		expect(eq(109, proto::AppearanceInstanceExtensions::kWearoutFieldNumber));
		expect(eq(110, proto::AppearanceInstanceExtensions::kExpireFieldNumber));
		expect(eq(112, proto::AppearanceInstanceExtensions::kDecoItemKitFieldNumber));

		// CumulativeObject's amount is field 2, not 1 - field 1 is one of the deleted-field
		// gaps the spec preserves. Same for LiquidObject.
		expect(eq(2, proto::CumulativeObject::kAmountFieldNumber));
		expect(eq(2, proto::LiquidObject::kFluidTypeFieldNumber));
		expect(eq(1, proto::UpgradeableObject::kTierFieldNumber));
		expect(eq(1, proto::WearoutObject::kChargesFieldNumber));
		expect(eq(1, proto::ExpireObject::kSecondsLeftFieldNumber));
	};

	test("object position arms match the official schema") = [] {
		expect(eq(1, proto::ObjectPosition::kWorldmapFieldNumber));
		expect(eq(2, proto::ObjectPosition::kInventoryFieldNumber));
		expect(eq(3, proto::ObjectPosition::kContainerFieldNumber));
		expect(eq(4, proto::ObjectPosition::kDepotSearchDepotFieldNumber));
		expect(eq(5, proto::ObjectPosition::kDepotSearchInboxFieldNumber));
		expect(eq(6, proto::ObjectPosition::kField6FieldNumber));

		expect(eq(1, proto::ObjectIdentifier::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::ObjectIdentifier::kTierOrSubtypeFieldNumber));

		expect(eq(1, proto::GameserverMessageContainer::kContainerIdFieldNumber));
		expect(eq(2, proto::GameserverMessageContainer::kContainerObjectFieldNumber));
		expect(eq(3, proto::GameserverMessageContainer::kNameFieldNumber));
		expect(eq(4, proto::GameserverMessageContainer::kCapacityFieldNumber));
		expect(eq(11, proto::GameserverMessageContainer::kItemsFieldNumber));
		expect(eq(13, proto::GameserverMessageContainer::kFilterOptionsFieldNumber));
		expect(eq(14, proto::GameserverMessageContainer::kIsMovableFieldNumber));
		expect(eq(15, proto::GameserverMessageContainer::kIsHoldingFieldNumber));
	};

	test("containers enums match the official values") = [] {
		expect(eq(0, proto::STASH_ACTION_STOW));
		expect(eq(1, proto::STASH_ACTION_STOWCONTAINERCONTENT));
		expect(eq(2, proto::STASH_ACTION_STOWALLOFTYPE));
		expect(eq(3, proto::STASH_ACTION_RETRIEVE));

		expect(eq(0, proto::QUICK_LOOT_MODE_SINGLE_CORPSE));
		expect(eq(1, proto::QUICK_LOOT_MODE_AREA_AT_CORPSE));
		expect(eq(2, proto::QUICK_LOOT_MODE_AREA_AT_PLAYER));

		expect(eq(0, proto::LOOT_LIST_TYPE_BLACKLIST));
		expect(eq(1, proto::LOOT_LIST_TYPE_WHITELIST));

		expect(eq(0, proto::LOOT_CONTAINER_SELECT));
		expect(eq(3, proto::LOOT_CONTAINER_FALLBACK));
		expect(eq(4, proto::OBTAIN_CONTAINER_SELECT));
		expect(eq(7, proto::OBTAIN_CONTAINER_FALLBACK));
	};
};
