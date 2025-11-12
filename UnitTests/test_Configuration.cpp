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

#include "test_Configuration.h"

// DSA headers
#include "Configuration.h"

// Qt headers
#include <QTest>

using namespace Dsa;

void test_Configuration::initTestCase()
{
}

void test_Configuration::test_constructor()
{
  //Configuration(const QString&, const QString&, bool, bool, int);
  Configuration c{TEST_CONFIGURATION_NAME_1, TEST_CONFIGURATION_URL_1, true, true, 100};
  QCOMPARE(c.canDelete(), false);
  QCOMPARE(c.canDownload(), false);
  QCOMPARE(c.downloadCancelled(), false);
  QCOMPARE(c.downloaded(), true);
  QCOMPARE(c.downloading(), false);
  QCOMPARE(c.downloading(), false);
  QCOMPARE(c.extracted(), true);
  QCOMPARE(c.extracting(), false);
  QCOMPARE(c.inProgress(), false);
  QCOMPARE(c.isCancellable(), true);
  QCOMPARE(c.loaded(), true);
  QCOMPARE(c.name(), TEST_CONFIGURATION_NAME_1);
  QCOMPARE(c.percentComplete(), 100);
  QCOMPARE(c.percentDownloaded(), 100);
  QCOMPARE(c.percentExtracted(), 100);
  QCOMPARE(c.requiresRestart(), false);
  QCOMPARE(c.selected(), true);
  QCOMPARE(c.url(), QUrl{TEST_CONFIGURATION_URL_1});
  QCOMPARE(c.urlStr(), TEST_CONFIGURATION_URL_1);
}

void test_Configuration::test_cancelDownload()
{
  Configuration c{TEST_CONFIGURATION_NAME_1, TEST_CONFIGURATION_URL_1, false, false, 100};
  c.cancelDownload();
  QCOMPARE(c.percentDownloaded(), 0);
  QCOMPARE(c.percentExtracted(), 0);
  QCOMPARE(c.downloadCancelled(), true);
}

void test_Configuration::test_download()
{
  Configuration c{TEST_CONFIGURATION_NAME_1, TEST_CONFIGURATION_URL_1, false, false, 100};
  c.download();
  QCOMPARE(c.downloadCancelled(), false);
  QCOMPARE(c.percentDownloaded(), 0);
  QCOMPARE(c.percentExtracted(), 0);
}

void test_Configuration::test_setName()
{
  Configuration c{};
  c.setName(TEST_CONFIGURATION_NAME_1);
  QCOMPARE(c.name(), TEST_CONFIGURATION_NAME_1);
}

void test_Configuration::test_setPercentDownloaded()
{
  Configuration c{};
  c.setPercentDownloaded(TEST_PERCENTAGE_1);
  QCOMPARE(c.percentDownloaded(), TEST_PERCENTAGE_1);
}

void test_Configuration::test_setPercentExtracted()
{
  Configuration c{};
  c.setPercentExtracted(TEST_PERCENTAGE_1);
  QCOMPARE(c.percentExtracted(), TEST_PERCENTAGE_1);
}

void test_Configuration::test_setSelected()
{
  Configuration c{};
  c.setSelected(true);
  QCOMPARE(c.selected(), true);
}

void test_Configuration::test_setUrl()
{
  Configuration c{};
  c.setUrl(TEST_CONFIGURATION_URL_1);
  QCOMPARE(c.urlStr(), TEST_CONFIGURATION_URL_1);
}

void test_Configuration::cleanupTestCase()
{
}
