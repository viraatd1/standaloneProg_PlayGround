import QtQuick 2.0
import ScitonComponents 1.0 as ScitonUIControls

Rectangle {
    id: qt_Item_Components_Root_keyValue

    property string comp_key: "key"
    property var comp_value: 0
    property string comp_value_bckgdClr: sc_Constants_ref.transparent
    property string comp_value_borderClr: sc_Constants_ref.midnightBlue
    property bool comp_use_border: false
    property alias keyItem: qt_Text_key

    //clip: true
    width: 100
    height: 40
    radius: 4
    color: sc_Constants_ref.transparent
    border.color: comp_use_border ? sc_Constants_ref.gray : sc_Constants_ref.transparent
    border.width: comp_use_border ? 1 : 0

    Constants {
        id: sc_Constants_ref
    }

    Text {
        id: qt_Text_key

        anchors {
            left: parent.left
            leftMargin: 4
            verticalCenter: parent.verticalCenter
        }
        width: parent.width * 0.5
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("%1:").arg(comp_key)
        font.pixelSize: 14
        color: "#272A2A"//sc_Constants_ref.peterRiver
    }

    Rectangle {
        id: qt_Rectangle_ValueContainer

        anchors {
            left: qt_Text_key.right
            leftMargin: 4
            verticalCenter: qt_Text_key.verticalCenter
            right: parent.right
            rightMargin: 4
        }
        height: parent.height * 0.8
        color: comp_value_bckgdClr
        border.color: comp_value_borderClr
        border.width: 1
        radius: 4
        clip: true

        Text {
            id: qt_Text_Value

            anchors {
                centerIn: parent
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: qsTr("%1").arg(comp_value == undefined ? "" : comp_value)
            font.pixelSize: 18
            color: "#E23B1D"
        }
    }
}
