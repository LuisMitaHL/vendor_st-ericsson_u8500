
//contains
contains  h264dec.arm_nmf.parser as parser
contains  h264dec.arm_nmf.decoder as decoder
contains  common.nmf.arm as video_common


//itf
provides h264dec.arm_nmf.api.nal                    as input
provides h264dec.arm_nmf.api.output_buffer          as output_buffer
requires h264dec.arm_nmf.api.picture                as output
requires h264dec.arm_nmf.api.nal_handshake          as nal_ctl
provides h264dec.arm_nmf.api.set_decoder_parameter  as set_decoder_parameter
provides h264dec.arm_nmf.api.set_parser_parameter   as set_parser_parameter
requires h264dec.arm_nmf.api.pendingCommandAck      as pendingCommandAck
provides h264dec.arm_nmf.api.decoder_trace_init     as decoder_trace_init
provides h264dec.arm_nmf.api.error_recovery         as error_recovery

//internal binding
binds this.input                    to parser.input
binds parser.output                 to decoder.input    asynchronous fifo=4
binds this.output_buffer            to decoder.output_buffer
binds this.set_decoder_parameter    to decoder.set_decoder_parameter
binds this.set_parser_parameter     to parser.set_parser_parameter
binds this.decoder_trace_init               to parser.decoder_trace_init_in
binds parser.decoder_trace_init_out             to decoder.decoder_trace_init asynchronous fifo=1
binds this.error_recovery           to decoder.error_recovery

binds decoder.output to this.output 

// Handshake
binds decoder.nal_ctl           to parser.nal_ctl_in asynchronous fifo=4
binds parser.nal_ctl_out        to this.nal_ctl
binds decoder.pendingCommandAck to this.pendingCommandAck
binds parser.pendingCommandAck  to this.pendingCommandAck

// OST trace
provides h264dec.mpc.api.trace_init					as iTraceInitIn
requires h264dec.mpc.api.trace_init					as iTraceInitOut

binds this.iTraceInitIn to decoder.iTraceInitIn
binds decoder.iTraceInitOut to this.iTraceInitOut

// OST trace event messages
//contains debug.arm_nmf.event                as ost_event
//binds decoder.event                            to ost_event.event        
