import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TextField {
    id: textField

    height: 40
    width: 175

    Constants {
        id: constants;
    }

    // TODO :Change text color on Error and Success
    property string borderColor: focus ? constants.emerald : constants.silver;
    property string backgroundColor: disabled ? "#f7f9f9" : "white";
    property int pointSize: 11;
    property bool disabled: false;
    property bool error: false;
    property bool success: false;

    signal userAcceptedText(string acceptedText);

    onDisabledChanged: {
        if (disabled) {
            textField.borderColor = "#e1e6e6";
            textField.textColor = "#e1e6e6";
        }

        else {
            textField.textColor = "black";
        }
    }

    onSuccessChanged: {
        if (success) {
            textField.textColor = constants.emerald;
            textField.borderColor = constants.emerald;
        }

        else {
            textField.textColor = "black";
            textField.borderColor = focus ? constants.emerald : constants.silver;
        }
    }

    onErrorChanged: {
        if (error) {
            textField.textColor = constants.alizarin;
            textField.borderColor = constants.alizarin;
        }

        else {
            textField.textColor = "black";
            textField.borderColor = focus ? constants.emerald : constants.silver;
        }

    }

    // Automatically show virtual keyboard when focused
    onFocusChanged: {
        if(text !== ""){
            userAcceptedText(text);
        }
        if (focus && Qt.inputMethod.visible) {
            Qt.inputMethod.show();
        }
        else if (!Qt.inputMethod.visible) {
            focus = false; // Remove focus if virtual keyboard is hidden
        }
    }

    // Handle Enter key press
    Keys.onPressed: {
        if(event.key === Qt.Key_Hide){
            console.log("Hide Keyboard Pressed");
            event.accepted = true;
            return;
        }

        if (event.key === Qt.Key_Return ||
                event.key === Qt.Key_Enter ||
                event.key === Qt.Key_Escape)
        {
            event.accepted = true;
            focus = false; // Remove focus when Enter is pressed
            userAcceptedText(text);
        }
    }

    placeholderText: "Input";
    style: TextFieldStyle {
        padding.left: 12;
        font {
            pointSize: control.pointSize;
        }

        background: Rectangle {
            height: control.height;
            width: control.width;
            color: control.backgroundColor;
            border {
                width: 2;
                color: control.borderColor;
            }
            radius: 4;
        }
    }

    MouseArea {
        anchors.fill: parent;
        enabled: textField.disabled;
    }
}

