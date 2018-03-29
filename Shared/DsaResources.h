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

#ifndef DSARESOURCES_H
#define DSARESOURCES_H

#include <QObject>
#include <QUrl>

class DsaResources : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QUrl icon2d READ icon2d CONSTANT)
  Q_PROPERTY(QUrl icon3d READ icon3d CONSTANT)
  Q_PROPERTY(QUrl iconAboutMap READ iconAboutMap CONSTANT)
  Q_PROPERTY(QUrl iconAdd READ iconAdd CONSTANT)
  Q_PROPERTY(QUrl iconAddLayer READ iconAddLayer CONSTANT)
  Q_PROPERTY(QUrl iconAlert READ iconAlert CONSTANT)
  Q_PROPERTY(QUrl iconAlertConditionList READ iconAlertConditionList CONSTANT)
  Q_PROPERTY(QUrl iconAlertList READ iconAlertList CONSTANT)
  Q_PROPERTY(QUrl iconAppLogo READ iconAppLogo CONSTANT)
  Q_PROPERTY(QUrl iconChooseBasemap READ iconChooseBasemap CONSTANT)
  Q_PROPERTY(QUrl iconClose READ iconClose CONSTANT)
  Q_PROPERTY(QUrl iconColorPalette READ iconColorPalette CONSTANT)
  Q_PROPERTY(QUrl iconComplete READ iconComplete CONSTANT)
  Q_PROPERTY(QUrl iconCoordinateConversion READ iconCoordinateConversion CONSTANT)
  Q_PROPERTY(QUrl iconCreateReport READ iconCreateReport CONSTANT)
  Q_PROPERTY(QUrl iconCurrentLocation READ iconCurrentLocation CONSTANT)
  Q_PROPERTY(QUrl iconDistressGray READ iconDistressGray CONSTANT)
  Q_PROPERTY(QUrl iconDistressRed READ iconDistressRed CONSTANT)
  Q_PROPERTY(QUrl iconDraw READ iconDraw CONSTANT)
  Q_PROPERTY(QUrl iconDrawer READ iconDrawer CONSTANT)
  Q_PROPERTY(QUrl iconDsaHome READ iconDsaHome CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocation READ iconFollowLocation CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocationNorthUp READ iconFollowLocationNorthUp CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocationOff READ iconFollowLocationOff CONSTANT)
  Q_PROPERTY(QUrl iconGps READ iconGps CONSTANT)
  Q_PROPERTY(QUrl iconGpsOff READ iconGpsOff CONSTANT)
  Q_PROPERTY(QUrl iconHome READ iconHome CONSTANT)
  Q_PROPERTY(QUrl iconLayers READ iconLayers CONSTANT)
  Q_PROPERTY(QUrl iconLineWidth READ iconLineWidth CONSTANT)
  Q_PROPERTY(QUrl iconListView READ iconListView CONSTANT)
  Q_PROPERTY(QUrl iconMap READ iconMap CONSTANT)
  Q_PROPERTY(QUrl iconMenu READ iconMenu CONSTANT)
  Q_PROPERTY(QUrl iconMessages READ iconMessages CONSTANT)
  Q_PROPERTY(QUrl iconPin READ iconPin CONSTANT)
  Q_PROPERTY(QUrl iconPoint READ iconPoint CONSTANT)
  Q_PROPERTY(QUrl iconPolygon READ iconPolygon CONSTANT)
  Q_PROPERTY(QUrl iconPolyline READ iconPolyline CONSTANT)
  Q_PROPERTY(QUrl iconRaster READ iconRaster CONSTANT)
  Q_PROPERTY(QUrl iconRemove READ iconRemove CONSTANT)
  Q_PROPERTY(QUrl iconReport READ iconReport CONSTANT)
  Q_PROPERTY(QUrl iconRotate READ iconRotate CONSTANT)
  Q_PROPERTY(QUrl iconSave READ iconSave CONSTANT)
  Q_PROPERTY(QUrl iconSendMap READ iconSendMap CONSTANT)
  Q_PROPERTY(QUrl iconSettings READ iconSettings CONSTANT)
  Q_PROPERTY(QUrl iconSketch READ iconSketch CONSTANT)
  Q_PROPERTY(QUrl iconTools READ iconTools CONSTANT)
  Q_PROPERTY(QUrl iconTouch READ iconTouch CONSTANT)
  Q_PROPERTY(QUrl iconTrash READ iconTrash CONSTANT)
  Q_PROPERTY(QUrl iconViewshed READ iconViewshed CONSTANT)
  Q_PROPERTY(QUrl iconZoomTo READ iconZoomTo CONSTANT)
  Q_PROPERTY(QUrl iconWarningGreen READ iconWarningGreen CONSTANT)
  Q_PROPERTY(QUrl iconWarningOrange READ iconWarningOrange CONSTANT)
  Q_PROPERTY(QUrl iconWarningRed READ iconWarningRed CONSTANT)
  Q_PROPERTY(QUrl iconWarningRedExclamation READ iconWarningRedExclamation CONSTANT)
  Q_PROPERTY(QUrl iconLineOfSight READ iconLineOfSight CONSTANT)

public:
  DsaResources(QObject* parent = nullptr);
  ~DsaResources();

private:
  QUrl icon2d() const;
  QUrl icon3d() const;
  QUrl iconAboutMap() const;
  QUrl iconAdd() const;
  QUrl iconAddLayer() const;
  QUrl iconAlert() const;
  QUrl iconAlertConditionList() const;
  QUrl iconAlertList() const;
  QUrl iconAppLogo() const;
  QUrl iconChooseBasemap() const;
  QUrl iconClose() const;
  QUrl iconColorPalette() const;
  QUrl iconComplete() const;
  QUrl iconCoordinateConversion() const;
  QUrl iconCreateReport() const;
  QUrl iconCurrentLocation() const;
  QUrl iconDistressGray() const;
  QUrl iconDistressRed() const;
  QUrl iconDraw() const;
  QUrl iconDrawer() const;
  QUrl iconDsaHome() const;
  QUrl iconFollowLocation() const;
  QUrl iconFollowLocationNorthUp() const;
  QUrl iconFollowLocationOff() const;
  QUrl iconGps() const;
  QUrl iconGpsOff() const;
  QUrl iconHome() const;
  QUrl iconLayers() const;
  QUrl iconLineWidth() const;
  QUrl iconListView() const;
  QUrl iconMap() const;
  QUrl iconMenu() const;
  QUrl iconMessages() const;
  QUrl iconPin() const;
  QUrl iconPoint() const;
  QUrl iconPolygon() const;
  QUrl iconPolyline() const;
  QUrl iconRaster() const;
  QUrl iconRemove() const;
  QUrl iconReport() const;
  QUrl iconRotate() const;
  QUrl iconSave() const;
  QUrl iconSendMap() const;
  QUrl iconSettings() const;
  QUrl iconSketch() const;
  QUrl iconTools() const;
  QUrl iconTouch() const;
  QUrl iconTrash() const;
  QUrl iconViewshed() const;
  QUrl iconZoomTo() const;
  QUrl iconWarningGreen() const;
  QUrl iconWarningOrange() const;
  QUrl iconWarningRed() const;
  QUrl iconWarningRedExclamation() const;
  QUrl iconLineOfSight() const;
};

#endif // DSARESOURCES_H
