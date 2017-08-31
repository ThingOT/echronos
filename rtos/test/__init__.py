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
__all__ = ['sched', 'simple_mutex', 'blocking_mutex', 'simple_semaphore']

import ctypes
import os
import random
import sys

from pylib.utils import get_executable_extension


class testSimple:
    @classmethod
    def setUpClass(cls):
        r = os.system(sys.executable + " ./prj/app/prj.py build posix.unittest.simple")
        system = "out/posix/unittest/simple/system" + get_executable_extension()
        assert r == 0
        cls.simple = ctypes.CDLL(system)

    def test_foo(self):
        assert self.simple.foo() == 37

    def test_bar(self):
        def check_add(x, y):
            assert x + y == self.simple.add(x, y)

        rand = random.Random()
        rand.seed(37)

        for i in range(20):
            x = rand.randint(0, 5000)
            y = rand.randint(0, 5000)
            yield "test_bar{}".format(i), check_add, x, y
