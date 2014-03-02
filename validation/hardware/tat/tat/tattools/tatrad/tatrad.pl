#!/usr/local/bin/perl
# Rapid Aided Developement for TAT (embedded)
# Use this program to generate or update a TAT module DTH and service interfaces

use strict;
use warnings;
#use diagnostics; # enable this if you have a perl error you don't understand to get more information about it

use XML::Twig; # simple, portable, efficient, both SAX and DOM but no XML validation
use Cwd;
use Getopt::Long;

#pp compatibility stuff
use LWP;
use LWP::Protocol::file;
use URI::file::Win32;


#################
# Program info
#################

my $appname	= "TAT RAD";
my $appversion	= "1.0";


#################
# Command line processing
#################

my $arg_file	= $ARGV[0];
my $arg_module	= $ARGV[1];

my $opt_debug		= 0;
my $opt_help		= 0;
my $opt_resource	= ".";
my $opt_output		= "srce/$arg_module.h srce/$arg_module.c";
my $opt_mode		= "make";
my $opt_prefix		= "";
my $opt_header		= "";
if ( $arg_module ) { $opt_header = $arg_module . ".h"; }
my $opt_mod_version = "";
my $opt_includes	= ".";

GetOptions(
	"template=s" => \$opt_resource,
	"output=s" => \$opt_output,
	"prefix=s" => \$opt_prefix,
	"mode=s" => \$opt_mode,
	"header=s" => \$opt_header,
	"version=s" => \$opt_mod_version,
	"incl=s" => \$opt_includes,
	"debug" => \$opt_debug,
	"help" => \$opt_help,
);


#################
# Globals
#################

my $dthroot;

# arg_output can be a filename of pathname.
# the @filename and @FILENAME tokens refer to the filename not the pathname
my $output_filename;

# prefix to apply to litterals in action@user_data, argument@user_data and enum_items@value values.
# the value of the variable is set by module@symbol-prefix tag.
my $symbol_prefix = '';


#################
# Main program
#################

sub usage;
sub help;
sub rad_dth;

if ( $opt_help == 1 ) {
	help ;
} elsif ( @ARGV == 2 ) {
	rad_dth $arg_file;
}
else {
	usage;
}


#################
# Sub programs
#################

# sub programs variables
my %hash_dth_defs;

# dth elements paths are unique but not their id code
# key is pathname, value is a hash containing the properties and their values
my @list_dth = ();

# keys and values of the following hash tables are as expected by sub program format_c_code_ex
# keys role:
# code:	 name
# value:	identifier, can be litterals or numeric
# comment:  short description
my %dth_element_ids;
my %dth_element_paths;
my %dth_enum_ids;

my %dth_enum_def;

sub usage {
	print "./tatrad file module --debug --help -output <output> -template <template> -prefix <prefix> -mode <mode> -header <header> -version <version>\n";
	print "option --help to display help\n";
}

sub help {
	usage;
	print "<file> is the filename of the XML file describing a DTH-based module API.\n";
	print "TAT RAD processes the <module> specified matching the one inside the XML file (XML attribute 'module')\n";
	print "<output> is mandatory and specifies the DTH source files to generate. First must be the .h file, next is the .c file\n";
	print "<mode> 'rad' to make TAT RAD generates DTH service skeleton (not implemented)\n";
	print "<header> specifies the name of the header file to be included in the generated .h file. This file should be the project global header file.\n";
	print "The module version used by DTH framework should be specified by the <version> option.\n";
	print "For any other value, TAT RAD will generate the DTH header and implementation files according to the XML file.\n";
}

# default sub pogram format_c_code_ex arguments to use when the calling arg is missing in $args
my %format_c_code_ex_prefs;

sub format_c_code_ex {
	my ($user_args) = @_;

	# each argument of the hash $args defines a caracteristic of the code to print
	# string code		   the source code of the instruction
	# int value			 optional value. Likely to be used for defines.
	# int column			optional column alignment between code and value
	# int head_spaces	   tells how many space are to be inserted before the code. 0 if not set.
	# string comment		an optional commentary string whithout comment markers
	# string comment_pos	defines where to place the commentary
	my %args = (%format_c_code_ex_prefs, %$user_args);

# DEBUG: print overall program arguments
#	print "format_c_code_ex(";
#	while ( my ($k, $v) = each (%args) )
#	{
#		print $k . "=" . $v.",";
#	}
#	print ")\n";

	my $text = "";

	my $nbsp = ($args{'head_spaces'} or 0);
	my $comment = ($args{'comment'} or "");
	my $comment_pos = ($args{'comment_pos'} or "vert");
	my $end = ($args{'end'} or "\n");

	if ( (length $comment) && ($comment_pos eq "vert") )
	{
		$text .= sprintf "%*s", $nbsp, " " if $nbsp > 0;
		$text .= "/* ".$comment." */\n";
	}

	$text .= sprintf ("%*s", $nbsp, " ") if $nbsp > 0;

	if ( exists $args{'column'} )
	{
		$text .= sprintf "%*s", $args{'column'}, $args{'code'};
	}
	else
	{
		$text .= $args{'code'}." ";
	}

	if ( defined $args{'value'} )
	{
		$text .= " ". $args{'value'};
	}

	if ( (length $comment) && ($comment_pos eq "horz") )
	{
		$text .= " /* ".$args{'comment'}." */";
	}

	$text .= $end if length $end;

	return $text;
}

sub debug_str {
	my ($str) = @_;

	if ( $opt_debug == 1 ) {
		print $str;
	}
}

# format to a valid C identifier composed alphanum plus underscore only.
# when a char is not valid it is replaced by an underscore.
sub format_c_name {
	my ($id) = @_;
	$id =~ s/[^0-9^a-z^A-Z^_]+/_/g;
	return $id;
}

# format C quoted string. escape intermediate quote if there is any.
sub format_c_qq {
	my ($in) = @_;
	$in =~ s/\"/\\\"/g;
	$in = '"' . $in . '"';

	return $in;
}

sub isuint {
	my ($v) = @_;
	my $r = 0;

	if ( $v =~ /^\d+$/ ) {
		$r = 1;
	}

	return $r;
}

sub dth_element_str {
	my ($prop) = @_;

	my $text = "{\n";

	$format_c_code_ex_prefs{'comment_pos'} = "vert";
	$format_c_code_ex_prefs{'column'} = -40;
	$format_c_code_ex_prefs{'head_spaces'} = 4;
	$format_c_code_ex_prefs{'end'} = "\n";

	$text .= format_c_code_ex({ code => format_c_qq($prop->{'path'}).","});
	$text .= format_c_code_ex({ code => $prop->{'type'}.","});

	if ( exists $prop->{'cols'} )
	{
		$text .= format_c_code_ex({ code => $prop->{'cols'}.", ".$prop->{'rows'}.",",
			comment => "nb cols, nb rows (array)"});
	}
	else
	{
		$text .= format_c_code_ex({ code => "0, 0,", comment => "not an array"});
	}

	if ( exists $prop->{'enum'} )
	{
		my $props = $prop->{'enum'};
		$text .= "	\"";
		my $ind_prop = 0;
		foreach (@{$props})
		{
			if ( defined $_ ) {
				$text .= $_."\\t".$ind_prop.'\n';
			}
			$ind_prop++;
		}
		$text .= "\",\n";
	}
	else
	{
		$text .= format_c_code_ex({ code => "NO_ENUM,"});
	}

	if ( exists $prop->{'info'} )
	{
		$text .= "\t".format_c_qq($prop->{'info'}).",\n";
	}
	else
	{
		$text .= format_c_code_ex({ code => "NO_INFO,"});
	}

	if ( exists $prop->{'min'} )
	{
		$text .= format_c_code_ex({ code => $prop->{'min'}.", ".$prop->{'max'}.","});
	}
	else
	{
		$text .= format_c_code_ex({ code => "0, 0,"});
	}

	if ( exists $prop->{'get'} ) {
		$text .= format_c_code_ex({ code => "$prop->{'get'},"});
	} else {
		$text .= format_c_code_ex({ code => "NO_GET,"});
	}

	if ( exists $prop->{'set'} ) {
		$text .= format_c_code_ex({ code => "$prop->{'set'},"});
	} else {
		$text .= format_c_code_ex({ code => "NO_SET,"});
	}

	if ( exists $prop->{'exec'} ) {
		$text .= format_c_code_ex({ code => "$prop->{'exec'},"});
	} else {
		$text .= format_c_code_ex({ code => "NO_EXEC,"});
	}

	$text .= format_c_code_ex({ code => "$prop->{'alias'}, "});

	delete $format_c_code_ex_prefs{'column'};

	# default value: always NULL
	$text .= format_c_code_ex({ code => "NULL\n},"});

	%format_c_code_ex_prefs = ();

	return $text;
}

sub resolve_symbol {
	my ($symbol, $path) = @_;
	my $val;

	# -h option to remove filename on result
	my $grepres = `grep -whe '.*#define.*$symbol' $path`;
	if ( $grepres )
	{
		if ( $grepres =~ /^\s*#define\s+$symbol\s+(\w+)/ )
		{
			$val = eval $1;
			debug_str "$symbol = $val\n";
		}
		else
		{
			die "fail to extract token! Error in script.\n";
		}
	}

	return $val;
}

sub set_symbol {
	my ($symbol_table, $symbol_args) = @_;
	my $skipped = 0; 

	my $symbol = $symbol_args->{'code'};
	die "symbol not specified!" unless $symbol;

	my $match_args = $symbol_table->{$symbol};
	if ( defined $match_args )
	{
		# if the symbol is already set then merge its args with the one provided except for arg 'code'
		if ( exists $match_args->{'value'} and (exists $symbol_args->{'value'}) )
		{
			if ("".$match_args->{'value'} ne "".$symbol_args->{'value'})
			{
				# error: redefinition of symbol!
				#die "Symbol '" . $symbol . "' is already defined. Cannot change its definition!\n";
				debug_str "Symbol '" . $symbol . "' is already defined. Cannot change its definition!\n";
				$skipped = 1;
			}
			else
			{
				# warn we are reusing an existing constant
				debug_str "warning: reusing symbol \"$match_args->{'code'}\". Was it your intent?\n";
				$match_args->{'callers'} ++;
			}
		}

		# merge the provided arguments except 'code'
		if(  $skipped == 0 )
		{
		while ( my ($k, $v) = each (%{$symbol_args}))
		{
			if ( $k ne "code") {
				$symbol_table->{$symbol}{$k} = $v;
			}
		}	
		}
	}
	else
	{
		# if symbol is not set yet then add a record for the symbol with the args provided.
		$symbol_table->{$symbol} = ();
		while ( my ($k, $v) = each (%{$symbol_args}) )
		{
			 $symbol_table->{$symbol}{$k} = $v;
		}
		$symbol_table->{$symbol}{'callers'} = 1;
	}
}

# normalize a min/max value so it is both readable dans not compiler interpretable.
sub dth_norm_min_max {
	my ($val, $type) = @_;
	my ($r, $sym);
	my $bitfield_length = 0;
	my $signess = substr $type, 0, 1;
	my $type_size = substr $type, 1, 2;
	#debug_str "signess = $signess\n";

	# check if bitfield type
	if ( $type =~/BITFIELD_(\d+)_bits/ )
	{
		#get bitfield length
		$bitfield_length = $1;
	}

	if ( $val =~ /^:(\w+)$/ )
	{
		$sym = $1;
		$val = resolve_symbol ( $sym, $opt_includes );
		die "FATAL! cannot resolve symbol $sym.\nCheck source and includes\n" if ( !defined ($val) );
	}

	$r = "".$val;
	if ( $signess eq "U" || $bitfield_length != 0 )
	{
		# unsigned
		$r .= "U";
	}

	if( $type_size eq "32" || (($bitfield_length > 16) && ($bitfield_length <= 32) ))
	{
		 # long
		$r .= "L";
	}
	elsif( $type_size eq "64" || (($bitfield_length > 32) && ($bitfield_length <= 64) ))
	{
		#long long
		$r .= "LL";
	}

	return $r;
}

sub subst_by {
	my ($src, $opts) = @_;
	my $result = $src;

	if ( defined $opts->{'subst.re'} )
	{
		$result =~ s/$opts->{'subst.re'}/$opts->{'subst.by'}/;
	}

	return $result;
}

sub dth_add_enum_typedef {
	my ($name, $items) = @_;

	die "Enum typedef $name is already defined!" unless
	!defined($dth_enum_def{name});

	$dth_enum_def{$name} = $items;
}

sub dth_struct_record {
	my ($node, $opts) = @_;
	my $field;
	my $prop = {};

	# path
	my $t = $node->att('path');

	# apply path substitution if any
	$prop->{'path'} = subst_by $t, $opts;

	# symoblic name
	if (!defined $dthroot)
	{
		$t =~ /^\/DTH\/(\w+)/;
		$dthroot = $1;
	}

	# short name
	$t =~ s/^\/DTH\///;
	$t = uc format_c_name ($t); # s/\//_/g;
	my $sname = $t;

	# long name (default)
	my $lname;
	if ( $node->att('exec') ) {
		$lname = $symbol_prefix.'ACT_'. $t;
	} else {
		$lname = $symbol_prefix.'ARG_'. $t;
	}

	# user data: can be of several formats
	$t = $node->att('user_data');

	#print "$t\n";

	if ( $t =~ /^\d+$/ )
	{
		debug_str "user_data='NUMBER'\n";

		$prop->{'id'} = int ( $t );
		set_symbol (\%dth_element_ids, { code => $lname, value => $prop->{'id'} } );
	}

	# case user_data="SYMBOL:?"
	elsif ( $t =~ /^[a-zA-Z_][0-9a-zA-Z_]*:/ )
	{
		$t =~ /^(\w*):([\w+-\/\*\s\(\)]+)/;
		$lname = $symbol_prefix.$1;
		debug_str "left=$1 right=$2\n";

		my $arg = $2;

		if ( isuint($arg) == 1 )
		{
			debug_str "user_data='SYMBOL:NUMBER'\n";
			$prop->{'id'} = int ( $2 );
		}
		else
		{
			debug_str "user_data='SYMBOL:EXPR'\n";
			$prop->{'id'} = "( $2 )"; # macros should ever be enclosed by parenthesis
		}

		# check if the alias is already defined
		set_symbol (\%dth_element_ids, { code => $lname, value => $prop->{'id'} } );
	}

	# case user_data="EXPR"
	elsif ( $t =~ /[^\w^:]+/ )
	{
		debug_str "user_data='EXPR'\n";

		# cannot determine either the name or id because both are arithmetic-based
		$prop->{'id'} = $t;

		# there is also no symbol to provide
		set_symbol (\%dth_element_ids, { code => $lname, value => $prop->{'id'} } );
	}

	# case user_data="SYMBOL", symbol using automatic value
	else
	{
		$lname = $symbol_prefix.$t;
		debug_str "user_data=$lname (SYMBOL)\n";

		set_symbol(\%dth_element_ids, { code => $lname, comment => $prop->{'id'} });
	}
	$prop->{'alias'} = $lname;

	debug_str "user_data=".$prop->{'id'}."\n" if exists $prop->{'id'};

	set_symbol(\%dth_element_ids, { code => $lname, comment => $prop->{'path'} });
	set_symbol(\%dth_element_paths, { code => "#define ".$lname."_PATH", value => "\"$prop->{'path'}\"" });

	# type
	my $type_short = $node->att('type');

	# bit field type begins by 'BITFIELD'
	if ( $type_short =~ '^BITFIELD' ) {
		$t = "DTH_" . $type_short;
	}
	# other types are passed without prefix
	else {
		$t = "DTH_TYPE_" . $type_short;
	}

	$prop->{'type'} = $t;

	# array dimensions
	if ( $field = $node->first_child('array') ) {
		$prop->{'cols'} = $field->att('cols');
		$prop->{'rows'} = $field->att('rows');
	}

	$format_c_code_ex_prefs{'comment_pos'} = "vert";
	$format_c_code_ex_prefs{'column'} = -64;

	# enumeration
	if ( $field = $node->first_child('enum') )
	{
		my $enum_nbr;
		my $enum_name;
		my $next_nbr = 0;
		my @items;

		$hash_dth_defs{'dth_enums'} .= "/* ".$lname." enumeration options */\n";

		my $enum_tpl = $field->att('template');
		if ($enum_tpl) {
			die "$lname use undefined typedef $enum_tpl" unless
			$dth_enum_def{$enum_tpl};

			$prop->{'enum'} = $dth_enum_def{$enum_tpl};
		} else {
			foreach my $enum_item ($field->children('enum_item'))
			{
				# enum_item value attribute can be of 3 different formats.
				my $enum_value = $enum_item->att('value');

				# set default enum name
				$enum_name = $symbol_prefix."ENUM_" . $sname . "_" . uc format_c_name($enum_item->text);

				debug_str "\n". $prop->{'path'} . ": enum / ".$enum_item->text." \"" . ($enum_value or "") . "\"\n";

				# case value not specified: use default
				if ( !defined $enum_value )
				{
					$enum_nbr = $next_nbr;
					$next_nbr++;
				}

				# case value="NUMBER"
				elsif ( $enum_value =~ /^\d+$/ )
				{
					$enum_nbr = int $enum_value;
					$next_nbr = $enum_nbr+1;
				}

				# case value="SYMBOL:ALPHA_NUM"
				elsif ( $enum_value =~ /^(\w*):(\w+)$/ )
				{
					if ( length $1 > 0 ) {
						$enum_name = $symbol_prefix.$1;
					}
					$enum_nbr = $2;

					# second argument can be a number or an external symbol
					if ( isuint($enum_nbr) )
					{
						$enum_nbr = int ( $2 );
					}
					else
					{
						my $extsym = $enum_nbr;
						$enum_nbr = resolve_symbol ( $extsym, $opt_includes );
						die "FATAL! $enum_name: cannot resolve symbol $extsym.\nCheck source and includes\n" if ( !defined ($enum_nbr) );
					}

					$next_nbr = $enum_nbr+1;
				}

				# case value="SYMBOL"
				else
				{
					$enum_nbr = $next_nbr;
					$next_nbr++;

					$enum_name = $symbol_prefix.$enum_value;
				}

				debug_str "enum: symbol=$enum_name, value=$enum_nbr\n";

				die "enum item $enum_value in $prop->{'path'} already defined!\n" if ( exists $items[$enum_nbr] );

				$items[$enum_nbr] = $enum_item->text ;

				if ( !exists $dth_enum_ids{$enum_name} )
				{
					$hash_dth_defs{'dth_enums'} .= format_c_code_ex ({
						code => "#define ". $enum_name,
						value => $enum_nbr,
					 });
				}

				set_symbol(\%dth_enum_ids, { code => $enum_name, value => "".$enum_nbr } );
			}
			$hash_dth_defs{'dth_enums'} .= "\n";

			if ($field->att('typedef')) {
				dth_add_enum_typedef $field->att('typedef'), \@items;
			}

			$prop->{'enum'} = \@items;
		}
	}

	%format_c_code_ex_prefs = ();

	# info field
	if ( $field = $node->first_child('info') ) {
		$prop->{'info'} = $field->text;
	}

	# number range
	if ( $field = $node->first_child('range') ) {
		$prop->{'min'} = dth_norm_min_max($field->att('min'), $type_short);
		$prop->{'max'} = dth_norm_min_max($field->att('max'), $type_short);
	}

	# get, set and exec handlers
	$t = $node->att('get');
	$prop->{'get'} = format_c_name($t) if $t;

	$t = $node->att('set');
	$prop->{'set'} = format_c_name($t) if $t;

	$t = $node->att('exec');
	$prop->{'exec'} = format_c_name($t) if $t;

	$list_dth[$#list_dth+1] = $prop;
}

sub xml_subst {
	my ($node, $opts) = @_;

	# this is a global modifier which can have any parent.
	# No need to check parent validity here.

	my %local_opts;
	my $substre; # subst regex

	if ( defined $opts ) {
		%local_opts = %$opts;
	}

	$substre = $node->att('pattern');
	# may be not well formed regex string if it contains non alphanum.
	# escape non alphanum in final regex.
	$substre =~ s/([^\w])/\\$1/g;
	$local_opts{'subst.re'} = $substre;

	my @list_by = split /,/, $node->att('for_each');
	foreach my $subst (@list_by)
	{
		debug_str 'subst re='.$substre.' by='.$subst."\n";
		$local_opts{'subst.by'} = "".$subst;

		xml_walk( $node, \%local_opts );
	}
}

sub xml_walk {
	my ( $parent, $opts ) = @_;

	my $parent_name = $parent->name;
	my %valid_parents = ();

	foreach my $node ( $parent->children )
	{
	 if ( $node->name eq 'group' )
		 {
		%valid_parents = (module => '' );
		die 'group can only be child of module' if not exists $valid_parents{$parent->name};

		xml_walk( $node, $opts );
	 }
	 elsif ( $node->name eq 'action' )
	 {
		%valid_parents = (module => '' , group => '' );
		die 'action can only be child of group or module' if not exists $valid_parents{$parent->name};

		debug_str $node->name." ".$node->att('path')."\n" if $node->att('path');
				dth_struct_record($node);

        xml_walk( $node, $opts );
     }
     elsif ( $node->name eq 'argument' )
     {
        %valid_parents = (action => '', group => '', module => '', subst => '', argument => '' );
        die "argument can only be child of module, group, subst or action" if not exists $valid_parents{$parent->name};

		debug_str $node->name." ".$node->att('path')."\n" if $node->att('path');
				dth_struct_record($node, $opts);
        xml_walk( $node, $opts );
     }
     elsif ( $node->name eq 'subst' )
     {
        xml_subst( $node, $opts );
     }
  }
}

sub xml_parse_module {
	my ($xml, $xml_node, $output) = @_;

	my $module = $xml_node->att('name');
	if ( $module eq $arg_module )
	{
		# good node. call the user specified sub on it
		debug_str "found xml node for module $module\n";

		# prefix to use for symbols
		if ( $xml_node->att('symbol-prefix') )
		{
			$symbol_prefix = $xml_node->att('symbol-prefix');
			debug_str "Prefix for symbols: $symbol_prefix\n";
		}

		xml_walk($xml_node);

		# build string to replace token {@dth_struct} occurrences and give a number to the literals id, if there is some.
		my $next_nbr = 0;
		my %yet_declared_ids;

		debug_str "\nAssigning a unique identifier to any DTH element with default identifier\n\n";

		my %interface_overrides;

		foreach my $dth_elt (@list_dth)
		{
			my $alias = $dth_elt->{'alias'};

			debug_str "name=$alias, id=".(exists $dth_elt->{'id'} ? $dth_elt->{'id'} : "(null)")."\n";

			# if the element has no id yet then must decide one to assigned to it.
			if ( !(exists $dth_elt->{'id'}) )
			{
				# see if this alias has already an id assigned
				my $defined_id = $dth_element_ids{$alias}{'value'};

				if ( defined $defined_id )
				{
					# if yes, use this id for next occurrences
					$dth_elt->{'id'} = $defined_id;
				}
				else
				{
					# else find the closest id not assigned yet and assign it to the alias
					my $elt_index;
					for ($elt_index = 0; $elt_index < $#list_dth; $elt_index++)
					{
						my $this_id = $list_dth[$elt_index]->{'id'};
						if ( (defined $this_id) and (isuint $this_id) and ($this_id == $next_nbr) )
						{
							# candidate id is already assigned.
							# redo the test with next potential id
							$next_nbr++;
							$elt_index = 0;
						}
					}

					# here $next_nbr is closest id available
					$dth_elt->{'id'} = $next_nbr;
					$next_nbr++;
				}

				set_symbol(\%dth_element_ids, { code => $alias, value => $dth_elt->{'id'} } );
			}

			if ( $dth_elt->{'path'} eq "/DTH/Debug/".parse_token("dthroot")."/TraceLevel" )
			{
				# debug handler is overriden.
				$interface_overrides{'debug'} = $dth_elt;
			}

			# add the structure element definition
			$hash_dth_defs{'dth_struct'} .= dth_element_str($dth_elt);

# DEBUG code
#			print "[\n";
#			foreach my $prop ( keys %{$dth_elt}) {
#				print "\t$prop = " . (%{$dth_elt})->{$prop} . ",\n";
#			}
#			print " ],\n";
# END DEBUG code
		}

		# add default debug DTH entry if not overriden
		if ( !exists $interface_overrides{'debug'} )
		{
			my %debug_elt;
			$debug_elt{'path'} = "/DTH/Debug/".parse_token('dthroot')."/TraceLevel";
			$debug_elt{'id'} = 0;
			$debug_elt{'alias'} = 0;
			$debug_elt{'type'} = 'DTH_TYPE_U32';
			$debug_elt{'info'} = 'Label box:Debug '.parse_token('module').'\nDynamic Control:TRUE';
			$debug_elt{'enum'} = ['SYSTEM', 'ALERT', 'CRITICAL', 'ERROR', 'WARNING', 'NOTICE', 'INFO', 'DEBUG'];
			$debug_elt{'get'} = parse_token('prefix')."get_debug";
			$debug_elt{'set'} = parse_token('prefix')."set_debug";

			$hash_dth_defs{'dth_struct'} .= dth_element_str (\%debug_elt);
		}


		# build the string for source code that will replace token {@dth_ids}
		$format_c_code_ex_prefs{'column'} = -64;
		$format_c_code_ex_prefs{'comment_pos'} = "vert";

		my @sorted = sort {
			my $va;
			my $vb;
			$va = $dth_element_ids{$a}{'value'};
			$vb = $dth_element_ids{$b}{'value'};
			if ( isuint $va and isuint $vb ) {
				return $va <=> $vb;
			} else {
				# macros should always be lasts else generated code may not compile due to precedence
				return "".$va cmp "".$vb;
			}
		}
		keys %dth_element_ids;

		foreach (@sorted)
		{
			$dth_element_ids{$_}{'code'} = "#define ".$dth_element_ids{$_}{'code'};

			$hash_dth_defs{'dth_ids'} .= format_c_code_ex ($dth_element_ids{$_});

			if ( $dth_element_ids{$_}{'callers'} > 1 ) {
				print "warning: symbol $_ used $dth_element_ids{$_}{'callers'} times. Was it your intent?\n";
			}
		}

		$hash_dth_defs{'dth_ids'} .= format_c_code_ex ({
			code => "#define ". $dthroot ."_ELEMENT_COUNT",
			value => $next_nbr,
		});

		foreach (keys %dth_element_paths)
		{
	        $hash_dth_defs{'dth_paths'} .= format_c_code_ex ($dth_element_paths{$_});
		}

		%format_c_code_ex_prefs = ();
	}

	$xml->purge;
	return;
}

# processes a RAD token and return the resulting string.
# @param RAD token formated as {@token}
# @return the processed result string or a generic message if token specified is not recognized.
sub parse_token {

	my ($token) = @_;

	# get token named without the enclosing chars
	$token =~ s/\{@([0-9a-zA-Z_]+)\}/$1/;

	my $text = "/* warning: no content for keyworld '$token' */";

	# if token is 'dth_ids' then replace token by the definition of the DTH elements indexes and names
	if ( $token eq 'dth_ids' ) {
		$text = $hash_dth_defs{'dth_ids'} if exists $hash_dth_defs{'dth_ids'};
	}

	# if token is 'dth_paths' then replace token by the definition of the DTH elements paths
 	if ( $token eq 'dth_paths' ) {
 		$text = $hash_dth_defs{'dth_paths'} if exists $hash_dth_defs{'dth_paths'};
	}

	# if token is 'dth_enums' then replace token by the definition of the multi value elements
	if ( $token eq 'dth_enums' ) {
		$text = $hash_dth_defs{'dth_enums'} if exists $hash_dth_defs{'dth_enums'};
	}

	# if token is 'dth_struct' then replace token by the module DTH structure definition
	elsif ( $token eq 'dth_struct' ) {
		$text = $hash_dth_defs{'dth_struct'} if exists $hash_dth_defs{'dth_struct'};
	}

	# if token is 'today' then insert current date and time in place of token
	elsif ( $token eq 'today' ) {
		use POSIX qw(strftime);
		$text = strftime "%a %b %e, %Y %H:%M:%S", localtime;
	}

	# author is TAT RAD
	elsif ( $token eq 'author' ) {
		$text = "$appname version $appversion";
	}

	# if token is 'filename' then replace token by fourth argument of script (don't change the case)
	elsif ( $token eq 'filename' ) {
		$text = $output_filename;
	}

	# if token is 'FILENAME' then replace token by fourth argument of script put to upper-case.
	elsif ( $token eq 'FILENAME' ) {
		$text = uc $output_filename;
	}

	# if token is 'input' then replace token by the provided input pathname
	elsif ( $token eq 'input' ) {
		$text = $arg_file;
	}

	# if token is 'module' then replace with provided argument.
	elsif ( $token eq 'module' ) {
		$text = lc $arg_module;
	}

	# token 'MODULE' same as 'module' but in uppercase.
	elsif ( $token eq 'MODULE' ) {
		$text = uc $arg_module;
	}

	# if token is 'prefix' then replace with provided argument.
	elsif ( $token eq 'prefix' ) {
		$text = $opt_prefix;
		$text .= '_' if ( $text ) ;
	}

	# if token is 'dthroot' then replace by the internal value.
	elsif ( $token eq 'dthroot' ) {
		if ( $dthroot ) {
			$text = $dthroot;
		} else {
			$text = $arg_module;
		}
	}

	# if token is 'header' then replace with provided argument 'module main header'.
	elsif ( $token eq 'headers' ) {

		# can have 0, 1 or more headers files
		$text = "";
		for my $header_file (split / +/, $opt_header)
		{
			debug_str "#include \"$header_file\"\n";
			$text .= "#include \"$header_file\"\n";
		}
	}

	# if token is 'version' then replace with the module version.
	elsif ( $token eq 'version' ) {
		$text = format_c_qq($opt_mod_version);
	}
	elsif ( $token eq 'version_len' ) {
		$text = length $opt_mod_version;
	}

	else {
		# this error case is already handled on initialization
	}

	return $text;
}

sub rad_file {
	my ($in, $out) = @_;

	$out =~ /\/?(\w+).\w*$/;
	$output_filename = $1;
	debug_str "output_filename = $output_filename\n";

	# create an empty C file named as <basename>.<ext> where <ext> is the file extension of the template file processed.
	print "processing input file $in as output file $out...\n";
	open ( my $tpl, '<'.$in ) or die "Cannot open template file $!\n";
	open ( my $src, '>'.$out ) or die "Cannot open output file $! for writting.\n";

	while (<$tpl>)
	{
		# replace all occurrences of RAD tokens in string
		# RAD tokens match mattern {@token}
		my $line = $_;
		while ( $line =~ /(\{@[0-9a-zA-Z_]+\})/ )
		{
			my $token = $1;

			# for debug
			#print "RAD token $1 found in the string $line";

			my $repl = parse_token($token);
			$line =~ s/$token/$repl/;
		}

		print $src $line;
	}
}

sub rtrim {
	my ($string) = @_;
	$string =~ s/\s+$//;
	return $string;
}

sub rad_dth {

	# trim passed module version string.
	# It is vital to do this because there can be unsollicited spaces or tabulations before and after the version string.
	# the result of such version strings is dramatically awful.
	$opt_mod_version = rtrim $opt_mod_version;

	my ($file) = @_;

	# these are resource files for the program
	my @templates = ( "$opt_resource/tatrad_dth.h", "$opt_resource/tatrad_dth.c" );

	debug_str "open XML file $file\n";

	# the XML input file must exist and be readable
	if ( $file and (-r ($file)))
	{
		# load XML for the specified module only
		my $xml = new XML::Twig ( Twig_handlers => { 'module' => \&xml_parse_module } );
		$xml->parsefile($file);

		# get output files to be generated from rad source code templates
		my @ofiles = split / /, $opt_output;

		# should be as many outputs as templates
		die "error: -output option expects ".($#templates + 1)." pathnames to be passed!" if ( $#templates <=> $#ofiles );

		my $findex = 0;
		foreach (@templates)
		{
			rad_file($_, $ofiles[$findex]);
			$findex ++;
		}
	}
	else
	{
		# if the XML file is not accessible then the program fails
		print "Cannot read $file\n";
	}
}

