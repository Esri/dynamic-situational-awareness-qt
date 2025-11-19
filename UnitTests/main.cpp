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
#include "test_Message.h"

// Qt headers
#include <QCoreApplication>
#include <QTest>

// StdLib headers
#include <cstdlib>

template<typename T>
bool runTest()
{
  T t{};
  return QTest::qExec(&t) == 0;
}

int main(int argc, char* argv[])
{
  QCoreApplication{argc, argv};

  bool success = true;
  success &= runTest<test_Configuration>();
  success &= runTest<test_Message>();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
