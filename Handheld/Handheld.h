// Copyright 2016 ESRI
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

#ifndef HANDHELD_H
#define HANDHELD_H

#include <QQuickItem>

class DsaController;

class Handheld : public QQuickItem
{
  Q_OBJECT

public:
  Handheld(QQuickItem* parent = nullptr);
  ~Handheld();

  void componentComplete() override;

private:
  DsaController*                          m_controller = nullptr;
};

#endif // HANDHELD_H
