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

#ifndef VIEWEDALERTSCONTROLLER_H
#define VIEWEDALERTSCONTROLLER_H

// dsa headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QObject>

namespace Dsa {

class AlertListProxyModel;
class StatusAlertFilter;

class ViewedAlertsController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(int unviewedCount READ unviewedCount NOTIFY unviewedCountChanged)

public:
  explicit ViewedAlertsController(QObject* parent = nullptr);
  ~ViewedAlertsController();

  int unviewedCount() const;

  // AbstractTool interface
  QString toolName() const;

signals:
  void unviewedCountChanged();

private slots:
  void handleDataChanged();

private:
  mutable int m_cachedCount = -1;
};

} // Dsa

#endif // VIEWEDALERTSCONTROLLER_H
