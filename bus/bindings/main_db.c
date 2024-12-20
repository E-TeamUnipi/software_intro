/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by cantools version 39.4.13 Mon Dec 16 13:40:02 2024.
 */

#include <string.h>

#include "main_db.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t unpack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) << shift);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

int main_iq_cmd_pack(
    uint8_t *dst_p,
    const struct main_iq_cmd_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->iq_rms, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->iq_rms, 8u, 0x07u);

    return (2);
}

int main_iq_cmd_unpack(
    struct main_iq_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->iq_rms = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->iq_rms |= unpack_left_shift_u16(src_p[1], 8u, 0x07u);

    return (0);
}

int main_iq_cmd_init(struct main_iq_cmd_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct main_iq_cmd_t));

    return 0;
}

uint16_t main_iq_cmd_iq_rms_encode(float value)
{
    return (uint16_t)(value / 0.1f);
}

float main_iq_cmd_iq_rms_decode(uint16_t value)
{
    return ((float)value * 0.1f);
}

bool main_iq_cmd_iq_rms_is_in_range(uint16_t value)
{
    return (value <= 2000u);
}

int main_fan_ctrl_pack(
    uint8_t *dst_p,
    const struct main_fan_ctrl_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->fan_speed, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->fan_speed, 8u, 0x07u);

    return (2);
}

int main_fan_ctrl_unpack(
    struct main_fan_ctrl_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->fan_speed = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->fan_speed |= unpack_left_shift_u16(src_p[1], 8u, 0x07u);

    return (0);
}

int main_fan_ctrl_init(struct main_fan_ctrl_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct main_fan_ctrl_t));

    return 0;
}

uint16_t main_fan_ctrl_fan_speed_encode(float value)
{
    return (uint16_t)(value);
}

float main_fan_ctrl_fan_speed_decode(uint16_t value)
{
    return ((float)value);
}

bool main_fan_ctrl_fan_speed_is_in_range(uint16_t value)
{
    return (value <= 2000u);
}

int main_pps_pack(
    uint8_t *dst_p,
    const struct main_pps_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u8(src_p->pps, 0u, 0x7fu);
    dst_p[0] |= pack_left_shift_u8(src_p->pps_status, 7u, 0x80u);
    dst_p[1] |= pack_right_shift_u8(src_p->pps_status, 1u, 0x03u);

    return (2);
}

int main_pps_unpack(
    struct main_pps_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->pps = unpack_right_shift_u8(src_p[0], 0u, 0x7fu);
    dst_p->pps_status = unpack_right_shift_u8(src_p[0], 7u, 0x80u);
    dst_p->pps_status |= unpack_left_shift_u8(src_p[1], 1u, 0x03u);

    return (0);
}

int main_pps_init(struct main_pps_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct main_pps_t));

    return 0;
}

uint8_t main_pps_pps_encode(float value)
{
    return (uint8_t)(value);
}

float main_pps_pps_decode(uint8_t value)
{
    return ((float)value);
}

bool main_pps_pps_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t main_pps_pps_status_encode(float value)
{
    return (uint8_t)(value);
}

float main_pps_pps_status_decode(uint8_t value)
{
    return ((float)value);
}

bool main_pps_pps_status_is_in_range(uint8_t value)
{
    return (value <= 4u);
}

int main_cooling_data_pack(
    uint8_t *dst_p,
    const struct main_cooling_data_t *src_p,
    size_t size)
{
    if (size < 1u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 1);

    dst_p[0] |= pack_left_shift_u8(src_p->water_temperature, 0u, 0x7fu);

    return (1);
}

int main_cooling_data_unpack(
    struct main_cooling_data_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 1u) {
        return (-EINVAL);
    }

    dst_p->water_temperature = unpack_right_shift_u8(src_p[0], 0u, 0x7fu);

    return (0);
}

int main_cooling_data_init(struct main_cooling_data_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct main_cooling_data_t));

    return 0;
}

uint8_t main_cooling_data_water_temperature_encode(float value)
{
    return (uint8_t)(value);
}

float main_cooling_data_water_temperature_decode(uint8_t value)
{
    return ((float)value);
}

bool main_cooling_data_water_temperature_is_in_range(uint8_t value)
{
    return (value <= 100u);
}
