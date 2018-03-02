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

#ifndef VIEWSHED360_H
#define VIEWSHED360_H

#include <QObject>
#include <QPointer>

namespace Esri {
  namespace ArcGISRuntime {
    class Viewshed;
    class AnalysisOverlay;
  }
}

class Viewshed360 : public QObject
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
  Q_PROPERTY(double offsetZ READ offsetZ WRITE setOffsetZ NOTIFY offsetZChanged)
  Q_PROPERTY(bool headingEnabled READ isHeadingEnabled NOTIFY headingEnabledChanged)
  Q_PROPERTY(bool pitchEnabled READ isPitchEnabled NOTIFY pitchEnabledChanged)
  Q_PROPERTY(bool offsetZEnabled READ isOffsetZEnabled NOTIFY offsetZEnabledChanged)
  Q_PROPERTY(bool is360Mode READ is360Mode WRITE set360Mode NOTIFY is360ModeChanged)

public:
  ~Viewshed360();

  void removeFromOverlay();

  bool isVisible() const;
  virtual void setVisible(bool visible);

  QString name() const;
  void setName(const QString& name);

  double minDistance() const;
  void setMinDistance(double minDistance);

  double maxDistance() const;
  void setMaxDistance(double maxDistance);

  double horizontalAngle() const;
  void setHorizontalAngle(double horizontalAngle);

  double verticalAngle() const;
  void setVerticalAngle(double verticalAngle);

  virtual double heading() const = 0;
  virtual void setHeading(double heading) = 0;

  virtual double pitch() const = 0;
  virtual void setPitch(double pitch) = 0;

  virtual double offsetZ() const;
  virtual void setOffsetZ(double offsetZ);

  virtual bool isHeadingEnabled() const;
  virtual bool isPitchEnabled() const;
  virtual bool isOffsetZEnabled() const;

  bool is360Mode() const;
  void set360Mode(bool is360Mode);

  Esri::ArcGISRuntime::Viewshed* viewshed() const;

  Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay() const;

signals:
  void visibleChanged();
  void nameChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();
  void offsetZChanged();
  void headingEnabledChanged();
  void pitchEnabledChanged();
  void offsetZEnabledChanged();
  void is360ModeChanged();

protected:
  Viewshed360(Esri::ArcGISRuntime::Viewshed* viewshed,
                   Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                   QObject* parent = nullptr);

  virtual void update360Mode(bool is360Mode) = 0;

  QList<Esri::ArcGISRuntime::Viewshed*> m_viewsheds360Offsets;

private:
  Q_DISABLE_COPY(Viewshed360)
  Viewshed360() = delete;

  QPointer<Esri::ArcGISRuntime::Viewshed> m_viewshed;
  QPointer<Esri::ArcGISRuntime::AnalysisOverlay> m_analysisOverlay;

  QString m_name;
  bool m_is360Mode = true;
};

#endif // VIEWSHED360_H
