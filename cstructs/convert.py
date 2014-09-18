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

first_word = True

def lowermatch(match):
  global first_word
  prefix = '__' if first_word else '_'
  ret = prefix + match.group(0).lower()
  first_word = False
  return ret

# Use this function as in:
#   line = re.sub(<my re>, from_camel_case, line)
def from_camel_case(match):
  global first_word
  first_word = True
  return re.sub(r'\B[A-Z]', lowermatch, match.group(0)[1:]).lower()

# These are ordered so, when old name A is a substring
# of old name B, then A appears later in the list than B.
# Thanks to the from_camel_case function below, we need
# only list nontrivially renamed functions here.
array_repls = [
               [r'CArrayStruct',              r'ArrayStruct'],
               [r'CArray.h',                  r'array.h'],
               [r'elementSize',               r'item_size'],
               [r'elements',                  r'items'],
               [r'CompareFunction',           r'array__CompareFunction'],
               [r'CArrayElementOfType',       r'array__item_val'],
               [r'CArrayElement',             r'array__item_ptr'],
               [r'CArrayAddElementByPointer', r'array__add_item_ptr'],
               [r'CArrayAddElement',          r'array__add_item_val'],
               [r'CArrayAppendContents',      r'array__append_array'],
               [r'CArrayNewElement',          r'array__new_item_ptr'],
               [r'CArrayRemoveElement',       r'array__remove_item'],
               [r'CArrayAddZeroedElements',   r'array__add_zeroed_items'],
               [r'CArray[A-Z]\w*',            from_camel_case],
               [r'\bCArray\b',                r'Array']]

list_repls = [
              [r'CListStruct', r'ListStruct'],
              [r'CList.h',     r'list.h'],
              [r'CList[A-Z]\w*', from_camel_case],
              [r'\bCList\b', r'List']]

map_repls = [
             [r'CMapStruct',        r'MapStruct'],
             [r'CMap.h',            r'map.h'],
             [r'\bHash\b',          r'map__Hash'],
             [r'\bEq\b',            r'map__Eq'],
             [r'\bAlloc\b',         r'map__Alloc'],
             [r'\bkeyReleaser\b',   r'key_releaser'],
             [r'\bvalueReleaser\b', r'value_releaser'],
             [r'\bpairAlloc\b',     r'pair_alloc'],
             [r'\bKeyValuePair\b',  r'map__key_value'],
             [r'CMap[A-Z]\w*',      from_camel_case],
             [r'\bCMap\b',          r'Map']]


def convert_line(line):
  for repl_list in [array_repls, list_repls, map_repls]:
    for repl in repl_list:
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

