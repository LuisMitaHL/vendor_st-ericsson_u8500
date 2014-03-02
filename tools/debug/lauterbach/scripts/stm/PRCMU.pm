#===============================================================================
#
#         File:  PRCM.pm
#
#  Description:  PRCMU trace decoder
#
#
#       Author:  Philippe Langlais (plang), philippe.langlais@linaro.com
#      Company:  ST-Ericsson
#      Version:  1.0
#      Created:  20/02/2011 10:35:35
#===============================================================================
package PRCMU;

use strict;
use warnings;

use subs qw(Init Decode);

my %prcmuVal2sym;

# Init PRCMU decoder
sub Init
{
	my $stmBegin;

	open(NF, "<$_[0]") || die "Can't open $_[0]: $!";
	while(<NF>) {
		chomp(my $l = $_);
		if (defined $stmBegin &&
				$l =~ /^\s*([A-Za-z0-9_]+)\s*=\s*0x([0-9A-Fa-f]+)[\s\,\}]*/) {
			$prcmuVal2sym{hex($2)} = $1;
#			printf("PRCMU %-20.20s = 0x%02X\n", $1, hex($2));
		}
		elsif ($l =~ /STM register/) {
			$stmBegin = 1;
		}
		elsif ($l =~ /\}\s*t_STM\s*\;/) {
			last;
		}
	}
	close NF;
}

sub Decode
{
	my $val = ord($_[0]);
	my $decodeVal = $prcmuVal2sym{$val};
	if (defined $decodeVal) {
		return $decodeVal;
	}
	else {
		return sprintf("0x%02X", $val);
	}
}

1;
__END__
