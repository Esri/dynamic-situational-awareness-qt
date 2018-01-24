// Copyright 2017 ESRI
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

#ifndef ABSTRACTALERTRULE_H
#define ABSTRACTALERTRULE_H

#include <QObject>

class AlertConditionData;

class AbstractAlertRule : public QObject
{
  Q_OBJECT

public:
  explicit AbstractAlertRule(QObject* parent = nullptr);
  ~AbstractAlertRule();

  virtual bool matchesRule(AlertConditionData* alert) const = 0;

  bool active() const;
  void setActive(bool active);

private:
  bool m_active = false;
};

#endif // ABSTRACTALERTRULE_H
