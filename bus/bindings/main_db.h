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

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EINVAL
#    define EINVAL 22
#endif

/* Frame ids. */
#define MAIN_IQ_CMD_FRAME_ID (0x10u)
#define MAIN_FAN_CTRL_FRAME_ID (0x18u)
#define MAIN_PPS_FRAME_ID (0x00u)
#define MAIN_COOLING_DATA_FRAME_ID (0x08u)

/* Frame lengths in bytes. */
#define MAIN_IQ_CMD_LENGTH (2u)
#define MAIN_FAN_CTRL_LENGTH (2u)
#define MAIN_PPS_LENGTH (2u)
#define MAIN_COOLING_DATA_LENGTH (1u)

/* Extended or standard frame types. */
#define MAIN_IQ_CMD_IS_EXTENDED (0)
#define MAIN_FAN_CTRL_IS_EXTENDED (0)
#define MAIN_PPS_IS_EXTENDED (0)
#define MAIN_COOLING_DATA_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */
#define MAIN_FAN_CTRL_CYCLE_TIME_MS (100u)
#define MAIN_PPS_CYCLE_TIME_MS (20u)
#define MAIN_COOLING_DATA_CYCLE_TIME_MS (50u)

/* Signal choices. */
#define MAIN_PPS_PPS_STATUS_OK_CHOICE (0u)
#define MAIN_PPS_PPS_STATUS_NINV_FAULT_CHOICE (1u)
#define MAIN_PPS_PPS_STATUS_INV_FAULT_CHOICE (2u)
#define MAIN_PPS_PPS_STATUS_UNCORRELATED_FAULT_CHOICE (3u)

/* Frame Names. */
#define MAIN_IQ_CMD_NAME "iq_cmd"
#define MAIN_FAN_CTRL_NAME "fan_ctrl"
#define MAIN_PPS_NAME "pps"
#define MAIN_COOLING_DATA_NAME "cooling_data"

/* Signal Names. */
#define MAIN_IQ_CMD_IQ_RMS_NAME "iq_rms"
#define MAIN_FAN_CTRL_FAN_SPEED_NAME "fan_speed"
#define MAIN_PPS_PPS_NAME "pps"
#define MAIN_PPS_PPS_STATUS_NAME "pps_status"
#define MAIN_COOLING_DATA_WATER_TEMPERATURE_NAME "water_temperature"

/**
 * Signals in message iq_cmd.
 *
 * Quadrature current command
 *
 * All signal values are as on the CAN bus.
 */
struct main_iq_cmd_t {
    /**
     * RMS value of quadrature current
     *
     * Range: 0..2000 (0..200 Arms)
     * Scale: 0.1
     * Offset: 0
     */
    uint16_t iq_rms;
};

/**
 * Signals in message fan_ctrl.
 *
 * fan speed setting
 *
 * All signal values are as on the CAN bus.
 */
struct main_fan_ctrl_t {
    /**
     * fan speed in rpm
     *
     * Range: 0..2000 (0..2000 rpm)
     * Scale: 1
     * Offset: 0
     */
    uint16_t fan_speed;
};

/**
 * Signals in message pps.
 *
 * pedal position reading
 *
 * All signal values are as on the CAN bus.
 */
struct main_pps_t {
    /**
     * percent pedal position
     *
     * Range: 0..100 (0..100 %)
     * Scale: 1
     * Offset: 0
     */
    uint8_t pps;

    /**
     * pps failure status
     *
     * Range: 0..4 (0..4 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t pps_status;
};

/**
 * Signals in message cooling_data.
 *
 * data from cooling plant
 *
 * All signal values are as on the CAN bus.
 */
struct main_cooling_data_t {
    /**
     * Water temperature in cooling plant
     *
     * Range: 0..100 (0..100 °C)
     * Scale: 1
     * Offset: 0
     */
    uint8_t water_temperature;
};

/**
 * Pack message iq_cmd.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int main_iq_cmd_pack(
    uint8_t *dst_p,
    const struct main_iq_cmd_t *src_p,
    size_t size);

/**
 * Unpack message iq_cmd.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int main_iq_cmd_unpack(
    struct main_iq_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from iq_cmd.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int main_iq_cmd_init(struct main_iq_cmd_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t main_iq_cmd_iq_rms_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float main_iq_cmd_iq_rms_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool main_iq_cmd_iq_rms_is_in_range(uint16_t value);

/**
 * Pack message fan_ctrl.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int main_fan_ctrl_pack(
    uint8_t *dst_p,
    const struct main_fan_ctrl_t *src_p,
    size_t size);

/**
 * Unpack message fan_ctrl.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int main_fan_ctrl_unpack(
    struct main_fan_ctrl_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from fan_ctrl.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int main_fan_ctrl_init(struct main_fan_ctrl_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t main_fan_ctrl_fan_speed_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float main_fan_ctrl_fan_speed_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool main_fan_ctrl_fan_speed_is_in_range(uint16_t value);

/**
 * Pack message pps.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int main_pps_pack(
    uint8_t *dst_p,
    const struct main_pps_t *src_p,
    size_t size);

/**
 * Unpack message pps.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int main_pps_unpack(
    struct main_pps_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from pps.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int main_pps_init(struct main_pps_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t main_pps_pps_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float main_pps_pps_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool main_pps_pps_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t main_pps_pps_status_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float main_pps_pps_status_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool main_pps_pps_status_is_in_range(uint8_t value);

/**
 * Pack message cooling_data.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int main_cooling_data_pack(
    uint8_t *dst_p,
    const struct main_cooling_data_t *src_p,
    size_t size);

/**
 * Unpack message cooling_data.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int main_cooling_data_unpack(
    struct main_cooling_data_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from cooling_data.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int main_cooling_data_init(struct main_cooling_data_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t main_cooling_data_water_temperature_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float main_cooling_data_water_temperature_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool main_cooling_data_water_temperature_is_in_range(uint8_t value);


#ifdef __cplusplus
}
#endif

#endif
