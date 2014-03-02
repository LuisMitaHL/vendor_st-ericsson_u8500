#===============================================================================
#
#         File:  ContextSwitch.pm
#
#  Description:  Scheduler Context Switch & Scheduler wakeup trace decoder
#
#
#       Author:  Philippe Langlais (plang), philippe.langlais@linaro.com
#      Company:  ST-Ericsson
#      Version:  1.0
#      Created:  march 2011
#===============================================================================
package ContextSwitch;

use strict;
use warnings;

use subs qw(Init Decode);

# Restriction: only little endian platform supported

# Init ContextSwitch decoder
sub Init
{
}

sub Decode
{
	my ($data, $wakeup) = @_;

	my $type = (defined $wakeup) ? "  +" : "==>";

	my ($next_pid, $prev_pid, $next_state, $next_prio,
		$prev_state, $prev_prio,  $next_cpu) = unpack("VVCCCCV", $_[0]);

	return sprintf("%5u:%3u:%s $type %5u:%3u:%s [%u]\n",
		$prev_pid, $prev_prio, ($prev_state ? "S" : "R"),
		$next_pid, $next_prio, ($next_state ? "S" : "R"), $next_cpu);
}

1;
__END__
