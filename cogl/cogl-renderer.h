/*
 * Cogl
 *
 * An object oriented GL/GLES Abstraction/Utility Layer
 *
 * Copyright (C) 2007,2008,2009 Intel Corporation.
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#if !defined(__COGL_H_INSIDE__) && !defined(CLUTTER_COMPILATION)
#error "Only <cogl/cogl.h> can be included directly."
#endif

#ifndef __COGL_RENDERER_H__
#define __COGL_RENDERER_H__

#include <glib.h>

#include <cogl/cogl-types.h>
#include <cogl/cogl-onscreen-template.h>

#if COGL_HAS_EGL_PLATFORM_WAYLAND_SUPPORT
#include <wayland-client.h>
#endif

G_BEGIN_DECLS

/**
 * SECTION:cogl-renderer
 * @short_description:
 *
 */

#define cogl_renderer_error_quark cogl_renderer_error_quark_EXP

#define COGL_RENDERER_ERROR cogl_renderer_error_quark ()
GQuark
cogl_renderer_error_quark (void);

typedef struct _CoglRenderer CoglRenderer;

#define cogl_is_renderer cogl_is_renderer_EXP
gboolean
cogl_is_renderer (void *object);

#define cogl_renderer_new cogl_renderer_new_EXP
CoglRenderer *
cogl_renderer_new (void);

/* optional configuration APIs */

/**
 * CoglWinsysID:
 * @COGL_WINSYS_ID_ANY: Implies no preference for which backend is used
 * @COGL_WINSYS_ID_STUB: Use the no-op stub backend
 * @COGL_WINSYS_ID_GLX: Use the GLX window system binding API
 * @COGL_WINSYS_ID_EGL: Use the Khronos EGL window system binding API
 * @COGL_WINSYS_ID_WGL: Use the Microsoft Windows WGL binding API
 *
 * Identifies specific window system backends that Cogl supports.
 *
 * These can be used to query what backend Cogl is using or to try and
 * explicitly select a backend to use.
 */
typedef enum
{
  COGL_WINSYS_ID_ANY,
  COGL_WINSYS_ID_STUB,
  COGL_WINSYS_ID_GLX,
  COGL_WINSYS_ID_EGL,
  COGL_WINSYS_ID_WGL
} CoglWinsysID;

/**
 * cogl_renderer_set_winsys_id:
 * @renderer: A #CoglRenderer
 * @winsys_id: An ID of the winsys you explicitly want to use.
 *
 * This allows you to explicitly select a winsys backend to use instead
 * of letting Cogl automatically select a backend.
 *
 * if you select an unsupported backend then cogl_renderer_connect()
 * will fail and report an error.
 *
 * This may only be called on an un-connected #CoglRenderer.
 */
#define cogl_renderer_set_winsys_id cogl_renderer_set_winsys_id_EXP
void
cogl_renderer_set_winsys_id (CoglRenderer *renderer,
                             CoglWinsysID winsys_id);

/**
 * cogl_renderer_get_winsys_id:
 * @renderer: A #CoglRenderer
 *
 * Queries which window system backend Cogl has chosen to use.
 *
 * This may only be called on a connected #CoglRenderer.
 *
 * Returns: The #CoglWinsysID corresponding to the chosen window
 *          system backend.
 */
#define cogl_renderer_get_winsys_id cogl_renderer_get_winsys_id_EXP
CoglWinsysID
cogl_renderer_get_winsys_id (CoglRenderer *renderer);

#if COGL_HAS_EGL_PLATFORM_WAYLAND_SUPPORT
#define cogl_renderer_wayland_set_foreign_display \
  cogl_renderer_wayland_set_foreign_display_EXP
void
cogl_renderer_wayland_set_foreign_display (CoglRenderer *renderer,
                                           struct wl_display *display);

#define cogl_renderer_wayland_get_display \
  cogl_renderer_wayland_get_display_EXP
struct wl_display *
cogl_renderer_wayland_get_display (CoglRenderer *renderer);

#define cogl_renderer_wayland_set_foreign_compositor \
  cogl_renderer_wayland_set_foreign_compositor_EXP
void
cogl_renderer_wayland_set_foreign_compositor (CoglRenderer *renderer,
                                              struct wl_compositor *compositor);

#define cogl_renderer_wayland_get_compositor \
  cogl_renderer_wayland_get_compositor_EXP
struct wl_compositor *
cogl_renderer_wayland_get_compositor (CoglRenderer *renderer);
#endif /* COGL_HAS_EGL_PLATFORM_WAYLAND_SUPPORT */

#ifdef COGL_HAS_WGL_SUPPORT

#define cogl_win32_renderer_handle_event \
  cogl_win32_renderer_handle_event_EXP
/*
 * cogl_win32_renderer_handle_event:
 * @message: A pointer to a win32 MSG struct
 *
 * This function processes a single event; it can be used to hook into
 * external event retrieval (for example that done by Clutter or
 * GDK).
 *
 * Return value: #CoglFilterReturn. %COGL_FILTER_REMOVE indicates that
 * Cogl has internally handled the event and the caller should do no
 * further processing. %COGL_FILTER_CONTINUE indicates that Cogl is
 * either not interested in the event, or has used the event to update
 * internal state without taking any exclusive action.
 */
CoglFilterReturn
cogl_win32_renderer_handle_event (CoglRenderer *renderer,
                                  MSG *message);

/*
 * CoglXlibFilterFunc:
 * @message: A pointer to a win32 MSG struct
 * @data: The data that was given when the filter was added
 *
 * A callback function that can be registered with
 * cogl_win32_renderer_add_filter(). The function should return
 * %COGL_FILTER_REMOVE if it wants to prevent further processing or
 * %COGL_FILTER_CONTINUE otherwise.
 */
typedef CoglFilterReturn (* CoglWin32FilterFunc) (MSG *message,
                                                  void *data);

#define cogl_win32_renderer_add_filter cogl_win32_renderer_add_filter_EXP
/*
 * cogl_win32_renderer_add_filter:
 *
 * Adds a callback function that will receive all native events. The
 * function can stop further processing of the event by return
 * %COGL_FILTER_REMOVE.
 */
void
cogl_win32_renderer_add_filter (CoglRenderer *renderer,
                                CoglWin32FilterFunc func,
                                void *data);

#define cogl_win32_renderer_remove_filter \
  cogl_win32_renderer_remove_filter_EXP
/*
 * cogl_win32_renderer_remove_filter:
 *
 * Removes a callback that was previously added with
 * cogl_win32_renderer_add_filter().
 */
void
cogl_win32_renderer_remove_filter (CoglRenderer *renderer,
                                   CoglWin32FilterFunc func,
                                   void *data);
#endif /* COGL_HAS_WGL_SUPPORT */

#define cogl_renderer_check_onscreen_template \
  cogl_renderer_check_onscreen_template_EXP
gboolean
cogl_renderer_check_onscreen_template (CoglRenderer *renderer,
                                       CoglOnscreenTemplate *onscreen_template,
                                       GError **error);

/* Final connection API */

#define cogl_renderer_connect cogl_renderer_connect_EXP
gboolean
cogl_renderer_connect (CoglRenderer *renderer, GError **error);

G_END_DECLS

#endif /* __COGL_RENDERER_H__ */

