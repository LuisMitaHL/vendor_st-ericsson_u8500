#!/bin/env perl
#--------------------------------------------------------------------
## @file
# Power is a script that allows to control the ePowerSwitch located 
# in the MMD Audio lab (room G447, Grenoble).
# Please contact Florent MARTIN (041 6493) for support, or visit:
# http://gnx772.gnb.st.com:8080/~martinfl/cgi-bin/ePowerSwitch/
#---------------------------------------------------------------------

use strict;
use HTTP::Request::Common;
require LWP::UserAgent;

my $cmd    = shift;
my $outlet = shift;
my $user   = `whoami`;
my $url_script = "http://gnx772.gnb.st.com:8080/~martinfl/cgi-bin/ePowerSwitch/power_exec.pl";

if ($ENV{POWER_SCRIPT_URL}) {
	$url_script = $ENV{POWER_SCRIPT_URL};
}

chomp($user);

$url_script = $url_script."?user=$user&cmd=$cmd&outlet=$outlet";

my $ua = LWP::UserAgent->new;
$ua->timeout(10);
$ua->env_proxy;
my $response = $ua->get($url_script);
if ($response->is_success) {
	print $response->decoded_content;
} else {
	die "power: a remote error occured.\nBlame Florent MARTIN (041 6493)!";
}
