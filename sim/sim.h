#pragma once

typedef struct {
    float Kt;  // quadrature current rms to torque constant
    float alpha;  // Pd = 0.5 alpha  Kt iq^2
    float eta; // inverters+motors efficiency

    float Twater0; // initial water temperature
    float Tamb; // ambient temperature

    float Cw; // total water heat capacity [J/K]
    float Rinv; // inverter dissipator thermal resisance
    float minRadiatorR; // radiator thermal resistance when fan off
    float maxRadiatorR; // radiator thermal resistance when fan to the max

    float pps_Vmin; // [mV]
    float pps_Vmax; // [mV]
    float pps_inv_Vmin; // [mV]
    float pps_inv_Vmax; // [mV]
    float temp_Vmin; // [mV] V at 0deg
    float temp_Vmax; // [mV] V at 100deg
} sim_params_t;
