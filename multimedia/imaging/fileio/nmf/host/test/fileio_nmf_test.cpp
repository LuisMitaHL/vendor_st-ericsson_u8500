/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "fileio_wrp.hpp"
#include "ee/api/ee.h"
// TODO CAUTION unistd.h is Linux specific
#include <unistd.h>

// Globals
static t_fio_error GFileIoErr = FIO_OK;
static t_fio_error GBigBuffer[32*1024];

// fileio_core response interface
class CFileIoResponseIntf: public fileio_core_api_responseDescriptor
{
  public:
    virtual void fileioresponse(t_fio_error);
};

void CFileIoResponseIntf::fileioresponse(t_fio_error aFioErr)
{
  NMF_LOG("[CFileIoResponseIntf::status] failed file I/O indication\n");
  switch(aFioErr)
  {
      case FIO_OK:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_OK\n");
        break;
      case FIO_BAD_PARAMETER:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_BAD_PARAMETER\n");
        break;
      case FIO_OPEN_FAILED:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_OPEN_FAILED\n");
        break;
      case FIO_SEEK_FAILED:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_SEEK_FAILED\n");
        break;
      case FIO_READ_FAILED:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_READ_FAILED\n");
        break;
      case FIO_WRITE_FAILED:
        NMF_LOG("[CFileIoResponseIntf::status] NMF component returned FIO_WRITE_FAILED\n");
        break;
  }
  GFileIoErr = aFioErr;
}

#ifdef __SYMBIAN32__
#include "e32std.h"
int mainL(int argc, char ** argv) ;
int main(int argc, char ** argv)
{
    TRAPD(error, mainL(argc, argv));
    __ASSERT_ALWAYS(!error, User::Invariant());
}

int mainL(int argc, char ** argv)
{
    RThread thread;
    thread.SetPriority(EPriorityAbsoluteRealTime4);
#else
int main(int argc, char ** argv)
{
#endif

    t_nmf_error               nmfErr;
    char*                     nmfMsg ;
    NMF::InterfaceReference*  nmfCtxt;
    Ifileio_core_api_request  fileIoRequestIntf;
    CFileIoResponseIntf       fileIoResponseIntf;
    t_nmf_channel             nmfCallBackChannel;
    fileio_wrp*               fileIoWrp;
    t_fio_size                fileSize = 0;
    t_fio_size                expectedFileSize = 10800;


    NMF_LOG("[fileio_nmf_test] Starting fileio NMF test application\n");
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");

    // NMF channel creation
    NMF_LOG("[fileio_nmf_test] Creating NMF callback channel\n");
    nmfErr = EE_CreateChannel(NMF_CHANNEL_PRIVATE, &nmfCallBackChannel);
    if( nmfErr != NMF_OK)
    {
        NMF_LOG("Could not create NMF channel (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    NMF_LOG("[fileio_nmf_test] NMF callback channel = 0x%X\n", (unsigned int)nmfCallBackChannel);

#ifdef __SYMBIAN32__
    NMF_LOG("Waiting initialization of HostEE...\n") ;
    User::After(500000) ;
#endif 

    // fileio NMF construction
    NMF_LOG("[fileio_nmf_test] NMF component instantiation\n");
    fileIoWrp = fileio_wrpCreate();
    if(fileIoWrp == NULL)
    {
        NMF_LOG("[fileio_nmf_test] NMF instantiation failed\n");
        goto failed;
    }

    NMF_LOG("[fileio_nmf_test] NMF construction\n");
    nmfErr = fileIoWrp->construct();
    if (nmfErr != NMF_OK)
    {
        NMF_LOG("[fileio_nmf_test] NMF construction failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }

    NMF_LOG("[fileio_nmf_test] binding NMF 'request' interface\n");
    nmfErr = fileIoWrp->bindFromUser("request", 1, &fileIoRequestIntf);
    if (nmfErr != NMF_OK)
    {
        NMF_LOG("[fileio_nmf_test] NMF 'request' interface binding failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }

    NMF_LOG("[fileio_nmf_test] binding NMF 'response' interface\n");
    nmfErr = fileIoWrp->bindToUser(nmfCallBackChannel, "response", &fileIoResponseIntf, 1);
    if (nmfErr != NMF_OK)
    {
        NMF_LOG("[fileio_nmf_test] NMF 'response' interface binding failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }

    NMF_LOG("[fileio_nmf_test] starting NMF component\n");
    fileIoWrp->start();

    /*
     * Test reading a file, passing bad parameters
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test reading with bad parameters\n");
    fileIoRequestIntf.read((t_fio_file_name)0, (t_fio_buffer)0, 0);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_BAD_PARAMETER) {
        NMF_LOG("[fileio_nmf_test] File IO error reason is 'bad parameter' => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected 'bad parameter' but did not uccur => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test reading a non-existing file
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test reading an non-existing file\n");
    fileIoRequestIntf.read((t_fio_file_name)"foobar.txt", (t_fio_buffer)GBigBuffer, 1024);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_OPEN_FAILED) {
        NMF_LOG("[fileio_nmf_test] File IO error reason is 'open error' => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected 'open error' but did not uccur => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test getting a file size
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test getting size of file\n");
    fileIoRequestIntf.size((t_fio_file_name)"input_file.txt", (t_fio_size_ptr)&fileSize);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_OK) {
        NMF_LOG("[fileio_nmf_test] File IO ok => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected response 'io ok' but got error => ko\n");
        goto failed;
    }
    if (fileSize==expectedFileSize){
        NMF_LOG("[fileio_nmf_test] File size == expected size (i.e. %d bytes) => ok\n", fileSize);
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected file size %d, got %d => ko\n", expectedFileSize, fileSize);
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test reading a existing file, having less bytes than requested
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test reading an existing file having less bytes than requested\n");
    fileIoRequestIntf.read((t_fio_file_name)"input_file.txt", (t_fio_buffer)GBigBuffer, fileSize+10);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_READ_FAILED) {
        NMF_LOG("[fileio_nmf_test] File IO error reason is 'read error' => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected response 'read error' but did not uccur => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test reading a existing file
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test reading an existing file\n");
    fileIoRequestIntf.read((t_fio_file_name)"input_file.txt", (t_fio_buffer)GBigBuffer, fileSize);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_OK) {
        NMF_LOG("[fileio_nmf_test] File IO ok => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected response 'io ok' but got error => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test writing to a file, with bad arguments
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test writing with bad parameters\n");
    fileIoRequestIntf.write((t_fio_file_name)0, (t_fio_buffer)0, 0);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_BAD_PARAMETER) {
        NMF_LOG("[fileio_nmf_test] File IO error reason is 'bad parameter' => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected 'bad parameter' but did not uccur => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test writing to a file
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test writing with bad parameters\n");
    fileIoRequestIntf.write((t_fio_file_name)"output_file.txt", (t_fio_buffer)GBigBuffer, fileSize);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_OK) {
        NMF_LOG("[fileio_nmf_test] File IO ok => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected response 'io ok' but got error => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * Test read back file size
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test size of file just written\n");
    fileIoRequestIntf.size((t_fio_file_name)"output_file.txt", (t_fio_size_ptr)&fileSize);
    NMF_LOG("[fileio_nmf_test] calling EE_GetMessage\n");
    nmfErr = EE_GetMessage(nmfCallBackChannel, (void**)&nmfCtxt, &nmfMsg, 1);
    if (nmfErr != NMF_OK) {
        NMF_LOG("[fileio_nmf_test] EE_GetMessage failed (NmfErr=%d)\n", nmfErr);
        goto failed;
    }
    EE_ExecuteMessage(nmfCtxt, nmfMsg);
    if(GFileIoErr==FIO_OK) {
        NMF_LOG("[fileio_nmf_test] File IO ok => ok\n");
    }
    else {
        NMF_LOG("[fileio_nmf_test] Expected response 'io ok' but got error => ko\n");
        goto failed;
    }
    GFileIoErr = FIO_OK;

    /*
     * All tests passed
     */
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test passed\n") ;
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    return 0;

    /*
     * Test failed
     */
failed:
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    NMF_LOG("[fileio_nmf_test] Test failed\n") ;
    NMF_LOG("[fileio_nmf_test] -------------------------------------------------------\n");
    return 1;
} /* main */
