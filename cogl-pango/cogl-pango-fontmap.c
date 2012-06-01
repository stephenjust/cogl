/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2008 OpenedHand
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
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:cogl-pango
 * @short_description: COGL-based text rendering using Pango
 *
 * FIXME
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* This is needed to get the Pango headers to export stuff needed to
   subclass */
#ifndef PANGO_ENABLE_BACKEND
#define PANGO_ENABLE_BACKEND 1
#endif

#include <pango/pango-fontmap.h>
#include <pango/pangocairo.h>
#include <pango/pango-renderer.h>

#include "cogl-pango.h"
#include "cogl-pango-private.h"
#include "cogl-util.h"

static GQuark cogl_pango_font_map_get_priv_key (void) G_GNUC_CONST;

typedef struct _CoglPangoFontMapPriv
{
  CoglContext *ctx;
  PangoRenderer *renderer;
} CoglPangoFontMapPriv;

static void
free_priv (gpointer data)
{
  CoglPangoFontMapPriv *priv = data;

  cogl_object_unref (priv->ctx);
  g_object_unref (priv->renderer);

  g_free (priv);
}

PangoFontMap *
cogl_pango_font_map_new (CoglContext *context)
{
  PangoFontMap *fm = pango_cairo_font_map_new ();
  CoglPangoFontMapPriv *priv = g_new0 (CoglPangoFontMapPriv, 1);

  priv->ctx = cogl_object_ref (context);

  /* XXX: The public pango api doesn't let us sub-class
   * PangoCairoFontMap so we attach our own private data using qdata
   * for now. */
  g_object_set_qdata_full (G_OBJECT (fm),
                           cogl_pango_font_map_get_priv_key (),
                           priv,
                           free_priv);

  return fm;
}

static CoglPangoFontMapPriv *
_cogl_pango_font_map_get_priv (CoglPangoFontMap *fm)
{
  return g_object_get_qdata (G_OBJECT (fm),
			     cogl_pango_font_map_get_priv_key ());
}

PangoRenderer *
_cogl_pango_font_map_get_renderer (CoglPangoFontMap *fm)
{
  CoglPangoFontMapPriv *priv = _cogl_pango_font_map_get_priv (fm);
  if (G_UNLIKELY (!priv->renderer))
    priv->renderer = _cogl_pango_renderer_new (priv->ctx);
  return priv->renderer;
}

CoglContext *
_cogl_pango_font_map_get_cogl_context (CoglPangoFontMap *fm)
{
  CoglPangoFontMapPriv *priv = _cogl_pango_font_map_get_priv (fm);
  return priv->ctx;
}

void
cogl_pango_font_map_set_resolution (CoglPangoFontMap *font_map,
				    double            dpi)
{
  _COGL_RETURN_IF_FAIL (COGL_PANGO_IS_FONT_MAP (font_map));

  pango_cairo_font_map_set_resolution (PANGO_CAIRO_FONT_MAP (font_map), dpi);
}

void
cogl_pango_font_map_clear_glyph_cache (CoglPangoFontMap *fm)
{
  PangoRenderer *renderer = _cogl_pango_font_map_get_renderer (fm);

  _cogl_pango_renderer_clear_glyph_cache (COGL_PANGO_RENDERER (renderer));
}

void
cogl_pango_font_map_set_use_mipmapping (CoglPangoFontMap *fm,
                                        CoglBool          value)
{
  PangoRenderer *renderer = _cogl_pango_font_map_get_renderer (fm);

  _cogl_pango_renderer_set_use_mipmapping (COGL_PANGO_RENDERER (renderer),
                                           value);
}

CoglBool
cogl_pango_font_map_get_use_mipmapping (CoglPangoFontMap *fm)
{
  PangoRenderer *renderer = _cogl_pango_font_map_get_renderer (fm);

  return
    _cogl_pango_renderer_get_use_mipmapping (COGL_PANGO_RENDERER (renderer));
}

static GQuark
cogl_pango_font_map_get_priv_key (void)
{
  static GQuark priv_key = 0;

  if (G_UNLIKELY (priv_key == 0))
      priv_key = g_quark_from_static_string ("CoglPangoFontMap");

  return priv_key;
}
