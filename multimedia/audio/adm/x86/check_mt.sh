#!/bin/bash
facit_pattern=$(tempfile)
test_pattern=$(tempfile)
echo $test_pattern
echo $facit_pattern
grep "\[MT\]" $1 > $test_pattern
grep "\[MT OK\]" $2 | sed 's/\[MT OK\]/\[MT\]/' > $facit_pattern
diff -u $facit_pattern $test_pattern
