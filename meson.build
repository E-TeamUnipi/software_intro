project('software_intro', 'c', version: '0.1')

add_global_arguments(
    '-DMAX_PERIODIC_TASKS=5',
    '-DMAX_APERIODIC_TASKS=30',
    language: 'c')

posix_hal_proj = subproject('posix_hal')
posix_hal = posix_hal_proj.get_variable('posix_hal_dep')

sim_sources = files('sim/main.c')
sb_sources = files('sb/main.c', 'sb/sb.c', 'sb/hal.c')
vcu_sources = files('vcu/vcu.c', 'vcu/main.c', 'vcu/hal.c')
can_bindings_inc = include_directories('bus/bindings')
can_bindings_src = files('bus/bindings/sim_db.c', 'bus/bindings/main_db.c')

executable('sim', 
    sim_sources + can_bindings_src,
    dependencies: posix_hal,
    include_directories: can_bindings_inc,
    install: true)

executable('sb1', 
    sb_sources + can_bindings_src,
    c_args: ['-DSB_BOARD_ID=0'],
    dependencies: posix_hal,
    include_directories: can_bindings_inc,
    install: true)

executable('sb2', 
    sb_sources + can_bindings_src,
    c_args: ['-DSB_BOARD_ID=1'],
    dependencies: posix_hal,
    include_directories: can_bindings_inc,
    install: true)

executable('fan_ctrl', 
    sb_sources + can_bindings_src,
    c_args: ['-DSB_BOARD_ID=2'],
    dependencies: posix_hal,
    include_directories: can_bindings_inc,
    install: true)

executable('vcu', 
    vcu_sources + can_bindings_src,
    dependencies: posix_hal,
    include_directories: can_bindings_inc,
    install: true)
