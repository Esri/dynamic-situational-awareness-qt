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

#ifndef ABSTRACTVIEWSHED_H
#define ABSTRACTVIEWSHED_H

#include "AbstractAnalysis.h"

namespace Esri {
  namespace ArcGISRuntime {
    class Viewshed;
  }
}

class AbstractViewshed : public AbstractAnalysis
{
  Q_OBJECT

  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
  Q_PROPERTY(bool headingEnabled READ isHeadingEnabled NOTIFY headingEnabledChanged)
  Q_PROPERTY(bool pitchEnabled READ isPitchEnabled NOTIFY pitchEnabledChanged)
  Q_PROPERTY(bool is360Mode READ is360Mode WRITE set360Mode NOTIFY is360ModeChanged)
  Q_PROPERTY(double offsetZ READ offsetZ WRITE setOffsetZ NOTIFY offsetZChanged)

public:
  ~AbstractViewshed();

  void removeFromOverlay() override;

  virtual void setVisible(bool visible) override;

  virtual double minDistance() const;
  virtual void setMinDistance(double minDistance);

  virtual double maxDistance() const;
  virtual void setMaxDistance(double maxDistance);

  virtual double horizontalAngle() const;
  virtual void setHorizontalAngle(double horizontalAngle);

  virtual double verticalAngle() const;
  virtual void setVerticalAngle(double verticalAngle);

  virtual double heading() const = 0;
  virtual void setHeading(double heading) = 0;

  virtual double pitch() const = 0;
  virtual void setPitch(double pitch) = 0;

  virtual bool isHeadingEnabled() const;
  virtual bool isPitchEnabled() const;

  bool is360Mode() const;
  void set360Mode(bool is360Mode);

  virtual double offsetZ() const;
  virtual void setOffsetZ(double offsetZ);

  Esri::ArcGISRuntime::Viewshed* viewshed() const;

signals:
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();
  void headingEnabledChanged();
  void pitchEnabledChanged();
  void is360ModeChanged();
  void offsetZChanged();

protected:
  AbstractViewshed(Esri::ArcGISRuntime::Viewshed* viewshed,
                   Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                   QObject* parent = nullptr);

  virtual void update360Mode(bool is360Mode) = 0;

  QList<Esri::ArcGISRuntime::Viewshed*> m_viewsheds360Offsets;

private:
  Q_DISABLE_COPY(AbstractViewshed)
  AbstractViewshed() = delete;

  bool m_is360Mode = false;
};

#endif // ABSTRACTVIEWSHED_H
