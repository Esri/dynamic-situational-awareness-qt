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

#ifndef CONTACTREPORTCONTROLLER_H
#define CONTACTREPORTCONTROLLER_H


// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "Point.h"

namespace Esri
{
namespace ArcGISRuntime
{
}
}

class ContactReportController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QString unitName READ unitName NOTIFY unitNameChanged)
  Q_PROPERTY(QString controlPoint READ controlPoint NOTIFY controlPointChanged)

public:

  explicit ContactReportController(QObject* parent = nullptr);
  ~ContactReportController();

  QString toolName() const override;

  void setProperties(const QVariantMap& properties) override;

  QString unitName() const;

  QString controlPoint() const;

signals:
  void unitNameChanged();
  void controlPointChanged();

private:
  void setUnitName(const QString& unitName);
  void setControlPoint(const Esri::ArcGISRuntime::Point& controlPoint);

  QString m_unitName;
  Esri::ArcGISRuntime::Point m_controlPoint;
};

#endif // CONTACTREPORTCONTROLLER_H
