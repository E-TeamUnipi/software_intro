import math
import copy
from typing import Type
from enum import IntEnum

import cantools

''' Message entry
Every message is encoded as a python dictionary with the following keys:
    {
        'name': 'message name',
        'senders': [NODE_ID_1, NODE_ID_2, ...],
        'receivers': [NODE_ID_1, NODE_ID_2, ...],
        'message_id': MsgId.X,
        'payload_length': 8, # OPTIONAL: can be computed automatically using signals bits
        'comment': 'comment about this message',
        'cycle_time': 100, # Cycle time in ms, None if not applicable
        'signals': [], # array of signal entries
    },

The signals array is again an array of dicts with the following format:
    {
        'name': 'Signal name',
        'scale': signal resolution, # default 1
        'is_float": True or False, whether this is a floating point. Default False.
        'range": (min_value, max_value), # max value is included, by default (0, 1)
        'start_bit': start_bit, # optional, tracked automatically following signal order
                                # but can be overwritten
        'bit_length': optional, number of bits of the signal
        'byte_order': either 'little_endian' or 'big_endian', default 'little_endian'
        'unit': 'string with unit (None when not applicable), default None
        'comment': 'Comment',

        'choices': {
            num: name, # pairs of signal value -> string name for better display
        }

        multiplexer: True or False, default False
    }
'''


def make_frame_id(node_id: IntEnum, message_id: int):
    from math import floor, log
    max_id = max(type(node_id))
    node_mask_bits = math.ceil(log(max_id.value, 2))

    # First 8 bits for message id, last 3 bits for node id
    return (message_id << node_mask_bits) | node_id.value


def fill_default_values(message_desc):
    for signal_desc in message_desc['signals']:
        signal_desc.setdefault('scale', 1)
        signal_desc.setdefault('range', (0, 1))
        signal_desc.setdefault('is_float', False)
        signal_desc.setdefault('multiplexer', False)
        signal_desc.setdefault('byte_order', 'little_endian')

    message_desc.setdefault('receivers', [])


def compute_is_signed(message_desc):
    for signal_desc in message_desc['signals']:
        range = signal_desc['range']
        signal_desc['is_signed'] = range[0] < 0

def compute_bit_lengths(message_desc):
    for signal_desc in message_desc['signals']:
        range = signal_desc['range']
        scale = signal_desc['scale']
        num_bits = math.ceil(math.log(((range[1]-range[0])+1) / scale, 2))

        if 'bit_length' in signal_desc:
            if signal_desc['bit_length'] < num_bits:
                exit('Wrong signal bit length in signal ' + signal_desc['name']
                     + ' of message ' + message_desc['name'])
        else:
            signal_desc['bit_length'] = num_bits

def compute_offset(message_desc):
    for signal_desc in message_desc['signals']:
        range = signal_desc['range']
        if range[0] >= range[1]:
            exit('Range of signal ' + signal_desc['name'] + ' of message ' + message_desc['name'] + ' is reversed')

        scale = signal_desc['scale']
        bits = signal_desc['bit_length']

        # allow symmetric ranges without offset
        if range[0] == -range[1]:
            signal_desc['offset'] = 0
        elif scale == 1 and range[0] == -2**(bits-1) and range[1] == 2**(bits-1)-1:
            signal_desc['offset'] = 0
        else:
            signal_desc['offset'] = range[0]

def compute_start_bits(message_desc):
    tot_bits = 0
    max_start_bit = 0
    for signal_desc in message_desc['signals']:
        if 'start_bit' in signal_desc:
            start_bit = signal_desc['start_bit']
        else:
            start_bit = tot_bits

        if start_bit >= max_start_bit:
            tot_bits += signal_desc['bit_length']
            max_start_bit = start_bit

        if 'start_bit' not in signal_desc:
            signal_desc['start_bit'] = start_bit

    payload_length = math.ceil(tot_bits/8)
    if 'payload_length' in message_desc:
        if message_desc['payload_length'] < payload_length:
            exit('wrong payload length in message ' + message_desc['name'])
    else:
        message_desc['payload_length'] = payload_length


def preprocess_message_dict(message_dict):
    fill_default_values(message_dict)
    compute_bit_lengths(message_dict)
    compute_offset(message_dict)
    compute_start_bits(message_dict)
    compute_is_signed(message_dict)

def create_messages_from_description(message_dict, prepend_sender_name):
    preprocess_message_dict(message_dict)

    messages = []
    for sender in message_dict['senders']:
        signals = []
        multiplexer_id = None
        multiplexer_choices = None
        for id, signal_desc in enumerate(message_dict['signals']):
            if signal_desc['multiplexer']:
                multiplexer_id = id
                multiplexer_choices = signal_desc['choices']

        multiplexer_signal = None
        for signal_desc in message_dict['signals']:
            choices = signal_desc.get('choices', None)


            cantools_signal = cantools.database.Signal(
                name = signal_desc['name'],
                comment = signal_desc['comment'],
                conversion = cantools.database.conversion.BaseConversion.factory(
                    scale=signal_desc['scale'],
                    offset=signal_desc['offset'],
                    is_float=signal_desc['is_float'],
                    choices=choices
                ),
                is_signed = signal_desc['is_signed'],
                is_multiplexer = signal_desc['multiplexer'],
                minimum = signal_desc['range'][0],
                maximum = signal_desc['range'][1],
                start = signal_desc['start_bit'],
                length = signal_desc['bit_length'],
                unit = signal_desc['unit'],
                byte_order = signal_desc['byte_order'],
                receivers = [x.name for x  in message_dict['receivers']],
            )


            # set multiplexer infos if needed
            if multiplexer_id is not None:
                if not signal_desc['multiplexer']:
                    m_signals = []
                    for choice, choice_name in multiplexer_choices.items():
                        signal = copy.copy(cantools_signal)
                        signal.multiplexer_ids = [choice]
                        signal.name = choice_name + '_' + signal.name

                        m_signals.append(signal)

                    signals.extend(m_signals)
                else:
                    multiplexer_signal = copy.copy(cantools_signal)
            else:
                signals.append(cantools_signal)

        if multiplexer_id is not None:
            signals.append(multiplexer_signal)
            for signal in signals:
                if not signal.is_multiplexer:
                    signal.multiplexer_signal = multiplexer_signal

        if prepend_sender_name:
            message_name = sender.name + '_' + message_dict['name']
        else:
            message_name = message_dict['name']

        messages.append(cantools.database.Message(
            frame_id = make_frame_id(sender, message_dict['message_id']),
            name =  message_name,
            length = message_dict['payload_length'],
            comment = message_dict['comment'],
            cycle_time = message_dict['cycle_time'],
            senders = [sender.name],
            signals = signals,
            is_extended_frame = False,
        ))

    return messages

def create_dbc_messages_from_descs(message_descs, prepend_sender_name = True):
    unique_ids = set()

    dbc_messages = []
    for message in message_descs:
        db_msgs = create_messages_from_description(message, prepend_sender_name)
        new_ids = set(m.frame_id for m in db_msgs)
        if len(new_ids.intersection(unique_ids)) != 0:
            exit("Error: messages " +
                 message['name'] +
                 " generated already existing ids: " +
                 str(new_ids))
        unique_ids.update(new_ids)
        dbc_messages.extend(db_msgs)

    return dbc_messages


def create_dbs(message_descs: list[dict],
               NodeClass: Type[IntEnum],
               BusNodeClass: Type[IntEnum],
               db_version: str
               ) -> tuple[cantools.database.Database, cantools.database.Database]:
    """create cantools Database from message list

    Returns the db intended for CAN bus decoding (i.e. output dbc) and the db intended for
    C bindings generation (which has sender node in the id set to 0)
    """

    bindings_message_descs = []
    for desc in message_descs:
        msg_no_node_desc = copy.copy(desc)
        msg_no_node_desc['senders'] = [NodeClass(0)]
        bindings_message_descs.append(msg_no_node_desc)

    dbc_messages = create_dbc_messages_from_descs(message_descs)
    bindings_messages = create_dbc_messages_from_descs(bindings_message_descs, False)

    Node = cantools.database.Node

    can_db = cantools.database.Database(
        version = db_version,
        messages = dbc_messages,
        nodes = [Node(x.name) for x in BusNodeClass]
    )
    can_bindings_db = cantools.database.Database(
        version = db_version,
        messages = bindings_messages,
        nodes = [Node(x.name) for x in BusNodeClass]
    )
    return can_db, can_bindings_db


def dbc_dict_from_cantools_db(db: cantools.database.Database) -> list[dict]:
    messages = []
    for message in db.messages:
        if message.is_extended_frame:
            continue
        message_dict = {}
        message_dict['name'] = message.name
        message_dict['frame_id'] = message.frame_id
        message_dict['senders'] = []
        message_dict['receivers'] = []
        message_dict['message_id'] = message.frame_id
        message_dict['payload_length'] = message.length
        message_dict['comment'] = message.comment if message.comment is not None else 'comment missing'
        message_dict['cycle_time'] = message.cycle_time

        signals = []
        for signal in message.signals:
            signal_dict = {}
            signal_dict['name'] = signal.name
            signal_dict['offset'] = signal.offset
            signal_dict['scale'] = signal.scale
            signal_dict['is_float'] = signal.is_float
            signal_dict['range'] = (signal.minimum, signal.maximum)
            signal_dict['start_bit'] = signal.start
            signal_dict['bit_length'] = signal.length
            signal_dict['byte_order'] = signal.byte_order
            signal_dict['unit'] = signal.unit
            signal_dict['comment'] = signal.comment if message.comment is not None else 'comment missing'
            signal_dict['choices'] = signal.choices
            signal_dict['is_multiplexer'] = signal.is_multiplexer

            signals.append(signal_dict)

        message_dict['signals'] = signals
        messages.append(message_dict)

    return messages


def check_overlapping_ids(dbs: list[cantools.database.Database]) -> set[int]:
    all_ids = set()
    conflicts = set()
    for db in dbs:
        db_ids = set(msg.frame_id for msg in db.messages)
        conflicts = all_ids.intersection(db_ids)
        if len(conflicts) > 0:
            break

        all_ids.update(db_ids)

    conflict_names = []
    for db in dbs:
        for id in conflicts:
            try:
                conflict_names.append(db.get_message_by_frame_id(id).name)
            except KeyError:
                pass

    return conflicts, conflict_names

def fail_on_overlapping_ids(dbs: list[cantools.database.Database]):
    id_conflicts, conflict_names = check_overlapping_ids(dbs)
    if len(id_conflicts) != 0:
        print(f"Conflicing ids in dbc merge: {conflict_names}")
        exit(1)
