from enum import IntEnum, auto

DBC_VERSION = '0.0.1'

class MsgId(IntEnum):
    PPS = 0
    COOLING = auto()
    IQ_CMD = auto()
    FAN_CMD = auto()

class VMsgId(IntEnum):
    PPS = auto()
    COOLING_ACTUATION = auto()
    COOLING_SENSORS = auto()
    INVERTER_TEMP = auto()

class NodeId(IntEnum):
    VCU = 0
    SB1 = auto()
    SB2 = auto()
    COOLING_CTRL = auto()
    INVERTER = auto()
    SIM = auto()

vcu_messages = [
    {
        'name': 'iq_cmd',
        'senders': [NodeId.VCU],
        'receivers': [NodeId.INVERTER],
        'message_id': MsgId.IQ_CMD,
        'comment': 'Quadrature current command',
        'cycle_time': None,
        'signals': [
            {
                'name': 'iq_rms',
                'scale': 0.1,
                'range': (0, 200),
                'unit': 'Arms',
                'comment': 'RMS value of quadrature current',
            },
        ]
    },
    {
        'name': 'fan_ctrl',
        'senders': [NodeId.VCU],
        'receivers': [NodeId.COOLING_CTRL],
        'message_id': MsgId.FAN_CMD,
        'comment': 'fan speed setting',
        'cycle_time': 100,
        'signals': [
            {
                'name': 'fan_speed',
                'scale': 1,
                'range': (0, 2000),
                'unit': 'rpm',
                'comment': 'fan speed in rpm',
            },
        ]
    },
]

sb_messages = [
    {
        'name': 'pps',
        'senders': [NodeId.SB1],
        'receivers': [NodeId.VCU],
        'message_id': MsgId.PPS,
        'comment': 'pedal position reading',
        'cycle_time': 20,
        'signals': [
            {
                'name': 'pps',
                'scale': 1,
                'range': (0, 100),
                'unit': '%',
                'comment': 'percent pedal position',
            },
            {
                'name': 'pps_status',
                'range': (0, 4),
                'comment': 'pps failure status',
                'unit': None,

                'choices': {
                    0: 'ok',
                    1: 'ninv_fault',
                    2: 'inv_fault',
                    3: 'uncorrelated_fault',
                },
            }
        ]
    },
    {
        'name': 'cooling_data',
        'senders': [NodeId.SB2],
        'receivers': [NodeId.VCU],
        'message_id': MsgId.COOLING,
        'comment': 'data from cooling plant',
        'cycle_time': 50,
        'signals': [
            {
                'name': 'water_temperature',
                'scale': 1,
                'range': (0, 100),
                'unit': '°C',
                'comment': 'Water temperature in cooling plant',
            },
        ]
    },
]

simulator_messages = [
    {
        'name': 'pps_sensors',
        'senders': [NodeId.SIM],
        'receivers': [NodeId.SB1],
        'message_id': VMsgId.PPS,
        'comment': 'pps readings',
        'cycle_time': None,
        'signals': [
            {
                'name': 'pps',
                'scale': 1,
                'range': (0, 5000),
                'unit': 'mV',
                'comment': 'non-inverting PPS voltage',
            },
            {
                'name': 'pps_inv',
                'scale': 1,
                'range': (0, 5000),
                'unit': 'mV',
                'comment': 'inverting PPS voltage',
            },
        ]
    },
    {
        'name': 'cooling_sensors',
        'senders': [NodeId.SIM],
        'receivers': [NodeId.SB2],
        'message_id': VMsgId.COOLING_SENSORS,
        'comment': 'thermocouple voltage',
        'cycle_time': None,
        'signals': [
            {
                'name': 'thermocouple',
                'scale': 1,
                'range': (0, 5000),
                'unit': 'mV',
                'comment': 'Voltage on the thermocouple end',
            },
        ]
    },
    {
        'name': 'inverter_temp',
        'senders': [NodeId.SIM],
        'receivers': [],
        'message_id': VMsgId.INVERTER_TEMP,
        'comment': 'Inverter temperature message for diagnostics',
        'cycle_time': 500,
        'signals': [
            {
                'name': 'temp',
                'scale': 0.1,
                'range': (0, 120),
                'unit': 'C',
                'comment': 'Inverter temperature',
            },
        ]
    },
    {
        'name': 'fan_ctrl',
        'senders': [NodeId.COOLING_CTRL],
        'receivers': [NodeId.SIM],
        'message_id': VMsgId.COOLING_ACTUATION,
        'comment': 'fan pwm control',
        'cycle_time': 100,
        'signals': [
            {
                'name': 'fan_pwm_duty',
                'scale': 1,
                'range': (0, 100),
                'unit': '%',
                'comment': 'fan PWM duty cycle',
            },
        ]
    },
]

def main_messages() -> list[dict]:
    return vcu_messages + sb_messages

def sim_messages() -> list[dict]:
    return simulator_messages
