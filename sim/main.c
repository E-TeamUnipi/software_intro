#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <posix_hal/scheduler.h>
#include <posix_hal/can.h>

#include <main_db.h>
#include <sim_db.h>
#include <nodes.h>

#include "sim.h"


static void init_sim_params(sim_params_t* sim_params);
static float map(float val, float A, float B, float a, float b);

static void physics(void*);
static void gen_analog_signals(void*);
static void send_inverter_temp(void*);
static void decrease_pps(void*);

static int can_sockets[2];

// simulator state and parameters
static float Twater;
static sim_params_t sim_params;

// simulator input
static volatile float iq = 0.0f;
static volatile float fan_duty = 0.0f; // [0, 1]

// simulator outputs
static volatile float pps_value = 1.0f; // [0,1]

// simulator step (ms)
static int sim_step = 5;

int main()
{
    init_sim_params(&sim_params);
    Twater = sim_params.Twater0;

    phal_can_init_socket(&can_sockets[0], "vcan");
    phal_can_init_socket(&can_sockets[1], "vcansim");

    phal_scheduler_init(250);
    phal_scheduler_add_periodic_callback(sim_step, physics, NULL);
    phal_scheduler_add_periodic_callback_us(1000, gen_analog_signals, NULL);
    phal_scheduler_add_periodic_callback(SIM_INVERTER_TEMP_CYCLE_TIME_MS, send_inverter_temp, NULL);
    phal_scheduler_add_oneshot_callback(30000, decrease_pps, NULL);
    phal_scheduler_start();

    bool res = phal_can_start_receiver_thread(can_sockets, 2, NULL, false);
    if (!res) {
        fputs("An error occurred while starting the CAN thread\n", stderr);
        return 1;
    }

    while (true);

    return 0;
}

void init_sim_params(sim_params_t* sim_params)
{
    sim_params->eta = 0.95f;
    sim_params->Kt = 0.54f;
    sim_params->alpha = 25.93f;
    sim_params->Twater0 = 35;
    sim_params->Tamb = 40;
    sim_params->Cw = 8.3e3/8.0f;
    sim_params->Rinv = 0.006;
    sim_params->minRadiatorR = 1.0f/2500.0f;
    sim_params->maxRadiatorR = 1.0f/100.0f;

    sim_params->pps_Vmin = 700.f;
    sim_params->pps_Vmax = 1700.f;

    sim_params->pps_inv_Vmin = 3400.f;
    sim_params->pps_inv_Vmax = 4400.f;

    // using 5V and divider with R=1k
    sim_params->temp_Vmin = 455.f; // 0 C
    sim_params->temp_Vmax = 630.f; // 100 C
}

float map(float val, float A, float B, float a, float b)
{
    if (val < A) {
        val = A;
    } else if (val > B) {
        val = B;
    }

    return (val-A)/(B-A) * (b-a) + a;
}

void phal_can_receiver_handle(int handle, uint32_t id, const uint8_t* payload, uint8_t len)
{
    if (handle == can_sockets[0]) {
        switch (id) {
            case MAIN_IQ_CMD_FRAME_ID | SIM_NODE_VCU: {
                struct main_iq_cmd_t data;
                main_iq_cmd_unpack(&data, payload, len);

                iq = main_iq_cmd_iq_rms_decode(data.iq_rms);
                break;
            }
        }
    } else if (handle == can_sockets[1]) {
        switch (id) {
            case SIM_FAN_CTRL_FRAME_ID | SIM_NODE_COOLING_CTRL: {
                struct sim_fan_ctrl_t data;
                sim_fan_ctrl_unpack(&data, payload, len);

                fan_duty = sim_fan_ctrl_fan_pwm_duty_decode(data.fan_pwm_duty) / 100.0f;
                break;
            }
        }
    }
}

void physics(void* data)
{
    (void) data;

    const float dt = sim_step * 1e-3f;

    float Rfan = (sim_params.maxRadiatorR-sim_params.minRadiatorR)*(1.0-fan_duty) + sim_params.minRadiatorR;
    float Pd = 0.5 * sim_params.Kt * iq * iq;
    float rate = 1.f/sim_params.Cw * (Pd - (Twater-sim_params.Tamb)/Rfan);

    Twater += rate * dt;
}

void gen_analog_signals(void* data)
{
    (void) data;

    struct sim_pps_sensors_t ps;
    ps.pps = sim_pps_sensors_pps_encode(
        map(pps_value, 0.f, 1.f, sim_params.pps_Vmin, sim_params.pps_Vmax));

    ps.pps_inv = sim_pps_sensors_pps_inv_encode(
        map(1.f-pps_value, 0.f, 1.f, sim_params.pps_inv_Vmin, sim_params.pps_inv_Vmax));

    struct sim_cooling_sensors_t cs;
    cs.thermocouple = sim_cooling_sensors_thermocouple_encode(
        map(Twater, 0, 100, sim_params.temp_Vmin, sim_params.temp_Vmax));

    uint8_t payload[8];
    sim_pps_sensors_pack(payload, &ps, SIM_PPS_SENSORS_LENGTH);
    phal_can_send_message(can_sockets[1],
        SIM_PPS_SENSORS_FRAME_ID | SIM_NODE_SIM,
        payload, SIM_PPS_SENSORS_LENGTH);

    sim_cooling_sensors_pack(payload, &cs, SIM_COOLING_SENSORS_LENGTH);
    phal_can_send_message(can_sockets[1],
        SIM_COOLING_SENSORS_FRAME_ID | SIM_NODE_SIM,
        payload, SIM_COOLING_SENSORS_LENGTH);
}

void decrease_pps(void*) {
    pps_value = 0.0f;
}

static void send_inverter_temp(void*) {
    float Pd = 0.5 * sim_params.Kt * iq * iq;
    float Tinverter = Twater + sim_params.Rinv * Pd;
    if (Tinverter > 120.0f) Tinverter = 120.0f;

    struct sim_inverter_temp_t data;
    data.temp = sim_inverter_temp_temp_encode(Tinverter);

    uint8_t payload[SIM_INVERTER_TEMP_LENGTH];
    sim_inverter_temp_pack(payload, &data, sizeof(payload));
    phal_can_send_message(can_sockets[1],
        SIM_INVERTER_TEMP_FRAME_ID | SIM_NODE_SIM,
        payload, SIM_INVERTER_TEMP_LENGTH);
}
