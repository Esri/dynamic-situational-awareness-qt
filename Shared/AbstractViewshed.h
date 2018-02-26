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

#include <QObject>

namespace Esri {
  namespace ArcGISRuntime {
    class Viewshed;
    class AnalysisOverlay;
  }
}

class AbstractViewshed : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
  Q_PROPERTY(bool is360Mode READ is360Mode WRITE set360Mode NOTIFY is360ModeChanged)

public:
  ~AbstractViewshed();

  virtual bool isVisible() const;
  virtual void setVisible(bool visible);

  virtual QString name() const;
  virtual void setName(const QString& name);

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

  bool is360Mode() const;
  void set360Mode(bool is360Mode);

  Esri::ArcGISRuntime::Viewshed* viewshed() const;

  Esri::ArcGISRuntime::AnalysisOverlay* anlysisOverlay() const;

signals:
  void visibleChanged();
  void nameChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();
  void is360ModeChanged();

protected:
  explicit AbstractViewshed(Esri::ArcGISRuntime::Viewshed* viewshed,
                            Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay, QObject* parent = nullptr);

  virtual void update360Mode(bool is360Mode) = 0;

  Esri::ArcGISRuntime::Viewshed* m_viewshed = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  QList<Esri::ArcGISRuntime::Viewshed*> m_viewsheds360Offsets;

private:
  Q_DISABLE_COPY(AbstractViewshed)

  QString m_name;
  bool m_is360Mode = false;
};

#endif // ABSTRACTVIEWSHED_H
