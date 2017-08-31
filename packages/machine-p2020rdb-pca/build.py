#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#
from prj import execute, commonpath, commonprefix
import os

schema = {'type': 'dict', 'name': 'module', 'dict_type':
          ([{'type': 'bool', 'name': 'double_precision_fp', 'optional': 'true', 'default': 'false'}], [])
          }


def run(system, prx_config={}):
    return system_build(system, prx_config)


def system_build(system, prx_config={}):
    inc_path_args = ['-I%s' % i for i in system.include_paths]
    common_flags = ['-g3']
    a_flags = common_flags
    c_flags = common_flags
    if prx_config.get('double_precision_fp'):
        c_float_gprs_flag = '-mfloat-gprs=double'
    else:
        c_float_gprs_flag = '-mfloat-gprs=single'

    all_input_files = system.c_files + system.asm_files
    all_input_files = [os.path.normpath(os.path.abspath(path)) for path in all_input_files]
    common_parent_path = commonpath(all_input_files)

    # Compile all C files.
    c_obj_files = [os.path.join(system.output, os.path.relpath(os.path.abspath(c.replace('.c', '.o')), common_parent_path)) for c in system.c_files]

    for c, o in zip(system.c_files, c_obj_files):
        os.makedirs(os.path.dirname(o), exist_ok=True)
        execute(['powerpc-eabispe-gcc', '-mcpu=8548', c_float_gprs_flag, '-meabi', '-mno-sdata', '-G', '0',
                '-mabi=spe', '-mspe', '-ffreestanding', '-c', c, '-o', o, '-Wall', '-Werror'] +
                c_flags + inc_path_args)

    # Assemble all asm files.
    asm_obj_files = [os.path.join(system.output, os.path.relpath(os.path.abspath(s.replace('.s', '.o')), common_parent_path)) for s in system.asm_files]
    for s, o in zip(system.asm_files, asm_obj_files):
        os.makedirs(os.path.dirname(o), exist_ok=True)
        execute(['powerpc-eabispe-as', '-me500', '-mspe', '-o', o, s] + a_flags + inc_path_args)

    # Perform final link
    obj_files = asm_obj_files + c_obj_files
    execute(['powerpc-eabispe-ld', '-G', '0', '-T', system.linker_script, '-o', system.output_file] + obj_files)
