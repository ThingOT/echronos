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
import os.path
from prj import Module
from operator import itemgetter


class PhactModule(Module):
    xml_schema_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema.xml')
    files = [
        {'input': 'rtos-phact.h', 'render': True},
        {'input': 'rtos-phact.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        # Create builtin signals
        config['signal_labels'].append({'name': '_task_timer', 'idx': len(config['signal_labels'])})

        # Create signal_set definitions from signal definitions:
        config['signal_sets'] = [{'name': sig['name'], 'value': 1 << sig['idx'], 'singleton': True}
                                 for sig in config['signal_labels']]

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''
        config['prefix_type'] = config['prefix'].capitalize() if config['prefix'] is not None else ''
        config['prefix_const'] = config['prefix'].upper() + '_' if config['prefix'] is not None else ''

        # get merged task and mutex lists to figure out priorities
        tasks = config['tasks']
        tasks.sort(key=itemgetter('priority'), reverse=True)
        for idx, t in enumerate(tasks):
            t['idx'] = idx
            # Create a timer for each task
            timer = {'name': '_task_' + t['name'],
                     'error': 0,
                     'reload': 0,
                     'task': t,
                     'idx': len(config['timers']),
                     'enabled': False,
                     'sig_set': '_task_timer'}
            t['timer'] = timer
            config['timers'].append(timer)

        mutexes = config['mutexes']
        mutexes.sort(key=itemgetter('priority'), reverse=True)
        for idx, m in enumerate(mutexes):
            m['idx'] = idx
        mutex_tasks = tasks + mutexes

        config['mutex_tasks_length'] = len(mutex_tasks)
        mutex_tasks.sort(key=itemgetter('priority'), reverse=True)
        for idx, mt in enumerate(mutex_tasks):
            mt['sched_idx'] = idx
        config['mutex_tasks'] = mutex_tasks

        # determine scheduling queue index type size
        if len(mutex_tasks) >= 65536:
            config['schedindex_size'] = 32
        elif len(mutex_tasks) >= 256:
            config['schedindex_size'] = 16
        else:
            config['schedindex_size'] = 8

        return config

module = PhactModule()
