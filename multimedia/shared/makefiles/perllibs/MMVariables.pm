package MMVariables;

# package
use strict;
use File::stat;

sub findMakefileValue{
	my ($platform, $type) = @_;
	my $variable = 'unknown';
	
	open (FILE,"make -f $ENV{MMROOT}/shared/makefiles/perllibs/GetFromMakefile PLATFORM=$platform p-$type|");
	while (my $line = <FILE>)
	{
		if ($line =~ /$type=([^;\s']+)/)
		{
			$variable = $1;
		}
	}
	close FILE;
	return($variable); 
}

sub getBIN_DIR
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'BIN_DIR'));
}

sub getCORE_FLAG
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'CORE_FLAG'));
}

sub getMMDSPPROCESSOR
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMDSPPROCESSOR'));
}

sub getMMPROCESSOR
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMPROCESSOR'));
}

sub getMMBOARD
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMBOARD'));
}

sub getMMSYSTEM
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMSYSTEM'));
}

sub getMMCOMPILER
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMCOMPILER'));
}

sub getMMPLATFORM
{
	my ($platform) = @_;
	return(&findMakefileValue($platform,'MMPLATFORM'));
}

sub getENVSETUP
{
	my $platform = shift;
	return(&findMakefileValue($platform,'ENVSETUP'));
}

sub getMCPROJECT
{
	my $platform = shift;
	return(&findMakefileValue($platform,'MCPROJECT'));
}

sub displayInfo {
	my ($platform,$core) = @_;
     if(defined($core)) {
         $core = "CORE=" . $core;
     }
     else {
         $core = "";
     }
    print "---------------------------------\n";
    print "PLATFORM=$platform $core\n";
    print "---------------------------------\n";

    my $cmd = "make -f $ENV{MMROOT}/shared/makefiles/perllibs/GetFromMakefile $core PLATFORM=$platform ";
    $cmd .= "p-MEDIUM ";
    $cmd .= "p-CORE_NUMBER ";
    $cmd .= "p-CORE_FLAG ";
    $cmd .= "p-MMDSPCORE ";
    $cmd .= "p-MMDSPSVAEXT ";
    $cmd .= "p-MMDSPSIAEXT ";
    $cmd .= "p-MMDSPEXT ";
    $cmd .= "p-MMDSPPROCESSOR ";
    $cmd .= "p-MMPROCESSOR ";
    $cmd .= "p-MMCOMPILER ";
    $cmd .= "p-MMBOARD ";
    $cmd .= "p-MMSYSTEM ";
    $cmd .= "p-MMPLATFORM ";
    $cmd .= "p-NMF_ENV ";
    $cmd .= "p-NMF_CONF ";
    $cmd .= "p-NMF_CHIP ";
    $cmd .= "p-NMF ";
    $cmd .= "p-BASE_REPOSITORY ";
    $cmd .= "p-REPOSITORY ";
    $cmd .= "p-SHARED_REPOSITORY ";
    $cmd .= "p-IMG_REPOSITORY ";
    $cmd .= "p-VIDEO_REPOSITORY ";
    $cmd .= "p-AUDIO_REPOSITORY ";
    $cmd .= "p-MODULE ";
    $cmd .= "p-OSI ";
    $cmd .= "p-ENVSETUP ";
    $cmd .= "p-MEDIUM_DIR ";
    $cmd .= "p-OBJ_DIR ";
    $cmd .= "p-DEP_DIR ";
    $cmd .= "p-BIN_DIR ";
    $cmd .= "p-LIB_DIR ";
    $cmd .= "p-INSTALL_HEADER_DIR ";
    $cmd .= "p-INSTALL_LIB_DIR ";
    $cmd .= "p-INSTALL_BIN_DIR ";
    $cmd .= "p-CPPFLAGS ";
    $cmd .= "p-CC ";
    $cmd .= "p-CXX ";
    $cmd .= "p-ASM ";
    $cmd .= "p-AR ";
    $cmd .= "p-LD ";
    $cmd .= "p-CFLAGS ";
    $cmd .= "p-ASMFLAGS ";
    $cmd .= "p-LDFLAGS ";
    $cmd .= "p-CXXFLAGS ";
    #$cmd .= "p- ";

    system $cmd;
    print "\n";
}

return 1;
