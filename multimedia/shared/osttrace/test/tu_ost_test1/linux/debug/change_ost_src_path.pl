#!/bin/env perl
# vim:syntax=perl
#--------------------------------------------------------------------
## @file
#/*****************************************************************************/
#
#  Copyright (C) ST-Ericsson 2010 - All rights reserved.
#  This code is ST-Ericsson proprietary and confidential.
#  Any use of the code for whatever purpose is subject to
#  specific written permission of ST-Ericsson SA.
#  
# Example of usage :
#
# perl change_ost_src_path.pl --name=OSTTEST --input=../../../../dictionaries
#  
# $Id: create_dico_list.pl 725 2010-01-13 20:07:23Z germaino $:
# 
#
#---------------------------------------------------------------------
use strict;
use warnings;
#use Smart::Comments '###','####';
use Getopt::Long;
use Term::ANSIColor;
use File::Basename;
use File::Glob qw(bsd_glob);
use IO::File;
use Time::Local;
use Cwd;

#############################################
# global variables
#############################################
$| = 1; # activate autoflush
my @component_name;
my %defines;
my $build = '$Rev: 725 $:';
my $version = '1.0';
my @dico_list;
my @dico_dir;


GetOptions (
	'help'    		=> \&help,
	'version' 		=> \&version,
	"defines:s"		=> \%defines,	
	"name=s"		=> \@component_name,
	"input=s"		=> \@dico_dir
);	   



if ( not defined ($component_name[0]))
{
	print color 'bold red';	
	die "Component NAME not defined $!\n";
}

if ( not defined ($dico_dir[0]))
{
	print color 'bold red';	
	die "Dictionary folder not defined $!\n";
}


SwapSrcPath($dico_dir[0],$component_name[0]);


exit 0;



##########################################################
###   Help
##########################################################
sub usage
{
	print "\n== Usage :  change_ost_src_path [OPTIONS] \n";
}


sub version
{
	printf "Version: %s build: %d\n", $version, substr($build,6,3);
	exit(0);
}

sub help
{
	print "Change source path in OST xml dictionnary to match execution environement\n";
	&usage();
	print "\n== Available Options are:
	--help				: prints this help and quit
	--version			: prints the version and quit
	--input=<string>	: Path to look for dictionaries (*.xml). Normmaly should be MMROOT/shared/dictionary
	--name=<string>		: Name of component for which to fecth dictionaries
	--defines 			: Configure some internal debug option
			--defines=debug=<numerical value> (configure verbosity)
	";
	exit(0);
}


sub SwapSrcPath
{
	my ($directory, $component) = @_;

	if ( defined ($defines{debug}) && ($defines{debug} >= 1))
	{
		printf "directory: %s\n", $directory;
		printf "component: %s\n", $component;
	}


	my $local_dir = getcwd();

	# Compute MMROOT
	$local_dir =~ /(.+)\/omxil\/(.+)/;
	my $winroot = $1 . "/omxil";
	#$winroot =~ s/\//\\/;

	# Get list of dictionnaries from which we need to generate top dico list
	my @l_dico_list = glob("$directory/*.{xml}");

	# for each dico found append MMROOT
	foreach my $dico_file (@l_dico_list) 
	{
		# Do not include existing top dictionnaries if available !!!
		next if ($dico_file =~ /_top/);

		# Do not include dictionaries not from given component
		next if ($dico_file !~ /$component/);

		open MY_DICO_FILE, "$dico_file" or die "Could not open file [$dico_file]: $!\n";
		while (my $line_tmp = <MY_DICO_FILE>)
		{
			if ($line_tmp =~ /(^<path val=")(.+)\/multimedia\/shared(.+)/)
			{
				$line_tmp =~ s/(^<path val=")(.+)\/multimedia\/shared(.+)/$1$winroot$3/;
				printf "%s\n", $line_tmp;
			}
		}
		close(MY_DICO_FILE);
	}

	printf "%s: Done !!\n", $0;

}

