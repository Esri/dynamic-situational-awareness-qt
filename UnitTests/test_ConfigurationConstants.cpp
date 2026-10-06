/*******************************************************************************
 *  Copyright 2012-2026 Esri
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

#include "test_ConfigurationConstants.h"

// DSA
#include "ConfigurationConstants.h"

// Qt
#include <QTest>

using namespace Dsa::ConfigurationConstants;
using namespace Esri::ArcGISRuntime;

static const QString TEST_SURFACE_VALUE_UNKNOWN = QStringLiteral("unknown");
static const QString TEST_SURFACE_VALUE_ABSOLUTE_CASE_INSENSITIVE = QStringLiteral("AbSoLuTe");
static const QString TEST_SURFACE_VALUE_RELATIVE_TO_SCENE = QStringLiteral("relativeToScene");

using namespace Dsa;

void test_ConfigurationConstants::test_toSurfacePlacement() const
{
  QCOMPARE(SurfacePlacement::DrapedBillboarded, toSurfacePlacement(SURFACE_PLACEMENT_DRAPED_BILLBOARDED));
  QCOMPARE(SurfacePlacement::DrapedBillboarded, toSurfacePlacement(TEST_SURFACE_VALUE_UNKNOWN));
  QCOMPARE(SurfacePlacement::DrapedBillboarded, toSurfacePlacement(TEST_SURFACE_VALUE_RELATIVE_TO_SCENE)); // should not be found
  QCOMPARE(SurfacePlacement::Absolute, toSurfacePlacement(SURFACE_PLACEMENT_ABSOLUTE));
  QCOMPARE(SurfacePlacement::Absolute, toSurfacePlacement(TEST_SURFACE_VALUE_ABSOLUTE_CASE_INSENSITIVE));
  QCOMPARE(SurfacePlacement::Absolute, toSurfacePlacement(SURFACE_PLACEMENT_ABSOLUTE, SurfacePlacement::DrapedFlat));
  QCOMPARE(SurfacePlacement::DrapedFlat, toSurfacePlacement(SURFACE_PLACEMENT_DRAPED_FLAT));
  QCOMPARE(SurfacePlacement::Relative, toSurfacePlacement(SURFACE_PLACEMENT_RELATIVE));
  // possible to return even an unsupported value if it is used as the default with an unrecognized input string
  QCOMPARE(SurfacePlacement::RelativeToScene, toSurfacePlacement(TEST_SURFACE_VALUE_UNKNOWN, SurfacePlacement::RelativeToScene));
}
