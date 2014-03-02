#!/usr/local/bin/perl

# Concatenate two binary files and delete the second one

local $/ = undef; # No input sep

my $file1=shift;
my $file2=shift;

open(OUTPUT, ">>$file1");
open(BUF2, "<$file2");
binmode OUTPUT;
binmode BUF2;
print OUTPUT <BUF2>;
close(OUTPUT);
close(BUF2);

unlink("$file2");

