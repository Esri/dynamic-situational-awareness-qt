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

#include "HandheldStyles.h"

// PCH header
#include "pch.hpp"

namespace Dsa {
namespace Handheld {

HandheldStyles::HandheldStyles(QObject* parent):
  QObject(parent)
{  
}

HandheldStyles::~HandheldStyles()
{ 
}

QString HandheldStyles::fontFamily() const
{
  return QStringLiteral("helvetica");
}

int HandheldStyles::titleFontPixelSize() const
{
  return 18;
}

int HandheldStyles::secondaryTitleFontPixelSize() const
{
  return 16;
}

int HandheldStyles::primaryIconSize() const
{
  return 30;
}

int HandheldStyles::secondaryIconSize() const
{
  return 30;
}

int HandheldStyles::toolFontPixelSize() const
{
  return 9;
}

int HandheldStyles::mainToolbarHeight() const
{
  return 48;
}

int HandheldStyles::categoryToolbarWidth() const
{
  return 42;
}

} // Handheld
} // Dsa
