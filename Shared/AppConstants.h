/*******************************************************************************
 *  Copyright 2012-2018 Esri
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

#ifndef APPCONSTANTS_H
#define APPCONSTANTS_H

// Qt headers
#include <QString>

namespace Dsa {

class AppConstants {
public:
  static const QString USERNAME_PROPERTYNAME;
  static const QString UNIT_OF_MEASUREMENT_PROPERTYNAME;
  static const QString UNIT_METERS;
  static const QString UNIT_FEET;
  static const QString LAYERS_PROPERTYNAME;
  static const QString CURRENTSCENE_PROPERTYNAME;
  static const QString SCENEINDEX_PROPERTYNAME;
  static const QString INITIALLOCATION_PROPERTYNAME;
};

} // Dsa

#endif // APPCONSTANTS_H
