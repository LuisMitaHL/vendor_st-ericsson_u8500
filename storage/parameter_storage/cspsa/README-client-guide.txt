Crash Safe Parameter Storage Area Client Guide
==============================================

This text is intended for developers that needs to implement a function that
accesses CSPSA from userspace in a ME software.

ME is an acronym for Mobile Equipment.

--------------------------------------------------------------------------------

During startup of the ME a CSPSA server application is started, and is
kept pending for clients until ME is shut down. A CSPSA client application needs
to interact with the server to access production parameters. This interaction
is transparent for clients by using the CPSPA client library 'libcspsa'.

All users both in non OS applications, host applications and
ME client application uses the same API exposed by  'cspsa.h'.

A userspace client normally only needs to bother about four of the public
functions of CSPSA:

 - CSPSA_Open()
 - CSPSA_ReadValue()
 - CSPSA_WriteValue()
 - CSPSA_Close()

Before reading and writing to CSPSA a connection must be opened to the CSPSA
server. This is done by using function CSPSA_Open(). The only in-parameter is a
name of the CSPSA area to use. There may be several areas, and the name can be
arbitrarily chosen, but in U8500 and U5500 the default CSPSA area is called
"CSPSA0".

After all accesses are done the client should close the connection by
calling CSPSA_Close(). After a write access a client may call CSPSA_Flush() to
explicitly make the CSPSA server store its contents to flash memory. However,
this is also done when closing the connection so in most cases this is not
necessary.

--------------------------------------------------------------------------------

Here is a simple example of a function calling CSPSA:

...
#define D_CSPSA_DEFAULT_NAME "CSPSA0"
{
...
  CSPSA_Handle_t CspsaHandle;
  CSPSA_Result_t CspsaResult;
...
  CspsaResult = CSPSA_Open(D_CSPSA_DEFAULT_NAME, &CspsaHandle);
  if (CspsaResult != T_CSPSA_RESULT_OK)
          goto cspsa_finished;

  CspsaResult = CSPSA_ReadValue(CspsaHandle, CSPSA_SOME_KEY_1, CSPSA_SOME_KEY_1_SIZE, &ParameterValue);
  if (CspsaResult != T_CSPSA_RESULT_OK)
          goto cspsa_finished;
...
  CspsaResult = CSPSA_WriteValue(CspsaHandle, CSPSA_SOME_KEY_1, CSPSA_SOME_KEY_1_SIZE, &NewParameterValue);
  if (CspsaResult != T_CSPSA_RESULT_OK)
          goto cspsa_finished;

cspsa_finished:
  (void) CSPSA_Close(&CspsaHandle);
...
}

--------------------------------------------------------------------------------

Note that CSPSA may return any of these warnings:

 - T_CSPSA_RESULT_W_SIZE: may be returned by both read and write to alert that the
   requested size differs from the stored parameter's size. A read access never
   returns more bytes than requested.

 - T_CSPSA_RESULT_W_NEW_KEY: may be returned by a write access if the parameter
   item did not exist before.

--------------------------------------------------------------------------------

If the size of an item is not fix a client can call CSPSA_GetSizeOfValue() to
get the size of the value before reading its value.

--------------------------------------------------------------------------------

In order to build with CSPSA shared client library under Android one needs to
add the following statements in the client's 'Android.mk':

LOCAL_C_INCLUDES := storage/parameter_storage/cspsa
LOCAL_SHARED_LIBRARIES := libcspsa

--------------------------------------------------------------------------------

In order to build with CSPSA shared client library under LBP one needs to add
the following statements in the client's make file.

LDFLAGS += -L<local path to CSPSA> -lcspsa

Where LDFLAGS is added to the link command.

--------------------------------------------------------------------------------

