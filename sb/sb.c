#include "sb.h"
#include "hal.h"
#include <tgmath.h>
#include <main_db.h>
#include <nodes.h>
#include <stdio.h>

#define MAX_MA_SAMPLES 20
#define MA_LENGTH 2

static float map(float val, float A, float B, float a, float b);
static uint16_t add_ma_sample(float sample, float* buf, uint16_t idx, uint16_t max_size);
static float compute_average(float* buf, uint16_t next_sample_pos, uint16_t length, uint16_t buf_length);

static void sb_init_pps();
static void sb_init_temp();
static void sb_init_fan_ctrl();

static void handle_pps_sensor(uint8_t num_sensor, uint16_t value);
static void handle_temp_sensor(uint8_t num_sensor, uint16_t value);

static void send_pps();
static void send_temp();
static void update_fan_speed();

static volatile uint16_t fan_speed_rpm = 0;

static float ma_buffer1[MAX_MA_SAMPLES] = {.0f};
static volatile uint16_t maidx1 = 0;
static float ma_buffer2[MAX_MA_SAMPLES] = {.0f};
static volatile uint16_t maidx2 = 0; 

const uint16_t max_fan_speed_rpm = 2000;

void sb_init()
{
    hal_init();

    board_id id = hal_board_id();

    switch (id)
    {
        case BOARD_ID_PPS:
            sb_init_pps();
            break;

        case BOARD_ID_TEMP:
            sb_init_temp();
            break;

        case BOARD_ID_FAN_CTRL:
            sb_init_fan_ctrl();
            break;
    }

    hal_can_init();
    hal_start_scheduler();
}

float map(float val, float A, float B, float a, float b)
{
    if (A < B) {
        if (val < A) {
            val = A;
        } else if (val > B) {
            val = B;
        }
    } else {
        if (val < B) {
            val = B;
        } else if (val > A) {
            val = A;
        }
    }

    return (val-A)/(B-A) * (b-a) + a;
}

uint16_t add_ma_sample(float sample, float* buf, uint16_t idx, uint16_t max_size) {
    buf[idx] = sample;

    return (idx+1) % max_size;

}

float compute_average(float* buf, uint16_t next_sample_pos, uint16_t length, uint16_t buf_length) {
    uint16_t first_pos = (next_sample_pos + buf_length - length) % buf_length;

    float sum = 0.0f;
    for (uint16_t i=0; i < length; i++) {
        sum += buf[(first_pos + i) % buf_length];
    }

    return sum / length;
}

void sb_init_pps() {
    hal_scheduler_add_task(15, send_pps);
}

void sb_init_temp() {
    hal_scheduler_add_task(1000, send_temp);
    
}
void sb_init_fan_ctrl() {
    fan_speed_rpm = 0;
    hal_scheduler_add_task(1000, update_fan_speed);
}

void hal_handle_can_message(uint32_t id, const uint8_t *data, uint8_t length) {
    if (hal_board_id() == BOARD_ID_FAN_CTRL) {
        switch (id) {
            case MAIN_FAN_CTRL_FRAME_ID | MAIN_NODE_VCU: {
                struct main_fan_ctrl_t msg;
                main_fan_ctrl_unpack(&msg, data, length);

                fan_speed_rpm = msg.fan_speed;
                break;
            }
        }
    }
}

void hal_analog_sensor_conversion_ready(uint8_t num_sensor, uint16_t value) {
    if (hal_board_id() == BOARD_ID_PPS) {
        handle_pps_sensor(num_sensor, value);
    }
    if (hal_board_id() == BOARD_ID_TEMP) {
        handle_temp_sensor(num_sensor, value);
    }
}

void send_pps(void) {
    // mV thresholds
    float pps_min = 0.700f;
    float pps_max = 1.700f;

    float pps_inv_min = 3.400f;
    float pps_inv_max = 4.400f;

    float pps_val_V = compute_average(ma_buffer1, maidx1, MA_LENGTH, MAX_MA_SAMPLES);
    float pps_inv_val_V = compute_average(ma_buffer2, maidx2, MA_LENGTH, MAX_MA_SAMPLES);

    uint8_t pps_status = MAIN_PPS_PPS_STATUS_OK_CHOICE;

    if (pps_val_V > 4.5f || pps_val_V < 0.5f) {
        pps_status = MAIN_PPS_PPS_STATUS_NINV_FAULT_CHOICE;
    }
    if (pps_inv_val_V > 4.5f || pps_inv_val_V < 0.5f) {
        pps_status = MAIN_PPS_PPS_STATUS_INV_FAULT_CHOICE;
    }

    float pps_val = map(pps_val_V, pps_min, pps_max, 0, 100);
    float pps_inv_val = map(pps_inv_val_V, pps_inv_min, pps_inv_max, 100, 0);

    if (fabs(pps_val - pps_inv_val)/100 > 10) {
        pps_status = MAIN_PPS_PPS_STATUS_UNCORRELATED_FAULT_CHOICE;
    }

    float pps_average = 0.0f;
    if (pps_status == MAIN_PPS_PPS_STATUS_OK_CHOICE) {
        pps_average = (pps_val + pps_inv_val) * .5f;
    }

    struct main_pps_t data;
    data.pps = main_pps_pps_encode(pps_average);
    data.pps_status = pps_status;

    uint8_t payload[MAIN_PPS_LENGTH];
    main_pps_pack(payload, &data, MAIN_PPS_LENGTH);

    hal_send_can_message(
        MAIN_PPS_FRAME_ID | MAIN_NODE_SB1,
        payload,
        MAIN_PPS_LENGTH);
}

void handle_pps_sensor(uint8_t num_sensor, uint16_t value) {
    const uint16_t adc_max = (1<<10)-1;
    const float Vref = 5;

    float value_V = (float)value / adc_max * Vref;

    if (num_sensor ==  0) {
        // pps
        maidx1 = add_ma_sample(value_V, ma_buffer1, maidx1, MAX_MA_SAMPLES);
    } else {
        // pps inv
        maidx2 = add_ma_sample(value_V, ma_buffer2, maidx2, MAX_MA_SAMPLES);
    }
}

void update_fan_speed(void) {
    float duty = (float)fan_speed_rpm / max_fan_speed_rpm;
    hal_set_pwm_duty((uint8_t) (100.f * duty));
}


void send_temp(void) {
    // TODO
}

static void handle_temp_sensor(uint8_t num_sensor, uint16_t value) {
    if (num_sensor == 0) {
        // TODO;
    }
}

