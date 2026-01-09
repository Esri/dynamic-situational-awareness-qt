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

#include "test_MessageFeed.h"

// C++ headers
#include <MapTypes.h>

// DSA headers
#include "Message.h"
#include "MessageFeed.h"

// Qt headers
#include <QTest>

using namespace Dsa;
namespace esri_rt = Esri::ArcGISRuntime;

static const QString TEST_FEED_NAME_1 = QStringLiteral("TEST_FEED_NAME_1");
static const QString TEST_FEED_NAME_2 = QStringLiteral("TEST_FEED_NAME_2");

void runFeedMessageTypeTest(const QString& feedName, const QString& feedMessageType, QObject* o)
{
  MessageFeed mf{feedName, feedMessageType};
  o->connect(&mf, &esri_rt::DynamicEntityDataSource::doneLoading, o, [&](const esri_rt::Error&)
  {
    QCOMPARE(mf.feedName(), feedName);
    QCOMPARE(mf.feedMessageType(), feedMessageType);
  });
  mf.load();
  while (mf.loadStatus() == esri_rt::LoadStatus::Loading);
}

void test_MessageFeed::test_onLoadedAsync() const
{
  int argc = 0;
  char** argv = nullptr;
  QCoreApplication app{argc, argv};
  QObject o{};
  runFeedMessageTypeTest(TEST_FEED_NAME_1, Message::MESSAGE_TYPE_VALUE_COT, &o);
  runFeedMessageTypeTest(TEST_FEED_NAME_2, Message::MESSAGE_TYPE_VALUE_GEOMESSAGE, &o);
}
