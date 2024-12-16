#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BOARD_ID_PPS,
    BOARD_ID_TEMP,
    BOARD_ID_FAN_CTRL,
} board_id;

typedef void (*scheduler_callback_t)(void*);

bool hal_init(void);
bool hal_can_init(void);
bool hal_send_can_message(uint32_t addr, const uint8_t* data, uint8_t length);
extern void hal_handle_can_message(uint32_t addr, const uint8_t* data, uint8_t length);

void hal_start_scheduler();
uint8_t hal_scheduler_add_task(uint32_t period_ms, scheduler_callback_t cb);
void hal_stop_task(uint8_t callback_id);
void hal_resume_task(uint8_t callback_id);

board_id hal_board_id(void);
extern void hal_analog_sensor_conversion_ready(uint8_t num_sensor, uint16_t value);

void hal_set_pwm_duty(uint8_t duty_percent);
