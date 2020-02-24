
/*******************************************************************************
 *  Copyright 2012-2020 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/
#include "CoordinateConversionToolProxy.h"

#include "ToolManager.h"

#include "Esri/ArcGISRuntime/Toolkit/CoordinateConversionController.h"
#include "Esri/ArcGISRuntime/Toolkit/CoordinateConversionConstants.h"
#include "Esri/ArcGISRuntime/Toolkit/CoordinateConversionResult.h"
#include "Esri/ArcGISRuntime/Toolkit/GenericListModel.h"

#include <SceneQuickView.h>

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

/*!
 * \class Dsa::CoordinateConversionToolProxy
 * \brief A wrapper around the Toolkit's CoordinateConversionController.
 *
 * This class is the glue between the DSA and the CoordianteConversionController
 * tool. The proxy "owns" the CoordinateConversionProxy, and will feed the
 * controller location updates whenver it is applicable to do so, (i.e: when
 * inInputMode is false).
 *
 * This tool also handles saving/loading a `CoordinateConversionResult` from
 * a json configuration, which helps track the user selected format in the
 * CoordinateConversion view's input format.
 *
 * Finally, this links up the ToolManager geoViewChanged signals. So that the
 * GeoView of the CoordianteConversionController always matches what the
 * ToolManager claims is the current GeoView.
 *
 */

namespace Dsa
{

CoordinateConversionToolProxy::CoordinateConversionToolProxy(QObject* parent) :
  AbstractTool(parent),
  m_inInputMode(false),
  m_controller(new CoordinateConversionController(this)),
  m_inputFormat(new CoordinateConversionResult(this))
{
  ToolManager::instance().addTool(this);
  connectController();
}

CoordinateConversionToolProxy::~CoordinateConversionToolProxy()
{
}

/*!
 * \brief Return the name of this tool as "CoordinateConversion".
 */
QString CoordinateConversionToolProxy::toolName() const
{
  return "CoordinateConversion";
}

/*!
 * \brief Feeds the clicked point into the controller.
 * \param pos Position of click on scene.
 */
bool CoordinateConversionToolProxy::handleClick(const Point& pos)
{
  m_controller->setCurrentPoint(pos);
  return true;
}

/*!
 * \brief Returns whether the proxy is in "input mode." When inputMode is true
 * this proxy object expects the user to be feeding positions to the controller
 * manually will not attempt to override the current controller position with
 * the DSA location.
 * When not in inputMode, for every DSA location update, this proxy object will
 * feed the location updates directly into the controller current location.
 * \return true if in input mode.
 */
bool CoordinateConversionToolProxy::inInputMode() const
{
  return m_inInputMode;
}

/*!
 * \brief Sets the "input mode."
 * \sa inInputMode
 * \param mode Boolean flag if in inputMode or not.
 */
void CoordinateConversionToolProxy::setInInputMode(bool mode)
{
  if (mode == m_inInputMode)
    return;

  m_inInputMode = mode;
  emit inInputMode();
}

/*!
 * \internal
 * The inputFormat is a conversion "result" that the DSA provides to the
 * CoordinateConversion view because (and only because) DSA provides
 * configuration data read from JSON. Here we do a lookup of all formats and
 * apply the one with the matching name to our inputFormat result object.
 */
void CoordinateConversionToolProxy::setProperties(const QVariantMap& properties)
{
  const auto formats = qobject_cast<GenericListModel*>(m_controller->coordinateFormats());
  if (!formats)
    return;

  auto findFormatIt = properties.find("CoordinateFormat");
  if (findFormatIt != properties.end())
  {
    const auto size = formats->rowCount();
    for (int i = 0; i < size; ++i)
    {
      auto index = formats->index(i);
      auto element = formats->element<CoordinateConversionOption>(index);
      if (element && element->name() == findFormatIt.value())
      {
        m_inputFormat->setType(element);
        break;
      }
    }
  }
}

/*!
 * \brief Returns a controller that is fed with DSA and GeoView updates when
 * applicable.
 * \return linked controller.
 */
CoordinateConversionController* CoordinateConversionToolProxy::controller() const
{
  return m_controller;
}

/*!
 * \brief The inputFormat is a conversion "result" that the DSA provides to the
 * CoordinateConversion view because (and only because) DSA provides
 * configuration data read from JSON as to what the format of inputFormat should
 * be at startup.
 * \return linked inputFormat.
 */
CoordinateConversionResult* CoordinateConversionToolProxy::inputFormat() const
{
  return m_inputFormat;
}

/*!
 * \internal
 * \brief Implementation detail.
 *
 * Connect up the controller to the location change and geoview updates that
 * comes from the toolManager.
 */
void CoordinateConversionToolProxy::connectController()
{
  auto geoView = ToolResourceProvider::instance()->geoView();
  m_controller->setGeoView(static_cast<SceneQuickView*>(geoView));

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::locationChanged,
    m_controller,
    [this](const Point& point)
    {
      if (isActive() && !m_inInputMode)
        m_controller->setCurrentPoint(point);
    });

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged, this, [this]()
  {
    m_controller->setGeoView(static_cast<SceneQuickView*>(ToolResourceProvider::instance()->geoView()));
  });
}

}
