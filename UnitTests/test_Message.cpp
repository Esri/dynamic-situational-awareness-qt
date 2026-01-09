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

#include "test_Message.h"

// C++ API
#include "Point.h"
#include "SpatialReference.h"
// DSA
#include "Message.h"
// Qt
#include <QByteArray>
#include <QString>
#include <QTest>

using namespace Dsa;
namespace esri_rt = Esri::ArcGISRuntime;

static const QString TEST_ATTRIBUTE_NAME_1 = QStringLiteral("TEST_ATTRIBUTE_NAME_1");
static const QString TEST_ATTRIBUTE_VALUE_1 = QStringLiteral("TEST_ATTRIBUTE_VALUE_1");
static const QString TEST_STRING_1 = QStringLiteral("TEST_STRING_1");

void test_Message::test_constructor() const
{
  // Message()
  const Message mEmpty{};
  QCOMPARE(mEmpty.attributes().isEmpty(), true);
  QCOMPARE(mEmpty.geometry().isEmpty(), true);
  QCOMPARE(mEmpty.isEmpty(), true);
  QCOMPARE(mEmpty.messageAction(), Message::MessageAction::Unknown);
  QCOMPARE(mEmpty.messageId().isEmpty(), true);
  QCOMPARE(mEmpty.messageName().isEmpty(), true);
  QCOMPARE(mEmpty.messageType().isEmpty(), true);
  QCOMPARE(mEmpty.symbolId().isEmpty(), true);

  // Message(MessageAction messageAction, const Esri::ArcGISRuntime::Geometry& geometry);
  const Message mActionGeometry{Message::MessageAction::Remove, esri_rt::Point{0, 0}};
  QCOMPARE(mActionGeometry.attributes().isEmpty(), true);
  QCOMPARE(mActionGeometry.geometry().isEmpty(), false);
  QCOMPARE(mActionGeometry.isEmpty(), false);
  QCOMPARE(mActionGeometry.messageAction(), Message::MessageAction::Remove);
  QCOMPARE(mActionGeometry.messageId().isEmpty(), true);
  QCOMPARE(mActionGeometry.messageName().isEmpty(), true);
  QCOMPARE(mActionGeometry.messageType().isEmpty(), true);
  QCOMPARE(mActionGeometry.symbolId().isEmpty(), true);
}

void test_Message::test_isEmpty() const
{
  const Message m{Message::MessageAction::Remove, esri_rt::Point{}};
  QCOMPARE(m.isEmpty(), false);
}

void test_Message::test_setMessageAction() const
{
  Message m{};
  m.setMessageAction(Message::MessageAction::Select);
  QCOMPARE(m.messageAction(), Message::MessageAction::Select);
}

void test_Message::test_setAttributes() const
{
  Message m{};
  const QVariantMap attributes{{TEST_ATTRIBUTE_NAME_1, TEST_ATTRIBUTE_VALUE_1}};
  m.setAttributes(attributes);
  QCOMPARE(m.attributes()[TEST_ATTRIBUTE_NAME_1].toString(), TEST_ATTRIBUTE_VALUE_1);
}

void test_Message::test_setGeometry() const
{
  Message m{};
  const auto p = esri_rt::Point(0.0f, 0.0f);
  m.setGeometry(p);
  const esri_rt::Point pm = esri_rt::geometry_cast<esri_rt::Point>(m.geometry());
  QCOMPARE(pm.x(), p.x());
  QCOMPARE(pm.y(), p.y());
}

void test_Message::test_setMessageId() const
{
  Message m{};
  m.setMessageId(TEST_STRING_1);
  QCOMPARE(m.messageId(), TEST_STRING_1);
}

void test_Message::test_setMessageName() const
{
  Message m{};
  m.setMessageName(TEST_STRING_1);
  QCOMPARE(m.messageName(), TEST_STRING_1);
}

void test_Message::test_setMessageType() const
{
  Message m{};
  m.setMessageName(TEST_STRING_1);
  QCOMPARE(m.messageName(), TEST_STRING_1);
}

void test_Message::test_setSymbolId() const
{
  Message m{};
  m.setSymbolId(TEST_STRING_1);
  QCOMPARE(m.symbolId(), TEST_STRING_1);
}

void test_Message::test_toGeoMessage() const
{
  Message m{};
  static constexpr std::string_view emptyMessageXML{R"(<geomessage><_type></_type><_action></_action><_id></_id><_control_points></_control_points><_wkid>0</_wkid></geomessage>)"};
  QByteArray b1{emptyMessageXML};
  QCOMPARE(m.toGeoMessage(), b1);

  static constexpr std::string_view pointAndSrXML{R"(<geomessage><_type></_type><_action></_action><_id></_id><_control_points>1,2</_control_points><_wkid>4326</_wkid></geomessage>)"};
  m.setGeometry(esri_rt::Point(1.0f, 2.0f, esri_rt::SpatialReference{4326}));
  QByteArray b2{pointAndSrXML};
  QCOMPARE(m.toGeoMessage(), b2);
}
