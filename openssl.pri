################################################################################
#  Copyright 2012-2026 Esri
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
################################################################################

android {
    SSL_PATH_RESOLVED = $$(SSL_PATH)
    if (isEmpty(SSL_PATH_RESOLVED)) {
        error("ssl path must be defined for Android builds")
    }

    !contains(QT.network_private.enabled_features, openssl-linked) {
        if (versionAtLeast(QT_VERSION, 6.5.0)) {
            ANDROID_EXTRA_LIBS += \
                $$(SSL_PATH)/ssl_3/arm64-v8a/libcrypto_3.so \
                $$(SSL_PATH)/ssl_3/arm64-v8a/libssl_3.so \
                $$(SSL_PATH)/ssl_3/armeabi-v7a/libcrypto_3.so \
                $$(SSL_PATH)/ssl_3/armeabi-v7a/libssl_3.so \
                $$(SSL_PATH)/ssl_3/x86/libcrypto_3.so \
                $$(SSL_PATH)/ssl_3/x86/libssl_3.so \
                $$(SSL_PATH)/ssl_3/x86_64/libcrypto_3.so \
                $$(SSL_PATH)/ssl_3/x86_64/libssl_3.so
        } else {
            ANDROID_EXTRA_LIBS += \
                $$(SSL_PATH)/ssl_1.1/arm64-v8a/libcrypto_1_1.so \
                $$(SSL_PATH)/ssl_1.1/arm64-v8a/libssl_1_1.so \
                $$(SSL_PATH)/ssl_1.1/armeabi-v7a/libcrypto_1_1.so \
                $$(SSL_PATH)/ssl_1.1/armeabi-v7a/libssl_1_1.so \
                $$(SSL_PATH)/ssl_1.1/x86/libcrypto_1_1.so \
                $$(SSL_PATH)/ssl_1.1/x86/libssl_1_1.so \
                $$(SSL_PATH)/ssl_1.1/x86_64/libcrypto_1_1.so \
                $$(SSL_PATH)/ssl_1.1/x86_64/libssl_1_1.so
        }
    }
}
