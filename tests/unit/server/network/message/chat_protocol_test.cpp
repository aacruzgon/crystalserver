#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 3 (chat). Constants only: this suite runs after protobuf's static
// teardown, so the client-side mirror owns the construction and wire-level tests.
suite<"networkmessage"> chatProtocolTest = [] {
	test("chat envelope numbers match the official schema") = [] {
		expect(eq(28, proto::GameserverMessageExtensions::kNpcTalkPartnersFieldNumber));
		expect(eq(170, proto::GameserverMessageExtensions::kTalkFieldNumber));
		expect(eq(171, proto::GameserverMessageExtensions::kChannelsFieldNumber));
		expect(eq(172, proto::GameserverMessageExtensions::kOpenChannelFieldNumber));
		expect(eq(173, proto::GameserverMessageExtensions::kPrivateChannelFieldNumber));
		expect(eq(174, proto::GameserverMessageExtensions::kEditGuildMessageFieldNumber));
		expect(eq(178, proto::GameserverMessageExtensions::kOpenOwnChannelFieldNumber));
		expect(eq(179, proto::GameserverMessageExtensions::kCloseChannelFieldNumber));
		expect(eq(180, proto::GameserverMessageExtensions::kMessageFieldNumber));
		expect(eq(243, proto::GameserverMessageExtensions::kChannelEventFieldNumber));

		expect(eq(28, proto::GAMESERVER_MESSAGE_TYPE_NPCTALKPARTERS));
		expect(eq(170, proto::GAMESERVER_MESSAGE_TYPE_TALK));
		expect(eq(171, proto::GAMESERVER_MESSAGE_TYPE_CHANNELS));
		expect(eq(172, proto::GAMESERVER_MESSAGE_TYPE_OPENCHANNEL));
		expect(eq(173, proto::GAMESERVER_MESSAGE_TYPE_PRIVATECHANNEL));
		expect(eq(174, proto::GAMESERVER_MESSAGE_TYPE_EDITGUILDMESSAGE));
		expect(eq(178, proto::GAMESERVER_MESSAGE_TYPE_OPENOWNCHANNEL));
		expect(eq(179, proto::GAMESERVER_MESSAGE_TYPE_CLOSECHANNEL));
		expect(eq(180, proto::GAMESERVER_MESSAGE_TYPE_MESSAGE));
		expect(eq(243, proto::GAMESERVER_MESSAGE_TYPE_CHANNELEVENT));

		expect(eq(150, proto::GameclientMessageExtensions::kTalkFieldNumber));
		expect(eq(151, proto::GameclientMessageExtensions::kGetChannelsFieldNumber));
		expect(eq(152, proto::GameclientMessageExtensions::kJoinChannelFieldNumber));
		expect(eq(153, proto::GameclientMessageExtensions::kLeaveChannelFieldNumber));
		expect(eq(154, proto::GameclientMessageExtensions::kPrivateChannelFieldNumber));
		expect(eq(155, proto::GameclientMessageExtensions::kGuildMessageFieldNumber));
		expect(eq(156, proto::GameclientMessageExtensions::kEditGuildMessageFieldNumber));
		expect(eq(158, proto::GameclientMessageExtensions::kCloseNpcChannelFieldNumber));
		expect(eq(170, proto::GameclientMessageExtensions::kOpenOwnChannelFieldNumber));
		expect(eq(171, proto::GameclientMessageExtensions::kInviteToChannelFieldNumber));
		expect(eq(172, proto::GameclientMessageExtensions::kExcludeFromChannelFieldNumber));
		expect(eq(238, proto::GameclientMessageExtensions::kGreetFieldNumber));

		expect(eq(150, proto::GAMECLIENT_MESSAGE_TYPE_TALK));
		expect(eq(151, proto::GAMECLIENT_MESSAGE_TYPE_GETCHANNELS));
		expect(eq(152, proto::GAMECLIENT_MESSAGE_TYPE_JOINCHANNEL));
		expect(eq(153, proto::GAMECLIENT_MESSAGE_TYPE_LEAVECHANNEL));
		expect(eq(154, proto::GAMECLIENT_MESSAGE_TYPE_PRIVATECHANNEL));
		expect(eq(155, proto::GAMECLIENT_MESSAGE_TYPE_GUILDMESSAGE));
		expect(eq(156, proto::GAMECLIENT_MESSAGE_TYPE_EDITGUILDMESSAGE));
		expect(eq(158, proto::GAMECLIENT_MESSAGE_TYPE_CLOSENPCCHANNEL));
		expect(eq(170, proto::GAMECLIENT_MESSAGE_TYPE_OPENCHANNEL));
		expect(eq(171, proto::GAMECLIENT_MESSAGE_TYPE_INVITETOCHANNEL));
		expect(eq(172, proto::GAMECLIENT_MESSAGE_TYPE_EXCLUDEFROMCHANNEL));
		expect(eq(238, proto::GAMECLIENT_MESSAGE_TYPE_GREET));
	};

	test("chat message fields match the official schema") = [] {
		expect(eq(1, proto::Channel::kChannelIdFieldNumber));
		expect(eq(2, proto::Channel::kNameFieldNumber));
		expect(eq(1, proto::ChatChannelParticipants::kParticipantsFieldNumber));
		expect(eq(2, proto::ChatChannelParticipants::kInviteesFieldNumber));
		expect(eq(1, proto::KeywordButton::kIconFieldNumber));
		expect(eq(2, proto::KeywordButton::kTextFieldNumber));

		expect(eq(1, proto::GameclientMessageTalk::kModeFieldNumber));
		expect(eq(2, proto::GameclientMessageTalk::kChannelIdFieldNumber));
		expect(eq(3, proto::GameclientMessageTalk::kReceiverFieldNumber));
		expect(eq(4, proto::GameclientMessageTalk::kTextFieldNumber));
		expect(eq(5, proto::GameclientMessageTalk::kField5FieldNumber));
		expect(eq(6, proto::GameclientMessageTalk::kPositionFieldNumber));

		expect(eq(1, proto::GameserverMessageTalk::kStatementIdFieldNumber));
		expect(eq(8, proto::GameserverMessageTalk::kField8FieldNumber));
		expect(eq(1, proto::GameserverMessageMessage::kModeFieldNumber));
		expect(eq(7, proto::GameserverMessageMessage::kField7FieldNumber));
		expect(eq(8, proto::GameserverMessageMessage::kTextFieldNumber));
	};

	test("channel event values match the official enum") = [] {
		expect(eq(0, proto::CHANNEL_EVENT_JOIN));
		expect(eq(1, proto::CHANNEL_EVENT_LEAVE));
		expect(eq(2, proto::CHANNEL_EVENT_INVITE));
		expect(eq(3, proto::CHANNEL_EVENT_EXCLUDE));
		expect(eq(4, proto::CHANNEL_EVENT_PENDING));
	};
};
