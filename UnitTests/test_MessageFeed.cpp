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

// DSA
#include "Message.h"
#include "MessageFeed.h"
// Qt
#include <QCoreApplication>
#include <QTest>

using namespace Dsa;

static const QString TEST_FEED_NAME_1 = QStringLiteral("TEST_FEED_NAME_1");
static const QString TEST_FEED_NAME_2 = QStringLiteral("TEST_FEED_NAME_2");

void test_MessageFeed::test_constructor() const
{
  int argc = 0;
  char** argv = nullptr;
  QCoreApplication app{argc, argv};
  MessageFeed mfCoT{TEST_FEED_NAME_1, Message::MESSAGE_TYPE_VALUE_COT};
  QCOMPARE(mfCoT.feedName(), TEST_FEED_NAME_1);
  QCOMPARE(mfCoT.feedMessageType(), Message::MESSAGE_TYPE_VALUE_COT);
}
