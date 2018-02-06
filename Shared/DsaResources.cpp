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

#include "DsaResources.h"

DsaResources::DsaResources(QObject* parent):
  QObject(parent)
{  
}

DsaResources::~DsaResources()
{ 
}

QUrl DsaResources::icon2d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/2D.png");
}

QUrl DsaResources::icon3d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/3D.png");
}

QUrl DsaResources::iconAboutMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_aboutmap_dark.png");
}

QUrl DsaResources::iconAdd() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png");
}

QUrl DsaResources::iconAddLayer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png");
}

QUrl DsaResources::iconAlert() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_failedlayer.png");
}

QUrl DsaResources::iconAppLogo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/applogo.png");
}

QUrl DsaResources::iconChooseBasemap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png");
}

QUrl DsaResources::iconClose() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png");
}

QUrl DsaResources::iconComplete() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_completedone_dark.png");
}

QUrl DsaResources::iconCoordinateConversion() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png");
}

QUrl DsaResources::iconCreateReport() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_editmap_dark_d-2.png");
}

QUrl DsaResources::iconDistress() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIcon.png");
}

QUrl DsaResources::iconDistressGray() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconGray.svg");
}

QUrl DsaResources::iconDistressRed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconRed.svg");
}

QUrl DsaResources::iconDraw() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_edit_dark_d.png");
}

QUrl DsaResources::iconDrawer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/drawer_large.png");
}

QUrl DsaResources::iconFollowLocation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation.png");
}

QUrl DsaResources::iconFollowLocationNorthUp() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_north.png");
}

QUrl DsaResources::iconFollowLocationOff() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_disabled.png");
}

QUrl DsaResources::iconGps() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png");
}

QUrl DsaResources::iconHome() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png");
}

QUrl DsaResources::iconLayers() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layergroup_dark_d.png");
}

QUrl DsaResources::iconListView() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_openlistview_dark.png");
}

QUrl DsaResources::iconMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_mapview_dark_d.png");
}

QUrl DsaResources::iconMenu() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_overflowcirclessmall_dark_d.png");
}

QUrl DsaResources::iconMessages() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_messages_dark.png");
}

QUrl DsaResources::iconPoint() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_pointlayer_light.png");
}

QUrl DsaResources::iconPolygon() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_polygonlayer_light.png");
}

QUrl DsaResources::iconPolyline() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_linelayer_light.png");
}

QUrl DsaResources::iconRaster() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/raster_layer.png");
}

QUrl DsaResources::iconRemove() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png");
}

QUrl DsaResources::iconRotate() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_refresh_dark.png");
}

QUrl DsaResources::iconSendMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png");
}

QUrl DsaResources::iconSettings() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png");
}

QUrl DsaResources::iconSketch() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png");
}

QUrl DsaResources::iconTools() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_tooloverflow_dark_d.png");
}

QUrl DsaResources::iconTrash() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_trash_dark_d.png");
}

QUrl DsaResources::iconViewshed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_video_dark_d.png");
}

QUrl DsaResources::iconZoomTo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png");
}
