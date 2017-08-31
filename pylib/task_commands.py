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
from .utils import TOP_DIR, BASE_DIR
from .cmdline import cmd, Arg
from .task import Task, TaskConfiguration

task_cfg = TaskConfiguration(repo_path=TOP_DIR,
                             tasks_path=os.path.join('pm', 'tasks'),
                             description_template_path=os.path.join(BASE_DIR, '.github', 'PULL_REQUEST_TEMPLATE.md'),
                             reviews_path=os.path.join('pm', 'reviews'),
                             mainline_branch='development')

_offline_arg = Arg('-o', '--offline', action='store_true',
                   help='Skip all git commands that require an Internet connection')
_taskname_arg = Arg('taskname', nargs='?', help='The name of the task to manage. Defaults to the active git branch.')

@cmd(args=(_offline_arg, Arg('taskname', help='The name of the task to manage.')),
     help='Developers: create a new task to work on, including a template for the task description.')
def create(args):
    task = Task(task_cfg, name=args.taskname, checkout=False)
    task.create(offline=args.offline)


@cmd(args=(_offline_arg, _taskname_arg),
     help='Developers: bring a task up-to-date with the latest changes on the mainline branch "{}". '
          'If the task is not yet on review, this rebases the task branch onto the mainline branch. '
          'If the task is on review, the mainline changes are merged into the task branch.'
          .format(task_cfg.mainline_branch))
def update(args):
    task = Task(task_cfg, name=args.taskname)
    task.update(offline=args.offline)


@cmd(args=(_offline_arg, _taskname_arg),
     help='Developers: request reviews for a task.')
def request_reviews(args):
    task = Task(task_cfg, name=args.taskname)
    task.request_reviews(args.offline)


@cmd(args=(_offline_arg, _taskname_arg,
     Arg('-a', '--accept', action='store_true',
         help='Create and complete the review with the conclusion "accepted", commit, and push it.')),
     help='Reviewers: create a stub for a new review of the active task branch.')
def review(args):
    task = Task(task_cfg, name=args.taskname)
    task.review(offline=args.offline, accept=args.accept)


@cmd(args=(_offline_arg, _taskname_arg),
     help='Reviewers: create, commit, and push a new review for the active task branch with the conclusion '
          '"Accepted". This is an alias for the command "x.py task review --accept".')
def accept(args):
    args.accept = True
    review(args)


@cmd(help='Developers: integrate a completed task branch into the mainline branch {}.'
          .format(task_cfg.mainline_branch),
     args=(_taskname_arg,))
def integrate(args):
    task = Task(task_cfg, name=args.taskname)
    task.integrate()
