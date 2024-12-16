#include "hal.h"
#include <posix_hal/scheduler.h>
#include <posix_hal/can.h>
#include <nodes.h>

#include <sim_db.h>

static int can_sockets[2];

static void handle_sim_message(uint32_t id, const uint8_t* payload, uint8_t len);

bool hal_init(void) {
    phal_scheduler_init(1000);
    return true;
}

bool hal_can_init(void) {
    phal_can_init_socket(&can_sockets[0], "vcan");
    phal_can_init_socket(&can_sockets[1], "vcansim");
    return phal_can_start_receiver_thread(can_sockets, 2, NULL, true);
}

bool hal_send_can_message(uint32_t addr, const uint8_t* data, uint8_t length) {
    return phal_can_send_message(
        can_sockets[0],
        addr,
        data,
        length);
}

void phal_can_receiver_handle(int sock, uint32_t id, const uint8_t *payload, uint8_t len) {
    if (sock == can_sockets[0]) {
        hal_handle_can_message(id, payload, len);
    } else if (sock == can_sockets[1]) {
        handle_sim_message(id, payload, len);
    }
}

void hal_start_scheduler() {
    phal_scheduler_start();
}

uint8_t hal_scheduler_add_task(uint32_t period_ms, scheduler_callback_t cb) {
    return phal_scheduler_add_periodic_callback(period_ms, cb, NULL);
}

void hal_stop_task(uint8_t callback_id) {
}

void hal_resume_task(uint8_t callback_id) {
}

board_id hal_board_id(void) {
    switch (SB_BOARD_ID) {
        case 0:
            return BOARD_ID_PPS;
            break;
        case 1:
            return BOARD_ID_TEMP;
            break;
        case 2:
            return BOARD_ID_FAN_CTRL;
            break;
    }
}

void hal_set_pwm_duty(uint8_t duty_percent) {
    if (duty_percent > 100) duty_percent = 100;

    uint8_t payload[SIM_FAN_CTRL_LENGTH];
    struct sim_fan_ctrl_t data;
    data.fan_pwm_duty = duty_percent;
    sim_fan_ctrl_pack(payload, &data, SIM_FAN_CTRL_LENGTH);

    phal_can_send_message(
        can_sockets[1],
        SIM_FAN_CTRL_FRAME_ID | SIM_NODE_COOLING_CTRL,
        payload,
        SIM_FAN_CTRL_LENGTH);
}

static inline uint16_t mV2ADC(uint32_t mV) {
    float d = (float)mV*((1u<<10)-1u) / 5000u;
    if (d > UINT16_MAX) {
        d = UINT16_MAX;
    }
    return (uint16_t)d;
}

void handle_sim_message(uint32_t id, const uint8_t* payload, uint8_t len) {
    switch (id) {
        #if SB_BOARD_ID == 1
        case SIM_COOLING_SENSORS_FRAME_ID | SIM_NODE_SIM: {
            struct sim_cooling_sensors_t data;
            sim_cooling_sensors_unpack(&data, payload, len);

            uint16_t reading = mV2ADC(data.thermocouple);

            hal_analog_sensor_conversion_ready(
                0, reading);
            break;
        }
        #endif

        #if SB_BOARD_ID == 0
        case SIM_PPS_SENSORS_FRAME_ID | SIM_NODE_SIM: {
            struct sim_pps_sensors_t data;
            sim_pps_sensors_unpack(&data, payload, len);

            uint16_t adc_pps = mV2ADC(data.pps);
            uint16_t adc_pps_inv = mV2ADC(data.pps_inv);

            hal_analog_sensor_conversion_ready(
                0, adc_pps);
            hal_analog_sensor_conversion_ready(
                1, adc_pps_inv);
            break;
        }
        #endif
    }
}
