// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/gl/gl_surface.h"

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "third_party/mesa/MesaLib/include/GL/osmesa.h"
#include "ui/gfx/gl/gl_bindings.h"
#include "ui/gfx/gl/gl_implementation.h"
#include "ui/gfx/gl/gl_surface_egl.h"
#include "ui/gfx/gl/gl_surface_osmesa.h"
#include "ui/gfx/gl/gl_surface_stub.h"
#include "ui/gfx/gl/gl_surface_wgl.h"

namespace gfx {

// This OSMesa GL surface can use GDI to swap the contents of the buffer to a
// view.
class NativeViewGLSurfaceOSMesa : public GLSurfaceOSMesa {
 public:
  explicit NativeViewGLSurfaceOSMesa(gfx::PluginWindowHandle window);
  virtual ~NativeViewGLSurfaceOSMesa();

  // Initializes the GL context.
  bool Initialize();

  // Implement subset of GLSurface.
  virtual void Destroy();
  virtual bool IsOffscreen();
  virtual bool SwapBuffers();

 private:
  void UpdateSize();

  gfx::PluginWindowHandle window_;
  HDC device_context_;

  DISALLOW_COPY_AND_ASSIGN(NativeViewGLSurfaceOSMesa);
};

// Helper routine that does one-off initialization like determining the
// pixel format and initializing the GL bindings.
bool GLSurface::InitializeOneOff() {
  static bool initialized = false;
  if (initialized)
    return true;

  static const GLImplementation kAllowedGLImplementations[] = {
    kGLImplementationEGLGLES2,
    kGLImplementationDesktopGL,
    kGLImplementationOSMesaGL
  };

  if (!InitializeRequestedGLBindings(
           kAllowedGLImplementations,
           kAllowedGLImplementations + arraysize(kAllowedGLImplementations),
           kGLImplementationEGLGLES2)) {
    LOG(ERROR) << "InitializeRequestedGLBindings failed.";
    return false;
  }

  switch (GetGLImplementation()) {
    case kGLImplementationDesktopGL:
      if (!GLSurfaceWGL::InitializeOneOff()) {
        LOG(ERROR) << "GLSurfaceWGL::InitializeOneOff failed.";
        return false;
      }
      break;
    case kGLImplementationEGLGLES2:
      if (!GLSurfaceEGL::InitializeOneOff()) {
        LOG(ERROR) << "GLSurfaceEGL::InitializeOneOff failed.";
        return false;
      }
      break;
  }

  initialized = true;
  return true;
}

NativeViewGLSurfaceOSMesa::NativeViewGLSurfaceOSMesa(
    gfx::PluginWindowHandle window)
  : GLSurfaceOSMesa(OSMESA_RGBA, gfx::Size()),
    window_(window),
    device_context_(NULL) {
  DCHECK(window);
}

NativeViewGLSurfaceOSMesa::~NativeViewGLSurfaceOSMesa() {
  Destroy();
}

bool NativeViewGLSurfaceOSMesa::Initialize() {
  if (!GLSurfaceOSMesa::Initialize())
    return false;

  device_context_ = GetDC(window_);
  UpdateSize();
  return true;
}

void NativeViewGLSurfaceOSMesa::Destroy() {
  if (window_ && device_context_)
    ReleaseDC(window_, device_context_);

  device_context_ = NULL;

  GLSurfaceOSMesa::Destroy();
}

bool NativeViewGLSurfaceOSMesa::IsOffscreen() {
  return false;
}

bool NativeViewGLSurfaceOSMesa::SwapBuffers() {
  DCHECK(device_context_);

  // Update the size before blitting so that the blit size is exactly the same
  // as the window.
  UpdateSize();

  gfx::Size size = GetSize();

  // Note: negating the height below causes GDI to treat the bitmap data as row
  // 0 being at the top.
  BITMAPV4HEADER info = { sizeof(BITMAPV4HEADER) };
  info.bV4Width = size.width();
  info.bV4Height = -size.height();
  info.bV4Planes = 1;
  info.bV4BitCount = 32;
  info.bV4V4Compression = BI_BITFIELDS;
  info.bV4RedMask = 0x000000FF;
  info.bV4GreenMask = 0x0000FF00;
  info.bV4BlueMask = 0x00FF0000;
  info.bV4AlphaMask = 0xFF000000;

  // Copy the back buffer to the window's device context. Do not check whether
  // StretchDIBits succeeds or not. It will fail if the window has been
  // destroyed but it is preferable to allow rendering to silently fail if the
  // window is destroyed. This is because the primary application of this
  // class of GLContext is for testing and we do not want every GL related ui /
  // browser test to become flaky if there is a race condition between GL
  // context destruction and window destruction.
  StretchDIBits(device_context_,
                0, 0, size.width(), size.height(),
                0, 0, size.width(), size.height(),
                GetHandle(),
                reinterpret_cast<BITMAPINFO*>(&info),
                DIB_RGB_COLORS,
                SRCCOPY);

  return true;
}

void NativeViewGLSurfaceOSMesa::UpdateSize() {
  // Change back buffer size to that of window. If window handle is invalid, do
  // not change the back buffer size.
  RECT rect;
  if (!GetClientRect(window_, &rect))
    return;

  gfx::Size window_size = gfx::Size(
    std::max(1, static_cast<int>(rect.right - rect.left)),
    std::max(1, static_cast<int>(rect.bottom - rect.top)));
  Resize(window_size);
}

GLSurface* GLSurface::CreateViewGLSurface(gfx::PluginWindowHandle window) {
  switch (GetGLImplementation()) {
    case kGLImplementationOSMesaGL: {
      scoped_ptr<NativeViewGLSurfaceOSMesa> surface(
          new NativeViewGLSurfaceOSMesa(window));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationEGLGLES2: {
      scoped_ptr<NativeViewGLSurfaceEGL> surface(new NativeViewGLSurfaceEGL(
          window));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationDesktopGL: {
      scoped_ptr<NativeViewGLSurfaceWGL> surface(new NativeViewGLSurfaceWGL(
          window));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationMockGL:
      return new GLSurfaceStub;
    default:
      NOTREACHED();
      return NULL;
  }
}

GLSurface* GLSurface::CreateOffscreenGLSurface(const gfx::Size& size) {
  switch (GetGLImplementation()) {
    case kGLImplementationOSMesaGL: {
      scoped_ptr<GLSurfaceOSMesa> surface(new GLSurfaceOSMesa(OSMESA_RGBA,
                                                              size));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationEGLGLES2: {
      scoped_ptr<PbufferGLSurfaceEGL> surface(new PbufferGLSurfaceEGL(size));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationDesktopGL: {
      scoped_ptr<PbufferGLSurfaceWGL> surface(new PbufferGLSurfaceWGL(size));
      if (!surface->Initialize())
        return NULL;

      return surface.release();
    }
    case kGLImplementationMockGL:
      return new GLSurfaceStub;
    default:
      NOTREACHED();
      return NULL;
  }
}

}  // namespace gfx
