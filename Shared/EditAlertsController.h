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

#ifndef EDITALERTSCONTROLLER_H
#define EDITALERTSCONTROLLER_H

#include "AbstractTool.h"

#include <QStringListModel>

class QStringList;

class EditAlertsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* layerNames READ layerNames NOTIFY layerNamesChanged)
  Q_PROPERTY(QAbstractItemModel* statusNames READ statusNames CONSTANT)

public:
  explicit EditAlertsController(QObject* parent = nullptr);
  ~EditAlertsController();

  // AbstractTool interface
  QString toolName() const override;

  Q_INVOKABLE void addWithinDistanceAlert(int statusIndex, int sourceLayerIndex, double distance, int itemId, int targetLayerIndex);

  QAbstractItemModel* layerNames() const;
  QAbstractItemModel* statusNames() const;

signals:
  void layerNamesChanged();

private slots:
  void onGeoviewChanged();
  void onLayersChanged();

private:
  void setLayerNames(const QStringList& layerNames);

  QStringListModel* m_layerNames;
  QStringListModel* m_statusNames;
};

#endif // EDITALERTSCONTROLLER_H
