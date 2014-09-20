#!/usr/local/bin/python3
#
# bye_camels.py from https://github.com/tylerneylon/cstructs
#
"""
 Usage:
  bye_camels.py <input_file> [<output_file>]
  bye_camels.py -i <input_file> <output_file>

 Replaces all camel-case identifiers in the input with
 a non-camel-case version in the output file.

 Using -i queries for each identifier, so that you can
 leave in some special-case camel-case identifiers.
 When not using -i and providing a single filename, output
 is sent to standard out; when using -i, it's required to
 provide the output filename as a command line parameter.
"""

import re
import sys

# This maps seen tokens to a <do_update> boolean.
update_token_status = {}

def from_camel(token):
  return re.sub(r'(\B[A-Z])', r'_\1', token).lower()

def do_update_token(token, is_interactive):
  global update_token_status
  if not is_interactive: return True
  if token in update_token_status: return update_token_status[token]

  user_reply = input('Update "%s"? [Y/n] ' % token)
  do_update = (user_reply.lower() != 'n')
  update_token_status[token] = (user_reply.lower() != 'n')
  return update_token_status[token]

def replace_camel_case_in_files(infile, outfile, is_interactive=False):
  for line in infile:
    camels = re.findall(r'\b[a-z]*[A-Z]+[a-z][A-Za-z]*', line)
    for camel in camels:
      if do_update_token(camel, is_interactive):
        line = re.sub(camel, from_camel(camel), line)
    outfile.write(line)

if __name__ == '__main__':

  nargs = len(sys.argv)
  if nargs < 2 or (sys.argv[1] == '-i' and nargs < 4):
    print(__doc__)
    exit(2)

  infile_index = 1

  is_interactive = (sys.argv[1] == '-i')
  if is_interactive: infile_index += 1

  infile_name  = sys.argv[infile_index]

  if infile_index + 1 < nargs:
    outfile_name = sys.argv[infile_index + 1]
    outfile = open(outfile_name, 'w')
  else:
    outfile = sys.stdout

  try:
    infile = open(infile_name, 'r')
  except:
    print('Couldn\'t open the file "%s".' % infile_name)
    exit(1)

  replace_camel_case_in_files(infile, outfile, is_interactive)

  infile.close()
  outfile.close()

