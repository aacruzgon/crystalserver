#include "pch.hpp"

#include <boost/ut.hpp>

#include <protocol.pb.h>

#include "server/server_definitions.hpp"

using namespace boost::ut;
namespace proto = tibia::protobuf::protocol;

// Phase 2 slice 5 (dialogs). Constants only - this suite runs after protobuf's static
// teardown. CrystalOTC mirrors these in tests/protocol/dialogs_schema_test.cpp because the
// two repositories compile separate verbatim copies of the official protocol.proto.
suite<"networkmessage"> dialogsProtocolTest = [] {
	test("dialog envelope numbers match the official schema") = [] {
		expect(eq(118, proto::GameserverMessageExtensions::kInspectionListFieldNumber));
		expect(eq(150, proto::GameserverMessageExtensions::kEditTextFieldNumber));
		expect(eq(151, proto::GameserverMessageExtensions::kEditListFieldNumber));
		expect(eq(237, proto::GameserverMessageExtensions::kShowMessageDialogFieldNumber));
		expect(eq(244, proto::GameserverMessageExtensions::kObjectInfoFieldNumber));
		expect(eq(250, proto::GameserverMessageExtensions::kShowModalDialogFieldNumber));

		expect(eq(118, proto::GAMESERVER_MESSAGE_TYPE_INSPECTIONLIST));
		expect(eq(150, proto::GAMESERVER_MESSAGE_TYPE_EDITTEXT));
		expect(eq(151, proto::GAMESERVER_MESSAGE_TYPE_EDITLIST));
		// The type enum's name for ShowMessageDialog (237).
		expect(eq(237, proto::GAMESERVER_MESSAGE_TYPE_SHOWRESULTDIALOG));
		expect(eq(244, proto::GAMESERVER_MESSAGE_TYPE_OBJECTINFO));
		expect(eq(250, proto::GAMESERVER_MESSAGE_TYPE_SHOWMODALDIALOG));

		expect(eq(137, proto::GameclientMessageExtensions::kEditTextFieldNumber));
		expect(eq(138, proto::GameclientMessageExtensions::kEditListFieldNumber));
		expect(eq(205, proto::GameclientMessageExtensions::kInspectObjectFieldNumber));
		expect(eq(206, proto::GameclientMessageExtensions::kInspectPlayerFieldNumber));
		expect(eq(243, proto::GameclientMessageExtensions::kGetObjectInfoFieldNumber));
		expect(eq(249, proto::GameclientMessageExtensions::kAnswerModalDialogFieldNumber));

		expect(eq(137, proto::GAMECLIENT_MESSAGE_TYPE_EDITTEXT));
		expect(eq(138, proto::GAMECLIENT_MESSAGE_TYPE_EDITLIST));
		expect(eq(205, proto::GAMECLIENT_MESSAGE_TYPE_INSPECTOBJECT));
		expect(eq(206, proto::GAMECLIENT_MESSAGE_TYPE_INSPECTPLAYER));
		expect(eq(243, proto::GAMECLIENT_MESSAGE_TYPE_GETOBJECTINFO));
		expect(eq(249, proto::GAMECLIENT_MESSAGE_TYPE_ANSWERMODALDIALOG));
	};

	test("dialog message fields match the official schema") = [] {
		expect(eq(1, proto::LabelValuePair::kLabelFieldNumber));
		expect(eq(2, proto::LabelValuePair::kValueFieldNumber));
		expect(eq(1, proto::KeyValuePair::kField1FieldNumber));
		expect(eq(2, proto::KeyValuePair::kField2FieldNumber));

		expect(eq(1, proto::GameserverMessageShowModalDialog::kDialogIdFieldNumber));
		expect(eq(2, proto::GameserverMessageShowModalDialog::kTitleFieldNumber));
		expect(eq(3, proto::GameserverMessageShowModalDialog::kMessageFieldNumber));
		expect(eq(4, proto::GameserverMessageShowModalDialog::kButtonsFieldNumber));
		expect(eq(5, proto::GameserverMessageShowModalDialog::kChoicesFieldNumber));
		expect(eq(6, proto::GameserverMessageShowModalDialog::kDefaultEnterButtonFieldNumber));
		expect(eq(7, proto::GameserverMessageShowModalDialog::kDefaultEscapeButtonFieldNumber));
		expect(eq(8, proto::GameserverMessageShowModalDialog::kPriorityFieldNumber));

		expect(eq(1, proto::GameclientMessageAnswerModalDialog::kDialogIdFieldNumber));
		expect(eq(2, proto::GameclientMessageAnswerModalDialog::kButtonFieldNumber));
		expect(eq(3, proto::GameclientMessageAnswerModalDialog::kChoiceFieldNumber));

		expect(eq(1, proto::GameserverMessageShowMessageDialog::kKindFieldNumber));
		expect(eq(2, proto::GameserverMessageShowMessageDialog::kTextFieldNumber));

		expect(eq(1, proto::GameserverMessageEditText::kWindowIdFieldNumber));
		expect(eq(2, proto::GameserverMessageEditText::kObjectFieldNumber));
		expect(eq(3, proto::GameserverMessageEditText::kMaxLengthFieldNumber));
		expect(eq(4, proto::GameserverMessageEditText::kTextFieldNumber));
		expect(eq(5, proto::GameserverMessageEditText::kWriterFieldNumber));
		expect(eq(6, proto::GameserverMessageEditText::kDateFieldNumber));
		expect(eq(1, proto::GameclientMessageEditText::kWindowIdFieldNumber));
		expect(eq(2, proto::GameclientMessageEditText::kTextFieldNumber));

		expect(eq(1, proto::GameserverMessageEditList::kListTypeFieldNumber));
		expect(eq(2, proto::GameserverMessageEditList::kWindowIdFieldNumber));
		expect(eq(3, proto::GameserverMessageEditList::kTextFieldNumber));
		expect(eq(1, proto::GameclientMessageEditList::kListTypeFieldNumber));
		expect(eq(2, proto::GameclientMessageEditList::kWindowIdFieldNumber));
		expect(eq(3, proto::GameclientMessageEditList::kTextFieldNumber));

		expect(eq(1, proto::GameserverMessageInspectionList::kSubjectFieldNumber));
		expect(eq(2, proto::GameserverMessageInspectionList::kWindowFieldNumber));
		expect(eq(3, proto::GameserverMessageInspectionList::kField3FieldNumber));
		expect(eq(4, proto::GameserverMessageInspectionList::kEntriesFieldNumber));
		expect(eq(1, proto::InspectObjectInformation::kObjectFieldNumber));
		expect(eq(2, proto::InspectObjectInformation::kNameFieldNumber));
		expect(eq(3, proto::InspectObjectInformation::kField3FieldNumber));
		expect(eq(4, proto::InspectObjectInformation::kDetailsFieldNumber));

		expect(eq(1, proto::GameclientMessageInspectObject::kLocationFieldNumber));
		expect(eq(2, proto::GameclientMessageInspectObject::kPositionFieldNumber));
		expect(eq(3, proto::GameclientMessageInspectObject::kIdentifierFieldNumber));
		expect(eq(4, proto::GameclientMessageInspectObject::kSubjectFieldNumber));
		expect(eq(1, proto::GameclientMessageInspectPlayer::kCreatureIdFieldNumber));
		expect(eq(2, proto::GameclientMessageInspectPlayer::kCommandFieldNumber));

		expect(eq(1, proto::GameserverMessageObjectInfo::kObjectsFieldNumber));
		expect(eq(1, proto::GameclientMessageGetObjectInfo::kIdentifiersFieldNumber));
	};

	test("dialog enums match the official schema on both layers") = [] {
		// MESSAGE numbers by decade; the two values this server sends are pinned against
		// the named server constants from phase 1 slice 5.
		expect(eq(static_cast<int>(proto::MESSAGE_PREY_MESSAGE), static_cast<int>(MESSAGEDIALOG_PREY_MESSAGE)));
		expect(eq(static_cast<int>(proto::MESSAGE_IMBUEMENT_ERROR), static_cast<int>(MESSAGEDIALOG_IMBUEMENT_ERROR)));

		expect(eq(0, proto::LIST_TYPE_UNKNOWN));
		expect(eq(1, proto::LIST_TYPE_HOUSE_GUESTS));
		expect(eq(2, proto::LIST_TYPE_HOUSE_SUBOWNERS));
		expect(eq(3, proto::LIST_TYPE_NAME_DOOR));

		// The server's INSPECT_* request constants are CipSoft's LOCATION_IDENTIFIER.
		expect(eq(static_cast<int>(proto::LOCATION_IDENTIFIER_POSITION), static_cast<int>(INSPECT_NORMALOBJECT)));
		expect(eq(static_cast<int>(proto::LOCATION_IDENTIFIER_NPCTRADE), static_cast<int>(INSPECT_NPCTRADE)));
		expect(eq(static_cast<int>(proto::LOCATION_IDENTIFIER_ITEMINFO), static_cast<int>(INSPECT_CYCLOPEDIA)));
		expect(eq(static_cast<int>(proto::LOCATION_IDENTIFIER_WEAPON_PROFICIENCY_ITEM_INFO), static_cast<int>(INSPECT_PROFICIENCY)));

		// The inspect-character actions are the official INSPECT_PLAYER_COMMAND values.
		expect(eq(static_cast<int>(proto::INSPECT_PLAYER_COMMAND_INVITE_TO_INSPECT), static_cast<int>(INSPECT_CHARACTER_INVITE)));
		expect(eq(static_cast<int>(proto::INSPECT_PLAYER_COMMAND_INSPECT), static_cast<int>(INSPECT_CHARACTER_INSPECT)));
		expect(eq(static_cast<int>(proto::INSPECT_PLAYER_COMMAND_SET_ALLOW_ALL), static_cast<int>(INSPECT_CHARACTER_ALLOW_ALL)));

		expect(eq(0, proto::INSPECT_OBJECT));
		expect(eq(1, proto::INSPECT_PLAYER));
		expect(eq(0, proto::INSPECT_WINDOW_INSPECT));
		expect(eq(1, proto::INSPECT_WINDOW_CYCLOPEDIA_ITEMINFO));
		expect(eq(2, proto::INSPECT_WINDOW_WEAPON_PROFICIENCY_ITEMINFO));
	};
};
