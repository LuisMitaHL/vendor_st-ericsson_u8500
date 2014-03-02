#!/usr/bin/perl
#===============================================================================
#
#         FILE:  changeCDA.pl
#
#        USAGE:  ./changeCDA.pl <CDA.cmm>
#
#  DESCRIPTION:
#
#===============================================================================

use strict;
use warnings;



my $file_name = 'CDA_STE.cmm';		# output file name
my @split_line;
my $flag;

$flag=0;

open my $IFILE, '<', $ARGV[0] or die  "$0 : failed to open  input file '$ARGV[0]' : $!\n";
open my $OFILE, '>', $file_name or die  "$0 : failed to open  output file '$file_name' : $!\n";
open my $NewCDAFile, '>NewCDAFileFlag'  or die  "$0 : failed to open  output file 'NewCDAFileFlag' : $!\n";

while (<$IFILE>) {
	if ($_ =~ /Setting register values/)
	{
		$flag=1;
	}
	next if $_ !~ /data.load.binary/;
	@split_line = split(/\\/);
	print $OFILE "$split_line[0]/$split_line[$#split_line]";
}
if ($flag == 0)
{
	print $NewCDAFile "0";
}
else
{ 
	print $NewCDAFile "1";
}

close  $IFILE or warn "$0 : failed to close input file '$ARGV[0]' : $!\n";
close  $OFILE or warn "$0 : failed to close output file '$file_name' : $!\n";
close  $NewCDAFile or warn "$0 : failed to close output file 'NewCDAFileFlag' : $!\n";

exit 0;

