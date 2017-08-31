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
import ctypes
import os
import sys

from rtos import sched
from pylib.utils import get_executable_extension


class testRrSched:
    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-rr")
        system = "out/posix/unittest/sched-rr/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sched = ctypes.POINTER(sched.get_rr_sched_struct(10)).in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            assert self.impl.pub_sched_get_next() == model.get_next()
            assert self.impl_sched == model

        states = sched.RrSchedModel.states(10, assume_runnable=True)
        for i, s in enumerate(states):
            yield "check_state.{}".format(i), check_state, s


class testPrioSched:
    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-prio")
        system = "out/posix/unittest/sched-prio/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        cls.impl_sched = ctypes.POINTER(sched.get_prio_sched_struct(10)).in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            assert self.impl.pub_sched_get_next() == model.get_next()
            assert self.impl_sched == model

        states = sched.PrioSchedModel.states(10, assume_runnable=True)
        for i, s in enumerate(states):
            yield "check_state.{}".format(i), check_state, s


class testPrioInheritSched:
    test_size = 5

    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.sched-prio-inherit")
        system = "out/posix/unittest/sched-prio-inherit/system" + get_executable_extension()
        assert r == 0
        cls.impl = ctypes.CDLL(system)
        pub_sched_tasks = ctypes.POINTER(sched.get_prio_inherit_sched_struct(cls.test_size))
        cls.impl_sched = pub_sched_tasks.in_dll(cls.impl, 'pub_sched_tasks')[0]

    def test_simple(self):
        def check_state(model):
            self.impl_sched.set(model)
            impl_next = self.impl.pub_sched_get_next()
            if impl_next == 255:
                impl_next = None
            model_next = model.get_next()
            assert impl_next == model_next, "Result: {} Expected: {}".format(impl_next, model_next)
            assert self.impl_sched == model

        states = sched.PrioInheritSchedModel.states(self.test_size, assume_runnable=True)
        for i, s in enumerate(states):
            yield "check_state.{}".format(i), check_state, s
