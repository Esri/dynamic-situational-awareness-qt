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


// Qt headers
#include <QObject>

namespace Dsa {
namespace Vehicle {

class VehicleStyles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
  Q_PROPERTY(int titleFontPixelSize READ titleFontPixelSize CONSTANT)
  Q_PROPERTY(int secondaryTitleFontPixelSize READ secondaryTitleFontPixelSize CONSTANT)
  Q_PROPERTY(int primaryIconSize READ primaryIconSize CONSTANT)
  Q_PROPERTY(int secondaryIconSize READ secondaryIconSize CONSTANT)
  Q_PROPERTY(int toolFontPixelSize READ toolFontPixelSize CONSTANT)
  Q_PROPERTY(int mainToolbarHeight READ mainToolbarHeight CONSTANT)
  Q_PROPERTY(int categoryToolbarWidth READ categoryToolbarWidth CONSTANT)

public:
  VehicleStyles(QObject* parent = nullptr);
  ~VehicleStyles();

private:
  QString fontFamily() const;
  int titleFontPixelSize() const;
  int secondaryTitleFontPixelSize() const;
  int primaryIconSize() const;
  int secondaryIconSize() const;
  int toolFontPixelSize() const;
  int mainToolbarHeight() const;
  int categoryToolbarWidth() const;
};

} // Vehicle
} // Dsa

#endif // VEHICLESTYLES_H
