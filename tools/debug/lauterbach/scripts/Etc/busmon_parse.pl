# busmon_parse.pl
#
# Translates busmon logs from logic analyzer to plain text.
#
# Usage:    perl busmon_parse.pl <infile> <-html> <-reg>
# Example:  perl busmon_parse.pl inFile.txt 
#           This will put the result in inFile_parsed.txt. Registry writes will not be seen. 
#
#           perl busmon_parse.pl inFile.txt -reg
#           This will put the result in inFile_parsed.txt. Registry writes will be seen.
#
#           perl busmon_parse.pl inFile.txt -reg
#           This will generate the same output as with the -reg flag. A html version will also be created.
#  
# TODO:  Keep current detail filename as a global.
#        Support for writing to other files (connect logpoint to file)
#        Standard matlab format for printing. (ecskrem style)
#        Speed opt.
#        Info prints (ex "Matlab output enabled, HTML output enabled")
#        Improve help prints
#        Comments and structure
#        
#        
# EHEISTE, 20060215
# QROSAND, 20071015

#import data
use busmon_data;
use busmon_config;
use Data::Dumper;
use Cwd;

my $LINES_PER_FILE = 100000;
my $ver = "v4.00_AnjaCamillaKajsa_Asic";

#                        sample       Inf         overflow    Data        Timestamp
#Adjust for B365/Kajsa	
my $AsicRegExpU365  = "^(\\s*)(\\d*)(\\s*)(\\S*)(\\s*)(\\S*)(\\s*)(\\w\{1\})(\\w\{8\})(\\s*)(\\S*)";
#Adjust for B330/Camilla	
my $AsicRegExpU330  = "^(\\s*)(\\d*)(\\s*)(\\S*)(\\s*)(\\S*)(\\s*)(\\w\{5\})(\\w\{4\})(\\s*)(\\S*)";
my $MainRegExp      = "^(\\s*)(\\d*)(\\s*)(\\S*)(\\s*)(\\S*)(\\s*)(\\S*)(\\s*)(\\S*) (\\S*)";
my $TimeStampTrimRegExp = "(,|<)";
my $TimeStampTrimRegExpPre = "(,|<|\s|s)";
my $doReg = 0;
my $doHtml = 0;
my $doLoad = "";
my $tagCounter = 0;

my $doU330 = 0;
my $doU365 = 0;
my %doWithAudio = 0;

my $htmlLineCount = 0;
my $htmlFileCount = 0;

my $inFile = @ARGV[0];
my $input = join(',',@ARGV);

my $targetDir;
my $targetDirHTML;
my $HTMLCatName = "HTML";
my $outFileName;
my $outFileHtmlOverview;
my $outFileHtmlDetail;
my $outFileHtmlMain;
my $outFileLoadMeas;
my $outFileLoadMeasExcel;

#some script state variables
my $RawData = "";
my $prevLine = "";
my $address;
my $data;
my $dataHex;
my $interface_id;
my $prevTimeStamp = -1;
my $timeWrapCounter = 0;
my $irqVector;
my $irqVectorHex;

my $MNTR_IO_BUS;
my $MNTR_SW_DEBUG;
my $MNTR_TIMEGEN;
my $MNTR_INTCON;

#Adjust for B365/Kajsa	
my $MNTR_IO_BUS_U365   = 17;
my $MNTR_SW_DEBUG_U365 = 19;
my $MNTR_INTCON_U365   = 13;

#Adjust for B330/Camilla	
my $MNTR_IO_BUS_U330   = 13;
my $MNTR_SW_DEBUG_U330 = 0;  #TODO findout
my $MNTR_INTCON_U330   = 0;  #TODO findout

my %State = InitStateListener();
my %AddressList = InitRegListener($ARGV[0]);

#some constants
my %r_w = (
	   0 => "=>",
	   1 => "<=",
	  );

my  %h_color = (
		RED => "FF0000",
		BLUE => "0000FF",
		BLACK => "000000",
		PINK => "FF00FF",
		GREEN => "00FF00",
		BROWN => "A52A2A",
		VIOLET => "9900CC",
		DARK_VIOLET => "660033",
		DARK_GREEN => "00CC33",
		LIGHT_GREEN => "00FF66",
		ORANGE => "FF9900",
	       );

#Will be filled in by GenerateCSSDefinitions
my %h_color_css = ();
my $CSSDefinition = "";

#*************************************************************
#*************************************************************
#                               MAIN                         *
#*************************************************************
#*************************************************************

# Parse commandline options
ParseCommandLineOptions();

# Open infile/outfile
OpenFiles();

#InitLoadMeassurement
InitLoadMeassurment($doLoad,$outFileLoadMeasExcel);

#iterate through INFILE (The actual parsing.)
ParseInfile();

CalculateLoadPerformance();

#Close filehandles
CloseFiles();

#*************************************************************
#*************************************************************
#                         SUB FUNCTIONS                      *
#*************************************************************
#*************************************************************

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                     TOP LEVEL PRINTING SUBS                +
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------------------
# PrintRegData
# 
# Print function for generic register write.
#-------------------------------------------------------------
sub PrintRegData 
  {
    (my $addr,my $dat,my $read_write, my $time) = @_;

    my $arrow  = $r_w{$read_write};

    $tmp_data = sprintf("0x%04X",$dat);

    if (exists $hw_register16{"a$addr"}) 
      {
	$tmp_addr = "a$addr";
	$tmp_addr = $hw_register16{"$tmp_addr"};
	$printStr = sprintf("%*s: %s\t%s\t%s\t (%s)",20,$time,$tmp_data,$arrow,$addr,$tmp_addr);
      }
    else 
      {
	$printStr = sprintf("%*s: %s\t%s\t%s\t",20,$time,$tmp_data,$arrow,$addr);
      }
    print OUTFILE "$printStr\n";
    if ($doHtml) 
      {
	printHTML($printStr,"YES","NO","BLACK");
      }
  }
  
#-------------------------------------------------------------
# PrintRawData
# 
# Print function for generic raw data.
#-------------------------------------------------------------
sub PrintRawData 
{
  (my $RawData) = @_;
  my $printStr = $RawData;
  
  print OUTFILE "$printStr";
  if ($doHtml) 
  {
    printHTML($printStr,"YES","NO","BLACK");
  }
}  
  
#-------------------------------------------------------------
# printHTML
#
# In:
# Out:
# Globals: 
#
# Info: Prints string to html file.
#-------------------------------------------------------------
sub printHTML 
  {
    #TODO: Make a vector with tags and loop through them when printing.
    ($strToPrint,$onlyDetail,$make_tag,$color) = @_;
    
    my $tag_start = " $h_color_css{$color}";
    my $tag_end = "</a>\n";
    my $link_start = " $h_color_css{$color}";;
    my $link_end = "</a>\n";
    if ($make_tag eq "YES") 
      {
	$tag_start = sprintf("%s name=\"\#T$tagCounter\"",$tag_start);
	$file_name = sprintf("%s.html",$htmlFileCount);
	$link_start = sprintf("%s target=\"d\" href=\"$fileName\#T$tagCounter\"",$link_start);
	$tagCounter = $tagCounter + 1;
      }

    if ($onlyDetail eq "NO") 
      {
	$strToPrintDetail  = $strToPrint;
	$strToPrintDetail =~ s/\"(.*)\"//g;
	print OUTFILE_HTML_OVERVIEW "<a$link_start>$strToPrintDetail$link_end";
      }
    
    $strToPrint =~ s/\"//g;
    print OUTFILE_HTML_DETAIL "<a$tag_start>$strToPrint$tag_end";
    $htmlLineCount++;
    
    if ($htmlLineCount >= $LINES_PER_FILE) 
    {
      $htmlLineCount = 0;
      $htmlFileCount++;
      writeTailDetailHTML();
      close(OUTFILE_HTML_DETAIL); 
      $fileName = sprintf("%s.html",$htmlFileCount);
      open(OUTFILE_HTML_DETAIL,">$targetDirHTML\\$fileName") or die "Can't open file $fileName: $!";
      print "Creating new file: $fileName\n";
      writeHeaderDetailHTML();
    }
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                     PRINTING HELPER SUBS                   +
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------------------
# createMainHTML
#
# In:
# Out:
# Globals: 
#
# Info: Generates main html file.
#-------------------------------------------------------------
sub createMainHTML 
  {
    $fileName = sprintf("%s.html",$htmlFileCount);
    print OUTFILE_HTML_MAIN "<html>\n";
    print OUTFILE_HTML_MAIN "<head>\n";
    print OUTFILE_HTML_MAIN "<title>Busmon parse v$ver : $inFile</title>";
    print OUTFILE_HTML_MAIN "</head>\n";
    print OUTFILE_HTML_MAIN "<FRAMESET COLS=\"45\%\,\*\">\n";
    print OUTFILE_HTML_MAIN "<frame name=\"o\" src=\"$HTMLCatName/$outFileHtmlOverview\">\n";
    print OUTFILE_HTML_MAIN "<frame name=\"d\" src=\"$HTMLCatName/$fileName\">\n";
    print OUTFILE_HTML_MAIN "</FRAMESET>\n";
    print OUTFILE_HTML_MAIN "</html>\n";
  }

#-------------------------------------------------------------
# writeHeaderDetailHTML
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub writeHeaderDetailHTML 
  {
    writeCSSDefinitions(\*OUTFILE_HTML_DETAIL);
    print OUTFILE_HTML_DETAIL "<PRE>\n";
  }
#-------------------------------------------------------------
# writeTailDetailHTML
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub writeTailDetailHTML 
  {
    print OUTFILE_HTML_DETAIL "</PRE>\n";
  }

#-------------------------------------------------------------
# writeHeaderOverviewHTML
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub  writeHeaderOverviewHTML
  {
    writeCSSDefinitions(\*OUTFILE_HTML_OVERVIEW);
    print OUTFILE_HTML_OVERVIEW "<PRE>\n";
  }

#-------------------------------------------------------------
# writeTailOverviewHTML
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub  writeTailOverviewHTML
  {
    print OUTFILE_HTML_OVERVIEW "</BODY>\n";
    print OUTFILE_HTML_OVERVIEW "</PRE>\n";
  }

#-------------------------------------------------------------
# writeCSSDefinitions
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub writeCSSDefinitions
  {
    ($OUTFILE) = @_;
    print $OUTFILE $CSSDefinition;
  }

#-------------------------------------------------------------
# GenerateCSSDefinitions
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub GenerateCSSDefinition
  {
    my $CSSClassCnt = 0;

    #TODO: Return this instead
    $CSSDefinition =  "<style type=\"text\/css\">\n";
    $CSSDefinition =  sprintf( "%sa \{text-decoration:none\}\n",$CSSDefinition);
    foreach $color (sort keys %h_color)
      {
	$CSSDefinition =  sprintf("%sa.%d \{color:\#%s\}\n",$CSSDefinition,$CSSClassCnt,$h_color{$color});

	#Don't waste space by specifying black font color
	if ($h_color{$color} eq "000000")
	  {
	    $h_color_css{$color} = "";
	  }
	else
	  {
	    $h_color_css{$color} = "class=\"$CSSClassCnt\"";
	  }
	$CSSClassCnt++;
      }
    $CSSDefinition =  sprintf("%s</style>\n",$CSSDefinition);
  }

#-------------------------------------------------------------
# OpenFiles
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub OpenFiles
  {
    open(INFILE,$inFile) or die "Can't open file $inFile: $!";
    
    #Create directories for target files
    $targetDir = $inFile;
    $targetDir =~ s/.txt$//;
    $targetDirHTML = "$targetDir\\$HTMLCatName";
    mkdir($targetDir);
    mkdir($targetDirHTML);

    #Create filename for outfile, add '_p' before dot (replace '.' with '_p.')
    $outFileName = $inFile;
    $outFileName =~ s/\./_p\./;
    #Open outfile for writing
    open(OUTFILE,">$targetDir\\$outFileName") or die "Can't open file $outFileName: $!";
    
    if ($doHtml == 1) 
      {
	#Format html file names
	$tmp = $outFileName;
	$tmp =~ s/.txt//;
	$outFileHtmlMain = sprintf("%s.html",$tmp);
	$outFileHtmlOverview = "o.html";
	$outFileHtmlDetail = sprintf("%s_d",$tmp);
	$outFileHtmlOverview =~ s/\s//g;
	$outFileHtmlDetail =~ s/\s//g;
	
	$outFileHtmlDetailNumbered = sprintf("%s.html",$htmlFileCount);
	open(OUTFILE_HTML_OVERVIEW,">$targetDirHTML\\$outFileHtmlOverview") or die "Can't open file $outFileHtmlOverview: $!";
	open(OUTFILE_HTML_DETAIL,">$targetDirHTML\\$outFileHtmlDetailNumbered") or die "Can't open file $outFileHtmlDetailNumbered: $!";
	open(OUTFILE_HTML_MAIN,">$targetDir\\$outFileHtmlMain") or die "Can't open file $outFileHtmlMain: $!";

	GenerateCSSDefinition();
	createMainHTML();
	writeHeaderOverviewHTML();
	writeHeaderDetailHTML();
      }
    if ($doLoad ne "")
      {

	$outFileLoadMeas = $inFile;
	$outFileLoadMeas =~ s/\.txt/_load_report.txt/;
	$outFileLoadMeasExcel = $outFileLoadMeas;
	$outFileLoadMeasExcel =~ s/\.txt/\.xls/;
	my $currWD = cwd;
	open(LOAD_MEAS,">$targetDir\\$outFileLoadMeas") or die "Could not open file $outFileLoadMeas: $!";
	$outFileLoadMeasExcel = "$currWD\\$targetDir\\$outFileLoadMeasExcel";
	$outFileLoadMeasExcel =~ s/\//\\/g;
	print "$outFileLoadMeasExcel\n";
	
      }
  }

#-------------------------------------------------------------
# CloseFiles
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub CloseFiles
  {
    close(INFILE);
    close(OUTFILE);
    
    if ($doHtml) 
      {
	writeTailOverviewHTML();
	writeTailDetailHTML();
	close(OUTFILE_HTML_OVERVIEW);
	close(OUTFILE_HTML_DETAIL);
      }
  }

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                     PARSER ENGINE SUBS                     +
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------------------
# AddAddressRange
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub AddAddressRange
  {
    (my $LogData, my $time) = @_;

    %{$LogData}->{'TimeStamp'} = $time;
    foreach $addr (keys %{$LogData->{'Address'}})
      {
	$AddressList{$addr} = $LogData;
      }
  }

#-------------------------------------------------------------
# DeleteAddressRange
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub DeleteAddressRange
  {
    (my $LogData, my $time) = @_;
    
    foreach $addr (keys %{$LogData->{'Address'}})
      {
	delete $AddressList{$addr};
      }

  }

#-------------------------------------------------------------
# StoreDataAndCheckIfFinished
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub StoreDataAndCheckIfFinished
  {
    (my $addr,my $dat,my $time) = @_;
    
    my $LogData = $AddressList{$addr};
    my $LastWrite = %{$LogData}->{'LastWrite'};
    my $retVal = 0;
    #Store data in correct hash
    %{$LogData}->{'Address'}->{$addr}->{'Data'} = $dat;
    %{$LogData}->{'Address'}->{$addr}->{'TimeStamp'} = $time;
   
    #Check if last address
    if ($address eq $LastWrite)
      {
	AddressToName($LogData);

	DeleteAddressRange($LogData);

	#Call Callback
	$Callback = %{$LogData}->{'Callback'};
	#TBD
	#$OtherFileHandle = 
	($printStr,$onlyDetail,$doTag,$c_color) = &$Callback($LogData);

	if ($printStr ne "") 
	  {
	    print OUTFILE "$printStr\n";
	    if ($doHtml) 
	      {
		printHTML($printStr,$onlyDetail,$doTag,$c_color);
	      }
	  }
      }

    if (exists %{$LogData}->{'PrintReg'})
      {
	$retVal = 1;
      }
   
    return $retVal;
   
  }

#-------------------------------------------------------------
# AddressToName
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub AddressToName
  {
    (my $LogData) = @_; 
    foreach $Address (keys %{$LogData->{Address}})
      {
	$Name = %{$LogData->{Address}}->{$Address}->{Name};
	$Data = %{$LogData->{Address}}->{$Address}->{Data};
	
	if ($Name =~ /___/)
	  #This is an array
	  {
	    $Name =~ /(\S*)___(\d*)/;
	    $Name = $1;
	    $Index = $2;
	    $ar = %{$LogData->{Data}}->{$Name};
	    @{$ar}->[$Index] = $Data;
	  }
	else
	  #This is a  scalar
	  {
	    %{$LogData->{Data}}->{$Name} = $Data;
	  }	
      }
  }

#-------------------------------------------------------------
# ParseInfile
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub ParseInfile
{
  #adjust for B365/Kajsa?	
  if ($doU365 == 1)
  {
    $AsicRegExp    = $AsicRegExpU365;
    $MNTR_IO_BUS   = $MNTR_IO_BUS_U365;
    $MNTR_SW_DEBUG = $MNTR_SW_DEBUG_U365;
    $MNTR_INTCON   = $MNTR_INTCON_U365;
  }
  #Adjust for B330/Camilla?	
  elsif ($doU330 == 1)
  {
    $AsicRegExp    = $AsicRegExpU330;
    $MNTR_IO_BUS   = $MNTR_IO_BUS_U330;
    $MNTR_SW_DEBUG = $MNTR_SW_DEBUG_U330;
    $MNTR_INTCON   = $MNTR_INTCON_U330;
  }

  while (<INFILE>)
  {
    #Process line
    $RawData = $_;
    /$AsicRegExp/;
    
    $interface_id = $4;
    if (length($4) > 2)
    {
      $interface_id = bin2dec($interface_id);
    }
    else
    {
      $interface_id = hex($interface_id);
    }

    if (($interface_id == $MNTR_IO_BUS)   ||
        ($interface_id == $MNTR_SW_DEBUG) ||
        ($interface_id == $MNTR_TIMEGEN)  ||
        ($interface_id == $MNTR_INTCON))
    {
      #################################################
      # Get timestamp and handle wraparound
      #################################################
      $time_stamp = $11;
      $time_stamp =~ s/(.*)://g;
      $time_stamp =~ s/<//g;
      
      $time_stamp =~ s/$TimeStampTrimRegExpPre//g;
      $time_stamp += $timeWrapCounter*10;
#      $time_stamp += $timeWrapCounter*60;

      $time_stamp = sprintf("%.12f",$time_stamp);
      
      if (($time_stamp - $prevTimeStamp) < 0)
      {
#printf("\nTime wrap, %.12f, %.12f", $time_stamp, $prevTimeStamp);
        $timeWrapCounter++;
        $time_stamp += 10;
#        $time_stamp += 60;
        $time_stamp = sprintf("%.12f",$time_stamp);
      }
      
      $prevTimeStamp = $time_stamp;
    }

    #################################################
    # Check if IO bus access
    #################################################
    if ($interface_id == $MNTR_IO_BUS)
    {
      #Adjust for B365/Kajsa?
      if ($doU365 == 1)
      {
        if ($doWithAudio == 1)
        {
          $address = sprintf("%.5X",(hex($9) & 0x0001FFF0));  #Always clear LSB bits
        }
        else
        {
          $address = sprintf("%.5X",(hex($9) & 0x0001FFFF));
        }

        $data = ((hex($8) & 0x1) << 15) + ((hex($9) & 0xFFFE0000) >> 17) ;
        $rw = (hex($8) & 0x2) >> 1;
      }
      #Adjust for B330/Camilla?       
      elsif ($doU330 == 1)
      {
        $address = sprintf("%.5X",(hex($8) & 0x0001FFFF));
        if ($doWithAudio == 1)
        {
          $data = hex($9) | 0xF;  #Always set LSB bits to 0xF
        }
        else
        {
          $data = hex($9);
        }
        $rw = (hex($8) & 0x20000) >> 17;
      }

      #printf "(%s) %s => %X\n","$8$9",$address,$data;

      $DoRegPrint = 1;
      
      #Check if state change
      if (exists $State{$address})
      {
        AddAddressRange($State{$address}->{$data},$time_stamp);

        $DoRegPrint = StoreDataAndCheckIfFinished($address,$data,$time_stamp);
        if ($DoRegPrint == 1)
        {
          PrintRegData ($address,$data,$rw,$time_stamp);
        }
      }
      elsif (exists $AddressList{'RegLogSpace'}->{$address})
      {
        AddAddressRange($AddressList{'RegLogSpace'}->{$address},$time_stamp);
        $DoRegPrint = StoreDataAndCheckIfFinished($address,$data,$time_stamp);
        if ($DoRegPrint == 1)
        {
          PrintRegData ($address,$data,$rw,$time_stamp);
        }
      }
      #Check if state/reg data is coming
      elsif (exists $AddressList{$address})
      {
        $DoRegPrint = StoreDataAndCheckIfFinished($address,$data,$time_stamp);
        if ($DoRegPrint == 1)
        {
          PrintRegData ($address,$data,$rw,$time_stamp);
        }
      }
      #Generic print
      else
      {
        PrintRegData ($address,$data,$rw,$time_stamp);
      }
    }

    #################################################
    # Check if Software Debug log
    #################################################
    elsif ($interface_id == $MNTR_SW_DEBUG)
    {
      #Adjust for B365/Kajsa?
      if ($doU365 == 1)
      {
        $data = hex($9);
      }
      #Adjust for B330/Camilla?       
      elsif ($doU330 == 1)
      {
        $data = (hex($9)<<16)+ (hex($8) & 0x0000FFFF);
      }

      $printStr = sprintf("%*s: Deadline mark %s",20,$time_stamp, $data);
      print OUTFILE "$printStr\n";
      if ($doHtml)
      {
        printHTML($printStr,"YES","NO","RED");
      }
#printf("\nSW debug, %s", $data);
    }

    #################################################
    # Check if Interrupt log
    #################################################
    elsif ($interface_id == $MNTR_INTCON)
    {
      #Adjust for B365/Kajsa?
      if ($doU365 == 1)
      {
        $address = hex($8) & 0x3;
        $dataHex = sprintf("%.8X",0xffffffff-hex($9));
        $data    = 0xffffffff-hex($9);

      }
      #Adjust for B330/Camilla?       
      elsif ($doU330 == 1)
      {
        $address = (hex($8) & 0x30000) >> 16;
        $dataHex = sprintf("%.8X",0xffffffff-((hex($9)<<16)+ (hex($8) & 0x0000FFFF)));
        $data    = 0xffffffff-((hex($9)<<16)+ (hex($8) & 0x0000FFFF));
      }
      
      #check if start of log
      if ($address == "0")
      {
        $irqVectorHex = sprintf("%-8s",$dataHex);
        $irqVector    = $data;
      }
      elsif ($address == "1")
      {
        $irqVectorHex = sprintf("%s%-8s",$irqVectorHex,$dataHex);
      }
      elsif ($address == "2")
      {
        $irqVectorHex = sprintf("%s%-8s",$irqVectorHex,$dataHex);
        
        print OUTFILE "$printStr\n";
        if ($doHtml)
        {
          printHTML($printStr,"YES","NO","RED");
        }
      }
    }

    #################################################
    # Other...
    #################################################
    else
    {
      PrintRawData ($RawData);
    }
  }
}

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#                     COMMAND LINE OPTIONS PROC SUBS         +
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------------------
# PrintBusmonHelp
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub PrintBusmonHelp
  {
  printf "\n\n%15s perl busmon_parse.pl <infile> <-U330|U365> <-html> <-reg> <-load(ALL|BCH|DCH)>\n\n","Usage:";
  printf "%15s perl busmon_parse.pl inFile.txt -U330\n","Example:";
  printf "%15s This will put the result in inFile_parsed.txt. Registry writes will not be seen. U330 platform\n\n","";
  printf "%15s perl busmon_parse.pl inFile.txt -reg -U365\n","";
  printf "%15s This will put the result in inFile_parsed.txt. Registry writes will be seen. U365 platform.\n\n","";
  printf "%15s perl busmon_parse.pl inFile.txt -html -U330\n","";
  printf "%15s This will generate the same output as with the -reg flag. A html version will also be created. U330 platform\n","";
  printf "%15s perl busmon_parse.pl inFile.txt -html -load(ALL) -U365\n","";
  printf "%15s This will generate a load meassurment report written to stdout. U365 platform\n\n","";
  printf "%15s Note that <-U330> or <-U365> must always be specified.\n","";
  printf "%15s Also note that <-WithAudio> should be used when logs come from audio-enabled builds.\n","";
  
  }

#-------------------------------------------------------------
# ParseCommandLineOptions
#
# In:
# Out:
# Globals: 
#
# Info: 
#-------------------------------------------------------------
sub ParseCommandLineOptions
  {
   
     if ($input =~ /-help/) 
      {
	PrintBusmonHelp();
	exit;
      }   
    if ($#ARGV < 0) 
    {
   	PrintBusmonHelp();
   	exit;
    }
    if ($input =~ /-reg/) 
      {
	$doReg = 1;
      }
    if ($input =~ /-html/) 
      {
	$doHtml = 1;
	$doReg = 1;
      }
    if ($input =~ /-load/) 
      {
	$input =~ /-load\((\s*)(\S*)(\s*)\)/;
	$doLoad = $2;
      }
    if ($input =~ /-U330/) 
      {
	$doU330 = 1;
      }
    if ($input =~ /-U365/) 
      {
	$doU365 = 1;
      }
    if ($input =~ /-WithAudio/) 
      {
	$doWithAudio = 1;
      }

    if (($doU330 == 0) && ($doU365 == 0))
  {
    printf "\n\nYou need to specify platform (-U330) or (-U365) as input paramter.\n\n";
    exit;
  }

  }


sub CalculateLoadPerformance
  {
    #This kind of functionality does not belong here. A filehandle in logdefinition is more appropiate.
    #Get load Meas Data as string
    if ($doLoad ne "")
      {
	$LoadMeasDataAsString = GetLoadMeassurementLog();
	print LOAD_MEAS "$LoadMeasDataAsString";
	close(LOAD_MEAS);
      }
  }

sub bin2dec {
    return unpack("N", pack("B32", substr("0" x 32 . shift, -32)));
}
