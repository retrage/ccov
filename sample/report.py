#!/usr/bin/env python
# SPDX-License-Identifier: MIT

import os
import sys

SIG = '#CCOV'
INDEX_WIDTH = 6
LINE_WIDTH = 6

class Trace:
    def __init__(self, string):
        assert string.startswith(SIG), "invalid trace format"
        tokens = string.split(':')
        assert len(tokens) >= 4, "unexpected number of tokens"
        self.file = tokens[1]
        self.func = tokens[2]
        self.line = int(tokens[3]) - 1 # 0-indexed

    def __repr__(self):
        return { 'file':self.file, 'func':self.func, 'line':self.line }

    def __str__(self):
        return 'Trace(file={},func={},line={})'.format(self.file, self.func, self.line)

def print_srcline(srcs, index, trace):
    code = srcs[trace.file][trace.line][:-1]
    index_str = str(index)
    index_padding = ' ' * (INDEX_WIDTH - len(index_str))
    line_str = str(trace.line)
    line_padding = ' ' * (LINE_WIDTH - len(line_str))
    print(index_padding + index_str + '|' \
        + line_padding + line_str + '|' \
        + code)

def usage(argv0):
    print('{} LOG SRC'.format(argv0))

def main():
    if len(sys.argv) < 3:
        usage(argv[0])
        return -1

    traces = []
    with open(sys.argv[1]) as logfile:
        for line in logfile:
            if line.startswith(SIG):
                traces.append(Trace(line))

    files = set()
    for trace in traces:
        files.add(trace.file)

    srcs = {}
    for file in files:
        src = []
        path = os.path.join(sys.argv[2], file)
        with open(path) as srcfile:
            srcs[file] = srcfile.readlines()

    for index, trace in enumerate(traces, 1):
        print_srcline(srcs, index, trace)

    return 0

if __name__ == '__main__':
    sys.exit(main())
