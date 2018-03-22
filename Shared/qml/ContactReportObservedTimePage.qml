
// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2
import Esri.DSA 1.0

Item {
    id: observedTimePage

    property bool valid: observedTimeString.length > 0
    property string instruction: "Date/Time observed"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    property date observedTime: new Date();

    property string observedTimeString: calendar.selectedDate.getFullYear() + "-"
                                  + (calendar.selectedDate.getMonth()+1)  + "-"
                                  + calendar.selectedDate.getDate() + " "
                                  + hoursTumbler.currentIndex + ":"
                                  + minutesTumbler.currentIndex + ":"
                                  + secondsTumbler.currentIndex;

    function clear() {
        var currentDate = new Date();
        hoursTumbler.currentIndex = currentDate.getHours();
        minutesTumbler.currentIndex = currentDate.getMinutes();
        secondsTumbler.currentIndex = currentDate.getSeconds();
    }

    function text() {
        return "observed:" + observedTimeString;
    }

    onObservedTimeStringChanged: {
        observedTime = calendar.selectedDate;
        observedTime.setHours(hoursTumbler.currentIndex);
        observedTime.setMinutes(minutesTumbler.currentIndex);
        observedTime.setSeconds(secondsTumbler.currentIndex);
    }

    onVisibleChanged: {
        if (!visible)
            return;

        clear();
    }

    Calendar {
        id: calendar
        minimumDate: new Date(2018, 0, 1)
        maximumDate: new Date()

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 16 * scaleFactor
        }

        height: parent.height * 0.5

        style: CalendarStyle {
            gridVisible: false
            dayDelegate: Rectangle {
                color: styleData.selected ? Material.accent :
                                            (styleData.visibleMonth && styleData.valid ? Material.primary : Material.background);

                Label {
                    text: styleData.date.getDate()
                    anchors.centerIn: parent
                    font {
                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                        family: DsaStyles.fontFamily
                    }
                    color: Material.foreground
                }

                Rectangle {
                    width: parent.width
                    height: 1 * scaleFactor
                    color: Material.background
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    width: 1 * scaleFactor
                    height: parent.height
                    color: Material.background
                    anchors.right: parent.right
                }
            }
        }
    }

    Row {
        id: timeRow
        anchors {
            top: calendar.bottom
            horizontalCenter: calendar.horizontalCenter
            margins: 4 * scaleFactor
        }
        spacing: 4 * scaleFactor

        Component {
            id: delegateComponent

            Label {
                text: index
                opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: Material.foreground
                font {
                    pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                    family: DsaStyles.fontFamily
                }
            }
        }

        Tumbler {
            id: hoursTumbler
            model: 23
            delegate: delegateComponent
        }

        Text {
            text: ":"
            color: Material.foreground
            anchors.verticalCenter: hoursTumbler.verticalCenter
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
            }
        }

        Tumbler {
            id: minutesTumbler
            anchors.verticalCenter: hoursTumbler.verticalCenter
            model: 59
            delegate: delegateComponent
        }

        Text {
            anchors.verticalCenter: hoursTumbler.verticalCenter
            text: ":"
            color: Material.foreground
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
            }
        }

        Tumbler {
            id: secondsTumbler
            anchors.verticalCenter: hoursTumbler.verticalCenter
            model: 59
            delegate: delegateComponent
        }
    }
}
