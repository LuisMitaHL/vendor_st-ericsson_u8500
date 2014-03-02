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
# perl tracecompiler.pl --id=45 --name="My Component" --inc=include toto.c titi.cpp tutu.c
#
# $Id: tracecompiler.pl 725 2010-01-13 20:07:23Z germaino $:
#
#
#---------------------------------------------------------------------

BEGIN {
  use File::Spec;
  use File::Basename;
  use FindBin;
  use lib File::Spec->catfile($FindBin::Bin => qw(.));
}




use Carp;
use strict;
use warnings;
#use Smart::Comments '###','####';
use Getopt::Long;
use Term::ANSIColor;
use File::Basename qw(dirname basename fileparse);
use lib dirname($0);
use XML::Simple;
use XML::Writer;
use Data::Dumper;
use File::Basename;
use File::Glob qw(bsd_glob);
use IO::File;
use Time::Local;
use Cwd 'abs_path';


#############################################
# global variables
#############################################
$| = 1; # activate autoflush
my @component_name;
my @component_dir;
my @include_dir;
my $component_id;
my %defines;
my $build = '$Rev: 725 $:';
my $version = '1.0';
my $now_string = localtime;
my @file_list;
my @file_list_sorted;
my @action;
my $xmltab = { };
my $tab_group = { };
my $save_tab;
my @trace_grp_def_file;
my @dico_list;
my %input;
my $NeedToRegenerateHeaderFile = 0;
my $count_global = 0;
my $count_inside_file = 0;
my $count_file = 0;
my $line = "";
my $linec = 0;
my $group_cnt = 0;
my $size = 0;
my $match = 0;
my $file_cnt = 0;
my $match_file = 0;
my @fixed_id_file_array;
my $dicodata;
my $xml = new XML::Simple;
my $file_count = 0;
my @dico_type = (
  {type => "hex32", size => "4", formatchar => "x", classification => "hex"},
  {type => "hex32", size => "4", formatchar => "lx", classification => "hex"},
  {type => "HEX32", size => "4", formatchar => "lX", classification => "hex"},
  {type => "HEX32", size => "4", formatchar => "X", classification => "hex"},
  {type => "void*", size => "4", formatchar => "p", classification => "hex"},
  {type => "void*", size => "4", formatchar => "lp", classification => "hex"},
  {type => "int32", size => "4", formatchar => "d", classification => "integer"},
  {type => "uint32", size => "4", formatchar => "u", classification => "integer"},
  {type => "uint32", size => "4", formatchar => "lu", classification => "integer"},
  {type => "ascii", size => "1", formatchar => "s", classification => "string"},
  {type => "ascii", size => "1", formatchar => "ls", classification => "string"},
  {type => "int16", size => "2", formatchar => "hd", classification => "integer"},
  {type => "uint16", size => "2", formatchar => "hu", classification => "integer"},
  {type => "hex16", size => "2", formatchar => "hx", classification => "hex"},
  {type => "HEX16", size => "2", formatchar => "hX", classification => "hex"},
  {type => "int8", size => "1", formatchar => "hhd", classification => "integer"},
  {type => "uint8", size => "1", formatchar => "hhu", classification => "integer"},
  {type => "hex8", size => "1", formatchar => "hhx", classification => "hex"},
  {type => "HEX8", size => "1", formatchar => "hhX", classification => "hex"},
  {type => "ffix", size => "8", formatchar => "f", classification => "float"},
  {type => "ffix", size => "8", formatchar => "Lf", classification => "float"}

);
my $cur_directory="";
my $verbose = 0;

#Group value mapping to OST constraint
my %fixed_dico_ost = ();


GetOptions (
  'help'        => \&help,
  'version'     => \&version,
  'essai'       => \&essai,
  "defines:s"   => \%defines,
  "id=i"        => \$component_id,
  "name=s"      => \@component_name,
  "action=s"    => \@action,
  "inc=s"       => \@include_dir,
  "tgdf=s"      => \@trace_grp_def_file,
  'dico=s'      => \@dico_list,
  "input:s"     => \%input,
  "inputdir=s"  => \@component_dir
);

$verbose = $defines{debug} if ( defined ($defines{debug}));

if (($verbose == 0) && (defined $ENV{VERBOSE})) {
  $verbose = $ENV{VERBOSE} if (! $ENV{VERBOSE} eq "");
}

#Internal STE group value mapping (aligned on power of 2 to use bitmask)
my %fixed_dico_ste = (
  FIXED_GROUP_TRACE_ERROR       => 1,
  FIXED_GROUP_TRACE_WARNING     => 2,
  FIXED_GROUP_TRACE_FLOW        => 4,
  FIXED_GROUP_TRACE_DEBUG       => 8,
  FIXED_GROUP_TRACE_API         => 16,
  FIXED_GROUP_TRACE_OMX_API     => 32,
  FIXED_GROUP_TRACE_OMX_BUFFER  => 64,
  FIXED_GROUP_TRACE_ALWAYS      => 128,
  FIXED_GROUP_TRACE_USER1       => 256,
  FIXED_GROUP_TRACE_USER2       => 512,
  FIXED_GROUP_TRACE_USER3       => 1024,
  FIXED_GROUP_TRACE_USER4       => 2048,
  FIXED_GROUP_TRACE_USER5       => 4096,
  FIXED_GROUP_TRACE_USER6       => 8192,
  FIXED_GROUP_TRACE_USER7       => 16384,
  FIXED_GROUP_TRACE_USER8       => 32768
);

if (defined ($defines{strict}))
{
  $SIG{__WARN__} = \&Carp::confess;
  $SIG{__DIE__} = \&Carp::confess;
}
#Filed up structure fixed_dico_ost from shared header file
ReadGroupValue();


# Matching table between internal and external group id values
my %dico_swap = (
  $fixed_dico_ste{FIXED_GROUP_TRACE_ERROR},       => $fixed_dico_ost{FIXED_GROUP_TRACE_ERROR},
  $fixed_dico_ste{FIXED_GROUP_TRACE_WARNING},     => $fixed_dico_ost{FIXED_GROUP_TRACE_WARNING},
  $fixed_dico_ste{FIXED_GROUP_TRACE_FLOW},        => $fixed_dico_ost{FIXED_GROUP_TRACE_FLOW},
  $fixed_dico_ste{FIXED_GROUP_TRACE_DEBUG},       => $fixed_dico_ost{FIXED_GROUP_TRACE_DEBUG},
  $fixed_dico_ste{FIXED_GROUP_TRACE_API},         => $fixed_dico_ost{FIXED_GROUP_TRACE_API},
  $fixed_dico_ste{FIXED_GROUP_TRACE_OMX_API},     => $fixed_dico_ost{FIXED_GROUP_TRACE_OMX_API},
  $fixed_dico_ste{FIXED_GROUP_TRACE_OMX_BUFFER},  => $fixed_dico_ost{FIXED_GROUP_TRACE_OMX_BUFFER},
  $fixed_dico_ste{FIXED_GROUP_TRACE_ALWAYS},      => $fixed_dico_ost{FIXED_GROUP_TRACE_ALWAYS},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER1},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER1},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER2},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER2},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER3},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER3},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER4},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER4},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER5},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER5},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER6},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER6},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER7},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER7},
  $fixed_dico_ste{FIXED_GROUP_TRACE_USER8},       => $fixed_dico_ost{FIXED_GROUP_TRACE_USER8}
);

$xmltab->{'component'}->[0]->{'group'}->[0] = init_group("TRACE_ERROR", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[1] = init_group("TRACE_WARNING", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[2] = init_group("TRACE_FLOW", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[3] = init_group("TRACE_DEBUG", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[4] = init_group("TRACE_API", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[5] = init_group("TRACE_OMX_API", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[6] = init_group("TRACE_OMX_BUFFER", \$group_cnt);
$xmltab->{'component'}->[0]->{'group'}->[7] = init_group("TRACE_ALWAYS", \$group_cnt);

# A trace group definition file has been provided
if (defined ($trace_grp_def_file[0]))
{
  open TRACE_GRP_DEF, "$trace_grp_def_file[0]" or die "Could not open file [$trace_grp_def_file[0]]: $!\n";

  while (my $line_tmp = <TRACE_GRP_DEF>)
  {
    if ($line_tmp =~/^\s*\[GROUP\]/)
    {
      #my @line_tab_tmp = ($line_tmp =~ /[\s?]\[GROUP\][\s*](\w+)[\s*](\d)/);
      my @line_tab_tmp = ($line_tmp =~ /^(\s*)\[GROUP\]\s*(\w+)\s*(\d+)/);
      #printf "TRACE GROUP: %s %d\n", $line_tab_tmp[1], $line_tab_tmp[2];
      my $toto = $line_tab_tmp[2];
      if (($line_tab_tmp[2] >= 0) && ($line_tab_tmp[2] <= 7))
      {
        print color 'bold red';
        die "ERROR: group ID: $line_tab_tmp[2] GROUP ID [0..7] are reserved and can not be used!!\n";
      }
      $xmltab->{'component'}->[0]->{'group'}->[$line_tab_tmp[2]] = init_group($line_tab_tmp[1], \$toto);
      $group_cnt = $toto++;
    }
  }
  close TRACE_GRP_DEF;
}



if ( defined ($action[0]))
{
  if ($action[0] eq "GenTraceDefinition")
  {

    printf "ST OST TRACECOMPILER: Create %s file\n", "$include_dir[0]/OstTraceDefinitions_ste.h" if ($verbose >= 1);
    CreateTraceDefinition("$include_dir[0]/OstTraceDefinitions_ste.h");
  }

  if ($action[0] eq "GenTopDictionary")
  {
    CreateTopDictionnary("$include_dir[0]",@component_name);
  }
  exit 0;
}


if ( not defined ($component_name[0]))
{
  print color 'bold red';
  die "Component NAME not defined $!\n"
}

if ( defined ($defines{forceheader}))
{
  $NeedToRegenerateHeaderFile = 1;
}

if ( not defined ($component_id))
{
  print color 'bold red';
  die "Component ID not defined $!\n"
}



printf "debug level: %d\n", $verbose if ($verbose >= 1);

if ( defined ($component_dir[0]))
{
  printf "component_dir: %s\n", $component_dir[0] if ($verbose >= 5);
  @file_list = glob("$component_dir[0]/*.{c,cpp}");
  foreach my $file (@file_list)
  {
    printf "filelist: %s\n", $file if ($verbose >= 5);
  }
}
else
{
  if (defined($input{filelist}))
  {
    my $MY_FP;

    open MY_FP, $input{filelist} or die "Could not open file [$input{filelist}]: $!\n";
    while (my $line_tmp = <MY_FP>)
    {
      $line_tmp =~ s/\n//g;
      push @file_list => $line_tmp;
    }
  }
  else
  {
    @file_list = @ARGV;
  }
}

#OpenDicoFile("$include_dir[0]/autogentraces/$component_name[0]" . ".xml");
#OpenTraceDefinition("$include_dir[0]/autogentraces/fixed_id.definitions");

# Sort file_list since list of input files (resulting from sort command)
# is different between win32 and Linux !!!
# In win32 1 is before 2 while in Linux this is the contrary
# 1) /prj/multimedia_shared/users/germaino/work_bridge/osi/build/multimedia/audio/afm/nmf/hst/bindings/shmpcm/shmout/src/shmout.cpp
# 2) /prj/multimedia_shared/users/germaino/work_bridge/osi/build/multimedia/audio/afm/nmf/hst/bindings/shmpcm/shm/src/shmin.cpp

@file_list_sorted = sort (@file_list);
foreach my $file (@file_list_sorted)
{

  $count_inside_file = 0;
  $linec = 1;
  my $cur_file_group_id_list = { };

  open FILE, "$file" or die "Could not open file [$file]: $!\n";

  while ($line = <FILE>)
  {

    # look for occurence of OstTrace string
#		if ($line =~/^[\s*{?\s*]OstTrace(Int\d|Array|FiltInst|FiltStatic)/)
    if ($line =~/^\s*{?\/*\**\s*OstTrace(Int\d|Float\d|Array|FiltInst|FiltStatic)/)
    {
      $match = 1;
      $match_file = 1;

      if (($line =~/[\s?]OstTrace(Array|FiltInstData|FiltStaticData)/) && ($line !~/(%?)(.*)(%?)(.*)%{(\w+)\[\]}(.*)/))
      {
        die("Error: file $file at line $linec content: $line\n");
      }


      print $line if ($verbose >=5);
      #Split line into (TRACE_GROUP,TRACE_STRING)
      my @line_tab = ($line =~ /.+\s?\(\s?(\w+)\s?\,\s*(\".*\")/);

      if ($verbose >=5)
      {
        foreach my $valeur (@line_tab) 
        {
          print "$valeur ";
        }
        printf "\n";
      }

      if (defined ($defines{strict}))
      {
        die "ERROR: TRACE GROUP not defined in line $line\n" if not defined ($line_tab[0]);
        die "ERROR: TRACE STRING not defined in line $line\n" if not defined ($line_tab[1]);
      }
      my $string_entry = $line_tab[1];

      # In case we use API with filtering add (hidden to user) OMX handle and Parent handle
      if ($line =~/OstTraceFilt/)
      {
        $string_entry = "OMX handle 0x%x: ID %d: ";
        $string_entry .= $line_tab[1] if defined ($line_tab[1]);
      }

      # Create entry in data section
      my $my_entry_tmp1 = { };
      $string_entry =~ s/\"//g;
      $my_entry_tmp1->{'content'} = $string_entry;
      $my_entry_tmp1->{'id'} = $count_global + 1;
      $my_entry_tmp1->{'type'} = "string";
      push @{$xmltab->{'data'}->[0]->{def}} => $my_entry_tmp1;


      # Generate string for TRACE ID
      my $gen_trace_id = "FILE_" . $file_count . "_" . $linec;


      my $my_index = get_group_index(\@{$xmltab->{'component'}->[0]->{'group'}}, $line_tab[0]);
      my $my_tab;

      # Get pointer to the group to which we should add the trace
      if (defined ($xmltab->{'component'}->[0]->{'group'}->[$my_index]))
      {
        # The trace group already exist so just initialize somehow a pointer
        $my_tab = $xmltab->{'component'}->[0]->{'group'}->[$my_index];
      }
      else
      {
        # The trace group is not yet referenced. So
        # - Initialize the group
        # - Insert the group
        $xmltab->{'component'}->[0]->{'group'}->[$my_index] = init_group($line_tab[0], \$group_cnt);
        $my_tab = $xmltab->{'component'}->[0]->{'group'}->[$my_index];
      }

      ###################################################
      # Insert the new trace entry for the group
      # ##################################################

      # A new trace need to be added for the current group
      my $my_entry = { };

      # Get position of last trace entry within the current group.
      # If first time trace in the group then return 0
      if (defined (scalar(@{$my_tab->{trace}})))
      {
        $size = scalar(@{$my_tab->{trace}});
      }
      else
      {
        $size = 0;
      }

      # Initialize "data-ref" field => scope is within a dictionary
      $my_entry->{'data-ref'} = $count_global + 1;

      # Initialize "name" field (see above how it is computed)
      $my_entry->{'name'} = $gen_trace_id;

      # Initialize "instance->ID" => position of the trace within the current group
      $my_entry->{'instance'}->{'id'} = $size + 1;

      # Initialize "instance->loc-ref" => In which source file the trace belong
      $my_entry->{'instance'}->{'loc-ref'} = $file_cnt + 1;

      # Initialize "instance->line" => position of the trace within the file its belong
      $my_entry->{'instance'}->{'line'} = $linec;

      # Initialize "instance->methodname" => position of the trace within the method/class it belongs
      # Would request C and C++ parser to compute it => too complex
      $my_entry->{'instance'}->{'methodname'} = "";
      $my_entry->{'instance'}->{'classname'} = "";

      if (defined $save_tab->{$line_tab[0]}->{$gen_trace_id})
      {
        printf "%s: Entry %s already exist in file %s\n", $file, $gen_trace_id, $save_tab->{$line_tab[0]}->{$gen_trace_id}->{file};
      }
      else
      {
        $save_tab->{$line_tab[0]}->{$gen_trace_id}->{id} = $size + 1;
        $save_tab->{$line_tab[0]}->{$gen_trace_id}->{file} = $file;
        $save_tab->{$line_tab[0]}->{$gen_trace_id}->{line} = $linec;
      }

      # Save new trace entry
      push @{$my_tab->{'trace'}} => $my_entry;

      ###################################################
      # Save also the entry in array specific to the current trace header file to generate
      # ##################################################

      my $save_entry = { };
      $save_entry->{'group_id'} = $my_tab->{'id'};
      $save_entry->{'trace_id'} = $my_entry->{'instance'}->{'id'};
      $save_entry->{'trace_name'} = $my_entry->{'name'};
      push @{$cur_file_group_id_list->{'used'}} => $save_entry;

      $count_inside_file++;
      $count_global++;

    }

    $linec++;

  }

  if ($match_file == 1)
  {

    $file_cnt++;

    # split between filename and directory name of the generated header file
    my ($filename_tmp, $directories) = fileparse($file);
    my @filename = split (/\./, $filename_tmp);

    #For Symbian environment (linux & win32 ) we consider that the media root is <...>/ext/adapt/<media>
    #For OSI environment (linux) we consider that the media root is <...>/multimedia/<media>
    #For OSI environment (android) we consider that the media root is <...>/mm/<media>
    #For OSI environement (linux) inside Symbian (linux) file tree environement we consider that the media root is <...>/ext/adapt/<media>
    my $mediaroot = $defines{mediaroot};
    die "MEDIA ROOT not defined \n" if (not defined ($mediaroot));

    # Expect to return <media>
    my $headername = $directories;
    if (defined ($defines{use_abs_path}))
    {
      $headername = abs_path($headername);
    }

    # Fill XML structure with directory location for source code
    if ($cur_directory ne $directories)
    {
      my $my_entry;
      $my_entry->{'val'} = $directories;
      if (defined($defines{strippath}))
      {
        $my_entry->{'val'} =~ s/(.*)\/multimedia(.*)/\/multimedia$2/g;
      }

      push @{$xmltab->{'locations'}->{'path'}} => $my_entry;
      $cur_directory = $directories;
    }
    my $index = scalar(@{$xmltab->{'locations'}->{'path'}}) - 1;
    printf "index: %d\n", $index if ($verbose >=2);
    $xmltab->{'locations'}->{'path'}->[$index]->{'file'}->[$count_file]->{'content'} = $filename_tmp;
    $xmltab->{'locations'}->{'path'}->[$index]->{'file'}->[$count_file]->{'id'} = $file_cnt;
    $match_file = 0;
    $count_file++;


    # Addeed for symbian compatibility
    if (defined ($defines{module}))
    {
      my $pattern = $defines{module};
      #$headername =~ /$mediaroot\/(.*)$pattern(\w+)([\.\w+]*)\/(.+)/;
      $headername =~ /$mediaroot\/(.*)$pattern\/(.*)/;
      $headername = $pattern . "\/" . $2;
    }
    else
    {
      $headername =~ /$mediaroot\/(.*)/;
      $headername = basename($mediaroot) . "\/" . $1;
    }

    # Addeed for video compatibility
    $headername =~ s/\.\.\///g;
    $headername =~ s/\.\///g;

    # Addeed for audio compatibility
    $headername =~ s/\//_/g;

    if (defined ($defines{prefix}))
    {
      $headername = $defines{prefix} . "_" . $headername;
    }

    my $tracefile_noext = $headername;
    if (defined ($defines{use_abs_path}))
    {
      $tracefile_noext .= "_" . $filename[0] . "Traces";
    }
    else
    {
      $tracefile_noext .= $filename[0] . "Traces";
    }

    my $tracefile = $tracefile_noext . ".h";

    printf "tracefile: %s\n", $tracefile if ($verbose >=5);

    if ( !-e $include_dir[0])
    {
      print color 'bold red';
      die "ERROR: Not a directory [$include_dir[0]]: $!\n";
    }

    # Check if generated header file already exist. Since trace compiler is run each time dependancies are evaluated
    # we need to make sure that we do not regenerate it if not needed to avoid compilation prerequisite rule to match
    if ( -e "$include_dir[0]/$tracefile")
    {
      # Generated header file already exist save some information for debug purpose
      my $cfile_date = -M "$file";
      my $hfile_date = -M "$include_dir[0]/$tracefile";

      if ($verbose >=4)
      {
        printf "file %s updated at %s\n", "$file", $cfile_date;
        printf "file %s updated at %s\n", "$include_dir[0]/$tracefile", $hfile_date;
      }

      # Check if corresponding C file has been modified or if another C file belonging the same dictionnary has been modified
      # In the last case we force header generation for all remaining files to avoid cache issue: group/trace duplication
      if (((-M $file) <= (-M "$include_dir[0]/$tracefile")) || ($NeedToRegenerateHeaderFile == 1))
      {
        if ($verbose >=4)
        {
          printf "file %s is more recent than file %s\n", $file, "$include_dir[0]/$tracefile";
        }
        if ($verbose >=1)
        {
          printf "ST OST TRACECOMPILER: Building traces for Component %s. Component UID: 0x%.2X. File: %s\n", $component_name[0], $component_id, $file;
        }
        CreateTraceHeaderFile($tracefile, $tracefile_noext, $cur_file_group_id_list, $file_count);
        $NeedToRegenerateHeaderFile = 1;
      }
    }
    else
    {
      #print "Building traces for component $component_name[0]. Component UID: 0x$component_id.\n";
      if ($verbose >=1)
      {
        printf "ST OST TRACECOMPILER: Building traces for Component %s. Component UID: 0x%.2X. File: %s\n", $component_name[0], $component_id, $file;
      }
      CreateTraceHeaderFile($tracefile, $tracefile_noext, $cur_file_group_id_list, $file_count);

      # Make sure that if a new C file has been added to be parsed by the TC between 2 runs we are rebuilding
      # all trace headers from other C file to avoid cache issue
      $NeedToRegenerateHeaderFile = 1;
    }

  }
  close FILE;
  $file_count++;
}

if ($match == 1)
{

  # Add stuff to be compliant with OST dico
  $xmltab->{'xmlns:xsi'} = "http://www.w3.org/2001/XMLSchema-instance";
  $xmltab->{'xsi:noNamespaceSchemaLocation'} = "DictionarySchema.xsd";
  foreach my $type (@dico_type)
  {
    push @{$xmltab->{'typedef'}->[0]->{'object'}} => $type;
  }


  $xmltab->{'component'}->[0]->{'name'} = "$component_name[0]";
  $xmltab->{'component'}->[0]->{'id'} = "$component_id";
  $xmltab->{'component'}->[0]->{'prefix'} = "$component_name[0]: ";
  $xmltab->{'component'}->[0]->{'suffix'} = "";

  my $outputfile = "$include_dir[0]/$component_name[0]" . ".xml";

  PrintXML($outputfile);

}
exit 0;



##########################################################
###   Help
##########################################################
sub usage
{
  print "\n== Usage :  tracecompiler [OPTIONS] [files list]\n";
}

sub essai
{
  print "\n== Usage :  tracecompiler [OPTIONS] [files list]\n";
  exit(0);
}


sub version
{
  printf "Version: %s build: %d\n", $version, substr($build,6,3);
  exit(0);
}

sub help
{
  print "ST OST Trace Compiler\n";
  &usage();
  print "\n== Available Options are:
  --help: prints this help and quit
  --version: prints the version and quit
  --id=<integer>: Unique ID use to identify a component (set of files) sharing the same dictionnary (mandatory)
  --action=<string>: Action to perform
  --action=GenTraceDefinition: Generate trace definition file
  --action=GenTopDictionary: Generate top dico
  --inc=<string>: Path to store generated header files and dictionnary (a directory autogentraces will be created) (mandatory)
  --tgdf=<string>: Trace group definition file (optional)
  --name=<string>: Name of dictionnary (usually same as component) (mandatory)
  --inputdir=<string> : Specify directory to look for file (parse all *.c and *.cpp files found in the current directory
  --defines: Configure some internal debug option
  --defines=debug=<numerical value> : configure verbosity
  --defines=force_top_dico : Force top dico generation even if no xml to parse
  --defines=mediaroot=<media installation path> : Define installation path for media (mandatory)
  --defines=grpmapfile=<group map file> : Define path for group mapping file (mandatory)
  --defines=module=<name> : Restrict path to build the autogenerated header filename up to module (optional/ default up to shared)
  --defines=strippath : strip path up to multimedia included to avoid dico change depending of build location
  --defines=strict : exit on warning plus some more checker
  --defines=forceheader : Bypass check on date for header files generation
  --defines=enable_x86 : Enable OSTTRACE macro in x86
  --defines=disable_trace : standard process, but with OST_TRACE_COMPILER_IN_USE undefined causing traces to be not included in the code
  --defines=use_abs_path : Enable transformation of relative path into absolute path when computing name for generated header file
  ";
  exit(0);
}

sub get_group_value
{
  my $group_id = $_[0];
  my $grp_count = $_[1];

  if ( defined ($tab_group->{$group_id}) )
  {
    return ($tab_group->{$group_id});
  }
  else
  {
    if ($$grp_count > 15)
    {
      print color 'bold red';
      die "get_group_value: Reach maximum number of groups allowed: $$grp_count (16)\n";
    }
    else
    {
      $tab_group->{$group_id}->{'id'} = 2**$$grp_count;
      $tab_group->{$group_id}->{'count'} = 0;
      $$grp_count++;
      return ($tab_group->{$group_id});

    }
  }
}



sub get_group_index
{
  my $tab_group = $_[0];
  my $group_id = $_[1];

  if (defined ($defines{strict}))
  {
    die "ERROR: GROUP_ID not defined !!\n" if not defined ($group_id);
  }

  my $i = 0;

  if (defined (@$tab_group))
  {
    foreach my $my_group (@$tab_group)
    {
      if (defined ($my_group))
      {
        if ($my_group->{'name'} eq $group_id)
        {
          return ($i);
        }
      }
      $i++;
    }
  }
  return ($i);
}

sub init_group
{
  my $my_group;
  my $group_id = $_[0];
  my $grp_count = $_[1];


  if ($$grp_count > 15)
  {
    print color 'bold red';
    die "init_group: Reach maximum number of groups allowed: $$grp_count (16)\n";
  }
  else
  {
    $my_group->{'name'} = $group_id;
    $my_group->{'id'} = 2**$$grp_count;
    $my_group->{'suffix'} = "";
    $my_group->{'prefix'} = "$group_id: ";

    $$grp_count++;
    return ($my_group);
  }


}

sub PrintXML
{
  my $filename = $_[0];
  my $output = new IO::File "$filename", "w";
  my $writer = XML::Writer->new(DATA_MODE => 1, OUTPUT => $output);

  $writer->xmlDecl("UTF-8");
  $writer->startTag('tracedictionary', 'xmlns:xsi' => "http://www.w3.org/2001/XMLSchema-instance", "xsi:noNamespaceSchemaLocation" => "DictionarySchema.xsd");

  $writer->startTag('typedefs');
  foreach my $typedef (@{$xmltab->{'typedef'}->[0]->{'object'}})
  {
    $writer->emptyTag('object', 'type' => $typedef->{'type'}, 'size' => $typedef->{'size'}, 'formatchar' => $typedef->{'formatchar'}, 'classification' => $typedef->{'classification'});
  }
  $writer->endTag('typedefs');

  $writer->startTag('data');
  foreach my $data (@{$xmltab->{'data'}->[0]->{'def'}})
  {
    $writer->startTag('def', 'id' => $data->{'id'}, 'type' => $data->{'type'});
    $writer->characters($data->{'content'});
    $writer->endTag('def');
  }
  $writer->endTag('data');

  $writer->startTag('locations');
  foreach my $path (@{$xmltab->{'locations'}->{'path'}})
  {
    $writer->startTag('path', 'val' => $path->{'val'});
    foreach my $file (@{$path->{'file'}})
    {
      if (defined ($file) && ($file->{'content'} ne ""))
      {
        $writer->startTag('file', 'id' => $file->{'id'});
        $writer->characters($file->{'content'});
        $writer->endTag('file');
      }
    }
    $writer->endTag('path');
  }
  $writer->endTag('locations');

  foreach my $component (@{$xmltab->{'component'}})
  {
    $writer->startTag('component', 'id' => $component->{'id'}, 'name' => $component->{'name'}, 'prefix' => $component->{'prefix'}, 'suffix' => $component->{'suffix'});
    foreach my $group (@{$component->{'group'}})
    {
      if (defined ($group) && ($group->{'name'} ne ""))
      {

        $writer->startTag('group', 'id' => $dico_swap{$group->{'id'}}, 'name' => $group->{'name'}, 'prefix' => $group->{'prefix'}, 'suffix' => $group->{'suffix'});
        foreach my $trace (@{$group->{'trace'}})
        {
          $writer->startTag('trace', 'data-ref' => $trace->{'data-ref'}, 'name' => $trace->{'name'});
          $writer->emptyTag('instance', 'id' => $trace->{'instance'}->{'id'}, 'loc-ref' => $trace->{'instance'}->{'loc-ref'}, 'line' => $trace->{'instance'}->{'line'}, 'methodname' => $trace->{'instance'}->{'methoname'}, 'classname' => $trace->{'instance'}->{'classname'});
          $writer->endTag('trace');
        }
        $writer->endTag('group');
      }
    }
    $writer->endTag('component');
  }
  $writer->endTag('tracedictionary');
  $writer->end;

  #print $string;

}

sub OpenDicoFile
{
  my $dicofile = $_[0];

  if ( -e $dicofile)
  {
    # lire le fichier XML
    print "dico: $dicofile\n" if ($verbose >=2);
    my $size = 0;
    $dicodata = $xml->XMLin($dicofile, keyattr => {group=>'id'}, forcearray => [qw(trace group data def)]);

    if (defined (scalar(@{$dicodata->{'data'}->[0]->{def}})))
    {
      $size = scalar(@{$dicodata->{'data'}->[0]->{def}});
    }
    else
    {
      $size = 0;
    }
    printf "size: %d\n", $size if ($verbose >=5);
  }

}

sub OpenTraceCacheDefinition
{
  my $fixed_id_file_name = $_[0];

  if ( -e $fixed_id_file_name)
  {
    open FP_FIXED_ID_FILE, '>>', $fixed_id_file_name or die "Could not open file [$fixed_id_file_name]: $!\n";
    @fixed_id_file_array = <FP_FIXED_ID_FILE>;
    print @fixed_id_file_array;
  }
  else
  {
    open FP_FIXED_ID_FILE, '>', "$fixed_id_file_name" or die "Could not open file [$fixed_id_file_name]: $!\n";
  }
}

sub WriteTraceCacheDefinition
{
  my $XmlHashTable = $_[0];

  foreach my $group (@{$XmlHashTable->{'component'}->[0]->{'group'}})
  {
    my $group_id = $group->{'id'};
    printf "group ID: %d\n", $group_id if ($verbose >=5);
    my $i = 1;
    foreach my $trace (@{$group->{'trace'}})
    {
      my $trace_name = $trace->{'name'};
      #my $group_id_tmp = (($group_id >> 4) & 0x0F) | (($group_id << 4) & 0xF0);
      my $trace_value = ($group_id << 16) + $trace->{'instance'}->{'id'};
      printf FP_FIXED_ID_FILE "%s %s 0x%.4xu\n", $group->{'name'}, $trace_name, $trace_value;
    }
  }
  close FP_FIXED_ID_FILE;
}




sub UpdateTraceDefinition
{
  my $defFile = $_[0];
  my $match = 0;


  ## Open OstTraceDefinitions.h and read stuff to array
  open FILE_TMP, '<', $defFile or die "Could not open file [$defFile]: $!\n";

  my @lines = <FILE_TMP>;
  close(FILE_TMP);

  foreach my $line (@lines)
  {
    # Comment out the define
    #$line =~ s/(.*)#define OST_TRACE_COMPILER_IN_USE/\/\/ #define OST_TRACE_COMPILER_IN_USE/;
    if ($line =~/\/\/(.*)#define OST_TRACE_COMPILER_IN_USE/)
    {
      $match = 1;
    }
  }


  # Write stuff back to OstTraceDefinitions.h
  if ($match)
  {
    open FILE_TMP, '>', $defFile or die "Could not open file [$defFile]: $!\n";

    foreach my $line (@lines)
    {
      # Comment out the define
      #$line =~ s/(.*)#define OST_TRACE_COMPILER_IN_USE/\/\/ #define OST_TRACE_COMPILER_IN_USE/;
      if ($line =~/\/\/(.*)#define OST_TRACE_COMPILER_IN_USE/)
      {
        $line =~ s/(.*)#define OST_TRACE_COMPILER_IN_USE/#define OST_TRACE_COMPILER_IN_USE/;
      }

      print FILE_TMP $line;
    }
    close(FILE_TMP);
  }
}


sub CreateTraceDefinition
{
  my $filename = $_[0];

  if ( !-e $filename)
  {

    if ( !-e $include_dir[0])
    {
      mkdir ($include_dir[0]);
    }

    open TRACEFILE, '>', $filename or die "Could not open file [$filename]: $!\n";
    PrintHeader(\*TRACEFILE);

    printf TRACEFILE "#ifndef __OSTTRACEDEFINITIONS_H__\n";
    printf TRACEFILE "#define __OSTTRACEDEFINITIONS_H__\n";
    if (not defined ($defines{enable_x86}))
    {
      printf TRACEFILE "\n#if !defined(WORKSTATION)\n";
    }
    if (not defined ($defines{disable_trace}))
    {
      printf TRACEFILE "#define OST_TRACE_COMPILER_IN_USE\n";
    }
    else
    {
      printf TRACEFILE "// WARNING TRACE COMPILER RUN WITH OPTION disable_trace\n";
      printf TRACEFILE "#undef OST_TRACE_COMPILER_IN_USE\n";
    }
    if (not defined ($defines{enable_x86}))
    {
      printf TRACEFILE "\n#endif\n";
    }
    printf TRACEFILE "#endif\n";

    close TRACEFILE;
  }
}

sub CreateTraceHeaderFile
{
  my $filename = $_[0];
  my $filename_noext = $_[1];
  my $XmlHashTable = $_[2];
  my $FILE_ID = $_[3];


  # C file has been modified with potentially new trace ID => save header file
  open TRACEFILE, '>', "$include_dir[0]/$filename" or die "Could not open file [$include_dir[0]/$filename]: $!\n";
  PrintHeader(\*TRACEFILE);
  printf TRACEFILE "\n#ifndef __%s_H__\n", $filename_noext;
  printf TRACEFILE "#define __%s_H__\n", $filename_noext;

  printf TRACEFILE "#define GET_FILE							$FILE_ID\n";
#	printf TRACEFILE "#define GET_LINE							__LINE__\n";
#	printf TRACEFILE "#define GEN2(GET_FILE, group, GET_LINE)	FILE_##GET_FILE##_##GET_LINE\n";
#	printf TRACEFILE "#define GEN1(GET_FILE, group, GET_LINE)	GEN2(GET_FILE, group, GET_LINE)\n";
#	printf TRACEFILE "#define GEN_ID							GEN1(GET_FILE,toto,GET_LINE)\n";

  printf TRACEFILE "\n#ifndef __mode16__\n";
  printf TRACEFILE "\n#define KOstTraceComponentID 0x%.2Xul\n\n", $component_id;


  foreach my $group_trace (@{$XmlHashTable->{'used'}})
  {
    my $trace_value = ($group_trace->{'group_id'} << 16) + $group_trace->{'trace_id'};
    printf TRACEFILE "#define %s 0x%.4xu\n", $group_trace->{'trace_name'}, $trace_value;
  }

  printf TRACEFILE "\n#else\n";
  printf TRACEFILE "\n#define KOstTraceComponentID 0x%.2Xul\n\n", $component_id;


  foreach my $group_trace (@{$XmlHashTable->{'used'}})
  {
    my $trace_value = ($group_trace->{'group_id'} << 16) + $group_trace->{'trace_id'};
    printf TRACEFILE "#define %s 0x%.4xul\n", $group_trace->{'trace_name'}, $trace_value;
  }

  printf TRACEFILE "#endif\n";
#	printf TRACEFILE "\n#ifdef __SYMBIAN32__\n";
#	printf TRACEFILE "#include <ost_symbian_wrapper_ste.h>\n";
#	printf TRACEFILE "#endif\n";
  printf TRACEFILE "#endif\n";

  close TRACEFILE;


}










sub PrintXML_2
{

  my $filename = $_[0];
  my $XmlHashTable = $_[1];
  my $output = new IO::File "$filename", "w";
  my $writer = XML::Writer->new(DATA_MODE => 1, OUTPUT => $output);

  $writer->xmlDecl("UTF-8");
  $writer->startTag('tracedictionary', 'xmlns:xsi' => "http://www.w3.org/2001/XMLSchema-instance", "xsi:noNamespaceSchemaLocation" => "DictionarySchema.xsd");

  $writer->startTag('typedefs');
  foreach my $typedef (@{$XmlHashTable->{'typedef'}->[0]->{'object'}})
  {
    $writer->emptyTag('object', 'type' => $typedef->{'type'}, 'size' => $typedef->{'size'}, 'formatchar' => $typedef->{'formatchar'}, 'classification' => $typedef->{'classification'});
  }
  $writer->endTag('typedefs');

  $writer->startTag('data');
  foreach my $data (@{$XmlHashTable->{'data'}->[0]->{'def'}})
  {
    $writer->startTag('def', 'id' => $data->{'id'}, 'type' => $data->{'type'});
    $writer->characters($data->{'content'});
    $writer->endTag('def');
  }
  $writer->endTag('data');

  $writer->startTag('locations');
  $writer->startTag('path', 'val' => $XmlHashTable->{'locations'}->{'path'}->{'val'});
  foreach my $file (@{$XmlHashTable->{'locations'}->{'path'}->{'file'}})
  {
    $writer->startTag('file', 'id' => $file->{'id'});
    $writer->characters($file->{'content'});
    $writer->endTag('file');
  }
  $writer->endTag('path');
  $writer->endTag('locations');

  foreach my $component (@{$XmlHashTable->{'component'}})
  {
    $writer->startTag('component', 'id' => $component->{'id'}, 'name' => $component->{'name'}, 'prefix' => $component->{'prefix'}, 'suffix' => $component->{'suffix'});
    foreach my $group (@{$component->{'group'}})
    {
      if (defined ($group) && ($group->{'id'} ne ""))
      {
        $writer->startTag('group', 'id' => $dico_swap{$group->{'id'}}, 'name' => $group->{'name'}, 'prefix' => $group->{'prefix'}, 'suffix' => $group->{'suffix'});
        foreach my $trace (@{$group->{'trace'}})
        {
          $writer->startTag('trace', 'data-ref' => $trace->{'data-ref'}, 'name' => $trace->{'name'});
          $writer->emptyTag('instance', 'id' => $trace->{'instance'}->{'id'}, 'loc-ref' => $trace->{'instance'}->{'loc-ref'}, 'line' => $trace->{'instance'}->{'line'}, 'methodname' => $trace->{'instance'}->{'methoname'}, 'classname' => $trace->{'instance'}->{'classname'});
          $writer->endTag('trace');
        }
        $writer->endTag('group');
      }
    }
    $writer->endTag('component');
  }
  $writer->endTag('tracedictionary');
  $writer->end;
}


sub PrintHeader
{
  my $FileHandle = $_[0];

  printf $FileHandle "/*****************************************************************************/\n";
  printf $FileHandle "/**\n";
  printf $FileHandle " *  (c) ST-Ericsson, 2010 - All rights reserved\n";
  printf $FileHandle " *  Reproduction and Communication of this document is strictly prohibited\n";
  printf $FileHandle " *  unless specifically authorized in writing by ST-Ericsson\n";
  printf $FileHandle " *\n";
  printf $FileHandle " * Author ST-Ericsson\n";
  printf $FileHandle " */\n";
  printf $FileHandle "/*****************************************************************************/\n";
#	printf $FileHandle "// Created by ST OST Trace Compiler version %s build: %d on %s\n", $version, substr($build,6,3), $now_string;				
  printf $FileHandle "// Created by STE OST Trace Compiler\n";
  printf $FileHandle "// DO NOT EDIT, CHANGES WILL BE LOST\n";
}

sub ReadGroupValue
{

  my $mediaroot = $defines{mediaroot};
  die "MEDIA ROOT not defined \n" if (not defined ($mediaroot));

  my $filename = $defines{grpmapfile};
  die "GRP MAP FILE  not defined \n" if (not defined ($filename));

  open FH, "$filename" or die "Could not open file [$filename]: $!\n";

  while (my $line_tmp = <FH>)
  {
    if ($line_tmp =~/^\s*#define FIXED_GROUP/)
    {
      #my @line_tab_tmp = ($line_tmp =~ /[\s?]\[GROUP\][\s*](\w+)[\s*](\d)/);
      my @line_tab_tmp = ($line_tmp =~ /^(\s*)#define\s*(\w+)\s*(\w+)/);
      #printf "TRACE GROUP: %s %d\n", $line_tab_tmp[1], $line_tab_tmp[2];
      $fixed_dico_ost{$line_tab_tmp[1]} = $line_tab_tmp[2];
    }
  }
  close FH;
}

sub CreateTopDictionnary
{
  my ($directory, @top_dico_list) = @_;


  printf "ST OST TRACECOMPILER: Generating top level dictionaries files\n" if ($verbose >=1);

  foreach my $file_tmp (@top_dico_list)
  {

    if ($verbose >=5)
    {
      foreach my $file (@top_dico_list)
      {
        printf "dictionnary: %s\n", $file;
      }
    }

    # Get top dico filename and TOP dico UID that we need to generate
    (my $top_dico, my $top_dico_ID) = split (/::/, $file_tmp);

    # Create top dictionnary XML file, + header file
    my $XmlOutputfile = $directory . "/" . $top_dico . "_top" . ".xml";
    my $HeaderOutputfile = $directory . "/" . $top_dico . "_top" . ".h";
    my %group_list;

    open OUT_XML_FILE, '>', $XmlOutputfile  or die "Could not open file [$XmlOutputfile]: $!\n";
    open (OUT_HEADER_FILE, '>', $HeaderOutputfile)  or die "Could not open file [$HeaderOutputfile]: $!\n";

    PrintHeader(\*OUT_HEADER_FILE);
    printf OUT_HEADER_FILE "\n#ifndef __%s_H__\n", $top_dico;
    printf OUT_HEADER_FILE "#define __%s_H__\n\n", $top_dico;
    printf OUT_HEADER_FILE "\n#define KOst%s_top_ID 0x%.2Xul\n\n", $top_dico, $top_dico_ID;


    printf OUT_XML_FILE "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    printf OUT_XML_FILE "<tracedictionary xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"DictionarySchema.xsd\">\n";
    printf OUT_XML_FILE "<component id=\"$top_dico_ID\" name=\"$top_dico\" prefix=\"$top_dico: \" suffix=\"\">\n";

    # Get list of dictionnaries from which we need to generate top dico list
    my @l_dico_list = ();
    @l_dico_list = glob("$directory/*.xml") if (not defined ($defines{force_top_dico}));

    if ($verbose >=5)
    {
      foreach my $file (@l_dico_list)
      {
        printf "dictionnary: %s\n", $file;
      }
    }
    if (@l_dico_list == 0)
    {
      foreach my $group (@{$xmltab->{'component'}->[0]->{'group'}})
      {
        print OUT_XML_FILE "<group id=\"$dico_swap{$group->{'id'}}\" name=\"$group->{'name'}\" prefix=\"$group->{'name'}: \" suffix=\"\"></group>\n";
        printf OUT_HEADER_FILE "#define GROUP_%s %d\n", $group->{'name'}, $dico_swap{$group->{'id'}};
      }
    }
    else
    {

      my $DicoListOutputfile = $directory . "/" . $top_dico . "_top" . ".txt";
      open OUT_TXT_DICOLIST, '>', $DicoListOutputfile  or die "Could not open file [$DicoListOutputfile]: $!\n";
      foreach my $dico_file (@l_dico_list)
      {
        # Do not include previous top dictionnaries in the list !!!
        next if ($dico_file =~ /_top/);

        open MY_FILE, "$dico_file" or die "Could not open file [$dico_file]: $!\n";

        printf OUT_TXT_DICOLIST "%s\n", $dico_file;

        while (my $line_tmp = <MY_FILE>)
        {
          if ($line_tmp =~ /(^\s*)(<(\s*)group)(\s*)id/)
          {
            if ($line_tmp !~ /(<\/)group/)
            {
              $line_tmp =~ s/(.*)/$1<\/group>/
            }

            if (not defined ($group_list{$line_tmp}))
            {
              print OUT_XML_FILE $line_tmp;
              $group_list{$line_tmp} = "TRUE";
              my @group = ($line_tmp =~ /(^\s*)(<(\s*)group)(\s*)id=\"(.+)\"(\s*)name=\"(\w+)\"/);
              printf OUT_HEADER_FILE "#define GROUP_%s %d\n", $group[6], $group[4];
            }
          }
        }
        close(MY_FILE);
      }
      close(OUT_TXT_DICOLIST);
    }

    printf OUT_XML_FILE "</component>\n";
    printf OUT_XML_FILE "</tracedictionary>\n";
    close(OUT_XML_FILE);
    printf OUT_HEADER_FILE "\n#endif\n";
    close(OUT_HEADER_FILE);
  }
}

