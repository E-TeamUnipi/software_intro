from typing import Type
from enum import IntEnum
import os

import cantools.database.can.c_source as can_c_generate

def write_dbc_files(db, node_prefix, header_path, src_path):
    (header, source, _, _)  = can_c_generate.generate(
        db,
        node_prefix,
        os.path.basename(header_path),
        os.path.basename(src_path),
        r'',
        use_float=True)

    with open(header_path, 'w') as f:
        f.write(header)
    with open(src_path, 'w') as f:
        f.write(source)

def node_enum_mask_info(NodeClass: Type[IntEnum]) -> tuple[int, int, int]:
    from math import floor, log
    node_mask_bits = int(floor(log(max(NodeClass).value, 2) + 1))
    node_mask = (1 << node_mask_bits) - 1
    msg_id_mask = ((1 << 11)-1) & ~node_mask

    return node_mask_bits, node_mask, msg_id_mask

def gen_bus_mask(bus_name: str, bus_version: str, NodeClass: Type[IntEnum]) -> str:
    bus_upper = bus_name.upper()

    node_mask_bits, node_mask, msg_id_mask = node_enum_mask_info(NodeClass)

    define_lines = (
        f'#define {bus_upper}_DBC_VERSION "{bus_version}"\n'
        f'#define {bus_upper}_NODE_BITS {node_mask_bits}U\n'
        f'#define {bus_upper}_NODE_MASK {hex(node_mask)}U\n'
        f'#define {bus_upper}_MSG_MASK {hex(msg_id_mask)}U\n\n'
    )

    for n in NodeClass:
        define_lines += f'#define {bus_upper}_NODE_' + n.name + f' {n.value}U\n'

    define_lines += '\n'

    return define_lines

def write_nodes_header(buses: list[tuple[str, str, Type[IntEnum]]], header_path):
    content = ''
    for bus_name, bus_version, NodeClass in buses:
        content += gen_bus_mask(bus_name, bus_version, NodeClass)

    header_content = f'''\
#pragma once

#ifdef __cplusplus
extern "C" {{
#endif

#include <stdint.h>

{content}

#ifdef __cplusplus
}}
#endif
'''

    with open(header_path, 'w') as f:
        f.write(header_content)

