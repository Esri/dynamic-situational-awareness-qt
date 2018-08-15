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

class MessageData;

class MessageFeedConstants
{
public:
  static const QString OBSERVATION_REPORT_CONFIG_PROPERTYNAME;
  static const QString OBSERVATION_REPORT_CONFIG_PORT;
  static const QString LOCATION_BROADCAST_CONFIG_PROPERTYNAME;
  static const QString LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE;
  static const QString LOCATION_BROADCAST_CONFIG_PORT;
  static const QString MESSAGE_FEEDS_PROPERTYNAME;
  static const QString MESSAGE_FEEDS_NAME;
  static const QString MESSAGE_FEEDS_TYPE;
  static const QString MESSAGE_FEEDS_RENDERER;
  static const QString MESSAGE_FEEDS_THUMBNAIL;
  static const QString MESSAGE_FEEDS_PLACEMENT;
  static const QString MESSAGE_FEED_UDP_PORTS_PROPERTYNAME;
};

} // Dsa

#endif // MESSAGEFEEDCONSTANTS_H
