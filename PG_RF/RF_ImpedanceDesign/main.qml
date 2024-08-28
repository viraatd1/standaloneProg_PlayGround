import QtQuick.VirtualKeyboard 2.4
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtCharts 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import "./ScitonComponents" as ScitonUIControls

/*PDPD : Pulse Data Point Description */

ApplicationWindow {
    id: qt_ApplicationWindow_root

    visible: true
    width: 1024
    height: 768

    property string currentSelectedPulse

    /* For keyboard activities */
    InputPanel {
        id: qt_InputPanel_VKB

        z: 99 // Ensure it's on top of other elements
        visible: Qt.inputMethod.visible
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    /* Root Item of QML Quick Type that the application will be using for the launch of QML Items */
    Item {
        id: qt_Item_root

        anchors.fill: parent

        readonly property int __rootItemBorderSpacing: 2
        readonly property int __containerBorderSpacing: 2
        readonly property int __containerItemInternalSpacing: 4

        Image {
            id: qt_Image_background

            anchors.fill: qt_Item_root
            source: "qrc:/mainGeometricBack.png"
        }

        Text {
            id: qt_Text_RFModuleHeadersLabel

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: qt_Item_root.__containerBorderSpacing
            }
            height: contentHeight
            text: qsTr("RFModule Headers")
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            id: qt_Rectangle_RFModuleHeadersContainer

            anchors {
                left: qt_Item_root.left
                right: qt_Item_root.right
                top: qt_Text_RFModuleHeadersLabel.bottom
                margins: qt_Item_root.__containerBorderSpacing
            }
            height: 220
            color: "transparent"
            z:1
            border.color: "#635F5E"
            border.width: 2
            radius: 4

            GridLayout {
                id: qt_GridLayout_ParametersLayout

                z:10
                columns: 5
                rows: 5
                columnSpacing: 2
                rowSpacing: 2
                anchors.fill: parent

                Repeater {
                    id: qt_Repeater_ModuleParameters

                    model: 20

                    ScitonUIControls.KeyValuePair {
                        z:12
                        width: 180
                        height: 40
                        comp_key: _capturedDataStore_.headerKeysList()[index]
                        comp_value: _capturedDataStore_.capturedHeaderValuesList[index]
                    }
                }
            }
        }

        Rectangle {
            id: qt_Rectangle_PulseDataPointsContainer

            anchors {
                left: qt_Item_root.left
                right: qt_Item_root.right
                top: qt_Rectangle_RFModuleHeadersContainer.bottom
            }
            height: 80

            /*LEFT : User Input TextField */
            Item {
                id: qt_Item_UserInputPDPD

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: 180

                ScitonUIControls.Input {
                    id: sc_Input_PDPDGroup

                    objectName: "sc_Input_PDPDGroup"
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                        margins: qt_Item_root.__containerItemInternalSpacing - 2
                    }
                    width: 172
                    height: 36
                    placeholderText: "PulseDataGroup"
                    text: _capturedDataStore_.capturedPDGroup
                    onUserAcceptedText: {
                        _capturedDataStore_.capturedPDGroup = acceptedText;
                    }
                }

                ScitonUIControls.Input {
                    id: sc_Input_PDPD

                    objectName: "sc_Input_PDPD"
                    anchors {
                        top: sc_Input_PDPDGroup.bottom
                        horizontalCenter: parent.horizontalCenter
                        topMargin: 4
                        margins: qt_Item_root.__containerItemInternalSpacing - 2
                    }
                    width: 172
                    height: 36
                    placeholderText: "PulseDataDescription"
                    text: _capturedDataStore_.capturedPDDescription
                    onUserAcceptedText: {
                        _capturedDataStore_.capturedPDDescription = acceptedText;
                    }
                }

                Rectangle {
                    id: qt_Rectangle_rightDivider

                    anchors {
                        right: parent.right
                        rightMargin: -3
                        verticalCenter: parent.verticalCenter
                    }
                    color: "grey"
                    width: 1
                    height: parent.height - 8
                    radius: 4
                }
            }

            /*RIGHT : Captured & Collected Pulse Data Counter and Pulses Meta List */
            Item {
                id: qt_Item_PDPDCounterAndList

                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                    margins: qt_Item_root.__containerItemInternalSpacing
                }
                width: 100

                Rectangle {
                    id: qt_Rectangle_leftDivider

                    anchors {
                        left: parent.left
                        leftMargin: -3
                        verticalCenter: parent.verticalCenter
                    }
                    color: "grey"
                    width: 1
                    height: parent.height - 8
                    radius: 4
                }

                Text {
                    id: qt_Text_key

                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    width: parent.width
                    height: contentHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("Captured\n Pulse Data Points")
                    font.pixelSize: 12
                    color: "#272A2A"
                }

                Rectangle {
                    id: qt_Rectangle_ValueContainer

                    anchors {
                        top: qt_Text_key.bottom
                        topMargin: 4
                        horizontalCenter: qt_Text_key.horizontalCenter
                        bottom: parent.bottom
                        bottomMargin: 4
                    }
                    width: parent.width * 0.8
                    color: "transparent"
                    border.color: "#2c3e50"
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
                        text: qsTr("%1").arg(_capturedDataStore_.capturedDPCount)
                        font.pixelSize: 18
                        color: "#E23B1D"
                    }
                }
            }

            /*CENTRE : Horizontal list with captured data points dynamically listed */
            Item {
                id: qt_Item_PulseDataPointsHList

                anchors {
                    left: qt_Item_UserInputPDPD.right
                    right: qt_Item_PDPDCounterAndList.left
                    verticalCenter: parent.verticalCenter
                    margins: qt_Item_root.__containerItemInternalSpacing
                }
                height: 70

                Text {
                    id: qt_Text_pulseNumberLabel

                    anchors {
                        top: qt_Item_PulseDataPointsHList.top
                        topMargin: 1
                        horizontalCenter: qt_Item_PulseDataPointsHList.horizontalCenter
                    }
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Pulse Data Point's")
                }

                ListView {
                    id: qt_ListView_pulseDataHorizontalList

                    anchors {
                        top: qt_Text_pulseNumberLabel.bottom
                        topMargin: qt_Item_root.__containerItemInternalSpacing
                        left: parent.left
                        leftMargin: qt_Item_root.__containerItemInternalSpacing
                        right: parent.right
                        rightMargin: qt_Item_root.__containerItemInternalSpacing
                    }
                    height: 50
                    orientation: ListView.Horizontal
                    model: _capturedDataStore_.capturedDPCount
                    spacing: 6
                    clip: true
                    currentIndex : _capturedDataStore_.capturedDPSelectedIndex
                    delegate: Rectangle {
                        width: 60
                        height: 46
                        color: qt_ListView_pulseDataHorizontalList.currentIndex === index ? "#95a5a6" : "transparent"
                        border.color: "#2c3e50"
                        border.width: 1.4
                        radius: 4
                        clip: true
                        z:2

                        Text {
                            anchors {
                                top: parent.top
                                topMargin: 4
                                horizontalCenter: parent.horizontalCenter
                            }
                            z:5
                            width: 58
                            height: 44
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            elide: Text.ElideRight
                            text: splitAndReconstruct(_capturedDataStore_.capturedPDGDList[index])
                            font.pixelSize: 12
                            font.italic: true
                            color: "#E23B1D"
                            clip: true

                            MouseArea {
                                anchors.fill: parent
                                onReleased: {
                                    _capturedDataStore_.graphLoaded = false
                                    _capturedDataStore_.capturedDPSelectedIndex = index
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: qt_Rectangle_pulseDataImpedanceGraph

            anchors {
                top: qt_Rectangle_PulseDataPointsContainer.bottom
                left: qt_Item_root.left
                right: qt_Item_root.right
                bottom: qt_Item_root.bottom
                margins: 2
            }
            border.color: "#635F5E"
            border.width: 2
            radius: 4

            Row {
                id: qt_Row_portReadingButtons

                anchors {
                    top: parent.top
                    topMargin: 4
                    right: parent.right
                    rightMargin: 4
                }
                width: childrenRect.width
                height: childrenRect.height
                spacing: 8
                visible: false
                z: 50

                Button {
                    text: "Start Reading"
                    onClicked: {
                        serialReader.startReading()
                    }
                }

                Button {
                    text: "Stop Reading"
                    onClicked: {
                        serialReader.stopReading()
                    }
                }

                Button {
                    text: "Send Data"
                    onClicked: {
                        serialReader.writeData("Hello, Serial!")
                    }
                }
            }

            Text {
                id: qt_Text_plotExternalLegend

                anchors {
                    top: parent.top
                    topMargin: 16
                    right: parent.right
                    rightMargin: 60
                }
                z:5
                width: 200
                height: 120
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                font.pixelSize: 12
                font.italic: true
                clip: true
                color: "White"
                textFormat: Text.RichText
                text: splitAndReconstruct(_capturedDataStore_.impedanceLegendMetaData)
                function splitAndReconstruct(inputString) {
                    // Split the input string into parts based on underscore _
                    var parts = inputString.split('_');

                    // Trim leading zeros from numeric parts and reconstruct with formatted output
                    var plot = "<b><u>Plot</u></b>";
                    var numberLabel = "# :";
                    var groupLabel = "Group :";
                    var descriptionLabel = "Description :";

                    var reconstructedString = plot + "<br>" +
                            numberLabel + " " + parseInt(parts[0], 10).toString() + "<br>" +
                            groupLabel + " " + parts[1] + "<br>" +
                            descriptionLabel + " " + parts[2];

                    // Return the reconstructed string
                    return reconstructedString;
                }
            }

            ChartView {
                id: qt_ChartView_ImpedanceGraph

                anchors {
                    left: parent.left
                    right: parent.right
                }
                width: parent.width
                height: parent.height
                antialiasing: true
                theme: ChartView.ChartThemeBlueCerulean
                legend.visible: true
                legend.alignment: Qt.AlignTop
                legend.backgroundVisible: false
                legend.markerShape: Legend.MarkerShapeFromSeries
                //visible: _capturedDataStore_.graphLoaded

                SplineSeries {
                    id: qt_SplineSeries_ImpedancePlot

                    name: "Impedance Plot"
                    axisX: qt_ValueAxis_ImpedancePlot_xAxis
                    axisY: qt_ValueAxis_ImpedancePlot_yAxis
                    useOpenGL: true
                    Component.onCompleted: {
                        /* Send this series pointer to the backend CPP, so as to update the data from the backend */
                        _capturedDataStore_.registerSeriesHandle(qt_SplineSeries_ImpedancePlot);
                    }
                }

                ValueAxis {
                    id: qt_ValueAxis_ImpedancePlot_xAxis

                    min: 0
                    max: 949
                    tickCount: 25
                    titleText: "Time (ms)"
                    titleFont: Qt.font({pointSize: 10, italic: true, bold: true})
                    labelsAngle: -30
                    labelsFont: Qt.font({pointSize: 6})
                }

                ValueAxis {
                    id: qt_ValueAxis_ImpedancePlot_yAxis

                    min: _capturedDataStore_.yAxisMin
                    max: _capturedDataStore_.yAxisMax + 10
                    tickCount: 10
                    titleText: "Impedance"
                    titleFont: Qt.font({pointSize: 10, italic: true, bold: true})
                    labelsFont: Qt.font({pointSize: 10})
                }
            }

        }
    }

    function splitAndReconstruct(inputString) {
        // Split the input string into parts based on underscore _
        var parts = inputString.split('_');

        // Trim leading zeros from each part and reconstruct with new lines if exactly three parts are found
        var reconstructedString = parts.map(function(part) {
            // Check if the part is numeric (starts with digits)
            if (/^\d+$/.test(part)) {
                return parseInt(part, 10).toString(); // Trim leading zeros
            } else {
                return part; // Keep non-numeric parts unchanged
            }
        }).filter(function(part) {
            // Filter out undefined or empty parts
            return part !== undefined && part !== '';
        }).join('\n');

        // Return the reconstructed string
        return reconstructedString;
    }
}



//Item {
//    id: qt_Item_MotionParameters

//    anchors {
//        right: qt_Item_root.right
//        rightMargin: 10
//        top: qt_Item_root.top
//        topMargin: 10
//    }
//    visible: false
//    width: 250
//    height: 220

//    Text {
//        id: qt_Text_MotionParametersLabel

//        anchors {
//            left: qt_Item_MotionParameters.left
//            right: qt_Item_MotionParameters.right
//            top: qt_Item_MotionParameters.top
//            topMargin: 2
//        }
//        height: contentHeight
//        text: qsTr("Handpiece\nMotion Parameters")
//        horizontalAlignment: Text.AlignHCenter
//    }



//    Rectangle {
//        //viraatDebugRectangle
//        anchors.fill: qt_Text_MotionParametersLabel
//        color: "black"
//        border.color: "red"
//        border.width: 2
//        opacity: 0.3
//    }
//}

//readonly property int __HListBorderSpacing: 6
//readonly property int __HListButtonSpacing: 6
//Button {
//    id: qt_Button_Left

//    text: "<"
//    anchors {
//        verticalCenter: qt_Item_PulseDataPointsHList.verticalCenter
//        left: qt_Item_PulseDataPointsHList.left
//        leftMargin: qt_Item_PulseDataPointsHList.__HListBorderSpacing
//    }
//    width: 52
//    height: 60
//    onClicked: {

//    }

//}

//Button {
//    id: qt_Button_Right

//    text: ">"
//    anchors {
//        verticalCenter: qt_Item_PulseDataPointsHList.verticalCenter
//        right: qt_Item_PulseDataPointsHList.right
//        rightMargin: qt_Item_PulseDataPointsHList.__HListBorderSpacing
//    }
//    width: 52
//    height: 60
//    onClicked: {

//    }
//}

//ScitonUIControls.KeyValuePair {
//    id: sc_KeyValuePair_Time

//    anchors {
//        horizontalCenter: parent.horizontalCenter
//        //margins: qt_Item_root.__containerItemInternalSpacing - 2
//    }
//    comp_key: "Time"
//    keyItem.width: parent.width * 0.2
//    width: 172
//    height: 30
//}
