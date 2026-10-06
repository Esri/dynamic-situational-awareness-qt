/*******************************************************************************
 *  Copyright 2012-2026 Esri
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

#ifndef CONFIGURATIONCONSTANTS_H
#define CONFIGURATIONCONSTANTS_H

// C++ API
#include "SceneViewTypes.h"
// Qt
#include <QString>

namespace Dsa::ConfigurationConstants
{

  using namespace Esri::ArcGISRuntime;

  inline static const QString SURFACE_PLACEMENT_ABSOLUTE = QStringLiteral("absolute");
  inline static const QString SURFACE_PLACEMENT_DRAPED_BILLBOARDED = QStringLiteral("draped");
  inline static const QString SURFACE_PLACEMENT_DRAPED_FLAT = QStringLiteral("drapedFlat");
  inline static const QString SURFACE_PLACEMENT_RELATIVE = QStringLiteral("relative");

  inline static SurfacePlacement toSurfacePlacement(const QString& surfacePlacement)
  {
    if (surfacePlacement.compare(SURFACE_PLACEMENT_DRAPED_BILLBOARDED, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::DrapedBillboarded;
    }

    if (surfacePlacement.compare(SURFACE_PLACEMENT_ABSOLUTE, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::Absolute;
    }

    if (surfacePlacement.compare(SURFACE_PLACEMENT_DRAPED_FLAT, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::DrapedFlat;
    }

    if (surfacePlacement.compare(SURFACE_PLACEMENT_RELATIVE, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::Relative;
    }

    return SurfacePlacement::DrapedBillboarded;
  }

} // namespace Dsa::ConfigurationConstants

#endif // CONFIGURATIONCONSTANTS_H
