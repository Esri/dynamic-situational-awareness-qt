/*******************************************************************************
 *  Copyright 2012-2020 Esri
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
// PCH header
#include "pch.hpp"

#include "RuntimePermissionRequest.h"

#ifndef Q_OS_ANDROID
bool Dsa::requestRequiredPermissions()
{
  return true;
}
#else
#  include <algorithm>
#  include <QtAndroid>

namespace
{
  const QList<QString> REQUIRED_PERMISSIONS = {
    "android.permission.WRITE_EXTERNAL_STORAGE", 
    "android.permission.READ_EXTERNAL_STORAGE"
  };
}

bool Dsa::requestRequiredPermissions()
{
  // Get our permissions not yet allowed.
  QList<QString> pendingPermissions;
  std::copy_if(std::begin(REQUIRED_PERMISSIONS), 
               std::end(REQUIRED_PERMISSIONS), 
               std::back_inserter(pendingPermissions), 
               [](const auto& p)
               {
                  return QtAndroid::PermissionResult::Denied == QtAndroid::checkPermission(p);
               }
  );

  if (pendingPermissions.isEmpty())
    return true;

  // Attempt to request the permissions.
  auto results = QtAndroid::requestPermissionsSync(pendingPermissions);

  // Return true only if all permissions were allowed.
  return !results.values().contains(QtAndroid::PermissionResult::Denied);
}
#endif
