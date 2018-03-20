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

#include "pch.hpp"

#include "VehicleStyles.h"

VehicleStyles::VehicleStyles(QObject* parent):
  QObject(parent)
{  
}

VehicleStyles::~VehicleStyles()
{ 
}

QString VehicleStyles::fontFamily() const
{
  return QStringLiteral("helvetica");
}

int VehicleStyles::titleFontPixelSize() const
{
  return 24;
}

int VehicleStyles::primaryIconSize() const
{
  return 42;
}

int VehicleStyles::secondaryIconSize() const
{
  return 38;
}

int VehicleStyles::toolFontPixelSize() const
{
  return 11;
}

int VehicleStyles::mainToolbarHeight() const
{
  return 64;
}

int VehicleStyles::categoryToolbarWidth() const
{
  return 56;
}
