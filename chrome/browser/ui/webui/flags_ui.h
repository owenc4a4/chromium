// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_FLAGS_UI_H_
#define CHROME_BROWSER_UI_WEBUI_FLAGS_UI_H_

#include "content/public/browser/web_ui_controller.h"
#include "ui/base/layout.h"

class PrefRegistrySimple;

namespace base {
class RefCountedMemory;
}

class FlagsUI : public content::WebUIController {
 public:
  explicit FlagsUI(content::WebUI* web_ui);

  static base::RefCountedMemory* GetFaviconResourceBytes(
      ui::ScaleFactor scale_factor);
  static void RegisterPrefs(PrefRegistrySimple* registry);

 private:
  DISALLOW_COPY_AND_ASSIGN(FlagsUI);
};

#endif  // CHROME_BROWSER_UI_WEBUI_FLAGS_UI_H_
