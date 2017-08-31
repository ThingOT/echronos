#!/usr/bin/env python3
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
def main():
    parser = argparse.ArgumentParser(prog='task.py')
    add_commands_to_parser(globals(), parser)
    args = parser.parse_args()

    if not args.command:
        # argparse does not support required subparsers so it does not itself reject a command line that lacks a
        # command or subcommand
        parser.print_help()
    else:
        args.execute(args)


if __name__ == "__main__":
    main()
