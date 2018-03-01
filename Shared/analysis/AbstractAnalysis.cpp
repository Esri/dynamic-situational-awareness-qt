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

#include "AbstractAnalysis.h"
#include "Analysis.h"
#include "AnalysisOverlay.h"

using namespace Esri::ArcGISRuntime;

AbstractAnalysis::AbstractAnalysis(Analysis* analysis, AnalysisOverlay* analysisOverlay, QObject* parent) :
  QObject(parent),
  m_analysis(analysis),
  m_analysisOverlay(analysisOverlay)
{
}

AbstractAnalysis::~AbstractAnalysis()
{
}

void AbstractAnalysis::removeFromOverlay()
{
  if (!m_analysisOverlay.isNull())
    m_analysisOverlay->analyses()->removeOne(m_analysis);
}

bool AbstractAnalysis::isVisible() const
{
  return m_analysis->isVisible();
}

void AbstractAnalysis::setVisible(bool visible)
{
  if (m_analysis->isVisible() == visible)
    return;

  m_analysis->setVisible(visible);

  emit visibleChanged();
}

QString AbstractAnalysis::name() const
{
  return m_name;
}

void AbstractAnalysis::setName(const QString& name)
{
  if (m_name == name)
    return;

  m_name = name;

  emit nameChanged();
}

Analysis* AbstractAnalysis::analysis() const
{
  return m_analysis;
}

AnalysisOverlay* AbstractAnalysis::analysisOverlay() const
{
  return m_analysisOverlay.data();
}
