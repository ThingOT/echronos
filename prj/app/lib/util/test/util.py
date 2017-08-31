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
from util.util import do_nothing, Singleton, s16l, check_unique, remove_multi, add_index, \
    LengthMixin, LengthList, config_traverse, config_set, list_search
from nose.tools import assert_raises, raises


def test_do_nothing_no_args():
    do_nothing()


def test_do_nothing_some_args():
    do_nothing(1, 2, 3)


def test_do_nothing_kw_args():
    do_nothing(x=1, b=2, c=3)


def test_singleton():
    x = Singleton('x')
    x1 = x
    y = Singleton('y')
    y1 = y

    assert x1 is x
    assert x1 == x
    assert y1 is y
    assert y1 == y
    assert x is not y

    assert str(x) == '<Singleton: x>'
    assert str(y) == '<Singleton: y>'


def check_s16l(value, n, expected):
    assert s16l(value, n) == expected


def test_s16l_zero():
    for n in range(16):
        yield 'n={}'.format(n), check_s16l, 0, n, 0


def test_s16l_ffff():
    for n, expected in [(1, 0xfffe), (8, 0xff00), (15, 0x8000), (16, 0)]:
        yield 'n={}'.format(n), check_s16l, 0xffff, n, expected


def test_check_unique_no_dups():
    check_unique(range(40))


def test_check_unique_dups():
    lst = list(range(40)) + [39]
    with assert_raises(ValueError) as cm:
        check_unique(lst)
    assert str(cm.exception) == "Duplicates found in list: [(39, 2)]"


def test_remove_multi():
    x = list(range(3))
    remove_multi(x)
    assert x == list(range(3))

    remove_multi(x, 0, 2)
    assert x == [1]

    x = list(range(3))
    remove_multi(x, *list(range(3)))
    assert x == []


def add_index_setup():
    lst = []
    for i in range(3):
        lst.append({'value': i})
    return lst


def add_index_check(lst):
    assert lst == [{'value': 0, 'idx': 0},
                   {'value': 1, 'idx': 1},
                   {'value': 2, 'idx': 2}]


def test_add_index_no_exist():
    lst = add_index_setup()

    add_index(lst, 'idx')

    add_index_check(lst)


def test_add_index_idx_is_none():
    lst = add_index_setup()
    for d in lst:
        d['idx'] = None

    add_index(lst, 'idx')

    add_index_check(lst)


def test_add_index_some_exist():
    lst = add_index_setup()
    lst[1]['idx'] = 1

    add_index(lst, 'idx')

    add_index_check(lst)


def test_add_index_sort():
    lst = [{'value': 1, 'idx': 1},
           {'value': 0},
           {'value': 2}]

    add_index(lst, 'idx')
    add_index_check(lst)


def test_add_index_idx_out_of_range():
    lst = add_index_setup()
    lst[0]['idx'] = 4

    with assert_raises(ValueError) as cm:
        add_index(lst, 'idx')
    assert str(cm.exception) == "Some index value are out-of-range: [4]"


def test_add_index_idx_duplicate():
    lst = add_index_setup()
    lst[0]['idx'] = 1
    lst[1]['idx'] = 1

    with assert_raises(ValueError) as cm:
        add_index(lst, 'idx')
    assert str(cm.exception) == "Duplicates found in list: [(1, 2)]"


def test_length_mixin():
    class Foo:
        def __len__(self):
            return 123

    class LengthFoo(LengthMixin, Foo):
        pass

    lf = LengthFoo()

    assert lf.length == 123


def test_length_list():
    assert LengthList(range(5)).length == 5


def test_config_traverse():
    cfg = {'foo': [5, 6, 7], 'bar': {'baz': 7, 'qux': 8}}
    assert set(config_traverse(cfg)) == set([(('foo', 0), 5),
                                             (('foo', 1), 6),
                                             (('foo', 2), 7),
                                             (('bar', 'baz'), 7),
                                             (('bar', 'qux'), 8)])

    cfg = {}
    assert set(config_traverse(cfg)) == set()


def test_config_set():
    cfg = {'foo': [5, 6, 7], 'bar': {'baz': 7, 'qux': 8}}

    config_set(cfg, ('foo', 0), 10)

    assert cfg['foo'][0] == 10


def test_list_search():
    lst = [{'foo': 5, 'bar': 'A'},
           {'foo': 6, 'bar': 'B'},
           {'foo': 7, 'bar': 'C'}]

    assert list_search(lst, 'foo', 6)['bar'] == 'B'

    with assert_raises(KeyError):
        list_search(lst, 'foo', 8)
