#!/usr/bin/perl
#===============================================================================
#
#         FILE:  parse_event-log-tags.pl
#
#        USAGE:  ./parse_event-log-tags.pl
#
#  DESCRIPTION:
#
#       AUTHOR:  Y. Gautier (YGA), yann PT gautier AT stericsson PT com
#      COMPANY:  ST-Ericsson Le Mans
#      VERSION:  1.0
#      CREATED:  05/20/2010 05:43:52 PM
#===============================================================================

use strict;
use warnings;


use Getopt::Std;

#==============================================================================
# Global variables
#==============================================================================
use vars qw($opt_v $opt_h);
use vars qw($tag_file $tag $out_file $paramString $dislpay_line $extr $extr2 $remaining $msg_type $index $length $lgth);
use vars qw(@tableLine @paramList @params);

#==============================================================================
# Sub-routines
#==============================================================================

## Debug print
sub dprint
{
	print STDERR @_ if ($opt_v);
}

## system avec print
sub dsystem
{
	print "@_\n" if ($opt_v);
	return (system @_) / 256;
}


sub display_format
{
	my $type = shift;
	if ($type==1) {
		# 1: Number of objects
		#$dislpay_line.= "objects";
	} elsif ($type==2) {
		# 2: Number of bytes
		#$dislpay_line.= "bytes";
	} elsif ($type==3) {
		# 3: Number of milliseconds
		$dislpay_line.= "ms";
	} elsif ($type==4) {
		# 4: Number of allocations
		#$dislpay_line.= "allocations ";
	} elsif ($type==5) {
		# 5: Id
		#$dislpay_line.= "Id";
	} elsif ($type==6) {
		# 6: Percent
		$dislpay_line.= "%";
	} else {
		# shouldn't occur
	}
}

#==============================================================================
# Main
#==============================================================================
# Input parameters management
#if ( !Getopt::Std::getopts('vh') || $#ARGV < 0 || defined($opt_h))
if ( !Getopt::Std::getopts('vh') || defined($opt_h))
{
    die("Usage: $0 [-v]erbose file\n");
}

$tag_file=shift;
$tag=shift;
$out_file=shift;

open(OFILE, "<$out_file");
binmode OFILE;
my $message=<OFILE>;
close(OFILE);
unlink $out_file;

open(IFILE, "<$tag_file") or die "File $tag_file not found: $!";
while(<IFILE>) {
	next if $_ !~ /^$tag /;
	chomp($_);
	@tableLine = split(" ",$_);
}
close(IFILE);


# In case there are spaces in parameters'names
$paramString=$tableLine[2];
for ( $index=3 ; $index <= $#tableLine ; $index++ ) {
	$paramString.=" ".$tableLine[$index];
}


@paramList = split(",",$paramString);

$dislpay_line = "$tableLine[1]: ";


# parse the event file line and put all parameters in @params
for ( $index=0 ; $index <= $#paramList ; $index++ ) {
	$paramList[$index] =~ s/\(//;
	$paramList[$index] =~ s/\)//;
	my @tmp_paramList = split(/\|/,$paramList[$index]);
	$tmp_paramList[2]=0 if ($#tmp_paramList==1);
	push @params,\@tmp_paramList;


}

($msg_type,$remaining) = unpack('Ca*',$message);
$message=$remaining;

if ($msg_type==0) {
	# 0: int
	($extr,$remaining) = unpack('La*',$message);
	$dislpay_line.= " $extr";
	display_format($params[0][2]);
} elsif ($msg_type==1) {
	# 1: long
	($extr,$extr2,$remaining) = unpack('l2a*',$message);
	$extr+=$extr2*0x100000000;
	$dislpay_line.= " $extr";
	display_format($params[0][2]);
} elsif ($msg_type==2) {
	# 2: string
	($lgth,$extr) = unpack('La*',$message);
	$message=$extr;
	($extr,$remaining) = unpack("a[$lgth]a*",$message);
	$dislpay_line.= " $extr";
} elsif ($msg_type==3) {
	# 3: list
	($length,$extr) = unpack('Ca*',$message);
	$message=$extr;
	for ( $index=0 ; $index < $length ; $index++ ) {
		$dislpay_line.=", " if (($index > 0)&&($index < $length));
		$dislpay_line.=$params[$index][0];
		$dislpay_line.=" ";
		($msg_type,$remaining) = unpack('Ca*',$message);
		$message=$remaining;
		if ($msg_type==0) {
			# 0: int
			($extr,$remaining) = unpack('La*',$message);
			$dislpay_line.= "$extr";
			display_format($params[$index][2]);
		} elsif ($msg_type==1) {
			# 1: long
			($extr,$extr2,$remaining) = unpack('l2a*',$message);
			$extr+=$extr2*0x100000000;
			$dislpay_line.= "$extr";
			display_format($params[$index][2]);
		} elsif ($msg_type==2) {
			# 2: string
			($lgth,$extr) = unpack('La*',$message);
			$message=$extr;
			($extr,$remaining) = unpack("a[$lgth]a*",$message);
			$dislpay_line.= "$extr";
		} elsif ($msg_type==3) {
			# 3: list
			# should be a recursive function
		}
		$message=$remaining;
	}
}



open(OFILE, ">$out_file");
print OFILE "$dislpay_line\n";
close(OFILE);

exit 0;

