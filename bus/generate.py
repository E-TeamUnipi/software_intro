from dbc_dict_processor import create_dbs, fail_on_overlapping_ids
from dbc_bindings_generation import write_dbc_files, write_nodes_header
from can_def import sim_messages, main_messages, NodeId, DBC_VERSION
import cantools

main_db, main_bindings_db = create_dbs(main_messages(), NodeId, NodeId, DBC_VERSION)
sim_db, sim_bindings_db = create_dbs(sim_messages(), NodeId, NodeId, DBC_VERSION)

fail_on_overlapping_ids([main_db])
fail_on_overlapping_ids([sim_db])

cantools.database.dump_file(main_db, r'dbc/main.dbc')
cantools.database.dump_file(sim_db, r'dbc/sim.dbc')

write_dbc_files(main_bindings_db, 'main', 'bindings/main_db.h', 'bindings/main_db.c')
write_dbc_files(sim_bindings_db, 'sim', 'bindings/sim_db.h', 'bindings/sim_db.c')

write_nodes_header([
    ('main', DBC_VERSION, NodeId),
    ('sim', DBC_VERSION, NodeId),
], 'bindings/nodes.h')
