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
from util.crc16 import Crc16Ccitt, crc16ccitt


def test_empty():
    assert crc16ccitt() == 0xffff


def is_expected(inp, expected):
    assert crc16ccitt(inp) == expected


def test_expected():
    # Test values determined from:
    #   http://www.lammertbies.nl/comm/info/crc-calculation.html
    for inp, expected in [
            (b'0', 0xD7A3),
            (b'0a', 0x641D),
            (b'123456789', 0x29B1),
            (b'foo_bar', 0x37DF)]:
        yield inp.decode('ascii'), is_expected, inp, expected


def is_expected_multi(inp, expected):
    assert crc16ccitt(*inp) == expected


def test_multi():
    for inp, expected in [
            ((b'0', b'a'), 0x641D),
            ((b'1234', b'56789'), 0x29B1),
            ((b'foo_', b'bar'), 0x37DF)]:
        yield ''.join([i.decode('ascii') for i in inp]), is_expected_multi, inp, expected


def test_reuse():
    c = Crc16Ccitt()

    for b in b'foo':
        c.add(b)
    assert c.result(reset=False) == 0x630A

    for b in b'bar':
        c.add(b)
    assert c.result(reset=False) == 0xBE35

    c.reset()

    for b in b'foo':
        c.add(b)
    assert c.result() == 0x630A

    for b in b'bar':
        c.add(b)
    assert c.result() == 0x5F59
