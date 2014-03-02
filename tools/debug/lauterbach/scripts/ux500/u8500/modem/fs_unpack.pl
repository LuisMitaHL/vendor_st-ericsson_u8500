#!/usr/bin/perl

#
# Unpacks a dynamic file system file by reading the internal table of contents (TOC) and extracting the
# bytes pointed to in the TOC to the original filenames stored in the TOC
#
# Command line parameters
# $1 = File system file (default: modem.flash_image.fs)
# $2 = output directory (default: ./unpacked_fs)
#
# Developer Notes
#  the fs toc record strings are in the following format (after octal dumping in hex)
#
#	 |-Addr-| |-Len--|
#	 000001c0 000000b9 00000000 00000000  >................<
#        00000000 65646f6d 2020206d 00766e65  >....modem   env.<
#                                                  |--name---|
#
#
#


use strict;
use warnings;
use vars qw($fs $outdir $read_bin $addr $len $name $ext $offset $IFILE $filename);

sub SaveFile
{
	my ($file, $start, $size) = @_;
	my $read_bytes;
	printf "Writing $file to $outdir/$file %dkb\n", ( ($size + 512) / 1024 );
	seek $IFILE, $start, 0;
	read $IFILE, $read_bytes, $size;
	open (OFIL, ">$outdir/$file") || die "Can't create $outdir/$file";
	binmode OFIL;
	print OFIL $read_bytes;
	close OFIL;
}


$fs = shift || "modem.flash_image.fs";
$outdir = shift || "./unpacked_fs";

print "Using: $0 $fs $outdir\n";


open $IFILE, '<', $fs or die  "$0 : failed to open  input file '$fs' : $!\n";
binmode $IFILE;

$offset = 0;
while ( $offset < 1024) {
	seek $IFILE, $offset, 0;
	read $IFILE, $read_bin, 1024 - $offset;
	($addr, $len, undef, undef, undef, $name, $ext) = unpack('V5a8a3', $read_bin);
	last if ( $addr == 0xffffffff ); # address 0xfffffffff denotes end of TOC
	$name =~ s/ //g;
	$ext =~ s/ //g;
	$filename = "${name}.${ext}";
	die "ERROR: ${fs} is not recognized as a supported file system file" if ( $filename !~ "[a-zA-Z0-9_\-]\.[a-zA-Z0-9_\-]" );
	SaveFile($filename, $addr, $len);
	$offset += 32;
};

close  $IFILE or warn "$0 : failed to close input file '$fs' : $!\n";
exit 0;

