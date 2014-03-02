
	contains mpeg2dec.mpc.ddep	as cMpeg2decDdep	domainSVACodec urgent
	contains fsm.generic		as cGenericFsm		domainSVACodec urgent
	contains fsm.component		as cComponentFsm	domainSVACodec urgent 
	//contains mmtrace 		as cTrace		domainSVACodec urgent
	contains osttrace.mmdsp		as cOSTTrace		domainSVACodec urgent
	contains nmf.mpc.shared_bufin   as cSharedBufInMPC	domainSVACodec urgent
	contains nmf.mpc.shared_bufout  as cSharedBufOutMPC	domainSVACodec urgent
									      
	contains mpeg2dec.mpc.mpeg2dec	as cMpeg2decAlgo	domainSVACodec
	contains resource_manager	as cResource_manager	domainSVACodec urgent
        contains vpp	                as cVpp	                domainSVACodec
        
	// FSM Bindings
	binds cComponentFsm.osttrace to cOSTTrace.osttrace
	 binds cMpeg2decDdep.osttrace  to cOSTTrace.osttrace
	//binds cComponentFsm.mmtrace to cTrace.mmtrace
	binds cComponentFsm.genericfsm to cGenericFsm.genericfsm
	binds cMpeg2decDdep.genericfsm to cGenericFsm.genericfsm
	binds cMpeg2decDdep.componentfsm to cComponentFsm.componentfsm

	binds cMpeg2decDdep.me to cMpeg2decDdep.postevent			 asynchronous fifo=8
	binds cMpeg2decDdep.iSendLogEvent to cMpeg2decDdep.iGetLogEvent 	 asynchronous fifo=8

	// Shared Buffer Component bindings with ddep
	binds cSharedBufInMPC.mpc to cMpeg2decDdep.emptythisbuffer
	binds cMpeg2decDdep.inputport to cSharedBufInMPC.fillthisbuffer

	binds cSharedBufOutMPC.mpc to cMpeg2decDdep.fillthisbuffer
	binds cMpeg2decDdep.outputport to cSharedBufOutMPC.emptythisbuffer
	
	// bind algo with ddep
	binds cMpeg2decDdep.iCodecAlgo to cMpeg2decAlgo.iAlgo  			 asynchronous fifo=8
	binds cMpeg2decDdep.iConfigureAlgo to cMpeg2decAlgo.iConfigureAlgo
	binds cMpeg2decDdep.iCodecUpdateAlgo to cMpeg2decAlgo.iUpdateAlgo
        
	// bind ddep with resource manager
	binds cMpeg2decDdep.iResource to cResource_manager.iResource		 asynchronous fifo=8
	binds cMpeg2decDdep.iSleep to cResource_manager.iSleep
	
	// bind Algo with resource manager
	binds cMpeg2decAlgo.iBuffer to cResource_manager.iBuffer
	binds cMpeg2decAlgo.iMtf to cResource_manager.iMtf
	binds cMpeg2decAlgo.iInterrupt to cResource_manager.iInterrupt
        
	binds cVpp.iInterrupt to cResource_manager.iInterrupt
	binds cMpeg2decDdep.vpp_algo to cVpp.iAlgo
        
