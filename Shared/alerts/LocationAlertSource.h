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

#ifndef LOCATIONALERTSOURCE_H
#define LOCATIONALERTSOURCE_H

// example app headers
#include "AlertSource.h"

namespace Dsa {

class LocationAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit LocationAlertSource(QObject* parent = nullptr);
  ~LocationAlertSource();

  Esri::ArcGISRuntime::Point location() const override;
  QVariant value(const QString& key) const override;

  void setSelected(bool selected) override;
private:
  Esri::ArcGISRuntime::Point m_location;

};

} // Dsa

#endif // LOCATIONALERTSOURCE_H
