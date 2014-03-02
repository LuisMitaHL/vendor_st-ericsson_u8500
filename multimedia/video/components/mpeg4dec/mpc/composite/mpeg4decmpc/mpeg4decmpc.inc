
	contains mpeg4dec.mpc.ddep	as cMpeg4decDdep	domainSVACodec urgent
	contains fsm.generic		as cGenericFsm		domainSVACodec urgent
	contains fsm.component		as cComponentFsm	domainSVACodec urgent 
	//contains mmtrace 		as cTrace		domainSVACodec urgent
	contains osttrace.mmdsp		as cOSTTrace		domainSVACodec urgent
	contains nmf.mpc.shared_bufin   as cSharedBufInMPC	domainSVACodec urgent
	contains nmf.mpc.shared_bufout  as cSharedBufOutMPC	domainSVACodec urgent

	contains mpeg4dec.mpc.mpeg4dec	as cMpeg4decAlgo	domainSVACodec 
	contains resource_manager	as cResource_manager	domainSVACodec urgent
        contains vpp	                as cVpp	                domainSVACodec 
        
	// FSM Bindings
	binds cComponentFsm.osttrace to cOSTTrace.osttrace
	binds cMpeg4decDdep.osttrace  to cOSTTrace.osttrace
	//binds cComponentFsm.mmtrace to cTrace.mmtrace
	binds cComponentFsm.genericfsm to cGenericFsm.genericfsm
	binds cMpeg4decDdep.genericfsm to cGenericFsm.genericfsm
	binds cMpeg4decDdep.componentfsm to cComponentFsm.componentfsm

	binds cMpeg4decDdep.me to cMpeg4decDdep.postevent			 asynchronous fifo=32
	binds cMpeg4decDdep.iSendLogEvent to cMpeg4decDdep.iGetLogEvent 	 asynchronous fifo=32

	// Shared Buffer Component bindings with ddep
	binds cSharedBufInMPC.mpc to cMpeg4decDdep.emptythisbuffer
	binds cMpeg4decDdep.inputport to cSharedBufInMPC.fillthisbuffer

	binds cSharedBufOutMPC.mpc to cMpeg4decDdep.fillthisbuffer
	binds cMpeg4decDdep.outputport to cSharedBufOutMPC.emptythisbuffer
	
	// bind algo with ddep
	binds cMpeg4decDdep.iCodecAlgo to cMpeg4decAlgo.iAlgo		          asynchronous fifo=32
        
	binds cMpeg4decDdep.iCodecUpdateAlgo to cMpeg4decAlgo.iUpdateAlgo
        
	// bind ddep with resource manager
	binds cMpeg4decDdep.iResource to cResource_manager.iResource		 asynchronous fifo=32
	binds cMpeg4decDdep.iSleep to cResource_manager.iSleep

	// bind Algo with resource manager
	binds cMpeg4decAlgo.iBuffer to cResource_manager.iBuffer
	binds cMpeg4decAlgo.iMtf to cResource_manager.iMtf
        binds cMpeg4decAlgo.iTcmAlloc to cResource_manager.iTcmAlloc
        binds cMpeg4decAlgo.iInterrupt to cResource_manager.iInterrupt
        
        binds cVpp.iInterrupt to cResource_manager.iInterrupt
        binds cMpeg4decDdep.vpp_algo to cVpp.iAlgo
        
