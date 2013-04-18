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

#ifndef __COGL_POLL_PRIVATE_H__
#define __COGL_POLL_PRIVATE_H__

#include "cogl-poll.h"
#include "cogl-renderer.h"
#include "cogl-closure-list-private.h"

void
_cogl_poll_renderer_remove_fd (CoglRenderer *renderer, int fd);

void
_cogl_poll_renderer_add_fd (CoglRenderer *renderer,
                            int fd,
                            CoglPollFDEvent events);

typedef void (*CoglIdleCallback) (void *user_data);

CoglClosure *
_cogl_poll_renderer_add_idle (CoglRenderer *renderer,
                              CoglIdleCallback idle_cb,
                              void *user_data,
                              CoglUserDataDestroyCallback destroy_cb);

#endif /* __COGL_POLL_PRIVATE_H__ */