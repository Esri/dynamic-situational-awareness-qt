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
    if (surfacePlacement.compare(SURFACE_PLACEMENT_ABSOLUTE, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::Absolute;
    }

    if (surfacePlacement.compare(SURFACE_PLACEMENT_RELATIVE, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::Relative;
    }

    if (surfacePlacement.compare(SURFACE_PLACEMENT_DRAPED_FLAT, Qt::CaseInsensitive) == 0)
    {
      return SurfacePlacement::DrapedFlat;
    }

    return SurfacePlacement::DrapedBillboarded;
  }

} // namespace Dsa::ConfigurationConstants

#endif // CONFIGURATIONCONSTANTS_H
