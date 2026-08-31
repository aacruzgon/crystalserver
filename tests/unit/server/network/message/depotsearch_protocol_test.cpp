#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

#include "server/server_definitions.hpp"

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 4 (depotsearch). Constants only: this suite runs after protobuf's static
// teardown, and the client of this stack implements no depot search at all (phase 1
// slice 4 finding), so the server suite is the only place these numbers are pinned.
suite<"networkmessage"> depotSearchProtocolTest = [] {
	test("depot search envelope numbers match the official schema") = [] {
		expect(eq(148, proto::GameserverMessageExtensions::kDepotSearchResultFieldNumber));
		expect(eq(153, proto::GameserverMessageExtensions::kDepotSearchDetailListFieldNumber));
		expect(eq(154, proto::GameserverMessageExtensions::kCloseDepotSearchFieldNumber));

		expect(eq(148, proto::GAMESERVER_MESSAGE_TYPE_DEPOTSEARCHRESULT));
		expect(eq(153, proto::GAMESERVER_MESSAGE_TYPE_DEPOTSEARCHDETAILLIST));
		expect(eq(154, proto::GAMESERVER_MESSAGE_TYPE_CLOSEDEPOTSEARCH));

		expect(eq(41, proto::GameclientMessageExtensions::kDepotSearchRetrieveFieldNumber));
		expect(eq(146, proto::GameclientMessageExtensions::kOpenDepotSearchFieldNumber));
		expect(eq(147, proto::GameclientMessageExtensions::kCloseDepotSearchFieldNumber));
		expect(eq(148, proto::GameclientMessageExtensions::kDepotSearchTypeFieldNumber));
		expect(eq(149, proto::GameclientMessageExtensions::kOpenParentContainerFieldNumber));

		expect(eq(41, proto::GAMECLIENT_MESSAGE_TYPE_DEPOTSEARCHRETRIEVE));
		expect(eq(146, proto::GAMECLIENT_MESSAGE_TYPE_OPENDEPOTSEARCH));
		expect(eq(147, proto::GAMECLIENT_MESSAGE_TYPE_CLOSEDEPOTSEARCH));
		expect(eq(148, proto::GAMECLIENT_MESSAGE_TYPE_DEPOTSEARCHTYPET));
		expect(eq(149, proto::GAMECLIENT_MESSAGE_TYPE_OPENPARENTCONTAINER));
	};

	test("depot search message fields match the official schema") = [] {
		expect(eq(1, proto::AppearanceTypeUpgradeable::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::AppearanceTypeUpgradeable::kTierFieldNumber));
		expect(eq(1, proto::UpgradeableAppearanceTypeAndCount::kIdentifierFieldNumber));
		expect(eq(2, proto::UpgradeableAppearanceTypeAndCount::kCountFieldNumber));
		expect(eq(1, proto::AppearanceTypeAndCount::kObjectTypeIdFieldNumber));
		expect(eq(2, proto::AppearanceTypeAndCount::kCountFieldNumber));

		expect(eq(1, proto::GameclientMessageDepotSearchRetrieve::kIdentifierFieldNumber));
		expect(eq(2, proto::GameclientMessageDepotSearchRetrieve::kSourceFieldNumber));
		expect(eq(1, proto::GameclientMessageDepotSearchType::kIdentifierFieldNumber));
		expect(eq(1, proto::GameclientMessageOpenParentContainer::kPositionFieldNumber));

		expect(eq(1, proto::GameserverMessageDepotSearchResult::kItemsFieldNumber));
		expect(eq(1, proto::GameserverMessageDepotSearchDetailList::kIdentifierFieldNumber));
		expect(eq(2, proto::GameserverMessageDepotSearchDetailList::kDepotCountFieldNumber));
		expect(eq(3, proto::GameserverMessageDepotSearchDetailList::kDepotItemsFieldNumber));
		expect(eq(4, proto::GameserverMessageDepotSearchDetailList::kInboxCountFieldNumber));
		expect(eq(5, proto::GameserverMessageDepotSearchDetailList::kInboxItemsFieldNumber));
		expect(eq(6, proto::GameserverMessageDepotSearchDetailList::kStashItemsFieldNumber));

		expect(eq(1, proto::DepotSearchDepotObjectPosition::kIndexFieldNumber));
		expect(eq(1, proto::DepotSearchInboxObjectPosition::kIndexFieldNumber));
	};

	test("retrieve source enum matches the official schema on both layers") = [] {
		// DEPOT is 1 and INBOX is 2, with no zero arm - the numbering phase 1 slice 4
		// named precisely because a zero-based reading retrieves everything to the inbox.
		expect(eq(1, proto::DEPOT_SEARCH_RETRIEVE_SOURCE_DEPOT));
		expect(eq(2, proto::DEPOT_SEARCH_RETRIEVE_SOURCE_INBOX));
		expect(eq(1, static_cast<int>(::DEPOT_SEARCH_RETRIEVE_SOURCE_DEPOT)));
		expect(eq(2, static_cast<int>(::DEPOT_SEARCH_RETRIEVE_SOURCE_INBOX)));
	};
};
