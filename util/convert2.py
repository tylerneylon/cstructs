#!/usr/local/bin/python3
#
# convert2.py from https://github.com/tylerneylon/cstructs
#
# A tool to convert existing C code that uses the old
# cstructs names over to using the new cstructs names.
#
# This only covers the renaming of map__find to map__get.
#

import os
import re
import sys

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print('Usage: %s <C or header file>' % os.path.basename(sys.argv[0]))
    print('Output is sent to stdout')
    exit(0)

  with open(sys.argv[1]) as f:
    old_lines = f.readlines()

  old_re = r'\bmap__find\b'
  repl   = r'map__get'
  new_lines = [re.sub(old_re, repl, line) for line in old_lines]
  for line in new_lines:
    sys.stdout.write(line)
