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

#include "DrawOrderLayerListModel.h"

namespace Dsa {

/*!
  \class DrawOrderLayerListModel
  \inherits QSortFilterProxyModel
  \brief A proxy model responsible for presenting layers in the
  app in their draw order. The top layer is first in the list.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */

DrawOrderLayerListModel::DrawOrderLayerListModel(QObject* parent):
  QSortFilterProxyModel(parent)
{
  sort(0);
}

/*!
  \brief Destructor.
 */
DrawOrderLayerListModel::~DrawOrderLayerListModel()
{

}

/*!
  \brief Returns whether \a sourceLeft should be listed before \a sourceRight.
 */
bool DrawOrderLayerListModel::lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const
{
  return sourceLeft.row() > sourceRight.row();
}

} // Dsa
