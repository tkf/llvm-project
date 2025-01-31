#  Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
#  See https://llvm.org/LICENSE.txt for license information.
#  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

try:
  from ..ir import *
except ImportError as e:
  raise RuntimeError("Error loading imports from extension module") from e

from typing import Any, Sequence


class ForOp:
  """Specialization for the SCF for op class."""

  def __init__(self,
               lower_bound,
               upper_bound,
               step,
               iter_args: Sequence[Any] = [],
               *,
               loc=None,
               ip=None):
    """Creates an SCF `for` operation.

    - `lower_bound` is the value to use as lower bound of the loop.
    - `upper_bound` is the value to use as upper bound of the loop.
    - `step` is the value to use as loop step.
    - `iter_args` is a list of additional loop-carried arguments.
    """
    results = [arg.type for arg in iter_args]
    super().__init__(
        self.build_generic(
            regions=1,
            results=results,
            operands=[lower_bound, upper_bound, step] + list(iter_args),
            loc=loc,
            ip=ip))
    self.regions[0].blocks.append(IndexType.get(), *results)

  @property
  def body(self):
    """Returns the body (block) of the loop."""
    return self.regions[0].blocks[0]

  @property
  def induction_variable(self):
    """Returns the induction variable of the loop."""
    return self.body.arguments[0]

  @property
  def inner_iter_args(self):
    """Returns the loop-carried arguments usable within the loop.

    To obtain the loop-carried operands, use `iter_args`.
    """
    return self.body.arguments[1:]
