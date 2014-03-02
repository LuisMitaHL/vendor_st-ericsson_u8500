#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"

enum msg_type
{
    MSG_GET_HANDLE,
    MSG_FREE_HANDLE,
    MSG_GET_PARAM,
    MSG_SET_PARAM,
    MSG_GET_CONFIG,
    MSG_SET_CONFIG,
    MSG_SEND_COMMAND,
    MSG_EVENT,
    MSG_SETUP_TUNNEL,
    MSG_GET_STATE,
    MSG_EMPTY_BUFFER_DONE,
    MSG_EMPTY_BUFFER,
    MSG_FREE_BUFFER,
    MSG_ALLOC_BUFFER,
    MSG_FILL_BUFFER_DONE,
    MSG_FILL_BUFFER,
};

struct msg
{
    enum msg_type type;
    OMX_ERRORTYPE result;

    union
    {
      struct
      {
        char name[160];
        int  in_client_handle;
        int  out_srv_handle;
      } get_handle;
      struct
      {
        int srv_handle;
      } free_handle;
      struct
      {
        int srv_handle;
        OMX_INDEXTYPE index;
        char buf[4096];
      } param;
      struct
      {
        int srv_handle;
        OMX_COMMANDTYPE cmd;
        OMX_U32 nParam1;
      } command;
      struct
      {
        int srv_handle_in;
        int srv_handle_out;
        int port_in;
        int port_out;
      } tunnel;
      struct
      {
        int in_srv_handle;
        OMX_STATETYPE  out_state;
      } get_state;
      struct
      {
        int client_handle;
        int client_bufhdr;
      } empty_buffer_done;
      struct
      {
        int client_handle;
        int client_bufhdr;
        char out_buf[8192];
        OMX_BUFFERHEADERTYPE out_buf_hdr;
      } fill_buffer_done;
      struct
      {
        int in_srv_handle;
        int port;
        int size;
        int in_client_buf_hdr;
        int out_srv_buf_hdr;
        OMX_BUFFERHEADERTYPE out_buf_hdr;
      } alloc_buffer;
      struct
      {
        int in_srv_handle;
        OMX_BUFFERHEADERTYPE in_buf_hdr;
        char in_buf[8192];
      } empty_buffer;
      struct
      {
        int in_srv_handle;
        OMX_BUFFERHEADERTYPE in_buf_hdr;
      } fill_buffer;
      struct
      {
        int in_srv_handle;
        int in_srv_buf_hdr;
        int in_port;
      } free_buffer;

      struct
      {
        int              client_handle;
        OMX_EVENTTYPE    ev;
        OMX_U32          nData1;
        OMX_U32          nData2;
      } event;

    } cmd;
};

