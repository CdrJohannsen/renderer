/*
 * largely based on swc: libswc/shm.c
 *
 * Copyright (c) 2013-2020 Michael Forney
 *
 * Based in part upon wayland-shm.c from wayland, which is:
 *
 *     Copyright © 2008 Kristian Høgsberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "wayland/shm.h"
#include "wayland/buffer.h"
#include "wayland/util.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-server.h>
#include <cstring>
#include <iostream>

struct pool {
    struct wl_resource *resource;
    struct wl3d_shm *shm;
    void *data;
    uint32_t size;
    unsigned references;
};

struct pool_reference {
    struct pool *pool;
};

static void unref_pool(struct pool *pool) {
    if (--pool->references > 0) return;

    munmap(pool->data, pool->size);
    delete pool;
}

static void destroy_pool_resource(struct wl_resource *resource) {
    struct pool *pool = static_cast<struct pool *>(wl_resource_get_user_data(resource));
    unref_pool(pool);
}

static void handle_buffer_destroy(struct wld_destructor *destructor) {
    std::cout << "handle_buffer_destroy" << std::endl;
    // TODO: Add destructor
    // struct pool_reference *reference = wl_container_of(destructor, reference, destructor);
    // unref_pool(reference->pool);
}

static void create_buffer(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t offset,
                          int32_t width, int32_t height, int32_t stride, uint32_t format) {
    struct pool *pool = static_cast<struct pool *>(wl_resource_get_user_data(resource));
    struct pool_reference *reference;
    struct wl3d_buffer *buffer;
    struct wl_resource *buffer_resource;

    if (offset > pool->size || offset < 0) {
        wl_resource_post_error(resource, WL_SHM_ERROR_INVALID_STRIDE, "offset is too big or negative");
        return;
    }

    buffer = (struct wl3d_buffer *)malloc(sizeof(*buffer));
    if (!buffer) goto error0;

    buffer->data = (uint8_t *)((uintptr_t)pool->data + offset);
    buffer->format = static_cast<wl_shm_format>(format);
    buffer->height = height;
    buffer->width = width;
    buffer->stride = stride;

    buffer_resource = wayland_buffer_create_resource(client, wl_resource_get_version(resource), id, buffer);

    if (!buffer_resource) goto error1;

    if (!(reference = (struct pool_reference *)malloc(sizeof(*reference)))) goto error2;

    reference->pool = pool;
    // TODO: Add destructor
    // buffer->extra_destructor = &handle_buffer_destroy;
    ++pool->references;

    return;

error2:
    wl_resource_destroy(buffer_resource);
error1:
    // wld_buffer_unreference(buffer);
    // TODO: Destructor like in destroy_buffer
error0:
    wl_resource_post_no_memory(resource);
}

static void resize(struct wl_client *client, struct wl_resource *resource, int32_t size) {
    struct pool *pool = static_cast<struct pool *>(wl_resource_get_user_data(resource));
    void *data;

    data = mremap(pool->data, pool->size, size, MREMAP_MAYMOVE);
    if (data == MAP_FAILED) {
        wl_resource_post_error(resource, WL_SHM_ERROR_INVALID_FD, "mremap failed: %s", strerror(errno));
        return;
    }
    pool->data = data;
    pool->size = size;
}

static const struct wl_shm_pool_interface shm_pool_impl = {
    .create_buffer = create_buffer,
    .destroy = destroy_resource,
    .resize = resize,
};

static void create_pool(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t fd, int32_t size) {
    struct wl3d_shm *shm = static_cast<struct wl3d_shm *>(wl_resource_get_user_data(resource));
    struct pool *pool;

    pool = new struct pool();
    if (!pool) {
        wl_resource_post_no_memory(resource);
        goto error0;
    }
    pool->shm = shm;
    pool->resource = wl_resource_create(client, &wl_shm_pool_interface, wl_resource_get_version(resource), id);
    if (!pool->resource) {
        wl_resource_post_no_memory(resource);
        goto error1;
    }
    wl_resource_set_implementation(pool->resource, &shm_pool_impl, pool, &destroy_pool_resource);
    pool->data = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (pool->data == MAP_FAILED) {
        wl_resource_post_error(resource, WL_SHM_ERROR_INVALID_FD, "mmap failed: %s", strerror(errno));
        goto error2;
    }
    close(fd);
    pool->size = size;
    pool->references = 1;
    return;

error2:
    wl_resource_destroy(pool->resource);
error1:
    delete pool;
error0:
    close(fd);
}

void release_shm(struct wl_client *client, struct wl_resource *resource) {}

static const struct wl_shm_interface shm_impl = {.create_pool = create_pool, .release = release_shm};

static void bind_shm(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl3d_shm *shm = static_cast<struct wl3d_shm *>(data);
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_shm_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &shm_impl, shm, nullptr);

    wl_shm_send_format(resource, WL_SHM_FORMAT_ARGB8888);
    wl_shm_send_format(resource, WL_SHM_FORMAT_XRGB8888);
    wl_shm_send_format(resource, WL_SHM_FORMAT_RGBA8888);
}

struct wl3d_shm *shm_create(struct wl_display *display) {
    struct wl3d_shm *shm = nullptr;

    shm = new struct wl3d_shm();
    if (shm) {
        shm->global = wl_global_create(display, &wl_shm_interface, 2, shm, &bind_shm);
        if (!shm->global) {
            delete shm;
            shm = nullptr;
        }
    }
    return shm;

error1:
    delete shm;
error0:
    return NULL;
}

void shm_destroy(struct wl3d_shm *shm) {
    wl_global_destroy(shm->global);
    delete shm;
}
