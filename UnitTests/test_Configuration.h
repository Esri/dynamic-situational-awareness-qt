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

#ifndef TEST_CONFIGURATION_H
#define TEST_CONFIGURATION_H

// Qt headers
#include <QObject>

class test_Configuration : public QObject
{
  Q_OBJECT

  inline static const QString TEST_CONFIGURATION_NAME_1 = "TEST_CONFIGURATION_NAME_1";
  inline static const QString TEST_CONFIGURATION_NAME_2 = "TEST_CONFIGURATION_NAME_2";
  inline static const QString TEST_CONFIGURATION_URL_1 = "TEST_CONFIGURATION_URL_1";
  inline static const int TEST_PERCENTAGE_1 = 50;

private slots:
  void initTestCase();

  void test_constructor();
  void test_cancelDownload();
  void test_download();
  void test_setName();
  void test_setPercentDownloaded();
  void test_setPercentExtracted();
  void test_setSelected();
  void test_setUrl();

  void cleanupTestCase();
};

#endif // TEST_CONFIGURATION_H
