/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "HandheldStyles.h"

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
