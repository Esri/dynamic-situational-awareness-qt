// Copyright 2018 ESRI
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

#ifndef GRAPHICVIEWSHED_H
#define GRAPHICVIEWSHED_H

#include "AbstractViewshed.h"

namespace Esri {
  namespace ArcGISRuntime {
    class Graphic;
  }
}

class GraphicViewshed : public AbstractViewshed
{
  Q_OBJECT

public:
  GraphicViewshed(Esri::ArcGISRuntime::Graphic* locationGraphic, Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                          const QString& headingAttribute, const QString& pitchAttribute, QObject* parent = nullptr);
  ~GraphicViewshed();

  double heading() const override;
  void setHeading(double heading) override;

  double pitch() const override;
  void setPitch(double pitch) override;

  double offsetZ() const;
  void setOffsetZ(double offsetZ);

  QString headingAttribute() const;
  QString pitchAttribute() const;

protected:
  void update360Mode(bool is360Mode) override;

private:
  Q_DISABLE_COPY(GraphicViewshed)
  GraphicViewshed() = delete;

  Esri::ArcGISRuntime::Graphic* m_locationGraphic = nullptr;
  QString m_headingAttribute;
  QString m_pitchAttribute;
};

#endif // GRAPHICVIEWSHED_H
