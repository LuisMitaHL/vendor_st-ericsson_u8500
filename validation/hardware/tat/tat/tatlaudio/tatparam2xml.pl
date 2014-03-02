#!/usr/local/bin/perl

#
# This program generates TATAUDIO audio parameters definition file.
# It is part of TATLAUDIO. TATLAUDIO is a subsystem of HATS TAT.
#
# Copyright (C) ST-Ericsson 2011
#

use strict;
use warnings;

# enable the following line if you have a perl error you don't understand,
# to get more information about it.
#use diagnostics;


use XML::Writer;
use Cwd;

#pp compatibility stuff
use LWP;
use LWP::Protocol::file;
use URI::file::Win32;
use IO::File;


#############################################################################
# Help
#############################################################################

sub usage {
	print "./tatparam2xml.pl <param> <map> <enum> <output>\n";
	print "param  file containing the audio parameters identifiers. \
    Section in file begins by AUDIO_PARAM_BEGIN_LIST and ends after AUDIO_PARAM_END_LIST.\n";
	print "map    file telling which enum type is related to each audio parameter\n";
	print "enum   file defining the enumeration and their possible values\n";
	print "output file to create. This file complies with DTH input format.\n";
}


#############################################################################
# Parse command line
#############################################################################
die usage if ($#ARGV != 3);

my $fname_param = $ARGV[0];
my $fname_map   = $ARGV[1];
my $fname_enum  = $ARGV[2];
my $fname_enum_bis  = $ARGV[1];
my $fname_out   = $ARGV[3];


#############################################################################
# GLOBALS
#############################################################################

# index param name to enumeration typedef name.
# key :     param name
# value :   enumeration name
my %param_to_enum;

# index enumeration values (names of enum items).
# key :     enumeration name
# value :   enum items as string seperated by '|'
my %enum_to_items;

my ($param_name, $enum_name);

# parameters display list
# defined by the order of enum e_tatlaudio_param in dthaudio.h
my @display_list = ();

# parameters path list
# defined in the enum e_tatlaudio_param in dthaudio.h
my @path_list = ();


#############################################################################
# Get the parameter identifiers (by name)
#############################################################################

# They are supposed to be defined by a C block starting by 
# AUDIO_BEGIN_PARAM and ending by AUDIO_END_PARAM.


# open and create required files
open FILEPARAM, $fname_param or die "Cannot open $fname_param";

# get the AUDIO parameters names (fill keys of %param_to_enum)
#print "\nfind the audio parameters...\n";
my $found = 0;
my $inline;
my $path="";

foreach $inline (<FILEPARAM>)
{
	if ( $found )
    {
        if ( $inline =~ /^\s*AUDIO_PARAM_(\w+),/ )
		{
            $param_name = $1;
            if ( $param_name eq 'END_LIST' )
            {
                last;
            }
            else
            {
                #print "param: $1\n";
                $param_to_enum{"$1"} = '';
				push(@display_list,$1);
				push(@path_list,$path);
            }
        }
		elsif ( $inline =~ /^\s*AUDIO_BLOCK_(\w+),/ )
		{
			$path=$1;
		}
    }
	elsif ( $inline =~ /^\s*AUDIO_PARAM_BEGIN_LIST/ )
	{
		$found = 1;
	}
}
close FILEPARAM;


#############################################################################
# find the corresponding enum typedef to each key
#############################################################################

#print "\nfind enum associated to each param...\n";
$found = 0;
open FILEMAP, $fname_map or die "Cannot open $fname_map";
foreach $inline (<FILEMAP>)
{	
	if ($found==1)
	{
        # AUDIO_PARAM(AUDIO_EAR, ear_class_ab_power, enum ear_class_ab_power, pp_value, vp_isSet)
        if ($inline =~ /^\s*AUDIO_PARAM\(\s*AUDIO_PARAM_(\w+),\s*\w+,\s*\w+,\s*enum\s+(\w+),/)
        {
            #print "testing param $1, enum $2\n";
            if ( exists $param_to_enum{$1} )
            {
                #print "param $1 => enum $2\n";
                $param_to_enum{$1} = $2;

                # insert a enum entry, if none
                $enum_to_items{$2} = '';
            }
        }
        elsif ($inline =~ /^\s*END_AUDIO_PARAM_MAP\s*\(/)
        {
            last;
        }
    }
    elsif ($inline =~ /^\s*BEGIN_AUDIO_PARAM_MAP\s*\(/)
    {
        $found = 1;
    }
}
close FILEMAP;

# DEBUG: print the enum typedefs found
#foreach $enum_name (keys %enum_to_items)
#{
#    print "$enum_name\n";
#}

#############################################################################
# TODO: check that all of the param has a corresponding enum set.
# if not, this is a fatal error to be notified.
#############################################################################


#############################################################################
# extract enum values Fom the file specified by FILEENUM
#############################################################################

open FILEENUM, $fname_enum or die "Cannot open $fname_enum";

#Search for the typedef enum 
#print "\nload enums definitions\n";
my $enum_is_used = 0;
$found = 0;
foreach $inline (<FILEENUM>)
{
    if ( $found )
    {
        if ( $inline =~ /^([^\w]+)([\w]+)(_E)(,{0,1})(.*)$/ )
        {
            if ( $enum_is_used )
            {
                #print "val $2\n";
                $enum_to_items{$enum_name} .= "$2|";
            }
        }
        elsif ( $inline =~ /^([^\w]+)([\w]+)( = )([\w]+)(,{0,1})(.*)$/ )
        {
            if ( $enum_is_used )
            {
                #print "val $2\n";
                $enum_to_items{$enum_name} .= "$2|";
            }
        }
        else
        {
            $found = 0;
        }
    }
    elsif ( $inline =~ /^.*typedef enum\s+(\w+)/ )
    {
        $enum_name = "$1";
    
        $enum_is_used = exists $enum_to_items{$enum_name};
        print "begin enum $enum_name\n" if ( $enum_is_used );
        $found = 1;
    }
}

#close the files
close FILEENUM;


#############################################################################
# extract enum values Fom the file specified by FILEENUM
#############################################################################

open FILEENUMBIS, $fname_enum_bis or die "Cannot open $fname_enum_bis";

#Search for the typedef enum 
#print "\nload enums definitions\n";
my $enum_is_used_2 = 0;
$found = 0;
foreach $inline (<FILEENUMBIS>)
{
    if ( $found )
    {
        if ( $inline =~ /^([^\w]+)([\w]+)(_E)(,{0,1})(.*)$/ )
        {
            if ( $enum_is_used_2 )
            {
                #print "val $2\n";
                $enum_to_items{$enum_name} .= "$2|";
            }
        }
        elsif ( $inline =~ /^([^\w]+)([\w]+)( = )([\w]+)(,{0,1})(.*)$/ )
        {
            if ( $enum_is_used_2 )
            {
                #print "val $2\n";
                $enum_to_items{$enum_name} .= "$2|";
            }
        }
        else
        {
            $found = 0;
        }
    }
    elsif ( $inline =~ /^.*typedef enum\s+(\w+)/ )
    {
        $enum_name = "$1";
    
        $enum_is_used_2 = exists $enum_to_items{$enum_name};
        print "begin enum $enum_name\n" if ( $enum_is_used_2 );
        $found = 1;
    }
}

#close the files
close FILEENUMBIS;

#############################################################################
# build XML output
#############################################################################

open FILEOUT, '>'.$fname_out or die "Cannot create $fname_out";

my $output = new IO::File('>'.$fname_out);
my $wr = new XML::Writer( OUTPUT => $output, NEWLINES => 1, UNSAFE => 1 );
my $path_param="";

# important: UNSAFE to true because the XML has no single root (not well-formed)


#while ( ( $param_name, $enum_name) = each (%param_to_enum) )
foreach my $param_name (@display_list)
{
 	$enum_name=$param_to_enum{$param_name};
	$path_param=shift(@path_list)."/".$param_name;
	
	#translation minuscule
	$path_param=~ tr/A-Z/a-z/;

	# begin argument
    $wr->startTag('argument',
        path      => "/DTH/AUDIO/Param/$path_param",
        type      => 'U32',
        user_data => $param_name.':AUDIO_PARAM_'.$param_name,
        get       =>'DthAudio_param_get',
        set       =>'DthAudio_param_set');

    # manage enum and items
    if ( $enum_to_items{$enum_name} )
    {
        # create enum
        $wr->startTag('enum');

        my @items = split (/\|/, $enum_to_items{$enum_name});
        foreach (@items)
        {
            $wr->startTag('enum_item', value => $_);

            my $enum_label = lc $_;

            #print "enum: $enum_name, item: $enum_label. ";

            if ( $enum_label =~ /unmute$/ )
            {
                $enum_label = "unmute";
            }
            elsif ( $enum_label =~ /mute$/ )
            {
                $enum_label = "mute";
            }
            elsif ( $enum_name =~ /gain$/ )
            {
                $enum_label =~ /gain(_\w+)$/;
                $enum_label = $1;
            }
            elsif ( $enum_name =~ /power$/ )
            {
                $enum_label =~ /(power_\w+)$/;
                $enum_label = $1;
            }
            elsif ( substr ($enum_label, 0, length $enum_name) eq $enum_name )
            {
                $enum_label = substr ($enum_label, (length $enum_name) + 1);
                $enum_label =~ s/driver_connected//g;
            }
            else
            {
                $enum_label =~ s/side_tone//g;
                $enum_label =~ s/selected//g;
                $enum_label =~ s/duty_cycle//g;
                $enum_label =~ s/connected//g;
                $enum_label =~ s/line_in_to_headset//g;
                $enum_label =~ s/vib1_driver//g;
                $enum_label =~ s/vib2_driver//g;

                $enum_label =~ s/vib_driver//g;
                $enum_label =~ s/dth_hs_analog_gain//g;
   		$enum_label =~ s/dthear_analog_gain//g;
    	        $enum_label =~ s/dth_hf_analog_gain//g;
                $enum_label =~ s/dth_vib_analog_gain//g;
                $enum_label =~ s/dth_headset_gain//g;
		$enum_label =~ s/dth_linein_to_hs_gain//g;
		$enum_label =~ s/dth_sidetone_gain//g;
		$enum_label =~ s/dth_st_txrx_gain//g;
        $enum_label =~ s/dth_vib_dig_gain//g;
       	$enum_label =~ s/dth_hf_dig_gain//g;
  	
            }

            $enum_label =~ s/_point_/./g;
            $enum_label =~ s/dth//g;
            $enum_label =~ s/minus_/-/g;
            $enum_label =~ s/_per_cent/%/g;
            $enum_label =~ s/\_/ /g;
            $enum_label =~ s/^\s+//;
 
            #print "label: $enum_label\n";

            $wr->characters($enum_label);
            $wr->endTag();
        }

        # end enum
	    $wr->endTag('enum');
    }
    elsif ( $enum_name ne '' )
    {
        print "no item for enumeration $enum_name!\n";

        # end argument
        $wr->endTag('argument');
        last;
    }

    # tag info provides the parameter name
    $wr->startTag('info');
	$param_name=~ tr/A-Z/a-z/;
    $wr->characters("Label Box: $param_name\\nDynamic Control: TRUE");
    $wr->endTag();

    # end argument
    $wr->endTag('argument');
}

# end XML writting
$wr->end();

#close files
close FILEOUT;

#############################################################################
# END OF FILE
#############################################################################
