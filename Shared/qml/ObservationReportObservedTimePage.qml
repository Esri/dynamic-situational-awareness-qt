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
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Window
import Esri.ArcGISRuntime.OpenSourceApps.DSA

Item {
    id: observedTimePage

    property bool valid: observedTimeString.length > 0
    property string instruction: "Date/Time observed"
    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    property date observedTime: new Date();

    property string observedTimeString: dateInput.text + " "
                                        + timeInput.text

    function clear() {
        dateInput.text = currentDate();
        timeInput.text = new Date().toLocaleTimeString();
    }

    function text() {
        return "observed:" + observedTimeString;
    }

    onObservedTimeStringChanged: {
        const isoDateString = Date.parse(dateInput.text + "T" + timeInput.text.substring(0, 8));
        // make sure the date is valid
        if (isNaN(isoDateString) === false) {
            observedTime = new Date(isoDateString);
        }
    }

    onVisibleChanged: {
        if (!visible)
            return;

        clear();
    }

    function currentDate() {
        const today = new Date();
        const dd = String(today.getDate()).padStart(2, '0');
        const mm = String(today.getMonth() + 1).padStart(2, '0');
        const yyyy = today.getFullYear();

        return yyyy + '-' + mm + '-' + dd;
    }

    ColumnLayout {
        id: dateColumn
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Date (YYYY-MM-DD)"
            color: Material.accent
            font.family: DsaStyles.fontFamily
        }

        TextEdit {
            id: dateInput
            Layout.alignment: Qt.AlignHCenter
            text: currentDate()
            color: Material.foreground
            font.family: DsaStyles.fontFamily
            selectByMouse: true
            selectByKeyboard: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Time (HH:MM:SS)"
            color: Material.accent
            font.family: DsaStyles.fontFamily
        }

        TextEdit {
            id: timeInput
            Layout.alignment: Qt.AlignHCenter
            text: new Date().toLocaleTimeString()
            color: Material.foreground
            font.family: DsaStyles.fontFamily
            selectByMouse: true
            selectByKeyboard: true
        }
    }
}
