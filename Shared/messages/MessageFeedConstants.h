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
#include <QUrl>

namespace Dsa::MessageFeedConstants {

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
} // Dsa::MessageFeedConstants


namespace Dsa::MessageFeeds::Types {
inline static const QString CURSOR_ON_TARGET = QStringLiteral("cot");
inline static const QString EOD              = QStringLiteral("eod");
inline static const QString POSITION_REPORT  = QStringLiteral("position_report");
inline static const QString SENSOR_OBS       = QStringLiteral("sensor_obs");
inline static const QString SITREP           = QStringLiteral("sitrep");
inline static const QString SPOTREP          = QStringLiteral("spotrep");
inline static const QString URL_PATTERN = QStringLiteral(":/Resources/messages/schemas/%1.json");
inline static const std::unordered_map<QString, QString> SCHEMA_URLS{
  { CURSOR_ON_TARGET, URL_PATTERN.arg(CURSOR_ON_TARGET) },
  { EOD,              URL_PATTERN.arg(EOD) },
  { POSITION_REPORT,  URL_PATTERN.arg(POSITION_REPORT) },
  { SENSOR_OBS,       URL_PATTERN.arg(SENSOR_OBS) },
  { SITREP,           URL_PATTERN.arg(SITREP) },
  { SPOTREP,          URL_PATTERN.arg(SPOTREP) },
};
}

namespace Dsa::MessageFeeds::Popups {
inline static const QString URL_PATTERN = QStringLiteral(":/Resources/messages/popupDefinitions/%1.json");
inline static const std::unordered_map<QString, QString> SCHEMA_URLS{
  { Types::CURSOR_ON_TARGET, URL_PATTERN.arg(Types::CURSOR_ON_TARGET) },
  { Types::EOD,              URL_PATTERN.arg(Types::EOD) },
  { Types::POSITION_REPORT,  URL_PATTERN.arg(Types::POSITION_REPORT) },
  { Types::SENSOR_OBS,       URL_PATTERN.arg(Types::SENSOR_OBS) },
  { Types::SITREP,           URL_PATTERN.arg(Types::SITREP) },
  { Types::SPOTREP,          URL_PATTERN.arg(Types::SPOTREP) },
};
}

namespace Dsa::MessageFeeds::Parsing::CoT {
inline static const QString ELEMENT_ROOT = QStringLiteral("events");
inline static const QString ELEMENT_MSG  = QStringLiteral("event");
inline static const QString POINT        = QStringLiteral("point");
}
namespace Dsa::MessageFeeds::Parsing::GeoMessage {
inline static const QString ELEMENT_ROOT = QStringLiteral("geomessages");
inline static const QString ELEMENT_MSG  = QStringLiteral("geomessage");
}

namespace Dsa::MessageFeeds::Fields::CoT {
inline static const QString TYPE  = QStringLiteral("type");
inline static const QString UID   = QStringLiteral("uid");
inline static const QString LAT   = QStringLiteral("lat");
inline static const QString LON   = QStringLiteral("lon");
inline static const QString HAE   = QStringLiteral("hae");
inline static const QString HOW   = QStringLiteral("how");
inline static const QString TIME   = QStringLiteral("time");
inline static const QString START   = QStringLiteral("start");
inline static const QString STALE   = QStringLiteral("stale");
}
namespace Dsa::MessageFeeds::Fields::GeoMessage {
inline static const QString TYPE               = QStringLiteral("_type");
inline static const QString ACTION             = QStringLiteral("_action");
inline static const QString ID                 = QStringLiteral("_id");
inline static const QString WKID               = QStringLiteral("_wkid");
inline static const QString SIC                = QStringLiteral("sic");
inline static const QString CONTROL_POINTS     = QStringLiteral("_control_points");
inline static const QString DATETIME_SUBMITTED = QStringLiteral("datetimesubmitted");
inline static const QString UNIQUE_DESIGNATION = QStringLiteral("uniquedesignation");
inline static const QString STATUS_911         = QStringLiteral("status911");
inline static const QString ENVIRONMENT        = QStringLiteral("environment");
inline static const QString EQUIPMENT          = QStringLiteral("equipment");
inline static const QString ACTIVITY           = QStringLiteral("activity");
inline static const QString LOCATION           = QStringLiteral("location");
inline static const QString SIZE               = QStringLiteral("size");
inline static const QString TIME_OBSERVED      = QStringLiteral("timeobserved");
inline static const QString UNIT               = QStringLiteral("unit");
}
namespace Dsa::MessageFeeds::Fields::Common {
inline static const QString SIDC = QStringLiteral("sidc");
}

#endif // MESSAGEFEEDCONSTANTS_H
