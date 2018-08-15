
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

#include "VehicleStyles.h"

namespace Dsa {
namespace Vehicle {

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

int VehicleStyles::secondaryTitleFontPixelSize() const
{
  return 16;
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

} // Vehicle
} // Dsa
