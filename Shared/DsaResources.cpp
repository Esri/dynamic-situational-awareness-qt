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

#include "pch.hpp"

#include "DsaResources.h"

/*!
  \class DsaResources
  \inherits QObject
  \brief Helper class which presents a series of resources such as icon URLs.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
DsaResources::DsaResources(QObject* parent):
  QObject(parent)
{  
}

/*!
  \brief Destructor.
 */
DsaResources::~DsaResources()
{ 
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::icon2d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/2D.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::icon3d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/3D.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAboutMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_aboutmap_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAdd() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAddLayer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAlert() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Alert.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAlertConditionList() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/View_Alert_Conditions.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAlertList() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/View_Alerts.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconAppLogo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/applogo.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconChooseBasemap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconClose() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconColorPalette() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_colorpalette_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconComplete() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_completedone_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconCoordinateConversion() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconCreateReport() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_editmap_dark_d-2.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconCurrentLocation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_usecurrentlocation_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconDistressGray() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconGray.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconDistressRed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconRed.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconDraw() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_edit_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconDrawer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/drawer_large.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconDsaHome() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/dsa_home.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconFollowLocation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconFollowLocationNorthUp() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_north.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconFollowLocationOff() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_disabled.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconGps() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconGpsOff() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/GPS_Off.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconHome() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconLayers() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layergroup_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconLineWidth() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_sketchlinepopover_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconListView() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_openlistview_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_mapview_dark_d.png");
}

QUrl DsaResources::iconMenu() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_overflowcirclessmall_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconMessages() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Real_Time_Feeds.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconPin() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/pin.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconPoint() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Point.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconPolygon() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Polygon.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconPolyline() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Polyline.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconRaster() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/raster_layer.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconRemove() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconReport() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Create_Report.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconRotate() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Rotate_Mode.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconSave() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/save.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconSendMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconSettings() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconSketch() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconTools() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_tooloverflow_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconTouch() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Touch.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconTrash() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_trash_dark_d.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconViewshed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Viewshed_Tool.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconZoomTo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconWarningGreen() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_green.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconWarningOrange() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_orange.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconWarningRed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_red.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconWarningRedExclamation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_red_exclamation.png");
}

/*!
  \brief Returns an icon URL.
 */
QUrl DsaResources::iconLineOfSight() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/line_of_sight.png");
}
