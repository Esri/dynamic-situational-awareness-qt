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

#ifndef MESSAGEFEEDSCONTROLLER_H
#define MESSAGEFEEDSCONTROLLER_H

#include <QObject>
#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class GeoView;
  }
}

class MessageFeedListModel;

class MessageFeedsController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)

public:
  explicit MessageFeedsController(QObject* parent = nullptr);
  ~MessageFeedsController();

  void init(Esri::ArcGISRuntime::GeoView* geoView);

  QAbstractListModel* messageFeeds() const;

private:
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
};

#endif // MESSAGEFEEDSCONTROLLER_H
