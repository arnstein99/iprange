# iprange
Utilities and libraries for manipulating lists of ranges of IPV4 addresses

## Building

Just type `make`. The Makefile specifies C++2a, but C++11 suffices. Edit the
Makefile to change the C++ version if desired.

## Programs

These programs read from standard input and write to standard
output. The input format is text. Within an input, a comment starts
with a space or newline follwed by a hash mark (#). The comment
continues to end-of-line.  Apart from comments, the input consists
of a sequence of ranges of IPV4 addresses. These ranges are separated
by spaces and/or end-of-line. A range may be in one of three forms:

* A single address `nn.nn.nn.nn`
* A range of addresses with a dash `nn.nn.nn.nn-nn.nn.nn.nn`. Note that spaces
around the dash are not permitted.
* A range of addresses in CIDR form `nn.nn.nn.nn/mm`.

Executing any of these programs with a single argument `help` will yield a
brief guide to usage.

### Program ipar_read

Transforms input into a sorted and standardized form. The result is written to
standard output in one of three possible formats. It is mostly used to sort
and remove redundancies.

### Program ipar_interactive

Similar to program ipar_read, but processes input one line at a time. This is
intended to be used interactively.

### Program ipar_intersect

Accepts an arbitrary number of command line arguments, each of which is a file
name. The content represented by these files is intersected with the content
represented by standard input. The result is written to standard output.

### Program ipar_subtract

Accepts an arbitrary number of command line arguments, each of which is a file
name. The content represented by these files is removed from the content
represented by standard input. The result is written to standard output.

### Program ipar_gap_analyzer

Analyzes the content represented by standard input. This program reports on
portions of the content that are "almost dense." Still under development.

### Program ipar_expand

Unlike the other programs, this one does no sorting and does not remove
redundancies. It is mainly useful for testing other programs.

### Script read_test.sh

A test script for program ipar_read.

### Script intersect_test.sh

A test script for program ipar_intersect.

### Script subtract_test.sh

A test script for program ipar_subtract.

## Reusable Software

### ipar_iplist software

The algorithms used to build the programs in the previous section. Includes
text processing as well. This software is contained in these files:
* Makefile
* ipar_iplist.h
* ipar_iplist.cpp
* ipar_common.h
* ipar_common.cpp
* ipar_numlist.h
* ipar_numlist.tcc

### ipar_iplist software

A C++ template library for manipulating intervals expressed using an arbitrary
integer-like type. This software is contained in these files:
* Makefile
* ipar_numlist.h
* ipar_numlist.tcc
