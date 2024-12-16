#include "vcu.h"
#include "sb/hal.h"
#include <stdint.h>

#include <main_db.h>
#include <nodes.h>

static volatile uint8_t water_temp = 0;
static volatile uint8_t pps = 0;
static volatile uint8_t pps_status = MAIN_PPS_PPS_STATUS_OK_CHOICE;

static void temp_control_loop(void*);
static void current_control_loop(void*);

void vcu_init() {
    hal_init();

    hal_scheduler_add_task(1000, temp_control_loop);
    hal_scheduler_add_task(20, current_control_loop);
    hal_start_scheduler();
    hal_can_init();
}

void vcu_main_loop() {
}

void hal_handle_can_message(uint32_t id, const uint8_t *payload, uint8_t len) {
    switch (id) {
        case MAIN_PPS_FRAME_ID | MAIN_NODE_SB1: {
            struct main_pps_t data;
            main_pps_unpack(&data, payload, len);

            pps = data.pps;
            pps_status = data.pps_status;
        }
        case MAIN_COOLING_DATA_FRAME_ID | MAIN_NODE_SB2: {
            struct main_cooling_data_t data;
            main_cooling_data_unpack(&data, payload, len);

            water_temp = data.water_temperature;
            break;
        }
    }
}

void current_control_loop(void* d) {
    (void) d;
    struct main_iq_cmd_t data;

    float current = (float)pps / 100.0f * 80.f;
    if (pps_status != MAIN_PPS_PPS_STATUS_OK_CHOICE) {
        current = 0.0f;
    }
    data.iq_rms = main_iq_cmd_iq_rms_encode(current);

    uint8_t payload[MAIN_IQ_CMD_LENGTH];
    main_iq_cmd_pack(payload, &data, sizeof(payload));

    hal_send_can_message(
        MAIN_IQ_CMD_FRAME_ID | MAIN_NODE_VCU,
        payload,
        MAIN_IQ_CMD_LENGTH);
}

void temp_control_loop(void* d) {
    (void) d;

    // TODO:1) scegliere la velocità della ventola in base a water_temp
    //      2) mandare un messaggio alla scheda di attuazione della ventola
}
