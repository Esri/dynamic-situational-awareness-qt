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

#ifndef HANDHELDSTYLES_H
#define HANDHELDSTYLES_H

// Qt headers
#include <QObject>

namespace Dsa {
namespace Handheld {

class HandheldStyles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
  Q_PROPERTY(int titleFontPixelSize READ titleFontPixelSize CONSTANT)
  Q_PROPERTY(int secondaryTitleFontPixelSize READ secondaryTitleFontPixelSize CONSTANT)
  Q_PROPERTY(int primaryIconSize READ primaryIconSize CONSTANT)
  Q_PROPERTY(int secondaryIconSize READ secondaryIconSize CONSTANT)
  Q_PROPERTY(int toolFontPixelSize READ toolFontPixelSize CONSTANT)
  Q_PROPERTY(int mainToolbarHeight READ mainToolbarHeight CONSTANT)
  Q_PROPERTY(int categoryToolbarWidth READ categoryToolbarWidth CONSTANT)

public:
  HandheldStyles(QObject* parent = nullptr);
  ~HandheldStyles();

private:
  QString fontFamily() const;
  int titleFontPixelSize() const;
  int secondaryTitleFontPixelSize() const;
  int primaryIconSize() const;
  int secondaryIconSize() const;
  int toolFontPixelSize() const;
  int mainToolbarHeight() const;
  int categoryToolbarWidth() const;
};

} // Handheld
} // Dsa

#endif // HANDHELDSTYLES_H
