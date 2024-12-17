/*******************************************************************************
 *  Copyright 2012-2024 Esri
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

namespace Dsa {

class Configuration
{
public:
  explicit Configuration(const QString& name,
                         const QString& url,
                         bool downloaded,
                         bool selected,
                         bool loaded,
                         int percentDownloaded);
  Configuration(Configuration&) = default;
  Configuration(const Configuration&) = default;
  Configuration(Configuration&&) = default;
  Configuration& operator=(const Configuration&) = default;
  Configuration& operator=(Configuration&&) = default;

  QString name() const;
  void setName(const QString& name);

  QString url() const;
  void setUrl(const QString& url);

  bool downloaded() const;
  void setDownloaded(bool downloaded);

  bool selected() const;
  void setSelected(bool selected);

  bool loaded() const;
  void setLoaded(bool loaded);

  int percentDownloaded() const;
  void setPercentDownloaded(int percentDownloaded);

private:
  QString m_name;
  QString m_url;
  bool m_downloaded;
  bool m_selected;
  bool m_loaded;
  int m_percentDownloaded;
};

}

#endif // CONFIGURATION_H
