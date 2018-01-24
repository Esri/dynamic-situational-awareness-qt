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

#ifndef LOCATIONTEXTCONTROLLER_H
#define LOCATIONTEXTCONTROLLER_H

namespace Esri {
namespace ArcGISRuntime {
class Point;
}
}

#include "AbstractTool.h"

class LocationTextController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QString currentLocationText READ currentLocationText NOTIFY currentLocationTextChanged)
  Q_PROPERTY(QString currentElevationText READ currentElevationText NOTIFY currentElevationTextChanged)

public:
  explicit LocationTextController(QObject* parent = nullptr);
  ~LocationTextController();

  QString toolName() const override;

signals:
  void currentLocationTextChanged();
  void currentElevationTextChanged();

private:
  QString currentLocationText() const;
  QString currentElevationText() const;
  void updateLocationText(const Esri::ArcGISRuntime::Point& pt);
  QString m_currentLocationText = "Location Unavailable";
  QString m_currentElevationText = "Elevation Unavailable";
};

#endif // LOCATIONTEXTCONTROLLER_H
