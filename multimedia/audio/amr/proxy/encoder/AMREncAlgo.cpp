/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Encoder Algo class
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "AMREncAlgo.h"
#include "cm/inc/cm_macros.h"
#include "AFM_nmf.h"

// AMR Memory need
static t_afm_mempreset memory_presets[5]=
{
	//                                 TCM  YTCM DDR24 DDR16 ESR24 ESR16
	{ MEM_PRESET_ALL_TCM,           {{ 1922,    0,    0,    0,    0,    0 }} },
	{ MEM_PRESET_ALL_DDR,           {{    0,    0, 1922,    0,    0,    0 }} },
	{ MEM_PRESET_ALL_ESRAM,         {{    0,    0,    0,    0, 1922,    0 }} },
	{ MEM_PRESET_MIX_DDR_TCM_1,     {{   80,    0, 1842,    0,    0,    0 }} },
	{ MEM_PRESET_MIX_ESRAM_OTHER_1, {{    0,    0,  580,    0, 1342,    0 }} }
};

OMX_ERRORTYPE AMREncAlgo::instantiateAlgoLibraries(void) {
	OMX_ERRORTYPE error;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "amr.codec.libamrenc24",
										 "libamrenc24", &mNmfAmrEnc24Lib, mMyPriority);
	if (error != OMX_ErrorNone) return error;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "amr.codec.libamrcommon",
										 "libamrcommon", &mNmfAmrCommonLib, mMyPriority);
	if (error != OMX_ErrorNone) return error;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "amr.codec.libamrenc",
										 "libamrenc", &mNmfAmrEncLib, mMyPriority);
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(*((t_cm_instance_handle*)mNmfMain), "libamrenc24", mNmfAmrEnc24Lib, "libamrenc24");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrEnc24Lib, "libamrenc", mNmfAmrEncLib, "libamrenc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrEnc24Lib, "libamrcommon", mNmfAmrCommonLib, "libamrcommon");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrEncLib, "libamrcommon", mNmfAmrCommonLib, "libamrcommon");
	if (error != OMX_ErrorNone) return error;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "malloc",
										 "malloc", &mNmfMalloc, mMyPriority);
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrEnc24Lib, "malloc", mNmfMalloc, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrEncLib, "malloc", mNmfMalloc, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrCommonLib, "malloc", mNmfMalloc, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "audiolibs.libbitstream",
										 "libbitstream", &mNmfBitstreamLib,mMyPriority);

	error = ENS::bindComponent(mNmfAmrEnc24Lib, "libbitstream", mNmfBitstreamLib, "libbitstream");
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(mNmfAmrCommonLib, "libbitstream", mNmfBitstreamLib, "libbitstream");
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::deInstantiateAlgoLibraries() {
	OMX_ERRORTYPE error;

	error = ENS::unbindComponent(*((t_cm_instance_handle*)mNmfMain), "libamrenc24");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEnc24Lib, "libamrcommon");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEnc24Lib, "libamrenc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEncLib, "libamrcommon");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEnc24Lib, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEncLib, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrCommonLib, "malloc");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrEnc24Lib, "libbitstream");
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfAmrCommonLib, "libbitstream");
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mNmfAmrEnc24Lib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mNmfAmrEncLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mNmfAmrCommonLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mNmfMalloc);
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mNmfBitstreamLib);
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
	OMX_ERRORTYPE error;

    setNMFDomainHandle(domainId);
    mMyPriority = priority;

    mNmfMain = new t_cm_instance_handle;

	error = ENS::instantiateNMFComponent(getNMFDomainHandle(), "amr.nmfil.encoder", "amrenc_nmfil", (t_cm_instance_handle*)mNmfMain, mMyPriority);
	if (error != OMX_ErrorNone) return error;

	error = instantiateAlgoLibraries();
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponentFromHostEx(*((t_cm_instance_handle*)mNmfMain), "setheap",  &mISetHeap, 6);
	if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponentFromHostEx(*((t_cm_instance_handle*)mNmfMain), "configure", &mIConfig, 2);
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::deInstantiate() {
	OMX_ERRORTYPE error;
	t_uint16 memory_bank;

	for (memory_bank=0;memory_bank<MEM_BANK_COUNT;memory_bank++)
	{
		if (mHeap[memory_bank])
		{
			ENS::freeMpcMemory(mHeap[memory_bank]);
			mHeap[memory_bank] = 0;
			mMemoryNeed.size[memory_bank] = 0;
		}
	}

	error = deInstantiateAlgoLibraries();
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponentFromHost( &mISetHeap);
	if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponentFromHost( &mIConfig);
	if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(*((t_cm_instance_handle*)mNmfMain));
	if (error != OMX_ErrorNone) return error;

    delete (t_cm_instance_handle*)mNmfMain;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::startAlgoLibraries() {
	OMX_ERRORTYPE error;

	error = ENS::startNMFComponent(mNmfAmrCommonLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::startNMFComponent(mNmfAmrEncLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::startNMFComponent(mNmfAmrEnc24Lib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::startNMFComponent(mNmfMalloc);
	if (error != OMX_ErrorNone) return error;

	error = ENS::startNMFComponent(mNmfBitstreamLib);
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::stopAlgoLibraries() {
	OMX_ERRORTYPE error;
	error = ENS::stopNMFComponent(mNmfAmrEnc24Lib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::stopNMFComponent(mNmfAmrEncLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::stopNMFComponent(mNmfAmrCommonLib);
	if (error != OMX_ErrorNone) return error;

	error = ENS::stopNMFComponent(mNmfMalloc);
	if (error != OMX_ErrorNone) return error;

	error = ENS::stopNMFComponent(mNmfBitstreamLib);
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::start() {
	OMX_ERRORTYPE error;

	error = ENS::startNMFComponent(*((t_cm_instance_handle*)mNmfMain));
	if (error != OMX_ErrorNone) return error;

	error = startAlgoLibraries();
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::stop() {
	OMX_ERRORTYPE error;

	error = ENS::stopNMFComponent(*((t_cm_instance_handle*)mNmfMain));
	if (error != OMX_ErrorNone) return error;

	error = stopAlgoLibraries();
	if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::setParameter(AmrEncParam_t amrNmfParams) {
    mMemoryNeed = memory_presets[amrNmfParams.memory_preset].memory_needs;
    amrNmfParams.memory_preset = memory_presets[amrNmfParams.memory_preset].mempreset;

    mIConfig.setParameter(amrNmfParams);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::setConfig(AmrEncConfig_t amrNmfConfig) {
    mIConfig.setConfig(amrNmfConfig);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMREncAlgo::configure() {
	OMX_ERRORTYPE error;
	t_uint16 memory_bank;
	t_uint32 memory_size;


	for (memory_bank = 0; memory_bank < MEM_BANK_COUNT ;memory_bank++) {
		if (mMemoryNeed.size[memory_bank]) {
			t_uint32 dsp_address;

			error = ENS::allocMpcMemory(getNMFDomainHandle(),
										AFM::memoryBank((t_memory_bank)memory_bank),
										mMemoryNeed.size[memory_bank], CM_MM_ALIGN_2WORDS,
										&mHeap[memory_bank]);
			if (error != OMX_ErrorNone) return error;

			CM_GetMpcMemoryMpcAddress(mHeap[memory_bank], &dsp_address);
			memory_size = mMemoryNeed.size[memory_bank];

			mISetHeap.setHeap((t_memory_bank)memory_bank,(void *) dsp_address,memory_size);
		} else {
			mHeap[memory_bank] = 0;
		}
	}
    return OMX_ErrorNone;
};

