/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    id: observedTimePage

    property bool valid: observedTimeString.length > 0
    property string instruction: "Date/Time observed"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

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

// TODO LD: Find alternative
//    Qt1.Calendar {
//        id: calendar
//        minimumDate: new Date(2018, 0, 1)
//        maximumDate: new Date()

//        anchors {
//            left: parent.left
//            right: parent.right
//            top: parent.top
//            margins: 16 * scaleFactor
//        }

//        height: parent.height * 0.5

//        style: CalendarStyle {

//            navigationBar: Rectangle {
//                color: Material.background
//                height: dateText.height * 2
//                border {
//                    width: 1 * scaleFactor
//                    color: Material.foreground
//                }

//                Button {
//                    id: previousMonth
//                    anchors{
//                        verticalCenter: parent.verticalCenter
//                        left: parent.left
//                    }
//                    width: parent.height
//                    height: width
//                    text: "<"
//                    font {
//                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                        family: DsaStyles.fontFamily
//                    }
//                    onClicked: control.showPreviousMonth()
//                }

//                Label {
//                    id: dateText
//                    anchors{
//                        verticalCenter: parent.verticalCenter
//                        left: previousMonth.right
//                        leftMargin: 2 * scaleFactor
//                        right: nextMonth.left
//                        rightMargin: 2 * scaleFactor
//                    }
//                    text: styleData.title
//                    font {
//                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                        family: DsaStyles.fontFamily
//                    }
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                    fontSizeMode: Text.Fit
//                    color: Material.foreground
//                }

//                Button {
//                    id: nextMonth
//                    anchors{
//                        verticalCenter: parent.verticalCenter
//                        right: parent.right
//                    }
//                    width: parent.height
//                    height: width
//                    text: ">"
//                    font {
//                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                        family: DsaStyles.fontFamily
//                    }
//                    onClicked: control.showNextMonth()
//                }
//            }

//            dayOfWeekDelegate: Rectangle {
//                color: Material.background
//                height: dayOfWeekText.height * 2

//                Label {
//                    id: dayOfWeekText
//                    anchors.centerIn: parent
//                    text: Qt.locale().dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
//                    color: Material.foreground
//                    font {
//                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                        family: DsaStyles.fontFamily
//                    }
//                }
//            }

//            gridVisible: false
//            dayDelegate: Rectangle {
//                color: styleData.selected ? Material.accent :
//                                            (styleData.visibleMonth && styleData.valid ? Material.primary : Material.background);

//                Label {
//                    text: styleData.date.getDate()
//                    anchors.centerIn: parent
//                    font {
//                        pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
//                        family: DsaStyles.fontFamily
//                    }
//                    color: Material.foreground
//                }

//                Rectangle {
//                    width: parent.width
//                    height: 1 * scaleFactor
//                    color: Material.background
//                    anchors.bottom: parent.bottom
//                }

//                Rectangle {
//                    width: 1 * scaleFactor
//                    height: parent.height
//                    color: Material.background
//                    anchors.right: parent.right
//                }
//            }
//        }
//    }

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
            width: 16 * scaleFactor
            height: 64 * scaleFactor
        }

        Text {
            text: ":"
            color: Material.foreground
            anchors.verticalCenter: hoursTumbler.verticalCenter
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
            }
            width: hoursTumbler.width
            height: hoursTumbler.height
            verticalAlignment: Text.AlignVCenter
        }

        Tumbler {
            id: minutesTumbler
            anchors.verticalCenter: hoursTumbler.verticalCenter
            model: 59
            delegate: delegateComponent
            width: hoursTumbler.width
            height: hoursTumbler.height
        }

        Text {
            anchors.verticalCenter: hoursTumbler.verticalCenter
            text: ":"
            color: Material.foreground
            font {
                pixelSize: DsaStyles.toolFontPixelSize * scaleFactor
                family: DsaStyles.fontFamily
            }
            width: hoursTumbler.width
            height: hoursTumbler.height
            verticalAlignment: Text.AlignVCenter
        }

        Tumbler {
            id: secondsTumbler
            anchors.verticalCenter: hoursTumbler.verticalCenter
            model: 59
            delegate: delegateComponent
            width: hoursTumbler.width
            height: hoursTumbler.height
        }
    }
}
