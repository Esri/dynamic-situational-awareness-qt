// Copyright 2017 ESRI
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

DrawOrderLayerListModel::DrawOrderLayerListModel(QObject* parent):
  QSortFilterProxyModel(parent)
{
  sort(0);
}

DrawOrderLayerListModel::~DrawOrderLayerListModel()
{

}

bool DrawOrderLayerListModel::lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const
{
  return sourceLeft.row() > sourceRight.row();
}

} // Dsa
