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
  Q_PROPERTY(QUrl iconAppLogo READ iconAppLogo CONSTANT)
  Q_PROPERTY(QUrl iconChooseBasemap READ iconChooseBasemap CONSTANT)
  Q_PROPERTY(QUrl iconClose READ iconClose CONSTANT)
  Q_PROPERTY(QUrl iconComplete READ iconComplete CONSTANT)
  Q_PROPERTY(QUrl iconCoordinateConversion READ iconCoordinateConversion CONSTANT)
  Q_PROPERTY(QUrl iconCreateReport READ iconCreateReport CONSTANT)
  Q_PROPERTY(QUrl iconDraw READ iconDraw CONSTANT)
  Q_PROPERTY(QUrl iconDrawer READ iconDrawer CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocation READ iconFollowLocation CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocationNorthUp READ iconFollowLocationNorthUp CONSTANT)
  Q_PROPERTY(QUrl iconFollowLocationOff READ iconFollowLocationOff CONSTANT)
  Q_PROPERTY(QUrl iconGps READ iconGps CONSTANT)
  Q_PROPERTY(QUrl iconHome READ iconHome CONSTANT)
  Q_PROPERTY(QUrl iconLayers READ iconLayers CONSTANT)
  Q_PROPERTY(QUrl iconListView READ iconListView CONSTANT)
  Q_PROPERTY(QUrl iconMap READ iconMap CONSTANT)
  Q_PROPERTY(QUrl iconMessages READ iconMessages CONSTANT)
  Q_PROPERTY(QUrl iconPoint READ iconPoint CONSTANT)
  Q_PROPERTY(QUrl iconPolygon READ iconPolygon CONSTANT)
  Q_PROPERTY(QUrl iconPolyline READ iconPolyline CONSTANT)
  Q_PROPERTY(QUrl iconRaster READ iconRaster CONSTANT)
  Q_PROPERTY(QUrl iconRemove READ iconRemove CONSTANT)
  Q_PROPERTY(QUrl iconRotate READ iconRotate CONSTANT)
  Q_PROPERTY(QUrl iconSendMap READ iconSendMap CONSTANT)
  Q_PROPERTY(QUrl iconSettings READ iconSettings CONSTANT)
  Q_PROPERTY(QUrl iconSketch READ iconSketch CONSTANT)
  Q_PROPERTY(QUrl iconTools READ iconTools CONSTANT)
  Q_PROPERTY(QUrl iconTrash READ iconTrash CONSTANT)
  Q_PROPERTY(QUrl iconViewshed READ iconViewshed CONSTANT)
  Q_PROPERTY(QUrl iconZoomTo READ iconZoomTo CONSTANT)

public:
  DsaResources(QObject* parent = nullptr);
  ~DsaResources();

private:
  QUrl icon2d() const { return "qrc:/Resources/icons/xhdpi/2D.png"; }
  QUrl icon3d() const { return "qrc:/Resources/icons/xhdpi/3D.png"; }
  QUrl iconAboutMap() const { return "qrc:/Resources/icons/xhdpi/ic_menu_aboutmap_dark.png"; }
  QUrl iconAdd() const { return "qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png"; }
  QUrl iconAddLayer() const { return "qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png"; }
  QUrl iconAlert() const { return "qrc:/Resources/icons/xhdpi/ic_menu_failedlayer.png"; }
  QUrl iconAppLogo() const { return "qrc:/Resources/icons/xhdpi/applogo.png"; }
  QUrl iconChooseBasemap() const { return "qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png"; }
  QUrl iconClose() const { return "qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png"; }
  QUrl iconComplete() const { return "qrc:/Resources/icons/xhdpi/ic_menu_completedone_dark.png"; }
  QUrl iconCoordinateConversion() const { return "qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png"; }
  QUrl iconCreateReport() const { return "qrc:/Resources/icons/xhdpi/ic_menu_editmap_dark_d-2.png"; }
  QUrl iconDraw() const { return "qrc:/Resources/icons/xhdpi/ic_menu_edit_dark_d.png"; }
  QUrl iconDrawer() const { return "qrc:/Resources/icons/xhdpi/drawer_large.png"; }
  QUrl iconFollowLocation() const { return "qrc:/Resources/icons/xhdpi/navigation.png"; }
  QUrl iconFollowLocationNorthUp() const { return "qrc:/Resources/icons/xhdpi/navigation_north.png"; }
  QUrl iconFollowLocationOff() const { return "qrc:/Resources/icons/xhdpi/navigation_disabled.png"; }
  QUrl iconGps() const { return "qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png"; }
  QUrl iconHome() const { return "qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png"; }
  QUrl iconLayers() const { return "qrc:/Resources/icons/xhdpi/ic_menu_layergroup_dark_d.png"; }
  QUrl iconListView() const { return "qrc:/Resources/icons/xhdpi/ic_menu_openlistview_dark.png"; }
  QUrl iconMap() const { return "qrc:/Resources/icons/xhdpi/ic_menu_mapview_dark_d.png"; }
  QUrl iconMessages() const { return "qrc:/Resources/icons/xhdpi/ic_menu_messages_dark.png"; }
  QUrl iconPoint() const { return "qrc:/Resources/icons/xhdpi/ic_menu_pointlayer_light.png"; }
  QUrl iconPolygon() const { return "qrc:/Resources/icons/xhdpi/ic_menu_polygonlayer_light.png"; }
  QUrl iconPolyline() const { return "qrc:/Resources/icons/xhdpi/ic_menu_linelayer_light.png"; }
  QUrl iconRaster() const { return "qrc:/Resources/icons/xhdpi/raster_layer.png"; }
  QUrl iconRemove() const { return "qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png"; }
  QUrl iconRotate() const { return "qrc:/Resources/icons/xhdpi/ic_menu_refresh_dark.png"; }
  QUrl iconSendMap() const { return "qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png"; }
  QUrl iconSettings() const { return "qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png"; }
  QUrl iconSketch() const { return "qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png"; }
  QUrl iconTools() const { return "qrc:/Resources/icons/xhdpi/ic_menu_tooloverflow_dark_d.png"; }
  QUrl iconTrash() const { return "qrc:/Resources/icons/xhdpi/ic_menu_trash_dark_d.png"; }
  QUrl iconViewshed() const { return "qrc:/Resources/icons/xhdpi/ic_menu_video_dark_d.png"; }
  QUrl iconZoomTo() const { return "qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png"; }
};

#endif // DSARESOURCES_H
