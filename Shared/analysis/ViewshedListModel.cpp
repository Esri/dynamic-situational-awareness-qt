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

// PCH header
#include "pch.hpp"

#include "ViewshedListModel.h"

// example app headers
#include "Viewshed360.h"

// C++ API headers
#include "AnalysisOverlay.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {
namespace Analysis {

ViewshedListModel::ViewshedListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();

  connect(this, &ViewshedListModel::rowsInserted, this, &ViewshedListModel::countChanged);
  connect(this, &ViewshedListModel::rowsRemoved, this, &ViewshedListModel::countChanged);
}

void ViewshedListModel::setupRoles()
{
  m_roles[ViewshedNameRole] = "name";
  m_roles[ViewshedVisibleRole] = "viewshedVisible";
  m_roles[ViewshedMinDistanceRole] = "minDistance";
  m_roles[ViewshedMaxDistanceRole] = "maxDistance";
  m_roles[ViewshedHorizontalAngleRole] = "horizontalAngle";
  m_roles[ViewshedVerticalAngleRole] = "verticalAngle";
  m_roles[ViewshedHeadingRole] = "heading";
  m_roles[ViewshedPitchRole] = "pitch";
  m_roles[Viewshed360ModeRole] = "is360Mode";
}

bool ViewshedListModel::isEmpty() const
{
  return rowCount() == 0;
}

void ViewshedListModel::append(Viewshed360* viewshed)
{
  if (!viewshed)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_viewsheds.append(viewshed);
  endInsertRows();

  emit viewshedAdded(rowCount() - 1);
}

Viewshed360* ViewshedListModel::at(int index) const
{
  if (index < 0 || (index + 1) > rowCount())
    return nullptr;

  return m_viewsheds.at(index);
}

int ViewshedListModel::indexOf(Viewshed360* viewshed) const
{
  return m_viewsheds.indexOf(viewshed);
}

bool ViewshedListModel::removeOne(Viewshed360* viewshed)
{
  bool ret = false;

  const int index = indexOf(viewshed);
  if (index != -1)
  {
    beginRemoveRows(QModelIndex(), index, index);
    ret = m_viewsheds.removeOne(viewshed);
    endRemoveRows();

    if (ret)
      emit viewshedRemoved(viewshed);
  }

  return ret;
}

void ViewshedListModel::removeAt(int index)
{
  if (index < 0 || (index + 1) > rowCount())
    return;

  auto viewshed = at(index);
  if (viewshed)
  {
    beginRemoveRows(QModelIndex(), index, index);
    m_viewsheds.removeAt(index);
    endRemoveRows();

    emit viewshedRemoved(viewshed);
  }
}

void ViewshedListModel::clear()
{
  auto viewsheds = m_viewsheds;

  beginResetModel();
  m_viewsheds.clear();
  endResetModel();

  for (auto viewshed : viewsheds)
    emit viewshedRemoved(viewshed);
}

int ViewshedListModel::rowCount(const QModelIndex&) const
{
  return m_viewsheds.count();
}

QVariant ViewshedListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount())
    return QVariant();

  QVariant retVal;

  Viewshed360* viewshed = at(index.row());
  if (!viewshed)
    return retVal;

  switch (role)
  {
  case ViewshedNameRole:
    retVal = viewshed->name();
    break;
  case ViewshedVisibleRole:
    retVal = viewshed->isVisible();
    break;
  case ViewshedMinDistanceRole:
    retVal = viewshed->minDistance();
    break;
  case ViewshedMaxDistanceRole:
    retVal = viewshed->maxDistance();
    break;
  case ViewshedHorizontalAngleRole:
    retVal = viewshed->horizontalAngle();
    break;
  case ViewshedVerticalAngleRole:
    retVal = viewshed->verticalAngle();
    break;
  case ViewshedHeadingRole:
    retVal = viewshed->heading();
    break;
  case ViewshedPitchRole:
    retVal = viewshed->pitch();
    break;
  case Viewshed360ModeRole:
    retVal = viewshed->is360Mode();
    break;
  default:
    break;
  }

  return retVal;
}

bool ViewshedListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  Viewshed360* viewshed = at(index.row());
  if (!viewshed)
    return false;

  bool isDataChanged = false;

  switch (role)
  {
  case ViewshedNameRole:
  {
    const auto val = value.toString();
    if (viewshed->name() != val)
    {
      viewshed->setName(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedVisibleRole:
  {
    const auto val = value.toBool();
    if (viewshed->isVisible() != val)
    {
      viewshed->setVisible(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedMinDistanceRole:
  {
    const auto val = value.toDouble();
    if (viewshed->minDistance() != val)
    {
      viewshed->setMinDistance(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedMaxDistanceRole:
  {
    const auto val = value.toDouble();
    if (viewshed->maxDistance() != val)
    {
      viewshed->setMaxDistance(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedHorizontalAngleRole:
  {
    const auto val = value.toDouble();
    if (viewshed->horizontalAngle() != val)
    {
      viewshed->setHorizontalAngle(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedVerticalAngleRole:
  {
    const auto val = value.toDouble();
    if (viewshed->verticalAngle() != val)
    {
      viewshed->setVerticalAngle(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedHeadingRole:
  {
    const auto val = value.toDouble();
    if (viewshed->heading() != val)
    {
      viewshed->setHeading(val);

      isDataChanged = true;
    }
    break;
  }
  case ViewshedPitchRole:
  {
    const auto val = value.toDouble();
    if (viewshed->pitch() != val)
    {
      viewshed->setPitch(val);

      isDataChanged = true;
    }
    break;
  }
  case Viewshed360ModeRole:
  {
    const auto val = value.toBool();
    if (viewshed->is360Mode() != val)
    {
      viewshed->set360Mode(val);

      isDataChanged = true;
    }
    break;
  }
  default:
    break;
  }

  if (isDataChanged)
    emit dataChanged(index, index, QVector<int>{role});

  return isDataChanged;
}

QHash<int, QByteArray> ViewshedListModel::roleNames() const
{
  return m_roles;
}

} // Analysis
} // Dsa
