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

namespace Esri
{
namespace ArcGISRuntime
{
}
}

class ContactReportController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:

  explicit ContactReportController(QObject* parent = nullptr);
  ~ContactReportController();

  QString toolName() const override;

signals:

private:
};

#endif // CONTACTREPORTCONTROLLER_H
