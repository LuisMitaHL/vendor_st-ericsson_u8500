#!/bin/env perl 

open(OUT, ">h264dec/mpc/algo/probes/gen_inst_dump.c");
open(OUT2, ">h264dec/mpc/algo/probes/gen_inst_dump.h");

$struct = 0;
$curline = 0;
$if = 0;
$cm = 0;

%types = (
	"t_ahb_address"  => "32",
	"t_ulong_value"  => "33",
	"t_long_value"   => "33",
	"t_ushort_value" => "16",
	"t_short_value"  => "16"
);

%types_struct = (
	"t_uint32"       => "32",
	"t_sint32"       => "32",
	"t_uint16"       => "16",
	"t_sint16"       => "16",
	"t_uint8"        => "8",
	"t_sint8"        => "8"
);

%types2 = %types;

%location = (
	"ts_t1xhv_vdc_h264_param_in"    => "h264dec/mpc/share/ndk20/t1xhv_vdc_h264.idt",
	"ts_t1xhv_vdc_h264_param_inout" => "h264dec/mpc/share/ndk20/t1xhv_vdc_h264.idt",
	"ts_t1xhv_vdc_h264_param_out"   => "h264dec/mpc/share/ndk20/t1xhv_vdc_h264.idt"
);

%def_struct = ();

%alias = (
#	"t_ahb_address" => "t_uint32"
);

%defines = (
#	"USE_ME_ST_ONLY"   => "1",
#	"DISABLE_HADAMARD" => "1"
);

my $level = 0;

my $itf = 1;

print OUT2 "#ifndef __GEN_INST_DUMP__\n";
print OUT2 "#define __GEN_INST_DUMP__\n";

print OUT2 "typedef struct{\n";
print OUT2 "	void* pstruct;\n";
print OUT2 "    int   size;\n";
print OUT2 "    int   num;\n";
print OUT2 "} t_desc_type;\n\n";

print OUT2 "typedef struct {\n";
print OUT2 "	int nb_types;\n";
print OUT2 "    t_desc_type types[200];\n";
print OUT2 "} t_desc_struct;\n\n";

print OUT "t_desc_struct desc_t_ahb_address[] = {0, {{NULL, 32, 0}}};\n";
print OUT "t_desc_struct desc_t_uint32[] = {0, {{NULL, 32, 0}}};\n";
print OUT "t_desc_struct desc_t_sint32[] = {0, {{NULL, 32, 0}}};\n";
print OUT "t_desc_struct desc_t_uint16[] = {0, {{NULL, 16, 0}}};\n";
print OUT "t_desc_struct desc_t_sint16[] = {0, {{NULL, 16, 0}}};\n";
print OUT "t_desc_struct desc_t_uint8[]  = {0, {{NULL, 8, 0}}};\n";
print OUT "t_desc_struct desc_t_sint8[]  = {0, {{NULL, 8, 0}}};\n\n";

sub parse_type_
{
	my $struct_name = $_[0];

	open(IN, "inc/hamac_interface.h");

	while(<IN>)
	{
		if (!$struct)
		{
			if (/^typedef struct {/)
			{
				$struct = 1;
				$curline = 0;
			}
			next;
		}

		if (/#if/)
		{
			#print "1 $_\n";
			$if = 1;
			next;
		}

		if (/#endif/)
		{
			#print "2 $_\n";
			$if = 0;
			next;
		}

		if (/^\S*\/\*.*\*\//)
		{
			print "A $_\n";
			next;
		}

		if (/^\S*\/\*/)
		{
			#print "B $_\n";
			$cm = 1;
			next;
		}

		if ($cm && /\*\//)
		{
			#print "C $_\n";
			$cm = 0;
			next;
		}

		if ($if || $cm)
		{
			next;
		}

		if (/^\s*(t_[^;]+);/)
		{
			$line[$curline++] = $1;
		}

		if (/^\s*}\s*(.*);/)
		{
			$struct = 0;
			if ($1 eq $struct_name)
			{
				my $i;

				print "Parsing struct $struct_name:\n";

				my $struct_def = "";
				my $struct_def2 = "t_desc_struct desc_$struct_name = {\n";

				my $add = 0;
				while($i < $curline)
				{
					#print "$line[$i]\n";
					if ($line[$i] =~ /^(\S*)\s+(\S*)\[(\d+)\]/)
					{
						# Tab
						$tab = $3;
						$type = $1;
						$field = $2;
					}
					elsif ($line[$i] =~ /^(\S*)\s+(\S*)/)
					{
						# Tab
						$tab = 0;
						$type = $1;
						$field = $2;
					}
					else
					{
						print "Syntax error\n";
						exit(-1);
					}
					#print "'$type $tab'\n";
					$size = $types{$type};

					if (!defined($size))
					{
						print "Unrecognized type $type\n\n";
						exit(-1);
					}

					if (($add % 4) && (($size eq "32") && ($size eq "33")))
					{
						print "Alignement error\n\n";
						exit(-1);
					}

					print "Adding member $field\n";

					#print "$size\n";
					if ($size eq "33")
					{
						my $i = $tab ? $tab : 1;
						#while ($i--)
						if (!$tab)
						{
							$struct_def = $struct_def."\t\t{ NULL, 33, $tab},\n";
							$add = $add + 4;
						}
						else
						{
							$struct_def = $struct_def."\t\t{ NULL, 33, $tab},\n";
							$add = $add + 4*$tab;
						}
					}
					else
					{
						my $i = $tab ? $tab : 1;
						while ($i--)
						{
							my $sep = $add % 4 ? "\\n" : "\\t";
							$add = $add + 2;
						}
						$struct_def = $struct_def."\t\t{ NULL, 16, $tab},\n";
						#else
						#{
						#	print OUT "for( i = 0; i < $tab; $i++)\n";
						#	print OUT "\tfprintf(fp, \"%02x\\t%02x$sep\",ptr[$add+0],ptr[$add+1]);\n";
						#	$add = $add + 2*$tab;
						#}
					}

					$i++;
				}

				if ($add % 16)
				{
					my $missing = 16 - ($add % 16);
					print "Not multiple of 16 bytes (128 bits), $missing bytes missing\n\n";
					exit(-1);
				}

				$struct_def = $struct_def2."\t$i,\n\t{\n".$struct_def."\t}\n}; /* size = $add */\n";

				print OUT "$struct_def\n";

				print OUT2 "extern t_desc_struct desc_$struct_name;\n";

				print "\n";
			}
		}
	}

	close(IN);
}

sub parse_type
{
	my $struct_name = $_[0];
	my $in = "make_crc_$level";
	my $curline;
	my @line;
	my $add = 0;
	my $found = 0;

	if (defined($alias{$struct_name}))
	{
		# To be completed
	}

	if (defined($def_struct{$struct_name}))
	{
		return;
	}

	$level++;

	if (!defined($location{$struct_name}))
	{
		printf("Cannot find file for $struct_name struct\n");
		exit(0);
	}
	open($in, $location{$struct_name});

	while(<$in>)
	{
		if (!$struct)
		{
			if (/^typedef struct {/)
			{
				$struct = 1;
				$curline = 0;
			}
			next;
		}

		# #if
		if (/#if/)
		{
			#print "1 $_\n";
			if ($if)
			{
				printf("Nested #if not supported\n");
				exit(0);
			}
			$if = 1;
			next;
		}

		# #endif
		if (/#endif/)
		{
			#print "2 $_\n";
			if (!$if)
			{
				printf("Nested #if not supported\n");
				exit(0);
			}
			$if = 0;
			next;
		}

		# 1 line comment 
		if (/^\S*\/\*.*\*\//)
		{
			print "A $_\n";
			next;
		}

		# start comment
		if (/^\S*\/\*/)
		{
			#print "B $_\n";
			if ($cm)
			{
				printf("Nested comment not allowed\n");
				exit(0);
			}
			$cm = 1;
			next;
		}

		# end comment
		if ($cm && /\*\//)
		{
			#print "C $_\n";
			if (!$cm)
			{
				printf("Nested comment not allowed\n");
				exit(0);
			}
			$cm = 0;
			next;
		}

		# skip if we are in a excluded section
		if ($if || $cm)
		{
			next;
		}

		# store member definition line
		if (/^\s*(t_[^;]+);/)
		{
			$line[$curline++] = $1;
		}

		# detect en of struct
		if (/^\s*}\s*(.*);/)
		{
			$struct = 0;
			print("$1 - $struct_name\n");
			if ($1 eq $struct_name)
			{
				my $i;

				$found = 1;
				
				print "Parsing struct $struct_name:\n";

				my $struct_def = "";
				my $struct_def2 = "t_desc_struct desc_$struct_name = {\n";

				my $type;
				my $tab;
				my $field;
				while($i < $curline)
				{
					#print "$line[$i]\n";
					if ($line[$i] =~ /^(\S*)\s+(\S*)\[(\d+)\]/)
					{
						# Array
						$tab = $3;
						$type = $1;
						$field = $2;
					}
					elsif ($line[$i] =~ /^(\S*)\s+(\S*)/)
					{
						# Value
						$tab = 0;
						$type = $1;
						$field = $2;
					}
					else
					{
						print "Syntax error\n";
						exit(-1);
					}
					
					my $struct = 0;

					# Check if type exists
					$size = $types2{$type};

					if (!defined($types2{$type}))
					{
						# if ($itf)
						# {
						# 	print "Unrecognized type $type\n\n";
						# 	exit(-1);
						# }

						if (defined($alias{$type}))
						{
							$type = $alias{$type};
						}

						if (defined($location{$type}))
						{
							$struct = 1;
							if (!defined($def_struct{$type}))
							{
								print "$type - $location{$type}\n";
								$size = parse_type($type);
							}
							# Quoi faire maintenant ?
						}
						elsif (!defined($types2{$type}))
						{
							$size = $types2{$type};
						}
						else
						{
							print "Unrecognized type $type\n\n";
							exit(-1);
						}
					}
					#print "'$type $tab'\n";

					# Check alignment of member
					if (($add % 4) && ($size >= 32))
					{
						print "Alignement error\n\n";
						exit(-1);
					}

					print "Adding member $field\n";

					#print "$size\n";

					# Generate code for member
					if ($struct)
					{
							$struct_def = $struct_def."\t\t{ &desc_$type, 0, $tab},\n";
							$add = $add + $size;
					}
					else
					{
						if (($size eq "32") || ($size eq "33"))
						{
							my $i = $tab ? $tab : 1;
							$struct_def = $struct_def."\t\t{ NULL, $size, $tab},\n";
							$add = $add + 4*$i;
						}
						elsif ($size eq "16")
						{
							my $i = $tab ? $tab : 1;
							$struct_def = $struct_def."\t\t{ NULL, 16, $tab},\n";
							$add = $add + 2*$i;
						}
						elsif ($size eq "8")
						{
							my $i = $tab ? $tab : 1;
							$struct_def = $struct_def."\t\t{ NULL, 8, $tab},\n";
							$add = $add + 2*$i;
						}
						else
						{
							print("Unknown size $size\n");
							exit(0);
						}
					}

					$i++;
				}

				if ($itf && ($add % 16))
				{
					my $missing = 16 - ($add % 16);
					print "Not multiple of 16 bytes (128 bits), $missing bytes missing\n\n";
					exit(-1);
				}

				$struct_def = $struct_def2."\t$i,\n\t{\n".$struct_def."\t}\n}; /* size = $add */\n";

				print OUT "$struct_def\n";

				print OUT2 "extern t_desc_struct desc_$struct_name;\n";

				print "Finish $struct_name\n";

				last;
			}
		}
	}

	close($in);
	
	$level--;

	if (!$found)
	{
		printf("Cannot find struct $struct_name\n");
		exit(0);
	}
	
	print "size $struct_name $add\n";

	if (!$level)
	{
		print "\n";
	}
	
	return $add;
}

#print "\n";

parse_type("ts_t1xhv_vdc_h264_param_in");
parse_type("ts_t1xhv_vdc_h264_param_inout");
parse_type("ts_t1xhv_vdc_h264_param_out");

$itf = 0;
%types2 = %types_struct;
#parse_type("mvp8815_desc");

print OUT2 "#endif // __GEN_INST_DUMP__\n";

close(OUT2);
close(OUT);

