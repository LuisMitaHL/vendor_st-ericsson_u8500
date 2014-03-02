
	contains jpegdec.mpc.ddep	as cJpegdecDdep		domainSVACodec urgent  
	contains fsm.generic		as cGenericFsm		domainSVACodec urgent
	contains fsm.component		as cComponentFsm	domainSVACodec urgent 
	//contains mmtrace 		as cTrace		domainSVACodec urgent
	contains osttrace.mmdsp		as cOSTTrace		domainSVACodec urgent
	contains nmf.mpc.shared_bufin   as cSharedBufInMPC	domainSVACodec urgent
	contains nmf.mpc.shared_bufout  as cSharedBufOutMPC	domainSVACodec urgent

	contains jpegdec.mpc.jpegdec	as cJpegdecAlgo		domainSVACodec
	contains resource_manager	as cResource_manager	domainSVACodec urgent

	
	// FSM Bindings
	binds cComponentFsm.osttrace to cOSTTrace.osttrace
        binds cJpegdecDdep.osttrace  to cOSTTrace.osttrace
	//binds cComponentFsm.mmtrace to cTrace.mmtrace
	binds cComponentFsm.genericfsm to cGenericFsm.genericfsm
	binds cJpegdecDdep.genericfsm to cGenericFsm.genericfsm
	binds cJpegdecDdep.componentfsm to cComponentFsm.componentfsm

	binds cJpegdecDdep.me to cJpegdecDdep.postevent			 asynchronous fifo=8
	binds cJpegdecDdep.iSendLogEvent to cJpegdecDdep.iGetLogEvent 	 asynchronous fifo=8

	// Shared Buffer Component bindings with ddep
	binds cSharedBufInMPC.mpc to cJpegdecDdep.emptythisbuffer
	binds cJpegdecDdep.inputport to cSharedBufInMPC.fillthisbuffer

	binds cSharedBufOutMPC.mpc to cJpegdecDdep.fillthisbuffer
	binds cJpegdecDdep.outputport to cSharedBufOutMPC.emptythisbuffer
	
	// bind algo with ddep
	binds cJpegdecDdep.iCodecAlgo to cJpegdecAlgo.iAlgo		asynchronous fifo=8
	binds cJpegdecDdep.iConfigureAlgo to cJpegdecAlgo.iConfigureAlgo
	binds cJpegdecDdep.iUpdateAlgo to cJpegdecAlgo.iUpdateAlgo
	
	// bind ddep with resource manager
	binds cJpegdecDdep.iResource to cResource_manager.iResource		 asynchronous fifo=8
	binds cJpegdecDdep.iSleep to cResource_manager.iSleep
	
	// bind Algo with resource manager
	binds cJpegdecAlgo.iBuffer to cResource_manager.iBuffer
	binds cJpegdecAlgo.iMtf to cResource_manager.iMtf
