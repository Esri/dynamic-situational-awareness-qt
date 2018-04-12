// Copyright 2018 ESRI
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

#ifndef MESSAGEFEEDCONSTANTS_H
#define MESSAGEFEEDCONSTANTS_H

// Qt headers
#include <QString>

namespace Dsa {

class MessageData;

class MessageFeedConstants
{
public:
  static const QString CONTACT_REPORT_CONFIG_PROPERTYNAME;
  static const QString CONTACT_REPORT_CONFIG_PORT;
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
