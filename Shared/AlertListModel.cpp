// Copyright 2016 ESRI
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

#include "AbstractAlert.h"
#include "AlertListModel.h"

#include <QUuid>

using namespace Esri::ArcGISRuntime;

AlertListModel* AlertListModel::instance()
{
  static AlertListModel s_instance;

  return &s_instance;
}

AlertListModel::AlertListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[AlertListRoles::AlertId] = "alertId";
  m_roles[AlertListRoles::Message] = "message";
  m_roles[AlertListRoles::Status] = "status";
  m_roles[AlertListRoles::Position] = "position";
}

AlertListModel::~AlertListModel()
{

}

bool AlertListModel::addAlert(AbstractAlert* alert)
{
  if (!alert)
    return false;

  const int size = m_alerts.size();
  const QUuid id = QUuid::createUuid();
  alert->setId(id);

  beginInsertRows(QModelIndex(), size, size);
  m_alerts.append(alert);
  endInsertRows();

  return true;
}

AbstractAlert* AlertListModel::alertAt(int rowIndex) const
{
  return m_alerts.value(rowIndex, nullptr);
}

int AlertListModel::rowCount(const QModelIndex&) const
{
  return m_alerts.size();
}

QVariant AlertListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  AbstractAlert* alert = m_alerts.at(index.row());
  if (!alert)
    return QVariant();

  switch (role)
  {
  case AlertListRoles::AlertId:
    return alert->id();
    break;
  case AlertListRoles::Status:
    return QVariant::fromValue(alert->status());
    break;
  case AlertListRoles::Message:
  {
    return alert->message();
    break;
  }
  case AlertListRoles::Position:
  {
    return QVariant::fromValue(alert->position());
    break;
  }
  default:
    break;
  }

  return QVariant();
}

QHash<int, QByteArray> AlertListModel::roleNames() const
{
  return m_roles;
}
