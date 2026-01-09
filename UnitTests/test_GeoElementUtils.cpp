/*******************************************************************************
 *  Copyright 2012-2025 Esri
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

#include "test_GeoElementUtils.h"

// C++ headers
#include "Graphic.h"

// DSA headers
#include "GeoElementUtils.h"

// Qt headers
#include <QTest>

using namespace Dsa;
namespace esri_rt = Esri::ArcGISRuntime;

QList<esri_rt::GeoElement*> getListOfGraphics(qsizetype count)
{
  QList<esri_rt::GeoElement*> geoElements{};
  geoElements.reserve(count);
  for (int i = 0; i < count; ++i)
    geoElements.emplaceBack(new esri_rt::Graphic());
  return geoElements;
}

void test_GeoElementUtils::test_setParent() const
{
  QObject o{};
  esri_rt::Graphic g{};
  QCOMPARE(g.parent(), nullptr);
  GeoElementUtils::setParent(&g, &o);
  QCOMPARE(g.parent(), &o);

  const QList<esri_rt::GeoElement*> graphics = getListOfGraphics(50);
  GeoElementUtils::setParent(graphics, &o);
  const auto itBeg = std::cbegin(graphics);
  const auto itEnd = std::cend(graphics);
  const auto it = std::find_if(itBeg, itEnd, [&](const esri_rt::GeoElement* ge) { return dynamic_cast<const QObject*>(ge)->parent() != &o; });
  QCOMPARE(it, itEnd);
}
