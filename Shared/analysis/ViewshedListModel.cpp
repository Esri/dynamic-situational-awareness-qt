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

/*!
  \class Dsa::ViewshedListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model responsible for storing \l Viewshed360 objects and reporting when they
  change.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li name
        \li QString
        \li The name of the viewshed.
    \row
        \li viewshedVisible
        \li bool
        \li Whether the viewshed is visible.
    \row
        \li minDistance
        \li double
        \li The minimum distance of the Viewshed.
    \row
        \li maxDistance
        \li double
        \li The maximum distance of the Viewshed.
    \row
        \li horizontalAngle
        \li double
        \li The horizontal angle of the Viewshed.
    \row
        \li verticalAngle
        \li double
        \li The vertical angle of the Viewshed.
    \row
        \li heading
        \li double
        \li The heading of the Viewshed.
    \row
        \li pitch
        \li double
        \li The pitch of the Viewshed.
    \row
        \li is360Mode
        \li bool
        \li Whether the Viewshed is in 360 mode.
  \endtable
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
ViewshedListModel::ViewshedListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();

  connect(this, &ViewshedListModel::rowsInserted, this, &ViewshedListModel::countChanged);
  connect(this, &ViewshedListModel::rowsRemoved, this, &ViewshedListModel::countChanged);
}

/*!
  \internal
 */
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

/*!
  \brief Returns whether the model is empty.
 */
bool ViewshedListModel::isEmpty() const
{
  return rowCount() == 0;
}

/*!
  \brief Adds the new \l Viewshed360 \a viewshed to the model.
 */
void ViewshedListModel::append(Viewshed360* viewshed)
{
  if (!viewshed)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_viewsheds.append(viewshed);
  endInsertRows();

  emit viewshedAdded(rowCount() - 1);
}

/*!
  \brief Returns the \l Viewshed360 at \a index in the model.

  Returns \c nullptr if the index is invalid.
 */
Viewshed360* ViewshedListModel::at(int index) const
{
  if (index < 0 || (index + 1) > rowCount())
    return nullptr;

  return m_viewsheds.at(index);
}

/*!
  \brief Returns the index of the \l Viewshed360 \a viewshed in the model.

  Returns -1 if the viewshed is not found.
 */
int ViewshedListModel::indexOf(Viewshed360* viewshed) const
{
  return m_viewsheds.indexOf(viewshed);
}

/*!
  \brief Removes the \l Viewshed360 \a viewshed from the model.

  Returns whether the removal was succesful.
 */
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

/*!
  \brief Removes a \l Viewshed360 from \a index in the model.
 */
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

/*!
  \brief Clears all viewsheds from the model.
 */
void ViewshedListModel::clear()
{
  auto viewsheds = m_viewsheds;

  beginResetModel();
  m_viewsheds.clear();
  endResetModel();

  for (auto viewshed : viewsheds)
    emit viewshedRemoved(viewshed);
}

/*!
  \property ViewshedListModel::count
  \brief Returns the number of rows in the model.
 */
int ViewshedListModel::rowCount(const QModelIndex&) const
{
  return m_viewsheds.count();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l ViewshedRoles enum.
 */
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

/*!
  \brief Sets the data stored under \a role at \a index in the model to \a value.

  The role should make use of the \l ViewshedRoles enum.

  Return \c true if the data was successfully set and \c false otherwise.
 */
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

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l ViewshedRoles enum.
 */
QHash<int, QByteArray> ViewshedListModel::roleNames() const
{
  return m_roles;
}

} // Dsa

// Signal Documentation
/*!
  \fn void ViewshedListModel::countChanged();
  \brief Signal emitted when the number of Viewsheds in the list model changes.
 */

/*!
  \fn void ViewshedListModel::viewshedAdded(int index);
  \brief Signal emitted when a Viewshed is added to the list model at the specified \a index.
 */

/*!
  \fn void ViewshedListModel::viewshedRemoved(Dsa::Viewshed360* viewshed);
  \brief Signal emitted when a \a viewshed is removed from the list model.
 */
