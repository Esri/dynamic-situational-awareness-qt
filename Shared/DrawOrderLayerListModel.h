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

#ifndef DRAWORDERLAYERLISTMODEL_H
#define DRAWORDERLAYERLISTMODEL_H

// Qt headers
#include <QSortFilterProxyModel>

namespace Dsa {

class DrawOrderLayerListModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit DrawOrderLayerListModel(QObject* parent = nullptr);
  ~DrawOrderLayerListModel();

protected:
  bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
};

} // Dsa

#endif // DRAWORDERLAYERLISTMODEL_H
