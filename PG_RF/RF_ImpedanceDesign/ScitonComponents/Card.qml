import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0

Rectangle {
    id: card

    width: 300
    height: 200
    radius: 10
    color: "white"
    border.color: "#dcdcdc"
    border.width: 1
    property alias title: titleText.text
    property alias content: contentText.text

    DropShadow {
        anchors.fill: parent
        horizontalOffset: 2
        verticalOffset: 2
        radius: 4
        samples: 16
        color: "#88888888"
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        Text {
            id: titleText
            text: "Title"
            font.pixelSize: 20
            font.bold: true
            color: "#333"
        }

        Text {
            id: contentText
            text: "Content goes here"
            font.pixelSize: 14
            color: "#666"
        }
    }
}
