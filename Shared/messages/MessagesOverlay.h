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

#ifndef MESSAGESOVERLAY_H
#define MESSAGESOVERLAY_H

// C++ API headers
#include "DynamicEntityLayer.h"

namespace Esri::ArcGISRuntime {
class DynamicEntity;
}

namespace Dsa {

class MessageFeed;

class MessagesOverlay : public Esri::ArcGISRuntime::DynamicEntityLayer
{
  Q_OBJECT
public:
  explicit MessagesOverlay(MessageFeed* messageFeed, const QString& messageType, QObject* parent = nullptr);
  ~MessagesOverlay();

  QString messageType() const;
  void setMessageType(const QString& messageType);
  Esri::ArcGISRuntime::DynamicEntity* getDynamicEntityById(quint64 entityId);
  const QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*>& dynamicEntities();

private:
  Q_DISABLE_COPY(MessagesOverlay)
  MessageFeed* m_messageFeed = nullptr;
};

} // Dsa

#endif // MESSAGESOVERLAY_H
