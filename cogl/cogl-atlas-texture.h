/*
 * Cogl
 *
 * An object oriented GL/GLES Abstraction/Utility Layer
 *
 * Copyright (C) 2013 Intel Corporation.
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
 */

#if !defined(__COGL_H_INSIDE__) && !defined(COGL_COMPILATION)
#error "Only <cogl/cogl.h> can be included directly."
#endif

#ifndef _COGL_ATLAS_TEXTURE_H_
#define _COGL_ATLAS_TEXTURE_H_

#include <cogl/cogl-context.h>

COGL_BEGIN_DECLS

/**
 * SECTION:cogl-atlas-texture
 * @short_description: Functions for managing textures in Cogl's global
 *                     set of texture atlases
 *
 * A texture atlas is a texture that contains many smaller images that
 * an application is interested in. These are packed together as a way
 * of optimizing drawing with those images by avoiding the costs of
 * repeatedly telling the hardware to change what texture it should
 * sample from.  This can enable more geometry to be batched together
 * into few draw calls.
 *
 * Each #CoglContext has an shared, pool of texture atlases that are
 * are managed by Cogl.
 *
 * This api lets applications upload texture data into one of Cogl's
 * shared texture atlases using a high-level #CoglAtlasTexture which
 * represents a sub-region of one of these atlases.
 *
 * <note>A #CoglAtlasTexture is a high-level meta texture which has
 * some limitations to be aware of. Please see the documentation for
 * #CoglMetaTexture for more details.</note>
 */


typedef struct _CoglAtlasTexture CoglAtlasTexture;
#define COGL_ATLAS_TEXTURE(tex) ((CoglAtlasTexture *) tex)


/**
 * cogl_atlas_texture_new_with_size:
 * @ctx: A #CoglContext
 * @width: The width of your atlased texture.
 * @height: The height of your atlased texture.
 *
 * Creates a #CoglAtlasTexture with a given @width and @height. A
 * #CoglAtlasTexture represents a sub-region within one of Cogl's
 * shared texture atlases.
 *
 * The storage for the texture is not allocated before this function
 * returns. You can call cogl_texture_allocate() to explicitly
 * allocate the underlying storage or let Cogl automatically allocate
 * storage lazily.
 *
 * The texture is still configurable until it has been allocated so
 * for example you can influence the internal format of the texture
 * using cogl_texture_set_components() and
 * cogl_texture_set_premultiplied().
 *
 * <note>Allocate call can fail if Cogl considers the internal
 * format to be incompatible with the format of its internal
 * atlases.</note>
 *
 * <note>The returned #CoglAtlasTexture is a high-level meta-texture
 * with some limitations. See the documentation for #CoglMetaTexture
 * for more details.</note>
 *
 * Returns: (transfer full): A new #CoglAtlasTexture object.
 * Since: 1.16
 * Stability: unstable
 */
CoglAtlasTexture *
cogl_atlas_texture_new_with_size (CoglContext *ctx,
                                  int width,
                                  int height);

/**
 * cogl_atlas_texture_new_from_file:
 * @ctx: A #CoglContext
 * @filename: the file to load
 * @error: A #CoglError to catch exceptional errors or %NULL
 *
 * Creates a #CoglAtlasTexture from an image file. A #CoglAtlasTexture
 * represents a sub-region within one of Cogl's shared texture
 * atlases.
 *
 * The storage for the texture is not allocated before this function
 * returns. You can call cogl_texture_allocate() to explicitly
 * allocate the underlying storage or let Cogl automatically allocate
 * storage lazily.
 *
 * The texture is still configurable until it has been allocated so
 * for example you can influence the internal format of the texture
 * using cogl_texture_set_components() and
 * cogl_texture_set_premultiplied().
 *
 * <note>Allocate call can fail if Cogl considers the internal
 * format to be incompatible with the format of its internal
 * atlases.</note>
 *
 * <note>The returned #CoglAtlasTexture is a high-level meta-texture
 * with some limitations. See the documentation for #CoglMetaTexture
 * for more details.</note>
 *
 * Return value: (transfer full): A new #CoglAtlasTexture object or
 *          %NULL on failure and @error will be updated.
 * Since: 1.16
 * Stability: unstable
 */
CoglAtlasTexture *
cogl_atlas_texture_new_from_file (CoglContext *ctx,
                                  const char *filename,
                                  CoglError **error);

/**
 * cogl_atlas_texture_new_from_data:
 * @ctx: A #CoglContext
 * @width: width of texture in pixels
 * @height: height of texture in pixels
 * @format: the #CoglPixelFormat the buffer is stored in in RAM
 * @rowstride: the memory offset in bytes between the start of each
 *    row in @data. A value of 0 will make Cogl automatically
 *    calculate @rowstride from @width and @format.
 * @data: pointer to the memory region where the source buffer resides
 * @error: A #CoglError to catch exceptional errors or %NULL
 *
 * Creates a new #CoglAtlasTexture texture based on data residing in
 * memory. A #CoglAtlasTexture represents a sub-region within one of
 * Cogl's shared texture atlases.
 *
 * <note>This api will always immediately allocate GPU memory for the
 * texture and upload the given data so that the @data pointer does
 * not need to remain valid once this function returns. This means it
 * is not possible to configure the texture before it is allocated. If
 * you do need to configure the texture before allocation (to specify
 * constraints on the internal format for example) then you can
 * instead create a #CoglBitmap for your data and use
 * cogl_atlas_texture_new_from_bitmap() or use
 * cogl_atlas_texture_new_with_size() and then upload data using
 * cogl_texture_set_data()</note>
 *
 * <note>Allocate call can fail if Cogl considers the internal
 * format to be incompatible with the format of its internal
 * atlases.</note>
 *
 * <note>The returned #CoglAtlasTexture is a high-level
 * meta-texture with some limitations. See the documentation for
 * #CoglMetaTexture for more details.</note>
 *
 * Return value: (transfer full): A new #CoglAtlasTexture object or
 *          %NULL on failure and @error will be updated.
 * Since: 1.16
 * Stability: unstable
 */
CoglAtlasTexture *
cogl_atlas_texture_new_from_data (CoglContext *ctx,
                                  int width,
                                  int height,
                                  CoglPixelFormat format,
                                  int rowstride,
                                  const uint8_t *data,
                                  CoglError **error);

/**
 * cogl_atlas_texture_new_from_bitmap:
 * @bitmap: A #CoglBitmap
 *
 * Creates a new #CoglAtlasTexture texture based on data residing in a
 * @bitmap. A #CoglAtlasTexture represents a sub-region within one of
 * Cogl's shared texture atlases.
 *
 * The storage for the texture is not allocated before this function
 * returns. You can call cogl_texture_allocate() to explicitly
 * allocate the underlying storage or preferably let Cogl
 * automatically allocate storage lazily when it may know more about
 * how the texture is being used and can optimize how it is allocated.
 *
 * The texture is still configurable until it has been allocated so
 * for example you can influence the internal format of the texture
 * using cogl_texture_set_components() and
 * cogl_texture_set_premultiplied().
 *
 * <note>Allocate call can fail if Cogl considers the internal
 * format to be incompatible with the format of its internal
 * atlases.</note>
 *
 * <note>The returned #CoglAtlasTexture is a high-level meta-texture
 * with some limitations. See the documentation for #CoglMetaTexture
 * for more details.</note>
 *
 * Returns: (transfer full): A new #CoglAtlasTexture object.
 * Since: 1.16
 * Stability: unstable
 */
CoglAtlasTexture *
cogl_atlas_texture_new_from_bitmap (CoglBitmap *bmp);

/**
 * cogl_is_atlas_texture:
 * @object: a #CoglObject
 *
 * Checks whether the given object references a #CoglAtlasTexture
 *
 * Return value: %TRUE if the passed object represents an atlas
 *   texture and %FALSE otherwise
 *
 * Since: 1.16
 * Stability: Unstable
 */
CoglBool
cogl_is_atlas_texture (void *object);

COGL_END_DECLS

#endif /* _COGL_ATLAS_TEXTURE_H_ */
