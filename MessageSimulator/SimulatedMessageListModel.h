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

#ifndef SIMULATEDMESSAGELISTMODEL_H
#define SIMULATEDMESSAGELISTMODEL_H

#include <QAbstractListModel>

class SimulatedMessage;

class SimulatedMessageListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum SimulatedMessageRoles
  {
    FormatRole = Qt::UserRole + 1,
    IdRole = Qt::UserRole + 2,
    ActionRole = Qt::UserRole + 3,
    SymbolIdRole = Qt::UserRole + 4
  };

  explicit SimulatedMessageListModel(QObject* parent = nullptr);
  ~SimulatedMessageListModel();

  void append(SimulatedMessage* message);

  void clear();

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  Q_DISABLE_COPY(SimulatedMessageListModel)

  void setupRoles();

  QHash<int, QByteArray> m_roles;
  QList<SimulatedMessage*> m_messages;
};

#endif // SIMULATEDMESSAGELISTMODEL_H
