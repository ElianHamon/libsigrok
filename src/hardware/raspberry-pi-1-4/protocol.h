/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2024 Elian Hamon <156338926+ElianHamon@users.noreply.github.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSIGROK_HARDWARE_RASPBERRY_PI_1_4_PROTOCOL_H
#define LIBSIGROK_HARDWARE_RASPBERRY_PI_1_4_PROTOCOL_H

#include <stdint.h>
#include <glib.h>
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"

#define LOG_PREFIX "raspberry-pi-1-4"

struct dev_context {
    uint64_t sample_rate;
};

SR_PRIV int raspberry_pi_1_4_receive_data(int fd, int revents, void *cb_data);

#endif
