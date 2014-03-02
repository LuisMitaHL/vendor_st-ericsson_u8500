
if ($#ARGV < 1)
{
	die "Usage update_interface\n";
}

#print "Args @ARGV\n";

$source = shift @ARGV;
$dest = shift @ARGV;

$tmp_file_in = "tmp_file_in";
$tmp_file_out = "tmp_file_out";

$error = 0;

if (! -e $source) {
	print "Source file $source not found\n";
	exit(1);
}

if (! -e $dest) {
	#print "Destination file $dest not found\n";
	#exit(1);
	print "$source\n";
	if ($source =~ /host_interface_common\.h/)
	{
		print "Take $source as a start for $dest\n";
		system("cp $source $dest");
		exit(0);
	}
	else
	{
		print "t1xhv_host_interface.h does not exist\n";
		exit(1);
		#system("touch $dest");
	}
}

@structs_list = ();

open IN, "$source";
while(<IN>)
{
	if (/typedef\s+struct\s+([^\s]+)\s*{/)
	{
		push @structs_list, $1;
	}
}
close IN;

#print "@structs_list\n";

open SRC, "$source";
#open DST, "$dest";
#open UPD, ">$tmp_file";
#unlink("~/toto");
system("cp $dest $tmp_file_in");

#while ($struct_name = shift @ARGV)
while ($struct_name = shift @structs_list)
{
	#$struct = $_;

	$found_in_dest = 0;
	$struct = 0;
	$last_line = "";

	#print "Processing struct $struct_name\n";

	open DST, "$tmp_file_in";
	open UPD, ">$tmp_file_out";
	while(<DST>)
	{
		$line = $_;
		if ($struct_name eq "t1xhv_dpl_param_out") {
			#print "$found_in_dest $line";
		}
		if ($line =~ /#endif \/\* __T1XHV_NMF_ENV \*\//)
		{
			#print "last $line";
			$last_line = $line;
			last;
		}

		if (!$struct)
		{
			#print "$line\n";
			if ($line =~ /typedef\s+struct\s+$struct_name\s*{*\s*\n$/)
			{
				$struct = 1;
				$found_in_dest = 1;
				#print "Found struct in dst file\n";
			}
			else
			{
				print UPD $line;
			}
		}
		else
		{
			if (/^\s*}\s*ts_$struct_name/)
			{
				#print "Found end\n";
				$struct = 0;
				last;
			}
		}
	}

	if ($struct)
	{
		system("@rm -f $tmp_file_in $tmp_file_out");
		die "Could not find end delimiter of $struct_name in file to update\n";
	}

	if (!$found_in_dest)
	{
		#system("@rm -f $tmp_file");
		#die "Could not find $struct_name in file to update\n";
	}

	$found = 0;

	while(<SRC>)
	{
		$line = $_;
		#print "$line\n";
		if (!$struct)
		{
			if ($line =~ /typedef\s+struct\s+$struct_name/)
			{
				$struct = 1;
				$found = 1;
				print UPD $line;
				#print "Found\n";
				#die toto;
			}
		}
		else
		{
			print UPD $line;
			if (/^\s*}\s*ts_$struct_name/)
			{
				$struct = 0;
				last;
			}
		}
	}

	if ($struct)
	{
		system("@rm -f $tmp_file_in $tmp_file_out");
		die "Could not find end delimiter of $struct_name in source file\n";
	}

	if (!$found)
	{
		system("@rm -f $tmp_file_in $tmp_file_out");
		die "Could not find $struct_name in source file\n";
	}

	if (!$found_in_dest)
	{
		print "Added struct $struct_name\n";
		print UPD "\n";
	}
	else
	{
		print "Updated struct $struct_name\n";
	}

	if ($last_line ne "")
	{
		print UPD $last_line;
	}
	while (<DST>)
	{
		print UPD $_;
	}
	close UPD;
	close DST;
	system("mv -f $tmp_file_out $tmp_file_in");
}

#while (<DST>)
#{
#	print UPD $_;
#}

#close UPD;
#close DST;
close SRC;

#print "mv -f $tmp_file $dest";
system("cmp $tmp_file_in $dest");
if ( $? != 0 ) {
	system("mv -f $tmp_file_in $dest");
	print "File updated successfully\n";
} else {
	print "File updated skipped (no difference)\n";
}
system("rm -f $tmp_file_in $tmp_file_out");

