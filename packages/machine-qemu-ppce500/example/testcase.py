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
import os
import subprocess
import unittest
from pylib import tests


class TestCase(tests.GdbTestCase):
    @unittest.skipIf(os.name == 'nt', "not supported on this operating system because cross-platform toolchain is not\
 available")
    def setUp(self):
        super(TestCase, self).setUp()
        self.qemu = subprocess.Popen(('qemu-system-ppc', '-S', '-nographic', '-gdb', 'tcp::18181', '-M', 'ppce500',
                                      '-kernel', self.executable_path))

    def _get_test_command(self):
        return ('powerpc-linux-gdb', '--batch', self.executable_path, '-x', self.gdb_commands_path)

    def tearDown(self):
        self.qemu.terminate()
        self.qemu.wait()
