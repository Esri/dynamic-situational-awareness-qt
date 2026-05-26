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

#ifndef MESSAGEFEEDCONSTANTS_H
#define MESSAGEFEEDCONSTANTS_H

// Qt headers
#include <QString>

namespace Dsa {

namespace MessageFeedConstants {
  inline static const QString OBSERVATION_REPORT_CONFIG_PROPERTYNAME = QStringLiteral("ObservationReportConfig");
  inline static const QString OBSERVATION_REPORT_CONFIG_PORT         = QStringLiteral("port");
  inline static const QString LOCATION_BROADCAST_CONFIG_PROPERTYNAME = QStringLiteral("LocationBroadcastConfig");
  inline static const QString LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE = QStringLiteral("messageType");
  inline static const QString LOCATION_BROADCAST_CONFIG_PORT         = QStringLiteral("port");
  inline static const QString MESSAGE_FEEDS_PROPERTYNAME             = QStringLiteral("MessageFeeds");
  inline static const QString MESSAGE_FEEDS_NAME                     = QStringLiteral("name");
  inline static const QString MESSAGE_FEEDS_TYPE                     = QStringLiteral("type");
  inline static const QString MESSAGE_FEEDS_RENDERER                 = QStringLiteral("renderer");
  inline static const QString MESSAGE_FEEDS_THUMBNAIL                = QStringLiteral("thumbnail");
  inline static const QString MESSAGE_FEEDS_PLACEMENT                = QStringLiteral("placement");
  inline static const QString MESSAGE_FEEDS_OBSERVATIONS_SHOW        = QStringLiteral("observationsShow");
  inline static const QString MESSAGE_FEEDS_OBSERVATIONS_COLOR       = QStringLiteral("observationsColor");
  inline static const QString MESSAGE_FEEDS_OBSERVATIONS_SIZE        = QStringLiteral("observationsSize");
  inline static const QString MESSAGE_FEEDS_OBSERVATIONS_MAXIMUM     = QStringLiteral("observationsMaximum");
  inline static const QString MESSAGE_FEEDS_TRACK_LINE_SHOW          = QStringLiteral("trackLineShow");
  inline static const QString MESSAGE_FEEDS_TRACK_LINE_COLOR         = QStringLiteral("trackLineColor");
  inline static const QString MESSAGE_FEEDS_TRACK_LINE_SIZE          = QStringLiteral("trackLineSize");
  inline static const QString MESSAGE_FEED_UDP_PORTS_PROPERTYNAME    = QStringLiteral("MessageFeedUdpPorts");

  inline static const QString MESSAGE_FEEDS_PLACEMENT_DEFAULT = QStringLiteral("draped");

  inline static const QString MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT = QStringLiteral("#377eb8"); // blue
  inline static const QStringList MESSAGE_FEEDS_TRACK_DISPLAY_COLORS{
    QStringLiteral("#e41a1c"), // red
    MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT,
    QStringLiteral("#4daf4a"), // green
    QStringLiteral("#984ea3"), // purple
    QStringLiteral("#ff7f00"), // orange
    QStringLiteral("#ffff33"), // yellow
    QStringLiteral("#a65628"), // brown
    QStringLiteral("#f781bf"), // pink
    QStringLiteral("#999999"), // gray
  };
} // MessageFeedConstants

} // Dsa

#endif // MESSAGEFEEDCONSTANTS_H
