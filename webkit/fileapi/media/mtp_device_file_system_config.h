// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_FILEAPI_MEDIA_MTP_DEVICE_FILE_SYSTEM_CONFIG_H_
#define WEBKIT_FILEAPI_MEDIA_MTP_DEVICE_FILE_SYSTEM_CONFIG_H_

#include "build/build_config.h"

// Support MTP device file system for Windows, Mac, Linux and ChromeOS.
// Note that OS_LINUX implies OS_CHROMEOS.
// TODO(gbillock): remove this define and make this default.
#if defined(OS_WIN) || defined(OS_MACOSX) || defined(OS_LINUX)
#define SUPPORT_MTP_DEVICE_FILESYSTEM
#endif

// Use asynchronous MTP device delegate API.
// TODO(kmadhusu): remove this define and make this default.
#if 0
#define USE_MTP_DEVICE_ASYNC_DELEGATE
#endif

#endif  // WEBKIT_FILEAPI_MEDIA_MTP_DEVICE_FILE_SYSTEM_CONFIG_H_
