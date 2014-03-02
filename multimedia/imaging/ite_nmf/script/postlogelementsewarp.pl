#!/bin/perl

if (@ARGV < 3) {
	print ("too few arguments\n");
	print ("Use: postLogElementsEWarp.pl <elementsDefinitionHeader.h> <convertingLogFile> <outputLogFile>\n");
	print ("Does support logs from NMF and from OMX (camera trace)\n");
	exit;
}

$elementsFile = $ARGV[0];
open(elementsFile) || die "Can't open $ARGV[0] header file. Reason: $!";
@elementsDef = <elementsFile>;
close(elementsFile);

foreach (@elementsDef) {
	if (m/#define/) {
		($defineTag, $elementName, $elementHash) = (split());
		$elementHash = hex $elementHash;
		@definitionsHash[$elementHash] = $elementName;
#		print($elementHash,"\n");
#		print(@definitionsHash[$elementHash],"\n");
	}
}

$logInFile = $ARGV[1];
open(logInFile) || die "Can't open $ARGV[1] log file. Reason: $!";
@logOriginal = <logInFile>;
close(logInFile);

open(logOutFile, ">$ARGV[2]");

foreach (@logOriginal) {
  if (m/at @/) {	# NMF test log
    ($a,$b) = (split("at @"));
    ($c,$d,$e) = (split(" ",$b));
    $_ =~ s/$c/@definitionsHash[hex $c]/  if (defined @definitionsHash[hex $c]); #replace only when defined
#    print();
  }
  if (m/Write PE:/) {	# OMX test log
    ($a,$b) = (split("PE:"));
    ($c,$d,$e) = (split("[ |:]",$b));
    $_ =~ s/$c/@definitionsHash[hex $c]/  if (defined @definitionsHash[hex $c]);
#    print();
  }
	print logOutFile;
}

close(logOutFile);
