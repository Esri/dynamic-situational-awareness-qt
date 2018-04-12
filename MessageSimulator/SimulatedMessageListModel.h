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
