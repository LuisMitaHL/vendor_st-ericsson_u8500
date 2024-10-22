#!/usr/bin/perl -W
#
# St-Ericsson Config parser for Hats
# 2011 - Alexandre Bouin
#

#*****************************************************************************#
#                                Init and Check                               #
#*****************************************************************************#

# Get script filename
use File::Basename;
my $prog = basename $0;

# Get first argument - config file in our case
if (!$ARGV[0]) {
	print "$prog : Please specify a config file.\n";
	print "  usage : ./$prog <configfile> <depsmakefile>\n";
	exit(1);
}

# Get second argument - dependencies makefile in our case
if (!$ARGV[1]) {
	print "$prog : Please specify a dirs file.\n";
	print "  usage : ./$prog <configfile> <depsmakefile>\n";
	exit(1);
}

#*****************************************************************************#
#                                  Functions                                  #
#*****************************************************************************#

sub hatsconf_parse {
	my ($hash, $regvariant, $file) = @_;
	foreach (@$file) {
		if(/HATSCONF_.+$regvariant/) {
			($name, $value) = split(/=/);
			chomp $value;
			# Get module name and remove all special characters
			$name =~ s/HATSCONF_(.+)$regvariant/$1/;
			$name =~ s/[^a-zA-Z0-9.\/\_\-\ ]+//g;
			# Get module path and remove all special characters
			$value =~ s/[^a-zA-Z0-9.\/\_\-\ ]+//g;
			$$hash{$name} = $value;
		}
	}
	return %$hash;
}

#*****************************************************************************#
#                        Open, read, close config file                        #
#*****************************************************************************#

unless (open(FILE, "<$ARGV[0]" )) {
	print "$prog : Cannot open $ARGV[0].\n";
	exit(1);
}
$i = 0;
foreach (<FILE>) {
	if(/^$/) { next; }	# Remove blank lines
	if(/# */) { next; }	# Remove comments
	$configfile[$i] = $_;
	$i++;
}
close(FILE);

#*****************************************************************************#
#                              Parse config file                              #
#*****************************************************************************#

# Parse directory variables
my %dirs;
%dirs = hatsconf_parse(\%dirs, "_DIR", \@configfile);

# Check if module exists twice
my %doubles;
foreach $key (keys(%dirs)) {
	$doubles{$key}++;
	foreach $value (keys(%dirs)) {
		if( ($dirs{$value} eq $dirs{$key}) && ($value ne $key)) {
			print "dirs_value=$dirs{$value} dirs_key=$dirs{$key} value=$value key=$key\n";
			$doubles{$key}++;
		}
	}
}
while( ($key,$value) = each(%doubles) ) {
	if($value > 1) {
		print "$prog : Module '$key' is present $value time\n";
		exit(1);
	}
}

# Check if directory exists
foreach $key (keys(%dirs)) {
	if( ! -e $dirs{$key} ) {
		print "$prog : Directory $dirs{$key} doesn't exists for module '$key'\n";
		exit(1);
	}
}

# Parse dependencies variables
my %deps;
%deps = hatsconf_parse(\%deps, "_DEPS", \@configfile);

# Check if dependencies exists
foreach $key (keys(%deps)) {
	@dependencies = split(/ /,$deps{$key});
	foreach $dependency (@dependencies) {
		if(! exists $dirs{$dependency}) {
			printf "$prog : Dependency $dependency for $key has not been found\n";
			exit(1);
		}
	}
}

# Parse strip variables
my %strip;
%strip = hatsconf_parse(\%strip, "_STRIP_DISABLED", \@configfile);

# Parse debug variables
my %debug;
%debug = hatsconf_parse(\%debug, "_DEBUG_ENABLED", \@configfile);

# Parse compile log variable
my $compile_logs = 0;
foreach (@configfile) {
	if(/HATSCONF_COMPILE_LOGS/) {
		$compile_logs = 1;
		last if($compile_logs == 1);
	}
}

#*****************************************************************************#
#                     Generate dependencies makefile                          #
#*****************************************************************************#

unless (open(FILE, "> $ARGV[1]" )) {
	print "$prog : Cannot open $ARGV[1].\n";
	exit(1);
}

# Makefile header
printf(FILE "# Autogenerated file for HATS dependencies.\n");
printf(FILE "# Do not edit this file !\n");
printf(FILE "# Do not include it under git !\n\n");

# Include config files
printf(FILE "include $ARGV[0]\n\n");

printf(FILE "include .config.mk\n\n");

# Fill and propagate generic Hats variables
printf(FILE "ifeq (\$(HATSCONF_ENABLE_EXTRA_CFLAGS),y)\n");
printf(FILE  "\texport HATS_EXTRA_CFLAGS+= \\\n");
printf(FILE  "\t\t\"\$(HATSCONF_EXTRA_CFLAGS)\"\n");
printf(FILE  "else\n");
printf(FILE  "\texport HATS_EXTRA_CFLAGS+=\n");
printf(FILE  "endif\n\n");

# Date variable
if($compile_logs == 1) {
	printf(FILE "DATE := \$(shell date +%%Y%%m%%d_%%H%%M%%S)\n\n");
}

# Create log directoy
if($compile_logs == 1) {
	printf(FILE "HATSCONF_COMPILE_LOGS_DIR = \$(HATS_OBJ)\/logs\/\$(DATE)\n\n");
}

# Generate module list
printf(FILE "DIRS = ");
foreach $key (keys(%dirs)) {
	printf(FILE " \\\n\t\$(HATSCONF_");
	printf(FILE "$key");
	printf(FILE "_DIR)");
}
printf(FILE "\n\n");

# Write main rules
printf(FILE ".PHONY: \$(DIRS) all clean\n\n");

printf(FILE "all clean: \$(DIRS)\n\n");

if($compile_logs == 1) {
	printf(FILE "\$(HATSCONF_COMPILE_LOGS_DIR):\n");
	printf(FILE "\t\$(MKDIR) -p \$@\n\n");
}

if($compile_logs == 1) {
	printf(FILE "\$(DIRS): \$(HATSCONF_COMPILE_LOGS_DIR)\n\n");
} else {
	printf(FILE "\$(DIRS):\n\n");
}

# Generate modules rules one-by-one
foreach $key (keys(%dirs)) {
	$alias=basename($dirs{$key});
	printf(FILE "$alias: ");
	printf(FILE "\$(HATSCONF_");
	printf(FILE "$key");
	printf(FILE "_DIR)\n");

	printf(FILE "\$(HATSCONF_");
	printf(FILE "$key");
	printf(FILE "_DIR) : \n");
	printf(FILE "\t@\$(ECHO) -e \"\\t-----> \$@ : \$(MAKECMDGOALS)\"\n");

	printf(FILE "\t@\$(MAKE) ");
	# Debug configuration
	if((!exists $debug{$key}) && ($compile_logs == 0)) {
		printf(FILE "-s ");
	}
	printf(FILE "-C \$@ \$(MAKECMDGOALS)");
	# Strip configuration
	if(exists $strip{$key}) {
		printf(FILE " \\\n\t\tSTRIP=\"echo STRIP DISABLED \> \/dev\/null\"");
	}
	# Compile log configuration
	if($compile_logs == 1) {
		printf(FILE " \\\n\t\t\> \$(HATSCONF_COMPILE_LOGS_DIR)\/$alias.log 2\>\&1");
	}
	printf(FILE "\n\n");
}

# Generate dependencies
foreach $key (keys(%deps)) {
	printf(FILE "\$(HATSCONF_");
	printf(FILE "$key");
	printf(FILE "_DIR) :");
	@dependencies = split(/ /,$deps{$key});
	foreach $dependency (@dependencies) {
		printf(FILE " \$(HATSCONF_");
		printf(FILE "$dependency");
		printf(FILE "_DIR)");
	}
	printf(FILE "\n");
}
close(FILE);

# Terminate Application
print "# $prog : File $ARGV[0] successfully parsed.\n";
exit(0);
