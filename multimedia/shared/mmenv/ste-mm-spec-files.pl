#!/usr/bin/env perl
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

use strict;
use warnings;
use feature "switch";

################################################################################

my $mminstall_file;
my $MMINSTALL_DIR = "/usr/share/ste-mm-devel";
my $MMHEADERDIR="/usr/include/ste-mm";

################################################################################

sub help {
  print "Usage: ste-mm-spec-files.sh COMMAND FILE\n";
  print "\n";
  print "  Utility to extract properties from a mminstall generated file.\n";
  print "\n";
  print "  COMMANDS:\n";
  print "      --has-bin   : return 1 in case argument provide bin package files\n";
  print "      --has-devel : return 1 in case argument provide devel package files\n";
  print "      --gen-bin   : generate to STDOUT the file content to be used by the\n";
  print "                    `%files bin` section of a .spec file\n";
  print "      --gen-devel : generate to STDOUT the file content to be used by the\n";
  print "                    `%files devel` section of a .spec file\n";
  print "\n";
}

################################################################################

sub has_bin {
  print "Error: has_bin not yet implemented";
}

sub gen_devel {

  print '%defattr(-,root,root,-)' . "\n";

  open (MMINSTALL_FILES, "< $mminstall_file");

  for my $line (<MMINSTALL_FILES>) {
    chomp $line;

    ## Ignore MMDSP libraries as they will not be installed
    next if ($line =~ /lib\/mmdsp_8500.*/);

    if($line =~ s/include\/u8500_v2-linux\///) {
      print "$MMHEADERDIR/$line\n";
      next;
    }

    ## Headers are to be installed under
    print "$MMINSTALL_DIR/$line\n";
  }

  close (MMINSTALL_FILES);
}

################################################################################

if(scalar(@ARGV) != 2) {
  help();
  exit(-1);
}

$mminstall_file = $ARGV[1];

if ( ! -e $mminstall_file ) {
  print "Error: $mminstall_file not found\n";
  exit(-1);
}

given($ARGV[0]) {
  when ("--has-bin") {
    has_bin();
  }
  when ("--has-devel") {
    has_devel();
  }
  when ("--gen-bin") {
    gen_bin();
  }
  when ("--gen-devel") {
    gen_devel();
  }
  default {
    print "Error: unknown command " . $ARGV[0] . "\n";
    exit (-1);
  }
}
