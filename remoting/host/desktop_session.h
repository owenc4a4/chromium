// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_HOST_DESKTOP_SESSION_H_
#define REMOTING_HOST_DESKTOP_SESSION_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"

namespace remoting {

class DaemonProcess;

// Represents the desktop session for a connected terminal. Each desktop session
// has a unique identifier used by cross-platform code to refer to it.
class DesktopSession {
 public:
  virtual ~DesktopSession();

  int id() const { return id_; }

 protected:
  // Creates a terminal and assigns a unique identifier to it. |daemon_process|
  // must outlive |this|.
  DesktopSession(DaemonProcess* daemon_process, int id);

  DaemonProcess* daemon_process() const { return daemon_process_; }

 private:
  // The owner of |this|.
  DaemonProcess* const daemon_process_;

  // A unique identifier of the terminal.
  const int id_;

  DISALLOW_COPY_AND_ASSIGN(DesktopSession);
};

}  // namespace remoting

#endif  // REMOTING_HOST_DESKTOP_SESSION_H_
