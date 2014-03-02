/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _ARMIVPROC_ENGINE_H
#define _ARMIVPROC_ENGINE_H

class  ImageStream;

class IMAGECONVERTER_API ArmIVProc_Engine
//*************************************************************************************
{ // Manage all operations need by ArmIVProc outside omx environment
public:
	ArmIVProc_Engine();
	~ArmIVProc_Engine();
	int ProcessImage(const _tImageBuffer &aInputBuffer, _tImageBuffer &OutBuffer);
	int Configure   (const _sImageFormat &aInput, const _sImageFormat &aOutput, eEnumRotation aRot, eEnumMirror aMirror);
	int Reset();//For reseting all  flags

	bool IsConfigurationOk() const {return(m_bConfigurationOk);}

	void SetRotation(eEnumRotation rot) { m_Rotation=rot; }
	eEnumRotation GetRotation() const { return(m_Rotation); }
protected:
	bool m_bNeedDownScale;
	bool m_bNeedRotation;
	bool m_bNeedBufferForRotation;
	bool m_bIsProcessing;
	bool m_bConfigurationOk;

	unsigned int m_InputFormat;

	_tImageBuffer m_InputBuffer;
	_tImageBuffer m_RotationBuffer;
	_tImageBuffer m_DownscaleBuffer;

	size_t m_Out_sizeX, m_Out_sizeY; //out size without rotation
	eEnumRotation m_Rotation;
	eEnumMirror   m_Mirroring;
};



#endif // End of #ifndef _ARMIVPROC_ENGINE_H
