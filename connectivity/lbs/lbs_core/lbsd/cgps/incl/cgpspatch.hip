/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSPATCH_HIP__
#define __CGPSPATCH_HIP__
/**
* \file cgpspatch.hip
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include file used by cgps2patch.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
* \fn void CGPS2_00UploadMEPatch(U4 Max_Num_Patch_Mess )
* \brief Upload patch to GPS module
*
* Upload the next set of patch code to the GPS baseband.
* This function sends up to a maximum of Max_Num_Patch_Mess sentences each time
* it is called.
* The complete set of patch data is divided into six stages.
*
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
*/
void CGPS2_00UploadMEPatch(U4 Max_Num_Patch_Mess );

/**
* \fn U2 CGPS2_01SetupMEPatch( U2 ROM_version, U2 Patch_CkSum )
* \brief Ask to start patch uploading
*
* This function is called from the GPS Library 'callback' function
* GN_GPS_Write_GNB_Patch() to start / set-up the GN Baseband Patch upload.
*
* \param ROM_version Current GN Baseband ROM version
* \param Patch_CkSum Current GN Baseband Reported Patch
*
* \return Patch checksum
*/
U2 CGPS2_01SetupMEPatch( U2 ROM_version, U2 Patch_CkSum);

/**
* \fn void CGPS2_02UploadMEPatch_502( U4 Max_Num_Patch_Mess )
* \brief Upload patch to a GNS7560 ROM 5.02 GPS module
*
* Upload the next set of patch code to the GNS7560 ROM 5.02 GPS baseband.
* This function sends up to a maximum of Max_Num_Patch_Mess sentences each time
* it is called.
* The complete set of patch data is divided into six stages.
*
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
*/
void CGPS2_02UploadMEPatch_502( U4 Max_Num_Patch_Mess );

/**
* \fn void CGPS2_03UploadMEPatch_506( U4 Max_Num_Patch_Mess )
* \brief Upload patch to a GNS7560 ROM 5.06 GPS module
*
* Upload the next set of patch code to the a GNS7560 ROM 5.06 GPS baseband.
* This function sends up to a maximum of Max_Num_Patch_Mess sentences each time
* it is called.
* The complete set of patch data is divided into six stages.
*
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
*/
void CGPS2_03UploadMEPatch_506( U4 Max_Num_Patch_Mess );

/**
* \fn void CGPS2_08UploadMEPatch_510( U4 Max_Num_Patch_Mess )
* \brief Upload patch to a GNS7560 ROM 5.10 GPS module
*
* Upload the next set of patch code to the a GNS7560 ROM 5.10 GPS baseband.
* This function sends up to a maximum of Max_Num_Patch_Mess sentences each time
* it is called.
* The complete set of patch data is divided into six stages.
*
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
*/
void CGPS2_08UploadMEPatch_510( U4 Max_Num_Patch_Mess );
/**
* \fn void CGPS2_09UploadMEPatchCG29xx(  U4 Max_Num_Patch_Mess , U1 *pp_CG2900_Patch_Ver, U2 vp_PatchLength )
* \brief Upload patch to a CG2900 chip, for different ROM version - 730, 747,748
*
* Upload the next set of patch code to the a CG2900 GPS baseband.
* This function sends up to a maximum of Max_Num_Patch_Mess sentences each time
* it is called.
* The complete set of patch data is divided into six stages.
*
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
*/

void CGPS2_09UploadMEPatchCG29xx( U4 Max_Num_Patch_Mess , const U1 *pp_CG2900_Patch_Ver, U2 vp_PatchLength);

/**
* \fn void CGPS2_10CheckMEPatchOnFS( U2 ROM_version )
* \brief find the presence of the patch file in the folder \etc\lbs\ and populate the variables for patch file upload by file
* \param ROM_version the ROM version of the patch
*/
void CGPS2_10CheckMEPatchOnFS( U2 ROM_version );
/**
* \fn void CGPS2_11UploadMEFSPatchCG29xx( CH * vp_patchFileName, U4 Max_Num_Patch_Mess, U2 vp_PatchLength)
* \brief uploads the patch from the Patch file present in the folder \etc\lbs
* \param vp_patchFileName name of the patch file to be uploaded
* \param Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
* \param vp_PatchLength size of the file thats is being uploaded
*/
void CGPS2_11UploadMEFSPatchCG29xx( int8_t* vp_patchFileName, U4 Max_Num_Patch_Mess, U2 vp_PatchLength);


#endif /* __CGPSPATCH_HIP__ */
