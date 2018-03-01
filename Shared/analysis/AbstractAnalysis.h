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

#ifndef ABSTRACTANALYSIS_H
#define ABSTRACTANALYSIS_H

#include <QObject>
#include <QPointer>

namespace Esri {
  namespace ArcGISRuntime {
    class Analysis;
    class AnalysisOverlay;
  }
}

class AbstractAnalysis : public QObject
{
  Q_OBJECT

  Q_PROPERTY(AnalysisType analysisType READ analysisType CONSTANT)
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
  enum AnalysisType
  {
    NoType = 0,
    PointViewshed,
    GraphicViewshed
  };

  Q_ENUM(AnalysisType)

  ~AbstractAnalysis();

  virtual void removeFromOverlay();

  virtual AnalysisType analysisType() const = 0;

  virtual bool isVisible() const;
  virtual void setVisible(bool visible);

  virtual QString name() const;
  virtual void setName(const QString& name);

  Esri::ArcGISRuntime::Analysis* analysis() const;

  Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay() const;

signals:
  void visibleChanged();
  void nameChanged();

protected:
  AbstractAnalysis(Esri::ArcGISRuntime::Analysis* analysis,
                   Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                   QObject* parent = nullptr);

  Esri::ArcGISRuntime::Analysis* m_analysis = nullptr;
  QPointer<Esri::ArcGISRuntime::AnalysisOverlay> m_analysisOverlay;

private:
  Q_DISABLE_COPY(AbstractAnalysis)
  AbstractAnalysis() = delete;

  QString m_name;
};

#endif // ABSTRACTANALYSIS_H
