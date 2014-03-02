#Todo: Seperate data from callback (Making it easier to have a custom definition file).
#      Write a general function for converting offset/range/address to real address.
package busmon_config;
use Data::Dumper;
use busmon_data;


#TODO: PRIO 1 Cleanup load meassurement
#             Define all interrupts in one logdefinition.
#             Make it possible to feel if absolute time or delta time.
#             General callback. (Should decrease codesize/complexity)
#             Write drivers for EXCEL
#             If script dies, kill EXCEL aswell
#             Close workbook before trying to open it. (If workbook is open when running script, data will be corrupted.)
#             Align variablenames, look etc with busmon_parse.pl. Move EXCEL drivers to busmon_parse.pl.
BEGIN 
  {
    use Exporter   ();
    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);        # set the version for version checking
    $VERSION     = 3.13;
    @ISA         = qw(Exporter);
#    @EXPORT      = qw(PrintLogEvent PrintLogError PrintSlotNumber PrintResced PrintSignal PrepareRegCallbacks InitStateListener InitRegListener);
    @EXPORT      = qw(InitStateListener InitRegListener InitLoadMeassurment GetLoadMeassurementLog);
  }


#***********************************************************************
# EXPORTED GLOBALS
#***********************************************************************

# State contains addresses and callbacks belonging to state logging. 
# I.e event logging.
my %State;
# Reg contains addresses and callbacks belonging to not state logging.
# I.e send/recv signal logging.
my %Reg;
# Deadline contains addresses and callback belonging to deadline checks.
my %Deadline;


#***********************************************************************
# BASE ADDRESSES
# Defines where data is beeing written.
#***********************************************************************
my $StateAddressSlot  = "0x1C000";
my $StateAddressFrame = "0x1DA00";
my $StateAddressCM    = "0x1DD00";
my $StateAddressCtrl  = "0x1DE00";
my $StateAddressRM    = "0x1EF00";

my $GlobalInterruptStart = 0;

my $StateAddressOS    = "0x1E000";
my $StateAddressIRQ   = "0x1E050";

#***********************************************************************
# LOG DEFINITIONS
#
# The hashes below defines what addresses that are going to be logged
# and their corresponding callback.
# The structure is as follows:

#my $StateLogExample = {
#	       StateAddress => $StateAddressSlot,  #Defines base address for state 
#	       State => 1,                         #Which state this log belongs to. (I.e when 1 is written to 
#                                                  #StateAddress above, this logpoint should be activated)
#	       Data => {                           #Defines what addresses to log.
#	               Handler =>  0x2,            #Handler  = $StateAddressSlot + 0x2. (If $StateAddressSlot = 0x17000, then $Handler = 0x17002 )
#	               Event => 0x4,               #Event    = $StateAddressSlot + 0x4  
#	               State => 0x6,               #State    = $StateAddressSlot + 0x6
#	               NewState => 0x8,            #NewState = $StateAddressSlot + 0x8
#	               END => 0x8,                 #END is the last address writen to. When this happens, the callback is called and it is
#                                                  #assumed that all data is collected.
#	               },
#	       TimeStamp => "",                    #TimeStamp is the time when the logpoint was "activated", i.e. when $StateAddressSlot was written to.
#	       EVENT_M => \@SLOT_HANDLER_EVENT,    #Data we need for printing. It is OK to add custom data and use it in the callbacks.
#	       STATE_M => \@SLOT_HANDLER_STATE,    
#	       Callback => \&PrintLogEvent,        #The callback, this function will be called when $StateAddressSlot + END is written to.
#                                                  #This function must be defined in this file, busmon_config.pm.
#	      };
#
#my $RegLogExample = {
#		     BaseAddress =>  $BaseAddressOSResced, #The only difference from StateLog above is BaseAddress. 
#                                                          #Data at BaseAddress is written to output wheras Data at StateAddress is not
#		     CurrPid => "",                        #Custom data used in print
#		     CurrPidStartTime => "",               #Custom data used in print
#		     Data => {
#			      NewPID => 0x0,
#			      END => 0x0,
#			     },
#		     TimeStamp => "",
#		     Callback => \&PrintOSResced,         
#		  };

# IMPORTANT NOTE: The following example is highly customizable. You can copy/modify/paste it in to the code and log whatever you want.
#                 If doing so, don't forgett to register the log definition in InitRegListener. 
#                 (I.e. putting the line 'RegisterRegListener($QuickLogExample);' last in InitRegListener.
#my $QuickLogExample = {
#	               BaseAddress => "0x17050",                                 #BaseAddress is treated in the same way as in $RegLogExample above.
#	               PrintString => "CombinerData:HEX CombinerDataAddr:DEC",   #Defines what to print and how. This translates to:
#                                                                                # Print CombinerData (specified in Data) in hexadecimal,
#                                                                                # Print CombinerAddr (specified in Data) in decimal
#	               Data => {
#		                CombinerData => "r_17052_17080_2",               #Here a range is specfied. When the callback is called
#                                                                                #CombinerData will be an array with the values written to
#                                                                                #address 17052 + 2, 17050 + 4,..., 17080
#                               CombinerDataAddr => "a_17050",                   #CombinerDataAddr will contain the value written to 17050.
#		                END => 0x28,                                     #The callback will be called when address BaseAddress + 0x28
#                                                                                #Is being written to.
#		               },
#	              TimeStamp => "",
#	              Callback => \&PrintRange,                                  #Callback PrintRange is a generic printfunction.
#                                                                                #If defining your own logs, you can use this function. It will print
#                                                                                #everything specified in Data in a nice matlab way.
#	              };

#***********************************************************************
my $StateSlotLogEvent = {
			 StateAddress => $StateAddressSlot,
			 State => 0x1f,
			 Data => {
				  Handler =>  0x010,
				  Event => 0x020,
				  State => 0x030,
				  NewState => 0x040,
				  END => 0x040,
				 },
			 TimeStamp => "",
			 ProcessString => "SC",
			 EVENT_M => \@SLOT_HANDLER_EVENT,
			 STATE_M => \@SLOT_HANDLER_STATE,
			 Callback => \&PrintLogEvent,
			};

my $StateSlotLogError = {
			 StateAddress => $StateAddressSlot,
			 State => 0x2f,
			 Data => {
				  Handler =>  0x010,
				  Event => 0x020,
				  State => 0x030,
				  NewState => 0x040,
				  END => 0x040,
				 },
			 TimeStamp => "",
			 ProcessString => "SC",
			 EVENT_M => \@SLOT_HANDLER_EVENT,
			 STATE_M => \@SLOT_HANDLER_STATE,
			 Callback => \&PrintLogError,
			};

my $StateSlotSlotNumber = {
       StateAddress => $StateAddressSlot,
       State => 0x3f,
       Data => {
            SlotNumber =>  0x010,
            FrameNumber =>  0x020,
            END => 0x020,
           },
       TimeStamp => "",
       Callback => \&PrintSlotNumber,
      };


my $StateFrameLogEvent = {
			 StateAddress => $StateAddressFrame,
			 State => 0x1f,
			 Data => {
				  Handler =>  0x10,
				  Event => 0x20,
				  State => 0x30,
				  NewState => 0x40,
				  END => 0x40,
				 },
			  TimeStamp => "",
			  ProcessString => "FC",
			  EVENT_M => \@FRAME_HANDLER_EVENT,
			  STATE_M => \@FRAME_HANDLER_STATE,
			  Callback => \&PrintLogEvent,
			};

my $StateFrameLogError = {
			  StateAddress => $StateAddressFrame,
			  State => 0x2f,
			  Data => {
				   Handler =>  0x10,
				   Event => 0x20,
				   State => 0x30,
				   END => 0x30,
				  },
			  TimeStamp => "",
			  ProcessString => "FC",
			  EVENT_M => \@FRAME_HANDLER_EVENT,
			  STATE_M => \@FRAME_HANDLER_STATE,
			  Callback => \&PrintLogError,
			};

my $StateCtrlLogEvent = {
			 StateAddress => $StateAddressCtrl,
			 State => 0x1f,
			 Data => {
				  Handler =>  0x2,
				  Event => 0x4,
				  State => 0x6,
				  NewState => 0x8,
				  END => 0x8,
				 },
			 TimeStamp => "",
			 ProcessString => "CT",
			 EVENT_M => \@CTRL_HANDLER_EVENT,
			 STATE_M => \@CTRL_HANDLER_STATE,
			 Callback => \&PrintLogEvent,
			};
			
my $StateCMLogEvent = {
			 StateAddress => $StateAddressCM,
			 State => 0x1f,
			 Data => {
				  Handler =>  0x2,
				  Event => 0x4,
				  State => 0x6,
				  NewState => 0x8,
				  END => 0x8,
				 },
			 TimeStamp => "",
			 ProcessString => "CM",
			 EVENT_M => \@CM_HANDLER_EVENT,
			 STATE_M => \@CM_HANDLER_STATE,
			 Callback => \&PrintLogEvent,
			};

my $StateCMLogError = {
			 StateAddress => $StateAddressCM,
			 State => 0x2f,
			 Data => {
				  Handler =>  0x2,
				  Event => 0x4,
				  State => 0x6,
				  END => 0x6,
				 },
			 TimeStamp => "",
			 ProcessString => "CM",
			 EVENT_M => \@CM_HANDLER_EVENT,
			 STATE_M => \@CM_HANDLER_STATE,
			 Callback => \&PrintLogError,
			};


my $StateOSResced = {
			 StateAddress => $StateAddressOS,
			 State => 0x1f,
			 Data => {
			    NewPID => 0x10,
				  END => 0x10,
				 },
			 TimeStamp => "",
		   CurrPid => "",
		   CurrPidStartTime => "",
		   Callback => \&PrintOSResced,
		  };

my $StateOSSendSignal = {
			 StateAddress => $StateAddressOS,
			 State => 0x2f,
       Data => {
          PortID => 0x10,
          SignalLSB => 0x20,
          SignalMSB => 0x30,
          END => 0x30,
       },
       TimeStamp => "",
       Callback => \&PrintSendSignal,
      };

my $StateOSRecvSignal = {
		   StateAddress => $StateAddressOS,
			 State => 0x3f,
       Data => {
          PortID => 0x10,
          SignalLSB => 0x20,
          SignalMSB => 0x30,
          END => 0x30,
       },
       TimeStamp => "",
       Callback => \&PrintRecvSignal,
      };

my $OSInterruptStart = {
		   StateAddress => $StateAddressIRQ,
			 State => 0x1f,
       Data => {
			    IntOSID => 0x10,
			    END => 0x10,
			 },
			 TimeStamp => "",
			 Callback => \&PrintOSInterruptStart,
		  };

my $StateOSInterrupt = {
		   StateAddress => $StateAddressIRQ,
			 State => 0x2f,
       Data => {
          InterruptID => 0x10,
          END => 0x10,
       },
       TimeStamp => "",
       Callback => \&PrintInterrupt,
      };

my $OSInterruptEnd = {
		   StateAddress => $StateAddressIRQ,
			 State => 0x4f,
       Data => {
          Empty => 0x0,
			    END => 0x0,
			 },
		   TimeStamp => "", 
		   Callback => \&PrintOSInterruptEnd,
		  };

#my $OSWideBandInterruptLowStart = {
#          BaseAddress => "0x0202C",
#          PrintReg => 1,
#          Data => {
#             IntIDLow => 0x0,
#             END => 0x0,
#            },
#          TimeStamp => "",
#          Callback => \&PrintWideBandInterruptLowStart,
#       };

#my $OSWideBandInterruptHighStart = {
#           BaseAddress => "0x02028",
#           PrintReg => 1,
#           Data => {
#              IntIDHigh => 0x0,
#              END => 0x0,
#             },
#           TimeStamp => "",
#           Callback => \&PrintWideBandInterruptHighStart,
#          };

#my $OSWideBandInterruptEnd = {
#           BaseAddress => "0x0201C",
#           PrintReg => 1,
#           Data => {
#              Zero => 0x0,
#              END => 0x0,
#             },
#           TimeStamp => "",
#           Callback => \&PrintWideBandInterruptEnd,
#          };

#my $OSCPUWideBandInterrupt = {
#           BaseAddress => "0x02024",
#           PrintReg => 1,
#           Data => {
#              IRQ_Index => 0x0,
#              END => 0x0,
#             },
#           TimeStamp => "",
#           Callback => \&PrintOSCPUWideBandInterrupt,
#          };

#my $RegDspCpuSigMeas = {
#			BaseAddress => "0x1C500",
#			PrintReg => 1,
#			Data => {
#				 DspTxSignalID => 0x0,
#				 CpuRxSignalID => "a_07B00",
#				 CpuProcRecvSignalID => "a_07B02",
#				 END => "a_07B02",
#				},
#			TimeStamp => "",
#			Callback => \&PrintRegDspCpuSigMeas,
#		       };
		       
#my $RegCtrlNextPlannedSFN = {
#			     BaseAddress => "0x1C016",
#			     PrintReg => 1,
#			     Data => {
#			               Event => 0x0,
#			               SFN   => 0x2,
#				       END   => 0x2,
#				     },
#			TimeStamp => "",
#			Callback => \&PrintRegCtrlNextPlannedSFN,
#		       };
		       
#my $RegCtrlPlannedActivitySFN = {
#                                 BaseAddress => "0x1C01A",
#                                 PrintReg => 1,
#                                 Data => {
#                                          Event  => 0x0,
#                                          SFN    => 0x2,                      
#                                          END    => 0x2,
#                                         },
#            			TimeStamp => "",
#         			Callback => \&PrintRegCtrlPlannedActivitySFN,
#		       };

#my $RegCtrlDSPNeededSFN = {
#			     BaseAddress => "0x1C010",
#			     PrintReg => 1,
#			     Data => {
#			               Event     => 0x0,
#			               Event_SFN => 0x2,
#			               Until_SFN => 0x4,			               
#				       END       => 0x4,
#				     },
#			TimeStamp => "",
#			Callback => \&PrintRegCtrlDSPNeededSFN,
#		       };

#my $RegCtrlDSPCurrentlyUsed = {
#                                 BaseAddress => "0x1C01E",
#                                 PrintReg => 1,
#                                 Data => {
#                                          Activity  => 0x0,
#                                          END       => 0x0,
#                                         },
#            			TimeStamp => "",
#         			Callback => \&PrintRegCtrlDSPCurrentlyUsed,
#		       };
		       
		       
#my $RegCtrlWCDMACurrentlyUsed = {
#                                 BaseAddress => "0x1C020",
#                                 PrintReg => 1,
#                                 Data => {
#                                          Activity  => 0x0,
#                                          END       => 0x0,
#                                         },
#            			TimeStamp => "",
#         			Callback => \&PrintRegCtrlWCDMACurrentlyUsed,
#		       };
		       
#my $RegCtrlSchedAfterSleep = {
#                              BaseAddress => "0x1C032",
#                              PrintReg => 1,
#                              Data => {
#                                       SFN_BeforeSleep  => 0x0,
#                                       SFN_AfterSleep   => 0x2,
#                                       Event            => 0x4,
#                                       SFN_Event        => 0x6,
#                                       END       => 0x6,
#                                       },
#            		      TimeStamp => "",
#         		      Callback => \&PrintRegCtrlSchedAfterSleep,
#		             };
		             
#my $RegCtrlScheduler = {
#                              BaseAddress => "0x1C022",
#                              PrintReg => 1,
#                              Data => {
#                                       Type        => 0x0,
#                                       Event       => 0x2,
#                                       SFN         => 0x4,
#                                       Periodicity => 0x6,
#                                       PrimarySC   => 0x8,
#                                       UARFCN      => 0xA,
#                                       END         => 0xA,
#                                       },
#            		      TimeStamp => "",
#         		      Callback => \&PrintRegCtrlScheduler,
#		             };
		       
#my $RegCtrlRecvSignal = {
#		       BaseAddress => "0x1C030",
#		       Data => {
#				SignalID => 0x0,
#				State    => 0x2.
#				END => 0x2,
#			       },
#		       TimeStamp => "",
#		       ProcessString => "Ctrl",
#		       Callback => \&PrintRegCtrlCmRecvSignal,
#		       };
		       
#my $RegCtrlState = {
#		       BaseAddress => "0x1C040",
#		       Data => {
#				NewState => 0x0,
#				OldState => 0x2,
#				END => 0x2,
#			       },
#		       TimeStamp => "",		       
#		       Callback => \&PrintRegCtrlState,
#		       };
		      		      

#my $RegCtrlSleepFreeSignal = {
#		       BaseAddress => "0x1C00A",
#		       Data => {
#				SignalID => 0x0,
#				END => 0x0,
#			       },
#		       TimeStamp => "",
#		       Callback => \&PrintRegCtrlSleepFreeSignal,
#		       };		       
		       
#my $RegCmRecvSignal = {
#		       BaseAddress => "0x1B030",
#		       Data => {
#				SignalID => 0x0,
#				END => 0x0,
#			       },
#		       TimeStamp => "",
#		       ProcessString => "CM",
#		       Callback => \&PrintRegCtrlCmRecvSignal,
#		       };
		       
#my $RegDSPSleepCnf = {
#		       BaseAddress => "0x1C00A",
#		       Data => {
#				Statis => 0x0,
#				END => 0x0,
#			       },
#		       TimeStamp => "",
#		       Callback => \&PrintRegDSPSleepCnf,
#		       };		       
		       
		       
		       
#my $RegBCHLogging = {
#		     BaseAddress => "0x1C500",
#		     PrintReg => 1,
#		     Data => {
#				FingerID => 0x0,
#				ChEstRe  => 0x2,
#				ChEstIm  => 0x4,
#		        	DespNumber => 0x6,
#				SlotNumber => 0x8,
#		        	GoldenIdx  => 0x2E,
#				SlotCnt    => 0xA,
#				DespSlotOffset => 0x16,
#		        	DespQChipOffset => 0x18,
#				END => 0x18,
#		             },
#		             TimeStamp => "",
#		             Callback => \&PrintRegBCHLogging,
#		           };


my %GlobalData = (
		  PrevEventTime => "",
		  PrevSlotTime => "",
		  CurrPID => "",
		  FrameCounter => 0,
		  FrameFirst => 1,
		  SlotFirst => 1,
		  FrameTimeStamp => 0,
		  SlotCounter => 0,
		  FrameExecStat => ,
		  SlotExecStat => ,
		  SlotInterruptStat => ,
		  FrameInterruptStat =>,
		  OSInterruptStart => ,
		  OSInterruptEnd => ,
		  OSInterruptTimeUsed => 0,
		  FirstCntx => 1,
		  ExcelWasStarted => 0,
		 );	

#TODO: Prio 2: Move all load meas data from GlobalData to LoadMeasData.
#TODO: Prio 2: It seems like when hash is no reference, init data not set? Investiagte this. It could explain some more strange behaviours.
my %LoadMeasData = (
		    BCH => ,
		    DCH => ,
		    TOTAL => ,

		    Config => {
			       LoadStartTrigger => "",
			       LoadEndTrigger => "",
			       LoadMeas => "NO_LOAD_MEAS",
			       IsRunning => 0,
			       IsStopped => 0,
			       BarsPerPage => 100,
			      },
		    
		    );

my $NumberOfSlots = 0;

#***********************************************************************
# InitListener
#
# Those function registers listeners, i.e what should be parsed.
# InitListener is called from busmon_config and returns addresses 
# ranges that have callbacks.
#***********************************************************************

#***********************************************************************
# InitStateListener
# Init function for state logging. This function registers what states
# that is going to be logged.
#***********************************************************************
sub InitStateListener
  {
    #OS event logging
    RegisterStateListener($StateOSResced);
    RegisterStateListener($StateOSSendSignal);
    RegisterStateListener($StateOSRecvSignal);
    RegisterStateListener($StateOSInterrupt);
    RegisterStateListener($OSInterruptStart);
    RegisterStateListener($OSInterruptEnd);

    #Slot event logging
    RegisterStateListener($StateSlotLogEvent);
    RegisterStateListener($StateSlotLogError);
    RegisterStateListener($StateSlotSlotNumber);

    #Frame event logging
    RegisterStateListener($StateFrameLogEvent);
    RegisterStateListener($StateFrameLogError);

    #Ctrl event logging
#    RegisterStateListener($StateCtrlLogEvent);

    #Cell Manager event logging
#    RegisterStateListener($StateCMLogEvent);
#    RegisterStateListener($StateCMLogError);

    return %State;
  }

#*************************************************************************
# InitRegListener
# Init function for special logging. This function registers callbacks
# that is not state logs. I.e. OS log point, Send signal and recv signal.
#*************************************************************************
sub InitRegListener
  {
    (my $Path) = @_;
    $Path =~ s/.txt$//;

    #RegisterRegListener($OSWideBandInterruptEnd);
    #RegisterRegListener($RegDspCpuSigMeas);
    #RegisterRegListener($RegBCHLogging);

#    RegisterRegListener($OSWideBandInterruptHighStart);
#    RegisterRegListener($OSWideBandInterruptLowStart);
#    RegisterRegListener($RegCtrlNextPlannedSFN);
#    RegisterRegListener($RegCtrlPlannedActivitySFN);
#    RegisterRegListener($RegCtrlDSPNeededSFN);
#    RegisterRegListener($RegCtrlDSPCurrentlyUsed);
#    RegisterRegListener($RegCtrlWCDMACurrentlyUsed);
#    RegisterRegListener($OSCPUWideBandInterrupt);
#    RegisterRegListener($RegCmRecvSignal);
#    RegisterRegListener($RegCtrlRecvSignal);
#    RegisterRegListener($RegCtrlSchedAfterSleep);
#    RegisterRegListener($RegDSPSleepCnf);
#    RegisterRegListener($RegCtrlSleepFreeSignal);
#    RegisterRegListener($RegCtrlState);
#    RegisterRegListener($RegCtrlScheduler);
    
    if (!(-e $Path))
    {
      mkdir($Path);
    }
    
    open(COMMON_CH_1_LOG,">$Path\\CC1.txt") or die "$!";
    return %Reg;
  }

#*************************************************************************
# InitRegListener
# Init function for special logging. This function registers callbacks
# that is not state logs. I.e. OS log point, Send signal and recv signal.
#*************************************************************************
sub InitDeadlineListener
  {
    RegisterDeadlineListener($Deadline1);
    return %Deadline;
  }


sub InitLoadMeassurment
  {
    (my $LoadMeas, my $ExcelFileName) = @_;
    
    if ($LoadMeas eq "DCH")
      {
	$LoadMeasData{'Config'}->{'LoadStartTrigger'} = "U_DCH_START";
	$LoadMeasData{'Config'}->{'LoadMeas'} = "DCH";
      }
    elsif ($LoadMeas eq "BCH")
      {
	$LoadMeasData{'Config'}->{'LoadStartTrigger'} = "BCH_START";
	$LoadMeasData{'Config'}->{'LoadEndTrigger'} = "BCH_STOP";
	$LoadMeasData{'Config'}->{'LoadMeas'} = "BCH";
      }
    elsif($LoadMeas eq "ALL")
      {
	$LoadMeasData{'Config'}->{'LoadStartTrigger'} = "NEVER_STOP";
	$LoadMeasData{'Config'}->{'LoadEndTrigger'} = "NEVER_STOP";
	$LoadMeasData{'Config'}->{'LoadMeas'} = "ALL";
	#Start the meassurement directly.
	$LoadMeasData{'Config'}->{'IsRunning'} = 1;
      }
    elsif($LoadMeas eq "EXCEL")
      {
	
	$LoadMeasData{'Config'}->{'LoadStartTrigger'} = "NEVER_STOP";
	$LoadMeasData{'Config'}->{'LoadEndTrigger'} = "NEVER_STOP";
	$LoadMeasData{'Config'}->{'LoadMeas'} = "ALL";
	#Start the meassurement directly.
	$LoadMeasData{'Config'}->{'IsRunning'} = 1;
	$LoadMeasData{'Config'}->{'Excel'} = 1;
	#Fix this
	$LoadMeasData{'Config'}->{'BarsPerPage'} = 100;
	OpenExcelDocument("$ExcelFileName");

      }
  } 
  
sub PrintRegCtrlScheduler
{
  (my $LogData) = @_;
  
  my $Type        = sprintf("a%d",%{$LogData}->{'Data'}->{'Type'});
  my $Event       = $CTRL_SCHED_EVENT{sprintf("a%.4X",%{$LogData}->{'Data'}->{'Event'})};
  my $SFN         = %{$LogData}->{'Data'}->{'SFN'};
  my $Periodicity = %{$LogData}->{'Data'}->{'Periodicity'};
  my $PrimarySC   = %{$LogData}->{'Data'}->{'PrimarySC'};
  my $UARFCN      = %{$LogData}->{'Data'}->{'UARFCN'};
  
  my %TypeH = (
               a0 => "INSERT",
               a1 => "REMOVE",
               a2 => "UNLINK"
             );
             
  if ($Event eq "")
  {
   $Event = sprintf("0x%.4x",%{$LogData}->{'Data'}->{'Event'});
  }
             
  my $printStr = sprintf("Sched: %s event %s (SFN: %d, Per: %d) (PrimarySC:%d, UARFCN: %d)",$TypeH{$Type},$Event, $SFN,$Periodicity,$PrimarySC,$UARFCN);
                         
  #print "$printStr\n";
  
  return ($printStr,"NO","YES","GREEN");
}
  
sub PrintRegCtrlState
{
  (my $LogData) = @_;
  
  my $NewState = %{$LogData}->{'Data'}->{'NewState'};
  my $OldState = %{$LogData}->{'Data'}->{'OldState'};
  
  my $printStr = sprintf("New CTRL state %s (%s -> %s)",$CTRL_STATES[$NewState],$CTRL_STATES[$OldState],$CTRL_STATES[$NewState]);
  
  return ($printStr,"NO","YES","GREEN");
}

sub PrintRegCtrlSleepFreeSignal
{
    (my $LogData) = @_;
    
    my $SignalID = %{$LogData}->{'Data'}->{'SignalID'};
    
    my $SignalNumber = sprintf("a0%0.4X",$SignalID);
    
    my $printStr = sprintf("Ctrl Sleep Signal freed: (%s) %s",$SignalNumber,$SIGNAL{$SignalNumber});
    
    return ($printStr,"YES","NO","DARK_GREEN");
    
    
}

sub PrintRegDSPSleepCnf	             
{
    (my $LogData) = @_;
    
    my $Status  = %{$LogData}->{'Data'}->{'Status'};
    
    my $printStr = sprintf("FRAME_SLEEP_CNF: Status: %d", $Status);
    
    return ($printStr,"NO","YES","GREEN");
}

sub PrintRegCtrlSchedAfterSleep
{
  (my $LogData) = @_;
  
  my $Event            = $CTRL_SCHED_EVENT{sprintf("a%.4X",%{$LogData}->{'Data'}->{'Event'})};
  my $SFN_BeforeSleep  = %{$LogData}->{'Data'}->{'SFN_BeforeSleep'};
  my $SFN_AfterSleep   = %{$LogData}->{'Data'}->{'SFN_AfterSleep'};
  my $SFN_Event        = %{$LogData}->{'Data'}->{'SFN_Event'};
  
  my $printStr         = sprintf("Late wake up: Slept %d -> %d. Missed $Event (SFN %d)",$SFN_BeforeSleep, $SFN_AfterSleep,$Event,$SFN_Event);
  
  return ($printStr,"NO","YES","GREEN");
}

sub PrintRegCtrlCmRecvSignal
{
  (my $LogData) = @_;	
  
  my $TimeStamp     = %{$LogData}->{'TimeStamp'};
  my $ProcessString = %{$LogData}->{'ProcessString'};
  my $SignalID      = %{$LogData}->{'Data'}->{'SignalID'};
  
  my $State         = %{$LogData}->{'Data'}->{'State'};
 
  my $printStr = "";
  $SignalNumber = sprintf("a0%0.4X",$SignalID);
  
  $RecvTime = $TimeStamp;
  $RecvTime =~ s/(\.|s|\s)//g;
  
  if ($State ne "")
  {
    $printStr = sprintf("%20s: !!%s RECV SIGNAL : 0x%X (%s) in State: %s",$TimeStamp,$ProcessString,$SignalID,$SIGNAL{$SignalNumber},$CTRL_STATES[$State]);
  }
  else
  {
    $printStr = sprintf("%20s: !!%s RECV SIGNAL : 0x%X (%s)",$TimeStamp,$ProcessString,$SignalID,$SIGNAL{$SignalNumber});
  }
      
  return ($printStr,"YES","NO","DARK_GREEN");
}

sub PrintRegCtrlWCDMACurrentlyUsed
{
  (my $LogData) = @_;	

  my $Activity = $CTRL_SCHED_ACTIVITY{sprintf("a%d",%{$LogData}->{'Data'}->{'Activity'})};
  
  #Add status (RUNNING, ORDERED TO STOP)
  my $printStr = sprintf("WCDMA Activity %s is currently running",$Activity);
  
  return ($printStr,"NO","YES","GREEN");
}

sub PrintRegCtrlDSPCurrentlyUsed
{
  (my $LogData) = @_;	
  
  my $Activity = $CTRL_SCHED_ACTIVITY{sprintf("a%d",%{$LogData}->{'Data'}->{'Activity'})};
  
  #Add status (RUNNING, ORDERED TO STOP)
  
  my $printStr = sprintf("DSP is currently used for activity %s",$Activity);
  
  return ($printStr,"NO","YES","GREEN");
  
}
sub PrintRegCtrlDSPNeededSFN
{
  (my $LogData) = @_;	
  
  my $Event     = $CTRL_SCHED_EVENT{sprintf("a%.4X",%{$LogData}->{'Data'}->{'Event'})};
  my $Event_SFN = %{$LogData}->{'Data'}->{'Event_SFN'};
  my $Until_SFN = %{$LogData}->{'Data'}->{'Until_SFN'};
  my $printStr;
  
  if (Event_SFN <= Until_SFN)
  {
     $printStr  = sprintf("DSP is NEEDED for Event %s at SFN %d (> SFN %d)",$Event,$Event_SFN,$Until_SFN);
  }
  else
  {
     $printStr  = sprintf("DSP is NOT NEEDED for Event %s until SFN %d (needed SFN %d)",$Event,$Until_SFN,$Event_SFN);
  }
  
  return ($printStr,"NO","YES","GREEN");
  
}

sub PrintRegCtrlPlannedActivitySFN
{
  (my $LogData) = @_;	
    
  my $Event = $CTRL_SCHED_EVENT{sprintf("a%.4X",%{$LogData}->{'Data'}->{'Event'})};
  my $SFN   = %{$LogData}->{'Data'}->{'SFN'};
    
  my $printStr = sprintf("Next planned WCDMA for Event %s is at SFN %d",$Event,$SFN);
    
  return ($printStr,"NO","YES","GREEN");
}

sub PrintRegCtrlNextPlannedSFN
{
  (my $LogData) = @_;	
  
  my $Event = $CTRL_SCHED_EVENT{sprintf("a%.4X",%{$LogData}->{'Data'}->{'Event'})};
  my $SFN   = %{$LogData}->{'Data'}->{'SFN'};
  
  my $printStr = sprintf("Next planned mandatory WCDMA Event is %s at SFN %d",$Event,$SFN);
  
  return ($printStr,"NO","YES","GREEN");
}
sub PrintRegBCHLogging
{
	(my $LogData) = @_;
	
	my $FingerID        = %{$LogData}->{'Data'}->{'FingerID'};	
	my $ChEstRe         = TwoComplToDec(16,%{$LogData}->{'Data'}->{'ChEstRe'});		
	my $ChEstIm         = TwoComplToDec(16,%{$LogData}->{'Data'}->{'ChEstIm'});
	my $DespNumber      = %{$LogData}->{'Data'}->{'DespNumber'};				
	my $SlotNumber      = %{$LogData}->{'Data'}->{'SlotNumber'};				
	my $GoldenIdx       = %{$LogData}->{'Data'}->{'GoldenIdx'};				
	my $SlotCnt         = %{$LogData}->{'Data'}->{'SlotCnt'};
	my $DespSlotOffset  = %{$LogData}->{'Data'}->{'DespSlotOffset'};								
	my $DespQChipOffset = %{$LogData}->{'Data'}->{'DespQChipOffset'};
	
	
	print COMMON_CH_1_LOG "$SlotNumber\t$SlotCnt\t$GoldenIdx\t$FingerID\t$DespNumber\t$ChEstRe\t$ChEstIm\t$DespSlotOffset\t$DespQChipOffset\n";
		
	return ("","NO","YES","GREEN");										
}

#sub PrintOSCPUWideBandInterrupt
#{
#  (my $LogData) = @_;
#  
#  my $TimeStamp =  %{$LogData}->{'TimeStamp'};
#  my $IRQ_Index = (%{$LogData}->{'Data'}->{'IRQ_Index'} & 0x003F) >> 4;
#  my $IRQ       = $CPU_INTERRUPT{sprintf("a%d",$IRQ_Index)};
# 
#  my $printStr = sprintf("%20s: CPU INTERRUPT: 0x%.4x (%s)",$TimeStamp,$IRQ_Index,$IRQ);
#  
#  return ($printStr,"YES","NO","DARK_GREEN");
#}

sub PrintRegDspCpuSigMeas
  {
    #DspTxSignalID => 0x0,
    #CpuRxSignalID => 0x2,
    #CpuProcRecvSignalID => 0x4,
    (my $LogData) = @_;
    my $printStr;
    my $DspTxSignalID        = %{$LogData}->{'Data'}->{DspTxSignalID} & 0xFFF0;
    my $CpuRxSignalID        = %{$LogData}->{'Data'}->{DspTxSignalID} & 0xFFF0;
    my $CpuProcRecvSignalID  = %{$LogData}->{'Data'}->{DspTxSignalID} & 0xFFF0;
    my $DspTxTimeStamp       = %{$LogData}->{'Address'}->{"1C500"}->{'TimeStamp'};
    my $CpuRxTimeStamp       = %{$LogData}->{'Address'}->{"07B00"}->{'TimeStamp'};
    my $CpuProcRecvTimeStamp = %{$LogData}->{'Address'}->{"07B02"}->{'TimeStamp'};

    $DspTxSignalID = sprintf("a0%0.4X",$DspTxSignalID);
    $CpuRxSignalID = sprintf("a0%0.4X",$CpuRxSignalID);
    $CpuProcRecvSignalID = sprintf("a0%0.4X",$CpuProcRecvSignalID);
    

    my $T1 = ($CpuRxTimeStamp - $DspTxTimeStamp)*(10**6);
    my $T2 = ($CpuProcRecvTimeStamp - $CpuRxTimeStamp)*(10**6);
    my $Ttot =  ($CpuProcRecvTimeStamp - $DspTxTimeStamp)*(10**6);
    printf "(%s)\t%d + %d = %d\n",,$SIGNAL{$DspTxSignalID},$T1,$T2,$Ttot;
    
    $printStr = sprintf("%20s: DSP->CPU SIGNAL %s (%d) DSP->CPU %dus CPU->PROC %dus","",$SIGNAL{$SignalNumber},$Ttot,$T1,$T2);
    #Calculate some times
    
    return ($printStr,"NO","YES","GREEN");
  }
#***********************************************************************
# PrinLogEvent
#
# Print callback for log event in state machines.
#***********************************************************************	
sub PrintLogEvent 
  {
    (my $LogData) = @_;

    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    $TimeStamp =~ s/(s|\.|\s)//g;
    my $timeDiff = $TimeStamp - $GlobalData{'PrevEventTime'};
    $GlobalData{'PrevEventTime'} = $TimeStamp;
    
    my $printStr;
    my $Process = "";

    #Print LogEvent data

    $HandleNumber   = (%{$LogData}->{'Data'}->{'Handler'}) >> 4;
    $EventNumber    = (%{$LogData}->{'Data'}->{'Event'}) >> 4;
    $OldStateNumber = (%{$LogData}->{'Data'}->{'State'}) >> 4;
    $NewStateNumber = (%{$LogData}->{'Data'}->{'NewState'}) >> 4;
    $Event = %{$LogData}->{'EVENT_M'}->[$HandleNumber][$EventNumber];
    $OldState = %{$LogData}->{'STATE_M'}->[$HandleNumber][$OldStateNumber];
    $NewState = %{$LogData}->{'STATE_M'}->[$HandleNumber][$NewStateNumber];
    
    if ($OldState eq "")
    {
      $OldState = $OldStateNumber;
    }
    
    if ($NewState eq "")
    {
      $NewState = $NewStateNumber;
    }
    
    if ($Event eq "")
    {
      $Event = $EventNumber;
    }

    $Process = %{$LogData}->{'ProcessString'};

    if ($OldState ne $NewState ) 
      {
	$printStr = sprintf("\"%15s us:\"%2s Event: %-25s State: %s -> %s",$timeDiff/1000000,$Process,$Event,$OldState,$NewState);
      }
    else 
      {
	$printStr = sprintf("\"%15s us:\"%2s Event: %-25s State: %s",$timeDiff/1000000,$Process,$Event,$OldState);
      }

    #Load meassurment
    CheckAndTriggerLoadMeas($Event);
    return ($printStr,"NO","YES","BLUE");
  }

#***********************************************************************
# PrinLogError
# 
# Print callback for log error in state machines.
#***********************************************************************
sub PrintLogError 
  {
    (my $LogData) = @_;
    
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    $TimeStamp =~ s/(s|\.|\s)//g;
    my $timeDiff = $TimeStamp - $GlobalData{'PrevEventTime'};
    $GlobalData{'PrevEventTime'} = $TimeStamp;
    $Process = %{$LogData}->{'ProcessString'};

    #Print LogError data
    $HandleNumber = (%{$LogData}->{'Data'}->{'Handler'}) >> 4;
    $EventNumber  = (%{$LogData}->{'Data'}->{'Event'}) >> 4;
    $StateNumber  = (%{$LogData}->{'Data'}->{'State'}) >> 4;
    $Event =  %{$LogData}->{'EVENT_M'}->[$HandleNumber][$EventNumber];
    $State =  %{$LogData}->{'STATE_M'}->[$HandleNumber][$StateNumber];
   
    $printStr = sprintf("\"%15s us:\"%2s Unexpected Event: %-25s State: %s",$timeDiff/1000000,$Process,$Event,$State);
    return ($printStr,"NO","YES","RED");
  }

#***********************************************************************
# PrinSlotNumber
#
# Print callback for slot number logging. This function also coalculates
# PID exec statistic per frame and per slot.
#
#***********************************************************************
sub PrintSlotNumber 
  {
    (my $LogData) = @_;
   
    my $printStr = "";
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    $TimeStamp =~ s/(s|\.|\s)//g;
    
    my $SlotLength  = $TimeStamp - $GlobalData{'PrevSlotTime'};
    my $FrameLength = $TimeStamp - $GlobalData{'FrameTimeStamp'};
    $NumberOfSlots++;
    $GlobalData{'PrevSlotTime'}   = $TimeStamp;
    
    my $SlotNumber;
    my $FrameNumber;

    #Print Slot number and Frame number
    $SlotNumber  = (%{$LogData}->{'Data'}->{'SlotNumber'}) >> 4;
    $FrameNumber = (%{$LogData}->{'Data'}->{'FrameNumber'}) >> 4;
    
    #Slot PID exec times
    $printStr = UpdateSlotLoadMeassurement($TimeStamp, $SlotLength);
    
    #When SlotNumber == 0, treat it as a new frame.
    if ($SlotNumber == 0)
      {
        $GlobalData{'FrameTimeStamp'} = $TimeStamp;
	      $printStr = sprintf("%s%s",$printStr,UpdateFrameLoadMeassurement($TimeStamp, $FrameLength));
      }

    $printStr = sprintf("%s:-----------------Slot number %d (SFN: %d)------------------",$printStr,$SlotNumber,$FrameNumber);
    return ($printStr,"NO","YES","PINK");
  }

#***********************************************************************
# PrintResced
#
# Print callback for reschedule logging.
#***********************************************************************
sub PrintOSResced 
  {
    (my $LogData) = @_;

    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    my $tim       = $TimeStamp;
    $TimeStamp    =~ s/(s|\.|\s)//g;
    my $newPID    = (%{$LogData}->{'Data'}->{'NewPID'}) >> 4;;
    my $oldPID    = %{$LogData}->{'CurrPID'};
    my $execTime  = $TimeStamp - %{$LogData}->{'CurrPidStartTime'};

    my $newPIDName = $ID_TO_PID{"a$newPID"};
    my $oldPIDName = $ID_TO_PID{"a$oldPID"};
    
    #Add exec time to $SlotExecStat and $FrameExecStat
    if ($GlobalData{'FirstCntx'} == 0)
      {
	      my $PID;
	      $GlobalData{'SlotExecStat'}->{$oldPID}->{'ExecTime'}  += $TimeStamp - $GlobalData{'SlotExecStat'}->{$oldPID}->{'StartTime'};
	      $GlobalData{'FrameExecStat'}->{$oldPID}->{'ExecTime'} += $TimeStamp - $GlobalData{'FrameExecStat'}->{$oldPID}->{'StartTime'};
	      $GlobalData{'TotExecStat'}->{$oldPID}->{'ExecTime'}   += $execTime;
	      
	      foreach $PID (keys %{$GlobalData{'TotExecStat'}})
          {
	          $ExecTime = $GlobalData{'TotExecStat'}->{$PID}->{'ExecTime'};
	    
	          $IntExecTime = $GlobalData{'TotExecStat'}->{$PID}->{'IntExecTime'};
	          $PIDExecTime = $ExecTime - $IntExecTime;
	          $LoadMeasData{'TOTAL'}->{'PID2'}->{$PID} = $PIDExecTime;
	        }

	      foreach $INT (keys %{$GlobalData{'TotInterruptStat'}})
	        {
	          $LoadMeasData{'TOTAL'}->{'INT2'}->{$INT} = $GlobalData{'TotInterruptStat'}->{$INT}->{'ExecTime'};
	        }
      }
    else
      {
	      $GlobalData{'FirstCntx'} = 0;
      }

    $GlobalData{'FrameExecStat'}->{$newPID}->{'StartTime'} = $TimeStamp;
    $GlobalData{'SlotExecStat'}->{$newPID}->{'StartTime'}  = $TimeStamp;

    #NewPID -> CurrPID
    %{$LogData}->{'CurrPID'}          = $newPID;
    $GlobalData{'CurrPID'}            = $newPID;
    %{$LogData}->{'CurrPidStartTime'} = $TimeStamp;
    
    $printStr = sprintf("%20s: CONTEXT SWITCH: New PID %s Old PID %s (exec time %d us)",$tim,$newPIDName,$oldPIDName,$execTime/1000000);
    return ($printStr,"YES","NO","ORANGE");
  }

#***********************************************************************
# PrintSendSignal
#
# Print callback for send signal logging.
#***********************************************************************
sub PrintSendSignal 
  {
    (my $LogData) = @_;
    
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    my $PortID    = (%{$LogData}->{'Data'}->{'PortID'}) >> 4;
    my $SignalLSB = (%{$LogData}->{'Data'}->{'SignalLSB'}) >> 4;
    my $SignalMSB = (%{$LogData}->{'Data'}->{'SignalMSB'}) >> 4;
    my $pidName = $ID_TO_PID{"a$GlobalData{'CurrPID'}"};
    
    $SignalNumber = sprintf("a%0.5X",($SignalLSB & 0x0fff) | (($SignalMSB & 0x0fff) << 12));
    $GlobalData{'SentSignals'}->{$SignalNumber} = $TimeStamp;
    $printStr = sprintf("%20s: SEND SIGNAL (PID %s -> PORT %s): %s (%s)",$TimeStamp,$pidName,$PortID,$SignalNumber,$SIGNAL{$SignalNumber});
 
    return ($printStr,"YES","NO","DARK_GREEN");
  }

#***********************************************************************
# PrintRecvSignal
# 
# Print callback for recieve signal logging.
#***********************************************************************
sub PrintRecvSignal 
  {
    (my $LogData) = @_;
    
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    my $PortID    = (%{$LogData}->{'Data'}->{'PortID'}) >> 4;
    my $SignalLSB = (%{$LogData}->{'Data'}->{'SignalLSB'}) >> 4;
    my $SignalMSB = (%{$LogData}->{'Data'}->{'SignalMSB'}) >> 4;
    
    $SignalNumber = sprintf("a%0.5X",($SignalLSB & 0x0fff) | (($SignalMSB & 0x0fff) << 12));
    if (exists $GlobalData{'SentSignals'}->{$SignalNumber})
      {
	$SentTime =  $GlobalData{'SentSignals'}->{$SignalNumber};
	$RecvTime = $TimeStamp;
	$SentTime =~ s/(\.|s|\s)//g;
	$RecvTime =~ s/(\.|s|\s)//g;
	delete $GlobalData{'SentSignals'}->{$SignalNumber};
	$Latency = $RecvTime - $SentTime;
	$printStr = sprintf("%20s: RECV SIGNAL (PORT %s): %s (%s) (latency: %2.1f us)",$TimeStamp,$PortID,$SignalNumber,$SIGNAL{$SignalNumber},$Latency / 1000000);
      }
    else
      {
	$printStr = sprintf("%20s: RECV SIGNAL (PORT %s): 0x%X (%s)",$TimeStamp,$PortID,$SignalID,$SIGNAL{$SignalNumber});
      }
    
    return ($printStr,"YES","NO","DARK_GREEN");
  }




#***********************************************************************
# PrintWideBandInterruptHighStart
#
# Print callback for Interrupt logging.
#***********************************************************************
#sub PrintWideBandInterruptHighStart
#  {
#    (my $LogData) = @_;
#    
#    my $TimeStamp =  $GlobalData{'OSInterruptStart'};
#
#    my $IntIDHigh  = %{$LogData}->{'Data'}->{'IntIDHigh'};
#    if (not $IntIDHigh == 0)
#      {
#	$IntIDHigh = $IntIDHigh & 0xFF;
#	my $IntIDName = $INTERRUPT{"a$IntIDHigh"};
#	
#	$GlobalData{'CurrInt'} = $IntIDName;
#	$GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'StartTime'}  = $TimeStamp;
#	$GlobalData{'FrameInterruptStat'}->{$IntIDName}->{'StartTime'} = $TimeStamp;
#     }
#
#   
#    return ("","NO","YES","BLUE");
#  }

#***********************************************************************
# PrintWideBandInterruptLowStart
#
# Print callback for Interrupt logging.
# TODO: This function could be merged with PrintWideBandInterruptHighStart().
#       They are exactly the same.
#***********************************************************************
#sub PrintWideBandInterruptLowStart
#  {
#    (my $LogData) = @_;
#    
#    my $TimeStamp =  $GlobalData{'OSInterruptStart'};
#
#    my $IntIDLow  = %{$LogData}->{'Data'}->{'IntIDLow'};
#    if (not ($IntIDLow == 0))
#      {
#	$IntIDLow = $IntIDLow & 0xFF;
#	my $IntIDName = $INTERRUPT{"a$IntIDLow"};
#	$GlobalData{'CurrInt'} = $IntIDName;
#	$GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'StartTime'}  = $TimeStamp;
#	$GlobalData{'FrameInterruptStat'}->{$IntIDName}->{'StartTime'} = $TimeStamp;
#     }
#    return ("","NO","YES","BLUE");
#  }
  
#***********************************************************************
# PrintWideBandInterruptEnd
#
# Print callback for Interrupt logging.
# TODO: Prio 2: You could possible merge all interrupt callbacks/logdefs
#               into one logpoint/callback. Will be more reliable.
# TODO: Prio 2: If full OS logging, this function is unecessary. Either check if OS -logging is there or don't care.
#***********************************************************************
#sub PrintWideBandInterruptEnd
#  {
#
#    (my $LogData) = @_;
#
#    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
#    $TimeStamp =~ s/(s|\.|\s)//g;
#
#    my $CurrPID = $GlobalData{'CurrPID'};
#    my $IntIDName = $GlobalData{'CurrInt'};
#
#    my $Finished =  %{$LogData}->{'Data'}->{'Zero'};
#    #if ($Finished == 1)
#    #  {
#    #	$StartTime =  $GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'StartTime'};
#    #	$ExecTime = $TimeStamp - $StartTime;
#    #	$GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'ExecTime'}  += $ExecTime;
#    #	$GlobalData{'SlotExecStat'}->{$CurrPID}->{'IntExecTime'}      += $ExecTime;
#    #	$GlobalData{'FrameInterruptStat'}->{$IntIDName}->{'ExecTime'} += $ExecTime;
#    #	$GlobalData{'FrameExecStat'}->{$CurrPID}->{'IntExecTime'}     += $ExecTime;
#	
#    #  }
#    return ("","NO","YES","BLUE");
#  }

#***********************************************************************
# PrintOSInterruptStart
#
# Print callback for Interrupt logging.
# TODO: PRIO 1: There comes two 1C110. You need to consider this. (Affects results).
#***********************************************************************
sub PrintOSInterruptStart
  {
    (my $LogData) = @_;
    my $InterruptID = (%{$LogData}->{'Data'}->{'IntOSID'}) >> 4;
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};

    $TimeStamp    =~ s/(s|\.|\s)//g;
    $GlobalData{'OSInterruptStart'} = $TimeStamp;
    $GlobalInterruptStart = 1;
    $GlobalData{'CurrInt'} = "OTHER_INTERRUPT";

    return ("","YES","NO","DARK_GREEN");
  }



#***********************************************************************
# PrintInterrupt
#
# Print callback for Interrupt logging.
#***********************************************************************
sub PrintInterrupt
    {
      (my $LogData) = @_;
      my $TimeStamp =  %{$LogData}->{'TimeStamp'};
      my $InterruptID = (%{$LogData}->{'Data'}->{'InterruptID'}) >> 4;
      my $IntIDName = $INTERRUPT{"a$InterruptID"};

      $GlobalData{'CurrInt'} = $IntIDName;
      $GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'StartTime'}  = $GlobalData{'OSInterruptStart'};
      $GlobalData{'FrameInterruptStat'}->{$IntIDName}->{'StartTime'} = $GlobalData{'OSInterruptStart'};

      $printStr = sprintf("%20s: INTERRUPT: 0x%0.4X (%s)",$TimeStamp,$InterruptID,$INTERRUPT{"a$InterruptID"});

      return ($printStr,"YES","NO","DARK_GREEN");
  }

#***********************************************************************
# PrintOSInterruptEnd
#
# Print callback for Interrupt logging.
#***********************************************************************
sub PrintOSInterruptEnd
  {
    (my $LogData) = @_;
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    $TimeStamp =~ s/(s|\.|\s)//g;

    if ($GlobalInterruptStart == 1)
    {
      if ($GlobalData{'FirstCntx'} == 0)
      {
        my $CurrPID = $GlobalData{'CurrPID'};
        my $IntIDName = $GlobalData{'CurrInt'};
        
        if ($IntIDName ne 'OTHER_INTERRUPT')
        {
          $StartTime =  $GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'StartTime'};
          $ExecTime = $TimeStamp - $StartTime;
          
          $GlobalData{'SlotInterruptStat'}->{$IntIDName}->{'ExecTime'}  += $ExecTime;
          $GlobalData{'SlotExecStat'}->{$CurrPID}->{'IntExecTime'}      += $ExecTime;
          $GlobalData{'FrameInterruptStat'}->{$IntIDName}->{'ExecTime'} += $ExecTime;
          $GlobalData{'FrameExecStat'}->{$CurrPID}->{'IntExecTime'}     += $ExecTime;
          
          $GlobalData{'TotInterruptStat'}->{$IntIDName}->{'ExecTime'}   += $ExecTime;
          $GlobalData{'TotExecStat'}->{$CurrPID}->{'IntExecTime'}       += $ExecTime;
        }
        else
        {
          $StartTime = $GlobalData{'OSInterruptStart'};
          $ExecTime = $TimeStamp - $StartTime;
          
          $GlobalData{'SlotInterruptStat'}->{'OTHER_INTERRUPT'}->{'ExecTime'}  += $ExecTime;
          $GlobalData{'SlotExecStat'}->{$CurrPID}->{'IntExecTime'}             += $ExecTime;
          $GlobalData{'FrameInterruptStat'}->{'OTHER_INTERRUPT'}->{'ExecTime'} += $ExecTime;
          $GlobalData{'FrameExecStat'}->{$CurrPID}->{'IntExecTime'}            += $ExecTime;
          
          $GlobalData{'TotInterruptStat'}->{'OTHER_INTERRUPT'}->{'ExecTime'}   += $ExecTime;
          $GlobalData{'TotExecStat'}->{$CurrPID}->{'IntExecTime'}              += $ExecTime;
        }
      }
    }
    $GlobalInterruptStart = 0;

    return ("","NO","YES","BLUE");
  }

#***********************************************************************
# PrintScalar
# 
# Standard print function for scalar
# Meant to be used for quickly setting up a new logpoint. 
# NOTE: This callback only prints one scalar.
#***********************************************************************
sub PrintScalar
  {
    (my $LogData) = @_;
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    my $printStr = "";

    foreach $DTP (keys %{$LogData->{'Data'}})
      {
	$Name  = %{$LogData}->{'PrintString'};
	$printStr =  sprintf("%20s: %s = %0.4X",$TimeStamp,$Name,%{$LogData->{'Data'}}->{$DTP});
      }
    return ($printStr,"YES","NO","BROWN");
  }
  
#***********************************************************************
# PrintRange
# 
# Standard print function for vectors
# Meant to be used for quickly setting up a new logpoint. 
#***********************************************************************
sub PrintRange
  {
    (my $LogData) = @_;
    my $TimeStamp =  %{$LogData}->{'TimeStamp'};
    
    my $printStr = "";

    $tmp = %{$LogData}->{'PrintString'};
    
    @DataToPrint = split('(\s+)',$tmp);
    foreach $DTP (@DataToPrint)
      {
	($vec,$type) = split(':',$DTP);
	
	if (exists %{$LogData}->{'Data'}->{$vec})
	  {
	    @dat = @{%{$LogData}->{'Data'}->{$vec}};
	    if ($#dat == -1)
	      {
		@dat[0] = %{$LogData}->{'Data'}->{$vec};
	      }
	    
	    $printStr = sprintf("%s%20s: %s\n",$printStr,$TimeStamp,$vec);
	    $printStr = sprintf("%s%20s  %s = \[\n",$printStr,"",$vec);
	    $lengthVec = length($vec)+4;
	    if ($type eq "HEX") 
	      {
		for ($i = 0; $i <= $#dat; $i++)
		  {
		    $printStr = sprintf( "%s%20s  %*s0x%0.4X,\n",$printStr,"",$lengthVec,"",$dat[$i]);
		  }
	      }
	    elsif ($type eq "DEC")
	      {
		for ($i = 0; $i <= $#dat; $i++)
		  {
		      $printStr = sprintf( "%s%20s  %*s%d,\n",$printStr,"",$lengthVec,"",$dat[$i]);
		    }
	      }
	    else
	      {
		for ($i = 0; $i <= $#dat; $i++)
		  {
		      $printStr = sprintf( "%s%20s  %*s%0.4X,\n",$printStr,"",$lengthVec,"",$dat[$i]);
		    }
	      }
	    $printStr = sprintf("%s%20s %*s\]\n",$printStr,"",$lengthVec,"");
	  }
      }
    return ($printStr,"YES","NO","BROWN");
  }

#***********************************************************************
# CheckAndTriggerLoadMeas
# 
# 
# Check if load meassurement should start.
#***********************************************************************
sub CheckAndTriggerLoadMeas
  {
    (my $event) = @_;
    
    if (not (($LoadMeasData{'Config'}->{'IsRunning'} or $LoadMeasData{'Config'}->{'IsStopped'})))
      {
	if ($event eq $LoadMeasData{'Config'}->{'LoadStartTrigger'})
	  {
	    $LoadMeasData{'Config'}->{'IsRunning'} = 1;
	    $LoadMeasData{'Config'}->{'IsStopped'} = 0;
	    print "Load meassurment started: $event\n";
	  }
      }
    elsif (($LoadMeasData{'Config'}->{'IsRunning'} and not $LoadMeasData{'Config'}->{'IsStopped'}))
      {
	if ($event eq $LoadMeasData{'Config'}->{'LoadEndTrigger'})
	  {
	    $LoadMeasData{'Config'}->{'IsRunning'} = 0;
	    $LoadMeasData{'Config'}->{'IsStopped'} = 1;
	    print "Load meassurment stopped: $event\n";
	  }
      }
  }

#***********************************************************************
# LoadMeassurementLog
# 
# 
# Add execution time to PID for load meassurement.
#***********************************************************************
sub LoadMeassurementLog
{
 (my $process,my $ExecTime,my $type) = @_;
 if ($GlobalData{'SlotFirst'} == 0)
   {
     if ($LoadMeasData{'Config'}->{'IsRunning'} == 1)
       {
	 $LoadMeasData{'TOTAL'}->{$type}->{$process} += $ExecTime;
       }
   }

}

#***********************************************************************
# UpdateSlotLoadMeassurement
# 
# 
# Add execution time to PID for load meassurement.
#***********************************************************************
sub UpdateSlotLoadMeassurement
{
  (my $TimeStamp, my $SlotLength) = @_;

  my @ExecTimes;
  my $printStr      = "";
  my $CurrPID       = $GlobalData{'CurrPID'};
  my $SlotStartTime = $GlobalData{'SlotExecStat'}->{$CurrPID}->{'StartTime'};
  
  $GlobalData{'SlotExecStat'}->{$CurrPID}->{'ExecTime'} += ($TimeStamp - $SlotStartTime);
  $GlobalData{'SlotExecStat'}->{$CurrPID}->{'StartTime'} = $TimeStamp;
  
  $printStr = sprintf("%s\tSlot length: %d us\n",$printStr,$SlotLength / 1000000);
  foreach $PID (sort keys %{$GlobalData{'SlotExecStat'}})
    {
      if (not (length($PID) == 0))
	      {
	        $ExecTime = $GlobalData{'SlotExecStat'}->{$PID}->{'ExecTime'};
          if (not $ExecTime == 0)
            {
	            $ExecTimeInt     = $GlobalData{'SlotExecStat'}->{$PID}->{'IntExecTime'};
	            $ExecTime        = $ExecTime - $ExecTimeInt;
	            $ExecTimes[$PID - $FIRST_PROCESS_NUMBER] = $ExecTime / 1000000;
	            LoadMeassurementLog($PID,$ExecTime,"PID");
	            $ExecTimePercent = ($ExecTime / $SlotLength) * 100;
	            $ExecTimeStr     = sprintf("%d",$ExecTime / 1000000);
              my $pidName = $ID_TO_PID{"a$PID"};
 	            $printStr = sprintf("%s\tPID: %-20s: Exec time: %5s us (%3.1f\%)\n",$printStr,$pidName,$ExecTimeStr,$ExecTimePercent);
	          }
	        $GlobalData{'SlotExecStat'}->{$PID}->{'ExecTime'}    = 0;
	        $GlobalData{'SlotExecStat'}->{$PID}->{'IntExecTime'} = 0;
	      }
      else
	      {
	        delete  $GlobalData{'SlotExecStat'}->{$PID};
	      }
    }

  my $IntExecTot = 0;;
  foreach $INT (sort keys %{$GlobalData{'SlotInterruptStat'}})
    {
      $IntExecTime = $GlobalData{'SlotInterruptStat'}->{$INT}->{'ExecTime'};
      $IntExecTot += $IntExecTime;
      $IntExecTimeStr = sprintf("%d",$IntExecTime / 1000000);
      LoadMeassurementLog($INT,$IntExecTime,"INT");
      $IntExecTimePercent = 0;
      if ($SlotLength ne "0")
	{
	  $IntExecTimePercent = ($IntExecTime / $SlotLength) * 100;
	}
      if (not $IntExecTime == 0)
	{
	  $printStr = sprintf("%s\tInt: %-20s: Exec time: %5s us (%3.1f\%)\n",$printStr,$INT,$IntExecTimeStr,$IntExecTimePercent);
	}
      $GlobalData{'SlotInterruptStat'}->{$INT}->{'ExecTime'} = 0;
    }

  
  if ($GlobalData{'SlotFirst'})
    {
      $GlobalData{'SlotFirst'} = 0;
      $printStr = "";
    }
  else
    {
      for ($i = 0; $i <= $LAST_PROCESS_NUMBER - $FIRST_PROCESS_NUMBER; $i++)
	{
	  if ($ExecTimes[$i] eq "")
	    {
	      $ExecTimes[$i] = 0;
	    }
	}
      if ($LoadMeasData{'Config'}->{'Excel'} == 1)
	{
	  WriteExcelExecData(($SlotLength,$TimeStamp / 1000000000, $IntExecTot / 1000000,@ExecTimes));
	}
    }
  return $printStr;
}

#***********************************************************************
# UpdateFrameLoadMeassurement
# 
# 
# Add execution time to PID for load meassurement.
#***********************************************************************
sub UpdateFrameLoadMeassurement
  {
    (my $TimeStamp, my $FrameLength) = @_;

    my $printStr = "";
    
    $GlobalData{'FrameCounter'}++;
    #Frame exex times
    $GlobalData{'FrameExecStat'}->{$CurrPID}->{'ExecTime'} += $TimeStamp - $GlobalData{'FrameExecStat'}->{$CurrPID}->{'StartTime'};
    $GlobalData{'FrameExecStat'}->{$CurrPID}->{'StartTime'} = $TimeStamp;
    
    if ($GlobalData{'FrameFirst'})
      {
	$printStr = sprintf("%s\nWARNING: FIRST FRAME IN LOG: DON'T TRUST THIS DATA",$printStr);
	$GlobalData{'FrameFirst'} = 0;
      }
    $printStr = sprintf("%s\n\tFrame length: %d us\n",$printStr,$FrameLength / 1000000);
    foreach $PID (sort keys %{$GlobalData{'FrameExecStat'}})
      {
	if (not (length($PID) == 0))
	  {
	    $ExecTime = $GlobalData{'FrameExecStat'}->{$PID}->{'ExecTime'};
	    if (not $ExecTime == 0)
	      {
		$ExecTimeInt = $GlobalData{'FrameExecStat'}->{$PID}->{'IntExecTime'};
		$ExecTime = $ExecTime - $ExecTimeInt;
		$ExecTimePercent = ($ExecTime / $FrameLength) * 100;
		$ExecTimeStr = sprintf("%d",$ExecTime / 1000000);
    my $pidName = $ID_TO_PID{"a$PID"};
    $printStr = sprintf("%s\tPID: %-20s: Exec time: %5s us (%3.1f\%)\n",$printStr,$pidName,$ExecTimeStr,$ExecTimePercent);
	      }
	    $GlobalData{'FrameExecStat'}->{$PID}->{'ExecTime'} = 0;
	    $GlobalData{'FrameExecStat'}->{$PID}->{'IntExecTime'} = 0;
	  }
	else
	  {
	    delete  $GlobalData{'FrameExecStat'}->{$PID};
	  }
      }
    foreach $INT (sort keys %{$GlobalData{'FrameInterruptStat'}})
      {
	$IntExecTime = $GlobalData{'FrameInterruptStat'}->{$INT}->{'ExecTime'};
	$IntExecTimeStr = sprintf("%d",$IntExecTime / 1000000);
	$IntExecTimePercent = 0;
	if ($FrameLength ne "0")
	  {
	    $IntExecTimePercent = ($IntExecTime / $FrameLength) * 100;
	      }
	if (not $IntExecTime == 0)
	  {
	    $printStr = sprintf("%s\tInt: %-20s: Exec time: %5s us (%3.1f\%)\n",$printStr,$INT,$IntExecTimeStr,$IntExecTimePercent);
	  }
	$GlobalData{'FrameInterruptStat'}->{$INT}->{'ExecTime'} = 0;
      }
    return $printStr;
  }

#***********************************************************************
# GetLoadMeassurementLog
# 
# 
# Print load meassurement log.
# TODO: This should be done from busmon_parse.pl. Atleast a callback 
#       should be called from there.
#***********************************************************************
sub GetLoadMeassurementLog
  {
    my $tot = 0;
    my $tot_pid = 0;
    my $tot_int = 0;
    my $LoadMeasStr = "";
   
    foreach $PID (sort keys %{$LoadMeasData{'TOTAL'}->{'PID2'}})
      {
	$dat = %{$LoadMeasData{'TOTAL'}->{'PID2'}}->{$PID};
	$tot +=$dat;
	$tot_pid +=$dat;
      }
    foreach $INT (sort keys %{$LoadMeasData{'TOTAL'}->{'INT2'}})
      {
        #FIXME!!! Sometimes first slot statistics becomes corrupted. This could result in empty PID/INT executing huge incorrect time.
      	if ($INT ne "")
      	{
		$dat = %{$LoadMeasData{'TOTAL'}->{'INT2'}}->{$INT};
		$tot +=$dat;
		$tot_int +=$dat;
	}
      }
    $pid_tot_per = sprintf("%.2f\%",($tot_pid/$tot)*100);
    $int_tot_per = sprintf("%.2f\%",($tot_int/$tot)*100);

    $tot_pid_str = sprintf("%d us",$tot_pid / 1000000);
    $tot_int_str = sprintf("%d us",$tot_int / 1000000);
    
    $LoadMeasStr = sprintf("***************************************************\n");
    $LoadMeasStr = sprintf("%sLoad statistics\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s***************************************************\n\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s----------------------------------------------------------------\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s%-30s%20s%10s\n",$LoadMeasStr,"PID","EXEC TIME","%");
    $LoadMeasStr = sprintf("%s----------------------------------------------------------------\n\n",$LoadMeasStr);
    foreach $PID (sort keys %{$LoadMeasData{'TOTAL'}->{'PID2'}})
      {
	$dat = %{$LoadMeasData{'TOTAL'}->{'PID2'}}->{$PID};
	$dat_str = sprintf("%s us",$dat / 1000000);
	$dat_per = sprintf("%.2f\%",($dat/$tot)*100);
	my $PIDName = $ID_TO_PID{"a$PID"};
	$LoadMeasStr = sprintf("%s%-30s%20s%10s\n",$LoadMeasStr,$ID_TO_PID{"a$PID"},$dat_str,$dat_per);
      }
    $LoadMeasStr = sprintf("%s----------------------------------------------------------------\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s%-30s%20s%10s\n\n\n",$LoadMeasStr,"SUM",$tot_pid_str,$pid_tot_per);
    $LoadMeasStr = sprintf("%s---------------------------------------------------\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s%-20s%20s%10s\n",$LoadMeasStr,"INT","EXEC TIME","%");
    $LoadMeasStr = sprintf("%s---------------------------------------------------\n\n",$LoadMeasStr);
    foreach $INT (sort keys %{$LoadMeasData{'TOTAL'}->{'INT2'}})
      {
      	if ($INT ne "")
      	{
		$dat = %{$LoadMeasData{'TOTAL'}->{'INT2'}}->{$INT};
		$dat_str = sprintf("%s us",$dat / 1000000);
		$dat_per = sprintf("%.2f\%",($dat/$tot)*100);
		$LoadMeasStr = sprintf("%s%-20s%20s%10s\n",$LoadMeasStr,$INT,$dat_str,$dat_per);
	}
      }
    $LoadMeasStr = sprintf("%s---------------------------------------------------\n",$LoadMeasStr);
    $LoadMeasStr = sprintf("%s%-20s%20s%10s\n\n",$LoadMeasStr,"SUM",$tot_int_str,$int_tot_per);
    
    $tot = sprintf("%d us",$tot / 1000000);
    $LoadMeasStr = sprintf("%s\nTotal exec time: $tot\n",$LoadMeasStr);
 
    if ($LoadMeasData{'Config'}->{'Excel'} == 1)
      {
	CloseExcelDocument();
      }
    #Some debug code
#    $GlobalData{'TotInterruptStat'}->{$IntIDName}->{'ExecTime'} += $ExecTime;
#    $GlobalData{'TotExecStat'}->{$CurrPID}->{'IntExecTime'}     += $ExecTime;
#    print  $GlobalData{'TotExecStat'}->{'13'}->{'IntExecTime'};
#   my $TotalTime = 0;
#   foreach $PID (sort keys %{$GlobalData{'TotExecStat'}})
#     {
#	$ExecTime = $GlobalData{'TotExecStat'}->{$PID}->{'ExecTime'}/1000000;

#	$IntExecTime = $GlobalData{'TotExecStat'}->{$PID}->{'IntExecTime'}/1000000;
#	$PIDExecTime = $ExecTime - $IntExecTime;
#	$TotalTime += $ExecTime;
#	printf "%s => %f - %f = %f\n",$ID_TO_PID{"a$PID"},$ExecTime,$IntExecTime,$PIDExecTime;
#     }

#   foreach $INT (sort keys %{$GlobalData{'TotInterruptStat'}})
#     {
#	printf "%s => %f\n",$INT,$GlobalData{'TotInterruptStat'}->{$INT}->{'ExecTime'}/1000000;
#     }

#   print "TotalTime $TotalTime\n";
    return $LoadMeasStr;     
  }


#**************************************************************************************************************
# UTILITY FUNCTIONS
#**************************************************************************************************************

#***********************************************************************
# TwoComplToDec
#***********************************************************************
sub TwoComplToDec 
  {
    (my $nbrBits,my $inData) = @_;
    
    #Determine <> 0
    $sign_inData = (1 << ($nbrBits-1) & $inData);
    $inData -= $sign_inData;
    
    if ($sign_inData > 0) 
      {
	$max_val = (1 << ($nbrBits - 1)) - 1;
	$inData = $inData - $max_val - 1;
      }
    return $inData;
  }
  
  sub SMFloatToDec
  {
    (my $inHex) = @_;
     my $sign     = -1 * ((0x8000 & $inHex) >> 15);
    if ($sign == 0)
      {
     	$sign = 1;
      }
    my $exponent = (0x7F00 & $inHex) >> 8;
    my $mantissa = (0x00FF & $inHex);
    my $outSMFloat;
    
    $exponent = 2**($exponent - 63);

    $mantissa = (256 + $mantissa) / 256;
    

    $outSMFloat = $sign * $exponent * $mantissa;
    
    return $outSMFloat;
  }

#**************************************************************************************************************
# REGISTER FUNCTIONS
#**************************************************************************************************************

#***********************************************************************
# RegisterStateListener
#***********************************************************************
sub RegisterStateListener
  {
    (my $StateListener) = @_;

    #Calculate END address
    my $StateAddress = hex(%{$StateListener}->{'StateAddress'});
    my $StateNumber = %{$StateListener}->{'State'};
    %{$StateListener}->{'LastWrite'} = ConvertToAddress($StateAddress,%{$StateListener->{'Data'}}->{'END'},"","ONLY_ADDR");
    $LastWrite = %{$StateListener}->{'LastWrite'};
    delete %{$StateListener->{'Data'}}->{'END'};
    
    #Calculate addresses to listen for
    foreach $addr (keys %{$StateListener->{'Data'}})
      {
	$value = %{$StateListener->{'Data'}}->{$addr};
	$newAddress = ConvertToAddress($StateAddress,$value,"","ONLY_ADDR");
	%{$StateListener->{'Address'}}->{$newAddress}->{'Name'} = $addr;
	%{$StateListener->{'Address'}}->{$newAddress}->{'Data'} = "";
	%{$StateListener->{'Data'}}->{$addr} = "";
      }
    
    #Put log information to parser in hash $State
    $StateAddress = sprintf("%.5X",$StateAddress);
    $State{$StateAddress}->{$StateNumber} = $StateListener;
  }

#***********************************************************************
# RegisterRegListener
#***********************************************************************
sub RegisterRegListener
  {
    (my $RegListener) = @_;
    
    #Calculate END address
    my $BaseAddress = hex(%{$RegListener}->{'BaseAddress'});
    %{$RegListener}->{'LastWrite'} = ConvertToAddress($BaseAddress,%{$RegListener->{'Data'}}->{'END'},"","ONLY_ADDR");
    $LastWrite = %{$RegListener}->{'LastWrite'};
    delete %{$RegListener->{'Data'}}->{'END'};
    #Calculate addresses to listen for
    foreach $addr (keys %{$RegListener->{'Data'}})
      {
	$value = %{$RegListener->{'Data'}}->{$addr};
	($type,%addrList) = ConvertToAddress($BaseAddress,$value,$addr);
	
	foreach $newAddress (%addrList)
	  {  
	    %{$RegListener->{'Address'}}->{$newAddress}->{'Name'} = $addrList{$newAddress};
	    %{$RegListener->{'Address'}}->{$newAddress}->{'Data'} = "";
	    $Reg{'RegLogSpace'}->{$newAddress} = $RegListener;
	  }
	
	if ($type eq "ARRAY")
	  {
	    %{$RegListener->{'Data'}}->{$addr} = \@;
	  }
	else
	  {
	    %{$RegListener->{'Data'}}->{$addr} = "";
	  }
      }
  }

#***********************************************************************
# ConvertToAddress
#
# Converts offsets, address ranges and addresses specified in log 
# definitions to real addresses.
#***********************************************************************
sub ConvertToAddress
  {
    ($base,$value,$VarName,$END) = @_;
    
    my %Addresses;
    my $AddrInHex;
    if ($value =~ /^r_/)
      { 
	$value =~ /^r_(\S*)_(\S*)_(\S*)$/;
	$StartAddr = hex($1);
	$EndAddr = hex($2);
	$Intervall = $3;
	for ($i = $StartAddr; $i <= $EndAddr; $i += $Intervall)
	  {
	    $index = ($i - $StartAddr) / $Intervall;
	    $AddrInHex = sprintf("%0.5X",$i);
	    $Name = sprintf("%s___%d",$VarName,$index);
	    $Addresses{$AddrInHex} = $Name;
	  }
	$type = "ARRAY";
      }
    elsif ($value =~ /^a_/)
      {
	$value =~ /^a_(\S*)$/;
	$i = hex($1);
	$AddrInHex = sprintf("%0.5X",$i);
	 $Addresses{$AddrInHex} = $VarName;
	$type = "SCALAR";
      }
    else
      {
	$AddrInHex = sprintf("%.5X",$base + $value);
	$Addresses{$AddrInHex} = $VarName;
	$type = "SCALAR";
      }
    if ($END eq "ONLY_ADDR")
      {
	return $AddrInHex;
      }
    else
      {
	return ($type,%Addresses);
      }
  }


#**************************************************************************************************************
# EXCEL FUNCTIONS
#**************************************************************************************************************
sub OpenExcelDocument
  {
    (my $ExcelFileName) = @_;

    my $LastColumn = ord("A") + ($LAST_PROCESS_NUMBER - $FIRST_PROCESS_NUMBER + 3);
    my $LastColumn = chr($LastColumn);
    my $PIDPosition = $PID_PROCESS_ID- $FIRST_PROCESS_NUMBER;
    
    $GlobalData{'ExcelLastColumn'} = $LastColumn;
    $GlobalData{'PIDPosition'}     = $PIDPosition;
    print "$PIDPosition\n";

#Nohau    use Win32::OLE qw(in with);
#Nohau    use Win32::OLE::Const 'Microsoft Excel';
#Nohau    use Win32::OLE::Variant;
#Nohau    use Win32::OLE::NLS qw(:LOCALE :DATE);
  #  use Win32::OLE;
  #  use Win32::OLE::Variant;

    if (-e $ExcelFileName)
      {
	unlink -f $ExcelFileName;
	print "Deleted file $ExcelFileName\n";
      }

    # use existing instance if Excel is already running
    eval {$Excel = Win32::OLE->GetActiveObject('Excel.Application')};
    die "Excel not installed" if $@;
    
    unless (defined $Excel)
      {
	$GlobalData{'ExcelWasStarted'} = 1;
	$Excel = Win32::OLE->new('Excel.Application', sub {$_[0]->Quit;})
	  or die "Failed to start Excel";
     }
    $Excel-> {'DisplayAlerts'} = 0;
    $Excel-> {'Visible'} = 0;
    

    $Excel->{'SheetsInNewWorkbook'} = 1;
    my $Book  = $Excel->Workbooks->Add;
    $Book->SaveAs($ExcelFileName);
    $Book = $Excel->Workbooks->Open($ExcelFileName);
    
    my $Sheet = $Book->Worksheets(1);
    $Sheet->{'Name'} = "Execution times";
    

    my $Range = $Sheet->Range(sprintf("A1:%s1",$LastColumn));
    my @PIDNames = GetPIDNamesInString();
    @PIDNames = MoveIndexToLast($PIDPosition,@PIDNames);
    $Range->{'Value'} = [("SlotLength","Timestamp","Interrupts",@PIDNames)];
    $Range->Font->{'Bold'} = 1;
    $GlobalData{'ExcelLineNumber'} = 2;

    # $Sheet->Columns("A:".$LastColumn)->{'NumberFormat'} = "Number";
    


    
    #Save handles
    $GlobalData{'ExcelBookHandle'}  = $Book;
    $GlobalData{'ExcelHandle'}      = $Excel;
    $GlobalData{'ExcelSheetHandle'} = $Sheet;

  }

sub CloseExcelDocument
  {
    #Todo: Prio 2: Move those LoadMeasData->Excel
    my $Book          = $GlobalData{'ExcelBookHandle'};
    my $Excel         = $GlobalData{'ExcelHandle'};
    my $Sheet         = $GlobalData{'ExcelSheetHandle'};
    my $NumberOfLines = $GlobalData{'ExcelLineNumber'} - 1;
    my $LastColumn    = $GlobalData{'ExcelLastColumn'};
    my $BarsPerPage   = $LoadMeasData{'Config'}->{'BarsPerPage'};
    my $SheetName     = "";
    my $range;

    print "Total number of Slot in log: $NumberOfSlots\n";
    #Create chart, bars

    
    my $LastPageRange = ($NumberOfLines - 1) % $BarsPerPage;
    my $LastChunk = $NumberOfLines - $LastPageRange;
    if ($LastChunk > 1)
      {
	#Todo: Prio 2: Move chart stuff to sub
	$SheetName = sprintf("Slot %d - %d",$LastChunk,$LastChunk + $LastPageRange);
	$range = sprintf("C1:%s1;C%d:$LastColumn%d",$LastColumn,$LastChunk,$LastChunk+$LastPageRange);
	AddExcelChart($Excel, $Sheet, $range, $Chart, xlColumnStacked,$SheetName);

	$Book->Save;
	my $i;
	for ($i = $LastChunk; $i > 1; $i -= 100)
	  {
	    if ($i - 100 == 1)
	      {
		$range = sprintf("C1:%s1;C%d:$LastColumn%d",$LastColumn,$i - 99,$i);
		$SheetName = sprintf("Slot %d - %d",$i-99,$i);
	      }
	    else
	      {
		$range = sprintf("C1:%s1;C%d:$LastColumn%d",$LastColumn,$i - 100,$i);
		$SheetName = sprintf("Slot %d - %d",$i-100,$i);
	      }

	    AddExcelChart($Excel, $Sheet, $range, $Chart, xlColumnStacked,$SheetName);
	  }
      }

    
    
    $range = sprintf("C1:%s1;C2:%s%d",$LastColumn,$LastColumn,$NumberOfLines-1);
    AddExcelChart($Excel, $Sheet, $range, $Chart, xlAreaStacked, "DSP Load Overview");
    
    $Book->Save;
    $Book->Close();

    #Close excel if it wan't running.
    if ($GlobalData{'ExcelWasStarted'})
      {
	$Excel->Quit;
      }
    else
      {
	$Excel-> {'Visible'} = 1;
	
      }
  }
    
sub WriteExcelExecData
  {
    (my @ExecTimes) = @_;

    my $Book        =  $GlobalData{'ExcelBookHandle'};
    my $Excel       =  $GlobalData{'ExcelHandle'};
    my $Sheet       =  $GlobalData{'ExcelSheetHandle'};
    my $LineNumber  =  $GlobalData{'ExcelLineNumber'};
    my $LastColumn  =  $GlobalData{'ExcelLastColumn'};
    my $PIDPosition =  $GlobalData{'PIDPosition'};

    @ExecTimes = MoveIndexToLast($PIDPosition + 3,@ExecTimes);
    
    my $Range = $Sheet->Range(sprintf("A%d:%s%d", $LineNumber,$LastColumn,$LineNumber));

    #print "ExcelWrite: A$LineNumber:B$LineNumber\n";
    #print "@ExecTimes\n";
    
    $Range->{'Value'} = \@ExecTimes;
    
    $GlobalData{'ExcelLineNumber'}++;

    #To slow
    #$Book->Save;
  }

sub GetPIDNamesInString
  {
    my @PIDNames;
    my $i = 0;
    foreach $PID (sort keys %ID_TO_PID)
      {
	@PIDNames[$i++] = $ID_TO_PID{$PID};
      }
    return @PIDNames;
  }

sub MoveIndexToLast 
  {
    (my $index, my @inVec) = @_;

    my $tmp = $inVec[$#inVec];
    $inVec[$#inVec] = $inVec[$index];
    $inVec[$index] = $tmp;
    return @inVec;
  }

sub AddExcelChart
  {
    (my $Excel, my $Sheet, my $range, my $Chart, my $ChartType, my $Name) = @_;
    my $Range = $Sheet->Range($range);
    
    my $Chart = $Excel->Charts->Add;
    $Chart->{'ChartType'} = $ChartType;
    $Chart->SetSourceData({Source => $Range, PlotBy => xlColumns});
    $Chart->{'HasTitle'} = 1;
    $Chart->{'Name'} = $Name;
    $Chart->ChartTitle->{'Text'} = $Name;
  }
1;




