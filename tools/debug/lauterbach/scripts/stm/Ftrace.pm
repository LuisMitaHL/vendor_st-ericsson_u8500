#===============================================================================
#
#         File:  Ftrace.pm
#
#  Description:  Ftrace trace decoder
#
#
#       Author:  Philippe Langlais (plang), philippe.langlais@linaro.com
#      Company:  ST-Ericsson
#      Version:  1.0
#      Created:  march 2011
#===============================================================================
package Ftrace;

use strict;
use warnings;

use subs qw(Init Decode GraphDecode DecodeStack FindNearestSymbol FindString);

use constant HASH_SIZE=>0x400;
use constant HASH_MASK=>0xFFFFFC00;

my ($start, $end);
my %ftraceSymbols;
my %symbolsArrays;
my %addrArrays;
my $strings;
my $addrStrings;

sub ReadPrintkFormats
{
	my $vmlinux = "$_[0]/vmlinux";

	$strings = "";
	my $startS = $addrArrays{kallsyms_token_index};
	my $endS = $addrArrays{__proc_info_begin};
	if (defined $startS) {
		if (open (OBJD, "objdump --section=.text -s --start-address=$startS ".
				"--stop-address=$endS $vmlinux |")) {
			while (<OBJD>) {
				if (/^\s+([[:xdigit:]]+)((\s[[:xdigit:]]+){4})/) {
					my ($addr, $values) = ($1, $2);
					$addrStrings = hex($addr) if (! defined $addrStrings);
					$values =~ s,\s+,,g;
					$values =~ s,(..),chr(hex($1)),eg;
					$strings .= $values;
				}
			}
			close OBJD;
#		print "\@$addrStrings $strings\n";
		}
	}
	else {
		print STDERR "Can't extract trace_printk formats\n";
	}
}

# Init Ftrace decoder
sub Init
{
	my $stmBegin;
	my $add;
	my $kernelRoot = $_[0];

	if (open(MAP, "<$kernelRoot/System.map")) {
		while(<MAP>) {
			if (/^([[:xdigit:]]+) [tT] ([\w\.]+)/) {
				$add = hex($1);
				$start = $add unless defined $start;
				$ftraceSymbols{$add} = $2;
				$addrArrays{$2} = $add;
				push (@{$symbolsArrays{$add & HASH_MASK}}, $add);
			}
			elsif (/^([[:xdigit:]]+) [Rr] ([\w\.]+)/) {
				$end = hex($1);
				last;
			}
		}
		close MAP;
		printf "Symbols found from @%08X to @%08X\n", $start, $end;
	}
	else {
		warn "$kernelRoot/System.map: $!\n";
	}

	undef $addrStrings;

	# Read printk string format
	ReadPrintkFormats($kernelRoot);
}

sub FindString
{
	my $add = $_[0];
	my $char;
	my $string = "";

	return "" if ($add < $addrStrings);
	my $offset = $add - $addrStrings;
	for (;;) {
		$char = substr($strings, $offset++, 1);
		last if ($char eq chr(0));
		$string .= $char;
	}
	return $string;
}

sub FindNearestSymbol
{
	my $add = $_[0];

	if (exists $ftraceSymbols{$add}) {
		return ($ftraceSymbols{$add}, 0);
	}
	else {
		my $hash = $add & HASH_MASK;
		my $symb;
		my $belowAdd;

		return (sprintf("@%08X",$add), 0) if ($add >= $end || $add < $start);

		while (!exists $symbolsArrays{$hash}) {
			$hash -= HASH_SIZE;
		}

		if ($symbolsArrays{$hash}[0] > $add) {
			do {
				$hash -= HASH_SIZE;
			} while (!exists $symbolsArrays{$hash});
			$belowAdd = $symbolsArrays{$hash}[$#{$symbolsArrays{$hash}}];
			$symb = $ftraceSymbols{$belowAdd};
			return ($symb, $add-$belowAdd);
		}
		else {
			for (my $i=$#{$symbolsArrays{$hash}}; $i >= 0 ; $i--) {
				$belowAdd = $symbolsArrays{$hash}[$i];
				if ($add >= $belowAdd) {
					$symb = $ftraceSymbols{$belowAdd};
					return ($symb, $add-$belowAdd);
				}
			}
		}
	}
}

sub Decode
{
	my ($parent_ip, $ip) = unpack("VV", $_[0]); # on little endian arch
	my ($symb, $off) = FindNearestSymbol($ip);
	my ($psymb, $poff) = FindNearestSymbol($parent_ip);
	return "$symb <-".$psymb.(($poff!=0) ? sprintf("+0x%X\n", $poff) : "\n");
}

sub GraphDecode
{
	my ($func, $depth, $calltimeH, $calltimeL, $rettimeH, $rettimeL, $overrun);
	if (length($_[0]) == 8) {
		($depth, $func) = unpack("VV", $_[0]); # on little endian arch
	}
	else {
		($depth, $func, $calltimeL, $calltimeH,
			$rettimeL, $rettimeH, $overrun) = unpack("VVVVVVV", $_[0]);
	}
	my ($symb, $off) = FindNearestSymbol($func);
	if (defined $overrun) {
		my $time = $rettimeL-$calltimeL;
		if ($time < 0) {
			$time ^= 0xFFFFFFFF;
			$time++;
		}
		return "${symb}[$depth], ${time}ns, over=$overrun\n";
	}
	else {
		return "${symb}[$depth]\n";
	}
}

sub DecodeStack
{
	my $caller = unpack("V", $_[0]); # on little endian arch
	my ($symb, $off) = FindNearestSymbol($caller);
	return $symb . (($off!=0) ? sprintf("+0x%X\n", $off) : "\n");
}

1;
__END__
