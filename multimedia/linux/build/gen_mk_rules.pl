#
#  Copyright (C) ST-Ericsson 2010 - All rights reserved.
#  This code is ST-Ericsson proprietary and confidential.
#  Any use of the code for whatever purpose is subject to
#  specific written permission of ST-Ericsson SA.
#

use strict;
use warnings;

my $component_name = shift(@ARGV);
my @list = @ARGV;

my $value;

## Below 3 entities are left to their default value as some components
## request mmdsp-nmf-src packing type !!??? while MMDSP compiler is
## not delivered to cutomers => forcing bin build

$value = "bin";
#$value = "src" if (join(" ", @list) =~/mmdsp-proc-src/);
print "BUILD_MMDSP_PROC_$component_name","=",$value,"\n";

$value = "1";
#$value = "" if (join(" ", @list) =~/mmdsp-proc-src/);
print "NO_STD_BUILD_$component_name","=",$value,"\n";

$value = "bin";
#$value = "src" if (join(" ", @list) =~/mmdsp-nmf-src/);
print "BUILD_MMDSP_NMF_$component_name","=",$value,"\n";

$value = "bin";
$value = "src" if (join(" ", @list) =~/arm-proc-src/);
print "BUILD_ARM_PROC_$component_name","=",$value,"\n";

$value = "bin";
$value = "src" if (join(" ", @list) =~/arm-proxy-src/);
print "BUILD_ARM_CODE_$component_name","=",$value,"\n";

$value = "bin";
$value = "src" if (join(" ", @list) =~/arm-nmf-src/);
print "BUILD_ARM_NMF_$component_name","=",$value,"\n";

$value = "false";
$value = "true" if (join(" ", @list) =~/xp70-proc-src/);
print "BUILD_XP70_SIA_CODE_$component_name","=",$value,"\n";
