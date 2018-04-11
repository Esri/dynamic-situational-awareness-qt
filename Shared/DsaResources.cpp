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

// PCH header
#include "pch.hpp"

#include "DsaResources.h"

namespace Dsa {

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
  \brief Returns the URL to the "2D" icon.
 */
QUrl DsaResources::icon2d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/2D.png");
}

/*!
  \brief Returns the URL to the "3D" icon.
 */
QUrl DsaResources::icon3d() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/3D.png");
}

/*!
  \brief Returns the URL to the "About Map" icon.
 */
QUrl DsaResources::iconAboutMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_aboutmap_dark.png");
}

/*!
  \brief Returns the URL to the "Add" icon.
 */
QUrl DsaResources::iconAdd() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_add_dark_d.png");
}

/*!
  \brief Returns the URL to the "Add Layer" icon.
 */
QUrl DsaResources::iconAddLayer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layervisibilitypopover_dark_d.png");
}

/*!
  \brief Returns the URL to the "Alert" icon.
 */
QUrl DsaResources::iconAlert() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Alert.png");
}

/*!
  \brief Returns the URL to the "Alert Condition List" icon.
 */
QUrl DsaResources::iconAlertConditionList() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/View_Alert_Conditions.png");
}

/*!
  \brief Returns the URL to the "Alert List" icon.
 */
QUrl DsaResources::iconAlertList() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/View_Alerts.png");
}

/*!
  \brief Returns the URL to the "App Logo" icon.
 */
QUrl DsaResources::iconAppLogo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/applogo.png");
}

/*!
  \brief Returns the URL to the "Choose Basemap" icon.
 */
QUrl DsaResources::iconChooseBasemap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_choosebasemap_dark.png");
}

/*!
  \brief Returns the URL to the "Close" icon.
 */
QUrl DsaResources::iconClose() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_closeclear_dark.png");
}

/*!
  \brief Returns the URL to the "Color Palette" icon.
 */
QUrl DsaResources::iconColorPalette() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_colorpalette_dark_d.png");
}

/*!
  \brief Returns the URL to the "Complete" icon.
 */
QUrl DsaResources::iconComplete() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_completedone_dark.png");
}

/*!
  \brief Returns the URL to the "Coordinate Conversion" icon.
 */
QUrl DsaResources::iconCoordinateConversion() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/icon-64-coorconv-white.png");
}

/*!
  \brief Returns the URL to the "Create Report" icon.
 */
QUrl DsaResources::iconCreateReport() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_editmap_dark_d-2.png");
}

/*!
  \brief Returns the URL to the "Current Location" icon.
 */
QUrl DsaResources::iconCurrentLocation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_usecurrentlocation_dark_d.png");
}

/*!
  \brief Returns the URL to the "Distress (Gray)" icon.
 */
QUrl DsaResources::iconDistressGray() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconGray.png");
}

/*!
  \brief Returns the URL to the "Distress (Red)" icon.
 */
QUrl DsaResources::iconDistressRed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/distressIconRed.png");
}

/*!
  \brief Returns the URL to the "Draw" icon.
 */
QUrl DsaResources::iconDraw() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_edit_dark_d.png");
}

/*!
  \brief Returns the URL to the "Drawer" icon.
 */
QUrl DsaResources::iconDrawer() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/drawer_large.png");
}

/*!
  \brief Returns the URL to the "Dsa Home" icon.
 */
QUrl DsaResources::iconDsaHome() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/dsa_home.png");
}

/*!
  \brief Returns the URL to the "Follow Location" icon.
 */
QUrl DsaResources::iconFollowLocation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation.png");
}

/*!
  \brief Returns the URL to the "Follow Location (North Up)" icon.
 */
QUrl DsaResources::iconFollowLocationNorthUp() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_north.png");
}

/*!
  \brief Returns the URL to the "Follow Location (Off)" icon.
 */
QUrl DsaResources::iconFollowLocationOff() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/navigation_disabled.png");
}

/*!
  \brief Returns the URL to the "GPS" icon.
 */
QUrl DsaResources::iconGps() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_gpsondontfollow_dark.png");
}

/*!
  \brief Returns the URL to the "GPS (Off)" icon.
 */
QUrl DsaResources::iconGpsOff() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/GPS_Off.png");
}

/*!
  \brief Returns the URL to the "Home" icon.
 */
QUrl DsaResources::iconHome() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_home_dark.png");
}

/*!
  \brief Returns the URL to the "Layers" icon.
 */
QUrl DsaResources::iconLayers() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_layergroup_dark_d.png");
}

/*!
  \brief Returns the URL to the "Line Width" icon.
 */
QUrl DsaResources::iconLineWidth() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_sketchlinepopover_dark_d.png");
}

/*!
  \brief Returns the URL to the "List View" icon.
 */
QUrl DsaResources::iconListView() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_openlistview_dark.png");
}

/*!
  \brief Returns the URL to the "Map" icon.
 */
QUrl DsaResources::iconMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_mapview_dark_d.png");
}

/*!
  \brief Returns the URL to the "Menu" icon.
 */
QUrl DsaResources::iconMenu() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_overflowcirclessmall_dark_d.png");
}

/*!
  \brief Returns the URL to the "Messages" icon.
 */
QUrl DsaResources::iconMessages() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Real_Time_Feeds.png");
}

/*!
  \brief Returns the URL to the "Pin" icon.
 */
QUrl DsaResources::iconPin() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/pin.png");
}

/*!
  \brief Returns the URL to the "Point" icon.
 */
QUrl DsaResources::iconPoint() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Point.png");
}

/*!
  \brief Returns the URL to the "Polygon" icon.
 */
QUrl DsaResources::iconPolygon() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Polygon.png");
}

/*!
  \brief Returns the URL to the "Polyline" icon.
 */
QUrl DsaResources::iconPolyline() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Polyline.png");
}

/*!
  \brief Returns the URL to the "Raster" icon.
 */
QUrl DsaResources::iconRaster() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/raster_layer.png");
}

/*!
  \brief Returns the URL to the "Remove" icon.
 */
QUrl DsaResources::iconRemove() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_subtract_dark_d.png");
}

/*!
  \brief Returns the URL to the "Report" icon.
 */
QUrl DsaResources::iconReport() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Create_Report.png");
}

/*!
  \brief Returns the URL to the "Rotate" icon.
 */
QUrl DsaResources::iconRotate() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Rotate_Mode.png");
}

/*!
  \brief Returns the URL to the "Save" icon.
 */
QUrl DsaResources::iconSave() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/save.png");
}

/*!
  \brief Returns the URL to the "Send Map" icon.
 */
QUrl DsaResources::iconSendMap() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_sendmap_dark_d.png");
}

/*!
  \brief Returns the URL to the "Settings" icon.
 */
QUrl DsaResources::iconSettings() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_settings_dark_d.png");
}

/*!
  \brief Returns the URL to the "Sketch" icon.
 */
QUrl DsaResources::iconSketch() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_freehandsketchoff_dark.png");
}

/*!
  \brief Returns the URL to the "Tools" icon.
 */
QUrl DsaResources::iconTools() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_tooloverflow_dark_d.png");
}

/*!
  \brief Returns the URL to the "Touch" icon.
 */
QUrl DsaResources::iconTouch() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Touch.png");
}

/*!
  \brief Returns the URL to the "Trash" icon.
 */
QUrl DsaResources::iconTrash() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_trash_dark_d.png");
}

/*!
  \brief Returns the URL to the "Viewshed" icon.
 */
QUrl DsaResources::iconViewshed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/Viewshed_Tool.png");
}

/*!
  \brief Returns the URL to the "Zoom To" icon.
 */
QUrl DsaResources::iconZoomTo() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/ic_menu_zoomtofeature_light.png");
}

/*!
  \brief Returns the URL to the "Warning (Green)" icon.
 */
QUrl DsaResources::iconWarningGreen() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_green.png");
}

/*!
  \brief Returns the URL to the "Warning (Orange)" icon.
 */
QUrl DsaResources::iconWarningOrange() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_orange.png");
}

/*!
  \brief Returns the URL to the "Warning (Red)" icon.
 */
QUrl DsaResources::iconWarningRed() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_red.png");
}

/*!
  \brief Returns the URL to the "Warning (Red Exclamation)" icon.
 */
QUrl DsaResources::iconWarningRedExclamation() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/warning_red_exclamation.png");
}

/*!
  \brief Returns the URL to the "Line Of Sight" icon.
 */
QUrl DsaResources::iconLineOfSight() const
{
  return QStringLiteral("qrc:/Resources/icons/xhdpi/line_of_sight.png");
}

} // Dsa
