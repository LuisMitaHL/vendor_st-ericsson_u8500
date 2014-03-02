// External interfaces
binds ddep.me                       to  ddep.postevent asynchronous fifo=1

provides sendcommand                        as  sendcommand
provides armnmf_emptythisbuffer             as  emptythisbuffer
provides armnmf_fillthisbuffer              as  fillthisbuffer
requires eventhandler                       as  proxy
requires armnmf_fillthisbuffer              as  inputport
requires armnmf_emptythisbuffer             as  outputport
provides h264dec.arm_nmf.api.set_config     as  set_config
provides h264dec.arm_nmf.api.set_configuration     as  set_configuration
requires h264dec.arm_nmf.api.get_config     as  get_config
provides h264dec.arm_nmf.api.error_recovery as  error_recovery

binds this.sendcommand              to  ddep.sendcommand
binds ddep.proxy                    to  this.proxy
binds this.emptythisbuffer          to  ddep.emptythisbuffer
binds ddep.inputport                to  this.inputport
binds this.fillthisbuffer           to  ddep.fillthisbuffer
binds ddep.outputport               to  this.outputport

binds this.set_config               to ddep.set_config
binds this.set_configuration        to ddep.set_configuration
binds ddep.get_config               to this.get_config
binds this.error_recovery           to ddep.error_recovery_in

// Internal interfaces - should be asynchronous to avoid re-entrency of primitive components
binds h264dec.output                to  ddep.output asynchronous fifo=100
binds h264dec.nal_ctl               to  ddep.nal_ctl asynchronous fifo=100
binds h264dec.pendingCommandAck     to  ddep.pendingCommandAck asynchronous fifo=50
binds ddep.nal                      to  h264dec.input asynchronous fifo=10
binds ddep.output_buffer            to  h264dec.output_buffer asynchronous fifo=100
binds ddep.set_decoder_parameter    to  h264dec.set_decoder_parameter asynchronous fifo=25
binds ddep.set_parser_parameter     to  h264dec.set_parser_parameter asynchronous fifo=1
binds ddep.error_recovery_out       to  h264dec.error_recovery  asynchronous fifo=1

provides fsminit                    as  fsminit
binds this.fsminit                  to  ddep.fsminit

// OST trace
provides h264dec.mpc.api.trace_init					as iTraceInit
binds    this.iTraceInit                    to h264dec.iTraceInitIn
binds    h264dec.iTraceInitOut                     to this.iTraceInitOut

requires h264dec.mpc.api.trace_init					as iTraceInitOut      optional

binds ddep.decoder_trace_init            to h264dec.decoder_trace_init asynchronous fifo=1

option CPPFLAGS "-g"
