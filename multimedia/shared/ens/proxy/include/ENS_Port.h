/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Port.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_PORT_H_
#define _ENS_PORT_H_

#include "ENS_Redefine_Class.h"

#include "OMX_Component.h"
#include "ENS_Component.h"
#include "mmhwbuffer.h"

class ENS_Component;
class ENS_Port_Fsm;
class EnsCommonPortData;

/// Pointer to a ENS_Port
typedef ENS_Port * ENS_Port_p;


//Specify buffer location
typedef enum {
    ENS_PORT_BUFFER_ALLOCATED   =1,
    ENS_PORT_BUFFER_ATCOMPONENT =2,
    ENS_PORT_BUFFER_ATPROCESSINGCOMP       =4,
    ENS_PORT_BUFFER_ATNEIGHBOR  =8,
	ENS_PORT_BUFFER_ATSHARINGPORT = 16			// To keep track of buffer give & take among internally connected ports.
} ENS_PORT_BUFFER_LOCATIONTYPE;

//Specify Role of Port
typedef enum {
    /// @brief Port is a supplier only. It reuses buffers provided by other port
    /// on same component. It is "NOT" an Allocator.
    ENS_SHARING_PORT,
    /// @brief Port is acting as a Non supplier. Its gets buffers from its tunneled port
    ENS_NON_SUPPLIER_PORT,
    /// @brief Port is an allocator of its own buffers & also supplies buffers to tunneled and sharingport.
    ENS_ALLOCATOR_PORT,
	/// @brief Port is non-tunneled.
	ENS_NON_TUNNELED_PORT
} ENS_PORT_BUFFER_SUPPLY_ROLE;


/// @brief Buffer Header table that is used to store Buffer 
/// header along with buffer current location
/// @allmedia
typedef struct ENS_BUFFERHEADER_TABLETYPE {
	/// @brief Buffer header pointer
	OMX_BUFFERHEADERTYPE * mBufHdrPtr;
	/// @brief Buffer flags to store current status of buffer
	OMX_U32 mBufHdrStatus;
	/// @brief Count to keep record of a shared buffer sent to multiple shairng ports.
	OMX_U32 mAtSharingPorts;
} ENS_BUFFERHEADER_TABLETYPE;

/// Ports of same component which are internally connected are referred to InterConnectecd ports here
/// and are sharing buffers with each other.
typedef struct ENS_INTER_CONNECTED_PORT_INFO {
    ENS_Port_p * mBufSharingPorts;
    OMX_U32 numPorts;
}ENS_INTER_CONNECTED_PORT_INFO;

/// @defgroup ens_port_class ENS Ports
/// Abstract base class for ports of ENS components
/// @ingroup ens_component_class
/// @{

class ENS_Port: public TraceObject {
    public:
        //----------------------------------------------------------------
        //      Constructors and Destructors
        //----------------------------------------------------------------
        
        ENS_API_IMPORT ENS_Port(
                const EnsCommonPortData& commonPortData, 
                ENS_Component &enscomp);

        ENS_API_IMPORT virtual ~ENS_Port(void) = 0;
        
        //----------------------------------------------------------------
        //      Getters and Setters
        //----------------------------------------------------------------
        
        inline ENS_Port_Fsm * getPortFsm(void) const;

        inline OMX_BOOL isPopulated(void) const;

        inline OMX_DIRTYPE getDirection(void) const;
        
        inline OMX_VIDEO_PORTDEFINITIONTYPE *getVideoPortDefinition(void);
        inline OMX_IMAGE_PORTDEFINITIONTYPE *getImagePortDefinition(void);

        inline OMX_U32  getPortIndex(void) const;
        
        inline OMX_BUFFERSUPPLIERTYPE getSupplierPreference(void) const;

        inline void setSupplierPreference(
                const OMX_BUFFERSUPPLIERTYPE eBufferSupplier);

		/// Set/Get the bit flags that will be used for tunneling
        inline void setTunnelFlags(const OMX_U32 nTunnelFlags);
        inline OMX_U32 getTunnelFlags(void) const;

        inline OMX_BOOL isBufferSupplier(void)  const;

        inline OMX_BUFFERSUPPLIERTYPE getBufferSupplier(void) const;
        
        inline void setBufferSupplier(
                const OMX_BUFFERSUPPLIERTYPE eBufferSupplier);

        inline OMX_BOOL isEnabled(void) const;

        inline void setEnabledFlag(const OMX_BOOL enabled);

        inline void setTunneled(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort,
                OMX_U32 nTunnelFlags);

        inline OMX_HANDLETYPE getTunneledComponent(void) const;
        inline OMX_U32 getTunneledPort(void) const;

        inline OMX_PORTDOMAINTYPE getDomain() const;

        inline OMX_U32 getBufferCountActual(void);

        inline OMX_U32 getBufferCountCurrent(void);

        /// Return OMX_TRUE if standard Tunneling is used for this port
        inline OMX_BOOL useStandardTunneling(void) const;

        /// Return OMX_TRUE if proprietary communication is used for this port
        inline OMX_BOOL useProprietaryCommunication(void) const;

        // Returns OMX component handle.
        inline ENS_Component&       getENSComponent(void);
        inline const ENS_Component& getENSComponent(void) const;

        // Returns OMX component handle.
        inline OMX_HANDLETYPE getComponent(void) const;

        // Get pointer to the ENS Tunneled port
        inline ENS_Port * getENSTunneledPort(void) const;

        // to update port settings from within the component
        // for eg when stream was parsed or to implement slaving behavior
        ENS_API_IMPORT OMX_ERRORTYPE updateSettings(
                OMX_INDEXTYPE nParamIndex,  
                OMX_PTR pComponentParameterStructure);
        
        // in case proprietary communication is supported, force
        // standard tunneling instead. For testing purposes.
        inline void forceStandardTunneling();

	// Only used by NmfHostMpc_ProcessingComponent derived classes
	inline bool isMpc(void) const;
	inline void setMpc(const bool mpc);
	
        //----------------------------------------------------------------
        //      Methods called from port FSM
        //----------------------------------------------------------------
        
        ENS_API_IMPORT virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,  
                OMX_PTR pComponentParameterStructure) const;

        ENS_API_IMPORT virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes,
                OMX_U8* pBuffer);

        ENS_API_IMPORT OMX_ERRORTYPE allocateBuffer(
                OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_PTR pAppPrivate,
                OMX_U32 nSizeBytes);

        ENS_API_IMPORT OMX_ERRORTYPE freeBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

        OMX_ERRORTYPE tunnelRequest(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort,
                OMX_TUNNELSETUPTYPE* pTunnelSetup);

        //Update Buffer location to MPC or Neighbor
        ENS_API_IMPORT OMX_BOOL isAllBuffersAtOwner();
        ENS_API_IMPORT OMX_BOOL isAllBuffersReturnedFromNeighbor(void);
		ENS_API_IMPORT OMX_BOOL isAllBuffersReturnedFromSharingPorts();
        ENS_API_IMPORT void bufferSentToProcessingComponent(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT void bufferReturnedFromProcessingComp(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT void bufferSentToNeighbor(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT void bufferReturnedFromNeighbor(OMX_BUFFERHEADERTYPE* pBuffer);
		

        /// These methods are only called when port is buffer supplier
        /// in order to init/reset standard tunneling procedure.
        ENS_API_IMPORT OMX_ERRORTYPE standardTunnelingInit(void);
        ENS_API_IMPORT OMX_ERRORTYPE standardTunnelingDeInit(void);

		

        /// These method is called when port is transitioning 
        /// from idle to executing in order to initialize standard tunneling 
        /// data transfert.
        ENS_API_IMPORT OMX_ERRORTYPE standardTunnelingTransfertInit(void);

        inline MMHwBuffer *getSharedChunk() const               { return mSharedChunk; }
        inline void setSharedChunk(MMHwBuffer *sharedChunk)     { 
            if (sharedChunk) DBC_ASSERT(mSharedChunk==0 ? 1 : mSharedChunk==sharedChunk);
            mSharedChunk = sharedChunk;
        }
        
        /// @brief Check whether or not an event handler must be sent on a change of 
        /// OMX_PARAM_PORTDEFINITIONTYPE value
        /// @param[in] pOld old value. New one can be taken from the port itself, in mParamPortDefinition
        /// @return OMX_TRUE if the event handler must be raised. OMX_FALSE otherwise
        ENS_API_IMPORT virtual OMX_BOOL mustSendEventHandler(const OMX_PARAM_PORTDEFINITIONTYPE *pOld);
        
        inline OMX_PARAM_PORTDEFINITIONTYPE getValueParamPortDefinition(void);
        
        //----------------------------------------------------------------

    protected:
        /// OpenMAX IL port definition structure
        OMX_PARAM_PORTDEFINITIONTYPE mParamPortDefinition;

        //----------------------------------------------------------------
        /// Following methods must be implemented by derived classes.
        //----------------------------------------------------------------
        
        /// When the IL Client calls SetParameter with 
        /// OMX_IndexParamPortDefinition index, this method is called 
        /// to set the "format" field in the OMX_PARAM_PORTDEFINITIONTYPE.
        virtual OMX_ERRORTYPE setFormatInPortDefinition(
                const OMX_PARAM_PORTDEFINITIONTYPE& portDef) = 0;

        virtual OMX_ERRORTYPE proprietaryCommunicationRequest(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort) { return OMX_ErrorNotImplemented; }

        virtual OMX_ERRORTYPE checkCompatibility(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort) const = 0; 

        //Get index of Buffer in buffer header table
        OMX_ERRORTYPE getBufferHdrTableIndex(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32& Index);
        inline ENS_BUFFERHEADER_TABLETYPE * getEnsBufferHdrTablePtr(void);
        
    private:
        ENS_Port_Fsm *          mPortFsm;

        ENS_Component &         mEnsComponent;
        
        OMX_HANDLETYPE          mTunneledComponent;
        OMX_U32                 mTunneledPort;
        OMX_U32                 mTunnelFlags;

        OMX_BUFFERSUPPLIERTYPE  mBufferSupplier;
        OMX_BUFFERSUPPLIERTYPE  mSupplierPreference; 
        OMX_BOOL                mUseStandardTunneling;
        bool                    mIsBufferAllocator;
		bool                    mIsMpc;
	
		ENS_BUFFERHEADER_TABLETYPE * mBufHdrTablePtr;
        OMX_U32                 mBufferCountCurrent;

        MMHwBuffer *mSharedChunk;
                
        //----------------------------------------------------------------
        // Private methods
        //----------------------------------------------------------------

        OMX_BUFFERHEADERTYPE * allocateBufferHeader(
                OMX_PTR pAppPrivate,OMX_U32 nSizeBytes);

        void freeBufferHeader(OMX_BUFFERHEADERTYPE *pBufferHdr);

        inline ProcessingComponent & getProcessingComponent() 
        { return mEnsComponent.getProcessingComponent(); }

        inline const ProcessingComponent & getProcessingComponent() const
        { return mEnsComponent.getProcessingComponent(); }

	//-----------------------------------------------------------//
	//		Buffer Sharing related methods & data members
	//-----------------------------------------------------------//

	private:

		ENS_INTER_CONNECTED_PORT_INFO mBufSharingPortList;
		OMX_BOOL  mIsAllocator;

		OMX_ERRORTYPE getSharingReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const;

		OMX_ERRORTYPE setSharingReqs(OMX_U32 bufferCountActual);

		OMX_ERRORTYPE forwardBuffer(OMX_BUFFERHEADERTYPE **ppBufferHdr,OMX_PTR pAppPrivate,
									OMX_U32 bufferSize, OMX_U8* pBuffer,
									void *bufferAllocInfo,void *portPrivateInfo);

	public:
		OMX_ERRORTYPE calculateBufferReqs(OMX_U32 *pMaxBufferSize, OMX_U32 *pMaxBufferCount) const;

		OMX_ERRORTYPE sendBufferReqs(OMX_INDEXTYPE nParamIndex,
                                             OMX_PTR pComponentParameterStructure);

		void bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
		OMX_BOOL bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
		OMX_ERRORTYPE bufferSharingDeinit();
	
	
		OMX_ERRORTYPE forwardInputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr);
		OMX_ERRORTYPE forwardOutputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr);
	
		OMX_ERRORTYPE mapSharedBufferHeader(OMX_BUFFERHEADERTYPE* pBuffer,
														  OMX_BUFFERHEADERTYPE** ppLocalBuffer);
		OMX_U32 getNumberInterConnectedPorts() const{
			return mBufSharingPortList.numPorts;
		}
		
		OMX_ERRORTYPE useSharedBuffer(OMX_PTR pAppPrivate,
									  OMX_U32 nSizeBytes,
									  OMX_U8* pBuffer,
									  void *pSharedChunk);
		
		OMX_BOOL isSharingPort(){
			if(getPortRole() == ENS_SHARING_PORT)
				return OMX_TRUE;
			else
				return OMX_FALSE;
		}

		OMX_BOOL isBufferSharingPort() const{
			if( mBufSharingPortList.numPorts) 
				return OMX_TRUE;
			else
				return OMX_FALSE;
		}

		OMX_BOOL isBufferSharingEnabled(){
			if(mBufSharingPortList.numPorts==0) {
				return OMX_FALSE;
			}
			else
				return OMX_FALSE;
		}

		void setBufferSharingPortList(ENS_Port_p *portList, OMX_U32 numPorts){
			mBufSharingPortList.mBufSharingPorts = portList;
			mBufSharingPortList.numPorts = numPorts;
		}

		ENS_PORT_BUFFER_SUPPLY_ROLE getPortRole() const{
			if(!isBufferSupplier()) {
				if(getTunneledComponent()) 
					return ENS_NON_SUPPLIER_PORT;
				else
					return ENS_NON_TUNNELED_PORT;
			}
			else{
				if(mIsAllocator) 
					return ENS_ALLOCATOR_PORT;
				else{
					DBC_ASSERT(getNumberInterConnectedPorts()!=0);
					return ENS_SHARING_PORT;
				}
			}
		}

		void setAllocatorRole(OMX_BOOL value){
			mIsAllocator = value;
		}

		OMX_BOOL isAllocatorPort() const{
		if(getPortRole() == ENS_ALLOCATOR_PORT)
			return OMX_TRUE;
		else
			return OMX_FALSE;

		}

		//Note: This flag will be removed in future
		bool BUFFER_SHARING_ENABLED;
		void bufferSharingEnabled();
};

#include "ENS_Port.inl"

/// Structure used to pass the common information that an ENS port needs
/// at instantiation time.
class  EnsCommonPortData{
    public:
        /// Port index
        OMX_U32 mPortIndex;
        /// Direction (input or output) of this port
        OMX_DIRTYPE mDirection;
        /// The minimum number of buffers this port requires
        OMX_U32 mBufferCountMin;
        /// Minimum size, in bytes, for buffers to be used for this port
        OMX_U32 mBufferSizeMin;
        /// Domain of the port
        OMX_PORTDOMAINTYPE mPortDomain;
        /// Port supplier preference when tunneling between two ports
        OMX_BUFFERSUPPLIERTYPE mBufferSupplierPref;

        // Constructor
        ENS_API_IMPORT EnsCommonPortData(
                OMX_U32 nPortIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferCountMin,
                OMX_U32 nBufferSizeMin,
                OMX_PORTDOMAINTYPE eDomain,
                OMX_BUFFERSUPPLIERTYPE eSupplierPref);
};




/// @}
#endif // _ENS_PORT_H_
