/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include "SharedTools.h"
#include "ImageTools.h"
#include "ImageBuffer.h"
#include "ImageStream.h"
#include "ImageConverter.h"
#include "Armivproc_engine.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


ArmIVProc_Engine::ArmIVProc_Engine()
//*************************************************************************************
{
	Reset();
}

ArmIVProc_Engine::~ArmIVProc_Engine()
//*************************************************************************************
{
}


int ArmIVProc_Engine::Reset()
//*************************************************************************************
{
	m_bNeedDownScale         =false;
	m_bNeedRotation          =false;
	m_bNeedBufferForRotation =false;
	
	m_Out_sizeX= 0;
	m_Out_sizeY= 0;

	m_Rotation    = eRotation_None;
	m_Mirroring   = eMirror_None;
	m_InputFormat = 0;

	m_bIsProcessing          =false; //set during a process
	m_bConfigurationOk       =false; //Configuration need to be done
	return(S_OK);
}

int ArmIVProc_Engine::Configure(const _sImageFormat &aInput, const _sImageFormat &aOutput, eEnumRotation aRot, eEnumMirror aMirror)
//*************************************************************************************
{ // Call each time a new config is set 
	m_Rotation=aRot;
	int res=S_OK;
	//const unsigned int InternalPixelDepth=3;
	const enum_ImageFormat InternalFormat= eImageFormat_RGB888;
	m_bConfigurationOk=false;
	if (   (m_Rotation == eRotation_None)  || (m_Rotation == eRotation_180) )
	{ // no need to swap x,y resolution
		m_Out_sizeX = aOutput.SizeX;
		m_Out_sizeY = aOutput.SizeY;
	}
	else if (   (m_Rotation == eRotation_90)  || (m_Rotation == eRotation_270) )
	{ // Need to swap x,y resolution
		m_Out_sizeX = aOutput.SizeY;
		m_Out_sizeY = aOutput.SizeX;
	}
	else if (m_Rotation == eRotation_NotDefined)
	{ // Do nothing
		m_Out_sizeX = aOutput.SizeX;
		m_Out_sizeY = aOutput.SizeY;
	}
	else
	{ // Error
		ASSERT(0); // unsupported case
		_fnReportError(eError_UnsupportedRotationCase, " Rotation case not supported %d", m_Rotation);
		return(eError_UnsupportedRotationCase);
	}
	m_bNeedRotation=false;
	if ((m_Rotation != eRotation_None) && (m_Rotation != eRotation_NotDefined))
	{
		m_bNeedRotation=true;
		// allocate memory for doing the rotation
		m_RotationBuffer.SetFormat(InternalFormatYUV);
		size_t alloc=ComputeNeededBufferSize(InternalFormat, aOutput.SizeX* aOutput.SizeY);
		res=m_RotationBuffer.Allocate(alloc);
		if (res!=S_OK)
		{
			_fnReportError(eError_CannotAllocateBuffer, " Cannot allocate memory for Rotation need %u", alloc);
			return(eError_CannotAllocateBuffer);
		}
		res=m_RotationBuffer.SetImageSize(aOutput.SizeX, aOutput.SizeY);
		if (res!=S_OK)
		{
			_fnReportError(eError_CannotAllocateBuffer, " Cannot set image size for rotation %u", res);
			return(eError_CannotAllocateBuffer);
		}
	}

	// Mirroring
	m_Mirroring=aMirror;


	m_bNeedDownScale=false;
	if ( (aInput.SizeX < m_Out_sizeX) || (aInput.SizeY < m_Out_sizeY) )
	{ //Upscaling not supported
		bool  bIgnoreError=false;
		if (m_Rotation == eRotation_NotDefined)
		{ // try with 90° rotation
			m_Out_sizeY = aOutput.SizeX;
			m_Out_sizeX = aOutput.SizeY; //swap resolution
			if ( (aInput.SizeX >= m_Out_sizeX) && (aInput.SizeY >= m_Out_sizeY) )
			{ // Ok with rotation ignore 
				bIgnoreError=true;
			}
		}

		if (bIgnoreError==false)
		{
			_fnReportError(eError_UpScalingNotsupported, " Up Scaling not supported (%d, %d) -> (%d, %d)", aInput.SizeX, aInput.SizeY, aOutput.SizeX, aOutput.SizeY);
			return(eError_UpScalingNotsupported);
		}
	}
	if ( (aInput.SizeX != m_Out_sizeX) || (aInput.SizeY != m_Out_sizeY) )
	{ //Need Down scaling
		m_bNeedDownScale=true;
		// allocate memory for doing the rotation
		size_t alloc=ComputeNeededBufferSize(InternalFormat, aOutput.SizeX* aOutput.SizeY);
		m_DownscaleBuffer.SetFormat(InternalFormatYUV);
		res=m_DownscaleBuffer.Allocate(alloc);
		if (res!=S_OK)
		{
			_fnReportError(eError_CannotAllocateBuffer, " Cannot allocate memory for Downscaling need %u", alloc);
			return(eError_CannotAllocateBuffer);
		}
		res=m_DownscaleBuffer.SetImageSize(m_Out_sizeX, m_Out_sizeY);
		if (res!=S_OK)
		{
			_fnReportError(eError_CannotAllocateBuffer, " Cannot set image size for Downscaling %u", res);
			return(eError_CannotAllocateBuffer);
		}
		ASSERT(res==S_OK);
	}

	// Now allocate memory for input buffer
	m_InputBuffer.SetFormat(InternalFormat);
	res=m_InputBuffer.Allocate(ComputeNeededBufferSize(InternalFormat, aInput.SizeX* aInput.SizeY));
	if (res!=S_OK)
	{
		_fnReportError(eError_CannotAllocateBuffer, " Cannot allocate memory for input internal buffer  %u= %u x %u", aInput.SizeX* aInput.SizeY, aInput.SizeX, aInput.SizeY);
		return(eError_CannotAllocateBuffer);
	}
	m_InputFormat=aInput.Format;
	CHECK_ERROR(m_InputFormat ==eImageFormat_UnknowFormat);
	res=m_InputBuffer.SetImageSize(aInput.SizeX, aInput.SizeY);
	m_bConfigurationOk=true; //Now all is ok
	ASSERT(res==S_OK);
	return(S_OK);
}

int ArmIVProc_Engine::ProcessImage(const _tImageBuffer &aInputBuffer, _tImageBuffer &aOutBuffer)
//*************************************************************************************
{
	int error = S_OK;
	int result=S_OK;

	if (aOutBuffer.GetSize()==0)
	{
		return(eError_NoOutputBufferProvided);
	}
	m_bIsProcessing=true; // Set processing on going
	_tImageBuffer *pInputBuffer= &m_InputBuffer;
	do
	{
		// Read input buffer make internal buffer
		result=ReadImage (aInputBuffer, m_InputBuffer, m_InputFormat);
		if (result!=S_OK)
		{
			error=result;
			break;
		}

		// Downscalling if needed
		if (m_bNeedDownScale==true)
		{
			result= RescaleImage(*pInputBuffer, m_DownscaleBuffer);
			if (result!=S_OK)
			{
				error=eError_DownscaleFailed;
				break;
			}
			pInputBuffer= &m_DownscaleBuffer; //
		}
		// Mirroring if needed
		if (m_Mirroring!=eMirror_None)
		{
			result=MirrorImage(*pInputBuffer, m_Mirroring);
			if (result!=S_OK)
			{
				error=eError_MirroringFailed;
				break;
			}
		}

		// Rotation if needed
		if (m_bNeedRotation==true)
		{
			result=RotateImage(*pInputBuffer, m_RotationBuffer, m_Rotation);
			if (result!=S_OK)
			{
				error=result;
				break;
			}
			if (m_bNeedBufferForRotation==true)
			{
			}
			pInputBuffer= &m_RotationBuffer;
		}
		// Write buffer
		result=WriteImage (*pInputBuffer, aOutBuffer, aOutBuffer.GetFormat());
		if (result!=S_OK)
		{
			error=result;
			break;
		}
	} while (0);
	m_bIsProcessing=false; // Reset processing on going
	return(error);
}

