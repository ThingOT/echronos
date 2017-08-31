#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file LICENSE_CSIRO_BSD for details.
#
# @TAG(CSIRO_BSD_MIT)
#
from prj import execute, SystemBuildError


schema = {
    'type': 'dict',
    'name': 'module',
    'dict_type': ([{'type': 'string', 'name': 'output_type', 'default': 'executable'}], [])
}


def run(system, configuration=None):
    return system_build(system, configuration)


def system_build(system, configuration):
    inc_path_args = ['-I%s' % i for i in system.include_paths]

    if len(system.c_files) == 0:
        raise SystemBuildError("Zero C files in system definition")

    shared_args = ['-shared', '-fPIC', '-std=c90'] if configuration['output_type'] == 'shared-library' else []

    execute(['gcc', '-o', system.output_file, '-Wall', '-Werror'] + shared_args + inc_path_args + system.c_files)
