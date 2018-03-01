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
  GraphicViewshed(Esri::ArcGISRuntime::Graphic* graphic, Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                  const QString& headingAttribute, const QString& pitchAttribute, QObject* parent = nullptr);
  ~GraphicViewshed();

  Esri::ArcGISRuntime::Graphic* graphic() const;

  double heading() const override;
  void setHeading(double heading) override;

  double pitch() const override;
  void setPitch(double pitch) override;

  bool isHeadingEnabled() const override;
  bool isPitchEnabled() const override;

  double offsetZ() const override;
  void setOffsetZ(double offsetZ) override;

  QString headingAttribute() const;
  QString pitchAttribute() const;

  AnalysisType analysisType() const override;

protected:
  void update360Mode(bool is360Mode) override;

private:
  Q_DISABLE_COPY(GraphicViewshed)
  GraphicViewshed() = delete;

  QPointer<Esri::ArcGISRuntime::Graphic> m_graphic;
  QString m_headingAttribute;
  QString m_pitchAttribute;
};

#endif // GRAPHICVIEWSHED_H
