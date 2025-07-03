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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// Qt headers
#include <QString>
#include <QUrl>

namespace Dsa {

class Configuration
{
public:
  Configuration() = default;
  Configuration(const QString& name,
                const QString& url,
                bool selected,
                bool loaded,
                int percentDownloaded);

  QString name() const;
  void setName(const QString& name);

  QString urlStr() const;
  void setUrl(const QString& url);

  QUrl url() const;

  void download();
  bool downloaded() const;
  bool downloading() const;
  bool requiresRestart() const;
  bool canDownload() const;
  bool canDelete() const;
  bool isCancellable() const;
  bool downloadCancelled() const;

  bool selected() const;
  void setSelected(bool selected);

  bool loaded() const;

  int percentDownloaded() const;
  void setPercentDownloaded(int percentDownloaded);
  void cancelDownload();

private:
  QString m_name;
  QString m_urlStr;
  QUrl m_url;
  bool m_selected = false;
  bool m_loaded = false;
  bool m_isCancellable = true;
  int m_percentDownloaded = 0;
  bool m_downloadCancelled = false;
};

}

#endif // CONFIGURATION_H
