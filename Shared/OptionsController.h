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

#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

class ArcGISCompassController;

#include "AbstractTool.h"
#include <QObject>

class OptionsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  QString toolName() const override;

private:
  void getUpdatedTools();
};

#endif // OPTIONSCONTROLLER_H