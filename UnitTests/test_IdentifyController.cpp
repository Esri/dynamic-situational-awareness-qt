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

#include "test_IdentifyController.h"

// C++ API
#include "Graphic.h"
#include "Point.h"
// DSA
#include "utilities/Common.h"
#include "IdentifyController.h"
// Qt
#include <QList>
#include <QTest>
// STL
#include <optional>
#include <vector>

using namespace Dsa;
namespace esri_rt = Esri::ArcGISRuntime;

static const QString TEST_LAYER_NAME_1 = QStringLiteral("TEST_LAYER_NAME_1");
static const QString TEST_ATTRIBUTE_NAME_1 = QStringLiteral("TEST_ATTRIBUTE_NAME_1");
static const QString TEST_ATTRIBUTE_VALUE_1 = QStringLiteral("TEST_ATTRIBUTE_VALUE_1");

static bool popupChangedEmitted = false;

bool checkReturnAndReset()
{
  bool checkedValue = popupChangedEmitted;
  popupChangedEmitted = false;
  return checkedValue;
}

void test_IdentifyController::test_setGeoElements() const
{
  QObject o{};
  IdentifyController i{};
  connect(&i, &IdentifyController::popupChanged, &o, []()
  {
    popupChangedEmitted = true;
  });

  // no elements
  std::vector<ContextMenu::Element> elements1{};
  i.setGeoElements(elements1);
  QCOMPARE(checkReturnAndReset(), true);

  // a single graphic with a single attribute
  elements1.emplace_back(TEST_LAYER_NAME_1,
                         new esri_rt::Graphic(esri_rt::Point{0.0, 0.0}, {{TEST_ATTRIBUTE_NAME_1, TEST_ATTRIBUTE_VALUE_1}}, &o),
                         nullptr);
  i.setGeoElements(elements1);
  QCOMPARE(checkReturnAndReset(), true);

  // graphic without attributes results in no elements added
  // but should still signal
  std::vector<ContextMenu::Element> elements2{};
  elements2.emplace_back(TEST_LAYER_NAME_1,
                         new esri_rt::Graphic(&o),
                         nullptr);
  i.setGeoElements(elements2);
  QCOMPARE(checkReturnAndReset(), true);
}
