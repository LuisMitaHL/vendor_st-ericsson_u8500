// External interfaces
binds ddep.me                       to  ddep.postevent asynchronous fifo=1

provides sendcommand         as  sendcommand
provides armnmf_emptythisbuffer     as  emptythisbuffer
provides armnmf_fillthisbuffer      as  fillthisbuffer
requires eventhandler        as  proxy
requires armnmf_fillthisbuffer      as  inputport
requires armnmf_emptythisbuffer     as  outputport
provides vc1dec.arm_nmf.api.set_config        as  set_config
provides vc1dec.arm_nmf.api.set_configuration        as  set_configuration
requires vc1dec.arm_nmf.api.get_config        as  get_config
//+ For ER 344032 
binds vc1dec.proxy to this.proxy
//- For ER 344032 
binds this.sendcommand              to  ddep.sendcommand
binds ddep.proxy                    to  this.proxy
binds this.emptythisbuffer          to  ddep.emptythisbuffer
binds ddep.inputport                to  this.inputport
binds this.fillthisbuffer           to  ddep.fillthisbuffer
binds ddep.outputport               to  this.outputport

binds this.set_config               to ddep.set_config
binds this.set_configuration               to ddep.set_configuration
binds ddep.get_config               to this.get_config

// Internal interfaces - should be asynchronous to avoid re-entrency of primitive components
binds vc1dec.output                 to  ddep.output asynchronous fifo=16
binds vc1dec.nal_ctl                to  ddep.nal_ctl asynchronous fifo=22
binds ddep.nal                      to  vc1dec.input asynchronous fifo=4
binds ddep.output_buffer            to  vc1dec.output_buffer asynchronous fifo=22
binds ddep.set_decoder_parameter    to  vc1dec.set_decoder_parameter asynchronous fifo=32

provides fsminit             as  fsminit
binds this.fsminit                  to  ddep.fsminit

// OST trace
binds ddep.decoder_trace_init            to vc1dec.decoder_trace_init asynchronous fifo=1
