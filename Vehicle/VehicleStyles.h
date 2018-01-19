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

#ifndef VEHICLESTYLES_H
#define VEHICLESTYLES_H

#include <QObject>

class VehicleStyles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
  Q_PROPERTY(int titleFontPixelSize READ titleFontPixelSize CONSTANT)
  Q_PROPERTY(int primaryIconSize READ primaryIconSize CONSTANT)
  Q_PROPERTY(int secondaryIconSize READ secondaryIconSize CONSTANT)
  Q_PROPERTY(int toolFontPixelSize READ toolFontPixelSize CONSTANT)

public:
  VehicleStyles(QObject* parent = nullptr);
  ~VehicleStyles();

private:
  QString fontFamily() const { return "helvetica"; }
  int titleFontPixelSize() const { return 24; }
  int primaryIconSize() const { return 42; }
  int secondaryIconSize() const { return 42; }
  int toolFontPixelSize() const { return 11; }
};

#endif // VEHICLESTYLES_H
