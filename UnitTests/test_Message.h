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

#ifndef TEST_MESSAGE_H
#define TEST_MESSAGE_H

// Qt headers
#include <QObject>

class test_Message : public QObject
{
  Q_OBJECT

  inline static const QString TEST_ATTRIBUTE_NAME_1 = QStringLiteral("TEST_ATTRIBUTE_NAME_1");
  inline static const QString TEST_ATTRIBUTE_VALUE_1 = QStringLiteral("TEST_ATTRIBUTE_VALUE_1");
  inline static const QString TEST_STRING_1 = QStringLiteral("TEST_STRING_1");

private slots:
  void initTestCase();

  void test_constructor() const;
  void test_isEmpty() const;
  void test_setMessageAction() const;
  void test_setAttributes() const;
  void test_setGeometry() const;
  void test_setMessageId() const;
  void test_setMessageName() const;
  void test_setMessageType() const;
  void test_setSymbolId() const;
  void test_toGeoMessage() const;

  void cleanupTestCase();
};

#endif // TEST_MESSAGE_H
