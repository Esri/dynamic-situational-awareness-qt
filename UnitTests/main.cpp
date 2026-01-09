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

// DSA headers
#include "test_Configuration.h"
#include "test_GeoElementUtils.h"
#include "test_IdentifyController.h"
#include "test_Message.h"
#include "test_MessageFeed.h"

// Qt headers
#include <QCoreApplication>
#include <QTest>

template<typename T>
int runTest()
{
  T t{};
  return QTest::qExec(&t);
}

int main(int argc, char* argv[])
{
  QCoreApplication{argc, argv};

  int failureCount = 0;
  failureCount += runTest<test_Configuration>();
  failureCount += runTest<test_GeoElementUtils>();
  failureCount += runTest<test_IdentifyController>();
  failureCount += runTest<test_Message>();
  failureCount += runTest<test_MessageFeed>();

  return failureCount;
}
