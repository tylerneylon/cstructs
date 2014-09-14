#!/usr/local/bin/python3
#
# convert.py from https://github.com/tylerneylon/cstructs
#
# A tool to convert existing C code that uses the old
# cstructs names over to using the new cstructs names.
#

import os
import re
import sys

# These are ordered so, when old name A is a substring
# of old name B, then A appears later in the list than B.
array_repls = [[r'CArrayInit', r'array__init'],
               [r'CArrayClear', r'array__clear'],
               [r'CArrayRelease', r'array__release'],
               [r'CArrayDelete', r'array__delete'],
               [r'CArray', r'Array']]
# TODO The above list is badly incomplete. Complete it!
#      Also avoid CArray -> Array in general; check for word boundaries.

def convert_line(line):
  for repl in array_repls:
    line = re.sub(repl[0], repl[1], line)
  return line

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print('Usage: %s <C or header file>' % os.path.basename(sys.argv[0]))
    print('Output is sent to stdout')
    exit(0)

  with open(sys.argv[1]) as f:
    old_lines = f.readlines()

  new_lines = [convert_line(line) for line in old_lines]
  for line in new_lines:
    sys.stdout.write(line)

