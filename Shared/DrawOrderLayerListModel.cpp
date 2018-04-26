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

// PCH header
#include "pch.hpp"

#include "DrawOrderLayerListModel.h"

namespace Dsa {

/*!
  \class Dsa::DrawOrderLayerListModel
  \inmodule Dsa
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
