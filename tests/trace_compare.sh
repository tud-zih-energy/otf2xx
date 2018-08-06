#!/bin/sh

$1 -G $2 > trace_A
$1 -G $2 >> trace_A

$1 -G $3 > trace_B
$1 -G $3 >> trace_B

diff trace_A trace_B

RETVAL=$?

rm -f trace_A trace_B

exit $RETVAL
