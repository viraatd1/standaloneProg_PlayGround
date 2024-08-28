import QtQuick.VirtualKeyboard 2.4
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtCharts 2.3

ApplicationWindow {
    id: qt_ApplicationWindow_root

    visible: true
    width: 1024
    height: 768

    /* Root Item of QML Quick Type that the application will be using for the launch of QML Items */
    Item {
        id: qt_Item_root

        anchors.fill: parent

        Image {
            id: qt_Image_background

            anchors.fill: qt_Item_root
            source: "qrc:/mainGeometricBack.png"
        }

        Item {
            id: qt_Item_MotionParameters

            anchors {
                right: qt_Item_root.right
                rightMargin: 10
                top: qt_Item_root.top
                topMargin: 10
            }

            width: 250
            height: 300

            Text {
                id: qt_Text_MotionParametersLabel

                anchors {
                    left: qt_Item_MotionParameters.left
                    right: qt_Item_MotionParameters.right
                    top: qt_Item_MotionParameters.top
                    topMargin: 2
                }
                height: contentHeight
                text: qsTr("Handpiece\nMotion Parameters")
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                //viraatDebugRectangle
                anchors.fill: parent
                color: "black"
                border.color: "red"
                border.width: 2
                opacity:0.3
            }
        }

        Item {
            id: qt_Item_RFModuleHeaders

            anchors {
                left: qt_Item_root.left
                right: qt_Item_MotionParameters.left
                rightMargin: 2
                top: qt_Item_root.top
                topMargin: 10
            }
            height: 220

            Text {
                id: qt_Text_RFModuleHeadersLabel

                anchors {
                    left: qt_Item_RFModuleHeaders.left
                    right: qt_Item_RFModuleHeaders.right
                    top: qt_Item_RFModuleHeaders.top
                    topMargin: 2
                }
                height: contentHeight
                text: qsTr("RFModule Headers")
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                //viraatDebugRectangle
                anchors.fill: parent
                color: "black"
                border.color: "green"
                border.width: 2
                opacity:0.3
            }
        }

        Item {
            id: qt_Item_pulseData

            anchors {
                left: qt_Item_root.left
                right: qt_Item_MotionParameters.left
                rightMargin: 2
                top: qt_Item_RFModuleHeaders.bottom
                topMargin: 2
            }
            height: 80

            readonly property int borderSpacing: 6
            readonly property int buttonSpacing: 6

            Text {
                id: qt_Text_pulseNumberLabel

                anchors {
                    top: qt_Item_pulseData.top
                    topMargin: 2
                    horizontalCenter: qt_Item_pulseData.horizontalCenter
                }
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Pulse Data Point's")
            }

            ListView {
                id: qt_ListView_pulseDataHorizontalList

                anchors {
                    //centerIn: parent
                    left: qt_Button_Left.right
                    leftMargin: qt_Item_pulseData.buttonSpacing
                    right: qt_Button_Right.left
                    rightMargin: qt_Item_pulseData.buttonSpacing
                    verticalCenter: qt_Item_pulseData.verticalCenter
                }
                height: 40
                orientation: ListView.Horizontal
                model: 15

                delegate: Rectangle {
                    width: 42
                    height: 40
                    color: "transparent"
                    border.color: "lightblue"
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: index
                        font.pointSize: 20
                    }
                }
            }

            Button {
                id: qt_Button_Left

                text: "<"
                anchors {
                    verticalCenter: qt_Item_pulseData.verticalCenter
                    left: qt_Item_pulseData.left
                    leftMargin: qt_Item_pulseData.borderSpacing
                }
                width: 52
                height: 60
                onClicked: {

                }
            }

            Button {
                id: qt_Button_Right

                text: ">"
                anchors {
                    verticalCenter: qt_Item_pulseData.verticalCenter
                    right: qt_Item_pulseData.right
                    rightMargin: qt_Item_pulseData.borderSpacing
                }
                width: 52
                height: 60
                onClicked: {

                }
            }

            Rectangle {
                //viraatDebugRectangle
                anchors.fill: parent
                color: "transparent"
                border.color: "pink"
                border.width: 2
                //opacity:0.3
            }

        }

        Item {
            id: qt_Item_pulseDataImpedanceGraph

            anchors {
                top: qt_Item_pulseData.bottom
                left: qt_Item_root.left
                right: qt_Item_root.right
                bottom: qt_Item_root.bottom
                margins: 2
            }

            ChartView {
                id: qt_ChartView_ImpedanceGraph

                anchors {
                    fill: parent
                }
                width: parent.width
                height: parent.height

                LineSeries {
                    id: series
                    name: "Impedance Graph"
                    axisX: xAxis
                    axisY: yAxis
                }

                ValueAxis {
                    id: xAxis
                    min: 0
                    max: 950
                    tickCount: 100
                    titleText: "Time (ms)"
                }

                ValueAxis {
                    id: yAxis
                    min: 0
                    max: 50
                    tickCount: 50
                    titleText: "Voltage (v)"
                }

//                ValueAxis {
//                    id: yAxis1
//                    min: 0
//                    max: 50
//                    tickCount: 10
//                }
            }

        }







    }


}
