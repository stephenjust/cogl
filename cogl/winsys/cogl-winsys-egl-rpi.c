/*
 * Cogl
 *
 * An object oriented GL/GLES Abstraction/Utility Layer
 *
 * Copyright (C) 2011 Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 *
 * Authors:
 *   Robert Bragg <robert@linux.intel.com>
 *   Neil Roberts <neil@linux.intel.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bcm_host.h>
#include <stdio.h>

#include "cogl-winsys-egl-rpi-private.h"
#include "cogl-winsys-egl-private.h"
#include "cogl-renderer-private.h"
#include "cogl-framebuffer-private.h"
#include "cogl-onscreen-private.h"
#include "cogl-private.h"

static const CoglWinsysEGLVtable _cogl_winsys_egl_vtable;

typedef struct _CoglDisplayRpi
{
  int32_t egl_surface_width;
  int32_t egl_surface_height;
  CoglBool have_onscreen;
} CoglDisplayRpi;

static EGL_DISPMANX_WINDOW_T *native_window;
static DISPMANX_ELEMENT_HANDLE_T dispman_element = DISPMANX_NO_HANDLE;
static DISPMANX_DISPLAY_HANDLE_T dispman_display = DISPMANX_NO_HANDLE;
static DISPMANX_UPDATE_HANDLE_T dispman_update = DISPMANX_NO_HANDLE;

void
cogl_rpi_set_native_window (EGL_DISPMANX_WINDOW_T *window)
{
  _cogl_init ();

  native_window = window;
}

static void
_cogl_winsys_renderer_disconnect (CoglRenderer *renderer)
{
  CoglRendererEGL *egl_renderer = renderer->winsys;

  eglTerminate (egl_renderer->edpy);
  bcm_host_deinit();
  g_slice_free (CoglRendererEGL, egl_renderer);
}

static CoglBool
_cogl_winsys_renderer_connect (CoglRenderer *renderer,
                               CoglError **error)
{
  printf("Rpi: _cogl_winsys_renderer_connect()\n");
  bcm_host_init();
  CoglRendererEGL *egl_renderer;

  renderer->winsys = g_slice_new0 (CoglRendererEGL);
  egl_renderer = renderer->winsys;
  renderer->driver = COGL_DRIVER_GLES2;

  egl_renderer->platform_vtable = &_cogl_winsys_egl_vtable;

  egl_renderer->edpy = eglGetDisplay (EGL_DEFAULT_DISPLAY);
  if (egl_renderer->edpy == EGL_NO_DISPLAY) {
    printf("Failed to get display\n");
    goto error;
  }

  if (!eglInitialize (egl_renderer->edpy, NULL, NULL))
    {
      _cogl_set_error (error, COGL_WINSYS_ERROR,
                   COGL_WINSYS_ERROR_INIT,
                   "Couldn't initialize EGL");
      goto error;
    }

  return TRUE;

error:
  _cogl_winsys_renderer_disconnect (renderer);
  return FALSE;
}

static int
_cogl_winsys_egl_add_config_attributes (CoglDisplay *display,
					CoglFramebufferConfig *config,
					EGLint *attb) {
  printf("Rpi: _cogl_winsys_egl_add_config_attributes()\n");
  int i = 0;
  attb[i++] = EGL_RED_SIZE;
  attb[i++] = 8;
  attb[i++] = EGL_BLUE_SIZE;
  attb[i++] = 8;
  attb[i++] = EGL_GREEN_SIZE;
  attb[i++] = 8;
  attb[i++] = EGL_ALPHA_SIZE;
  attb[i++] = 8;
  attb[i++] = EGL_DEPTH_SIZE;
  attb[i++] = 16;
  attb[i++] = EGL_SAMPLES;
  attb[i++] = 4;
  attb[i++] = EGL_SURFACE_TYPE;
  attb[i++] = EGL_WINDOW_BIT;
  attb[i++] = EGL_NONE;
  return i;
}

static CoglBool
_cogl_winsys_egl_context_created (CoglDisplay *display,
                                  CoglError **error)
{
  printf("Rpi: _cogl_winsys_egl_context_created()\n");
  CoglRenderer *renderer = display->renderer;
  CoglRendererEGL *egl_renderer = renderer->winsys;
  CoglDisplayEGL *egl_display = display->winsys;
  CoglDisplayRpi *rpi_display = egl_display->platform;
  const char *error_message;
  EGLint format;
  VC_RECT_T src_rect;
  VC_RECT_T dst_rect;
  DISPMANX_MODEINFO_T dispman_info;
  EGL_DISPMANX_WINDOW_T window;
  EGLint err;

  // Open screen
  if (dispman_display == DISPMANX_NO_HANDLE) {
    printf("Opening dispman display");
    dispman_display = vc_dispmanx_display_open(0 /* LCD */);
  }
  if (dispman_update == DISPMANX_NO_HANDLE) {
    printf("Starting dispman update");
    dispman_update = vc_dispmanx_update_start(0);
    if (dispman_update == DISPMANX_NO_HANDLE) {
      error_message = "Failed to start dispman update";
      goto fail;
    }
  }
  if (vc_dispmanx_display_get_info(dispman_display, &dispman_info)) {
    error_message = "Unable to get display info";
    goto fail;
  }

  rpi_display->egl_surface_width = dispman_info.width;
  rpi_display->egl_surface_height = dispman_info.height;
  printf("Display is %i x %i\n", dispman_info.width, dispman_info.height);


  // Create native window
  vc_dispmanx_rect_set(&dst_rect, 0, 0,
		       rpi_display->egl_surface_width,
		       rpi_display->egl_surface_height);
  vc_dispmanx_rect_set(&src_rect, 0, 0,
		       rpi_display->egl_surface_width << 16,
		       rpi_display->egl_surface_height << 16);
  if (dispman_element == DISPMANX_NO_HANDLE) {
    printf("Creating new dispman element\n");
    dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,
					      0 /* layer */, &dst_rect, 0 /* src */,
					      &src_rect, DISPMANX_PROTECTION_NONE,
					      0 /* alpha */, 0 /* clamp */, 0 /* transform */);
  } else {
    printf("Not creating dispman element\n");
  }

  window.element = dispman_element;
  window.width = dispman_info.width;
  window.height = dispman_info.height;

  printf("Start dispman update\n");
  vc_dispmanx_update_submit_sync(dispman_update);

  // Check EGLConfig
  if (egl_display->egl_config == NULL) {
    error_message = "EGLConfig is null!";
    goto fail;
  }
  // Check edpy
  if (egl_renderer->edpy == NULL) {
    error_message = "edpy is null!";
    goto fail;
  }

  egl_display->egl_surface =
    eglCreateWindowSurface (egl_renderer->edpy,
                            egl_display->egl_config,
                            &window,
                            NULL);
  err = eglGetError();
  if (err == EGL_BAD_DISPLAY) {
    error_message = "Bad display creating egl surface";
    goto fail;
  } else if (err == EGL_NOT_INITIALIZED) {
    error_message = "Egl not initialized before creating surface";
    goto fail;
  } else if (err == EGL_BAD_CONFIG) {
    error_message = "Bad egl config";
    goto fail;
  } else if (err == EGL_BAD_NATIVE_WINDOW) {
    error_message = "Bad native window";
    goto fail;
  } else if (err == EGL_BAD_ATTRIBUTE) {
    error_message = "Bad attribute";
    goto fail;
  } else if (err == EGL_BAD_ALLOC) {
    error_message = "Bad alloc";
    goto fail;
  } else if (err == EGL_BAD_MATCH) {
    error_message = "Bad match";
    goto fail;
  }
  if (egl_display->egl_surface == EGL_NO_SURFACE) {
    error_message = "Unable to create EGL window surface";
    goto fail;
  }

  if (!_cogl_winsys_egl_make_current (display,
                                      egl_display->egl_surface,
                                      egl_display->egl_surface,
                                      egl_display->egl_context))
    {
      error_message = "Unable to eglMakeCurrent with egl surface";
      goto fail;
    }

  // Clear screen
  printf("Clear screen\n");
  glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  printf("End clear screen\n");

  return TRUE;

 fail:
  _cogl_set_error (error, COGL_WINSYS_ERROR,
               COGL_WINSYS_ERROR_CREATE_CONTEXT,
               "%s", error_message);
  return FALSE;
}

static void
cleanup_context(CoglDisplay *display) {
  if (dispman_element != DISPMANX_NO_HANDLE)
    vc_dispmanx_element_remove(dispman_update, dispman_element);
  vc_dispmanx_display_close(0 /* LCD */);
}

static CoglBool
_cogl_winsys_egl_display_setup (CoglDisplay *display,
                                CoglError **error)
{
  printf("Rpi: _cogl_winsys_egl_display_setup()\n");
  CoglDisplayEGL *egl_display = display->winsys;
  CoglDisplayRpi *rpi_display;

  rpi_display = g_slice_new0 (CoglDisplayRpi);
  egl_display->platform = rpi_display;

  return TRUE;
}

static void
_cogl_winsys_egl_display_destroy (CoglDisplay *display)
{
  printf("Rpi: _cogl_winsys_egl_display_destroy()\n");
  CoglDisplayEGL *egl_display = display->winsys;

  g_slice_free (CoglDisplayRpi, egl_display->platform);
}

static CoglBool
_cogl_winsys_egl_onscreen_init (CoglOnscreen *onscreen,
                                EGLConfig egl_config,
                                CoglError **error)
{
  printf("Rpi: _cogl_winsys_egl_onscreen_init()\n");
  CoglFramebuffer *framebuffer = COGL_FRAMEBUFFER (onscreen);
  CoglContext *context = framebuffer->context;
  CoglDisplay *display = context->display;
  CoglDisplayEGL *egl_display = display->winsys;
  CoglDisplayRpi *rpi_display = egl_display->platform;
  CoglOnscreenEGL *egl_onscreen = onscreen->winsys;

  if (rpi_display->have_onscreen)
    {
      _cogl_set_error (error, COGL_WINSYS_ERROR,
                   COGL_WINSYS_ERROR_CREATE_ONSCREEN,
                   "EGL platform only supports a single onscreen window");
      return FALSE;
    }

  egl_onscreen->egl_surface = egl_display->egl_surface;

  _cogl_framebuffer_winsys_update_size (framebuffer,
                                        rpi_display->egl_surface_width,
                                        rpi_display->egl_surface_height);

  rpi_display->have_onscreen = TRUE;

  return TRUE;
}

static void
_cogl_winsys_onscreen_bind (CoglOnscreen *onscreen) {
  printf("Rpi: _cogl_winsys_onscreen_bind()\n");
  CoglFramebuffer *fb = COGL_FRAMEBUFFER (onscreen);
  CoglContext *context = fb->context;
  CoglDisplayEGL *egl_display = context->display->winsys;

  //CoglBool status = _cogl_winsys_egl_make_current (context->display,
  //                                                 egl_display->egl_surface,
  //                                                 egl_display->egl_surface,
  //                                                 egl_display->egl_context);
}

static const CoglWinsysEGLVtable
_cogl_winsys_egl_vtable =
  {
    .display_setup = _cogl_winsys_egl_display_setup,
    .display_destroy = _cogl_winsys_egl_display_destroy,
    .context_created = _cogl_winsys_egl_context_created,
    .cleanup_context = cleanup_context,
    .onscreen_init = _cogl_winsys_egl_onscreen_init,
    .add_config_attributes = _cogl_winsys_egl_add_config_attributes,
  };

const CoglWinsysVtable *
_cogl_winsys_egl_rpi_get_vtable (void)
{
  static CoglBool vtable_inited = FALSE;
  static CoglWinsysVtable vtable;

  if (!vtable_inited)
    {
      /* The EGL_RPI winsys is a subclass of the EGL winsys so we
         start by copying its vtable */

      vtable = *_cogl_winsys_egl_get_vtable ();

      vtable.id = COGL_WINSYS_ID_EGL_RPI;
      vtable.name = "EGL_RPI";

      vtable.renderer_connect = _cogl_winsys_renderer_connect;
      vtable.renderer_disconnect = _cogl_winsys_renderer_disconnect;
      vtable.onscreen_bind = _cogl_winsys_onscreen_bind;

      vtable_inited = TRUE;
    }

  return &vtable;
}
