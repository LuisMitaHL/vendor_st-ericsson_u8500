/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include "mpl_msg.h"
#include "example_msg.h"


void mylog_func(void* user_p, int level, const char* file, int line, const char *format, ...);
#define MY_DBG(format, ...) mylog_func(NULL, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

void my_print_message(mpl_msg_t *msg_p);

int my_make_request(mpl_msg_t *req_msg_p);
int my_make_response(mpl_msg_t *resp_msg_p);
int my_make_event(mpl_msg_t *event_msg_p);


/**
 * This demo program demonstrates simple usage of the msg- and param-library (mpl).
 *
 * Messages are allocated, filled in, encoded (packed) and decoded (unpacked). This is done
 * for the three message types req, resp and event. Different parameter types are demonstrated.
 * Results along the way are printed to stdout.
 *
 * The program uses the parameter/message definitions in example_msg.{h,c}.
 *
 */
int main(int argc, char **argv)
{
  mpl_msg_t *send_msg_p;
  mpl_msg_t *receive_msg_p;  
  char *buf_p;
  size_t len;
  int i;
  int res;

  /* Init */
  if (example_init(NULL, mylog_func) < 0)
  {
    MY_DBG("example_init() failed\n");
    return -1;
  }


  for (i = 0; i < 3; i++)
  {
    switch (i)
    {
    case 0:
      /* Allocate and build request message */
      send_msg_p = mpl_req_msg_alloc(EXAMPLE_PARAM_SET_ID);  
      res = my_make_request(send_msg_p);
      break;
    case 1:
      /* Allocate and build response message */
      send_msg_p = mpl_resp_msg_alloc(EXAMPLE_PARAM_SET_ID);  
      res = my_make_response(send_msg_p);
      break;
    case 2:
      /* Allocate and build event message */
      send_msg_p = mpl_event_msg_alloc(EXAMPLE_PARAM_SET_ID);
      res = my_make_event(send_msg_p);
      break;
    }

    if (res < 0)  
    {
      MY_DBG("Failed making message\n");
      mpl_msg_free(send_msg_p);
      return -1;
    }

    /* Pack message and print result */
    buf_p = NULL;
    len = 0;
    if (mpl_msg_pack(send_msg_p, &buf_p, &len) < 0)
    {
      MY_DBG("Failed packing message\n");
      mpl_msg_free(send_msg_p);
      return -1;
    }  
    mpl_msg_free(send_msg_p);

    printf("Packed message (len %d): '%s'\n", len, buf_p);
  
    /* Unpack message and print result */
    receive_msg_p = mpl_msg_alloc(EXAMPLE_PARAM_SET_ID);
    if (mpl_msg_unpack(buf_p, len, receive_msg_p) < 0)
    {
      MY_DBG("Failed unpacking message\n");
      mpl_msg_free(send_msg_p);
      return -1;
    }
    printf("Unpacked message: ");
    my_print_message(receive_msg_p);
    mpl_msg_free(receive_msg_p);
    mpl_msg_buf_free(buf_p);
  }
  
  return 0;
}

int my_make_request(mpl_msg_t *req_msg_p)
{  
  int myint = 55;
  char *mystring = "string value in request message";
  example_my_enum_t myenum = example_my_enum_val1;
  bool mybool = true;
  uint8_t myuint8 = 0x5b;
  uint32_t myuint32 = 0xbadebabe;
  
  req_msg_p->req.id = example_mycommand;

  if (mpl_msg_add_param(req_msg_p, example_paramid_myint, &myint) < 0)
  {
    MY_DBG("Failed to add myint to message\n");
    return -1;
  }

  if (mpl_msg_add_param(req_msg_p, example_paramid_mystring, mystring) < 0)
  {
    MY_DBG("Failed to add mystring to message\n");
    return -1;
  }
  
  if (mpl_msg_add_param(req_msg_p, example_paramid_myenum, &myenum) < 0)
  {
    MY_DBG("Failed to add myenum to message\n");
    return -1;
  }
  
  if (mpl_msg_add_param(req_msg_p, example_paramid_myuint8, &myuint8) < 0)
  {
    MY_DBG("Failed to add myuint8 to message\n");
    return -1;
  }

  if (mpl_msg_add_param(req_msg_p, example_paramid_myuint32, &myuint32) < 0)
  {
    MY_DBG("Failed to add myuint32 to message\n");
    return -1;
  }

  if (mpl_msg_add_param(req_msg_p, example_paramid_mybool, &mybool) < 0)
  {
    MY_DBG("Failed to add mybool to message\n");
    return -1;
  }

  return 0;
}

int my_make_response(mpl_msg_t *resp_msg_p)
{  
  char *mystring = "string value in response message";

  resp_msg_p->resp.id = example_mycommand;
  resp_msg_p->resp.result = example_result_ok;
  if (mpl_msg_add_param(resp_msg_p, example_paramid_mystring, mystring) < 0)
  {
    MY_DBG("Failed to add mystring to message\n");
    return -1;
  }
  
  return 0;
}

int my_make_event(mpl_msg_t *event_msg_p)
{  
  char *mystring = "string value in event message";

  event_msg_p->resp.id = example_myevent;
  if (mpl_msg_add_param(event_msg_p, example_paramid_mystring, mystring) < 0)
  {
    MY_DBG("Failed to add mystring to message\n");
    return -1;
  }
  
  return 0;
}


void my_print_message(mpl_msg_t *msg_p)
{
  int myint_rec;
  char *mystring_rec;
  example_my_enum_t myenum_rec;
  bool mybool_rec;
  uint8_t myuint8_rec;
  uint32_t myuint32_rec;

  printf("type = '%s', ", mpl_param_value_get_string(example_paramid_message_type, &msg_p->common.type));
  printf("message = '%s'", mpl_param_value_get_string(example_paramid_message, &msg_p->common.id));
  if (example_msgtype_resp == msg_p->common.type)
  { 
    printf(", result = %d (%s)", msg_p->resp.result, mpl_param_value_get_string(example_paramid_result, &msg_p->resp.result));
  }

  printf("\n");
  
  if (NULL != msg_p->common.param_list_p)
    printf("  Parameters:\n");
  
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_myint))
  {
    myint_rec = MPL_MSG_GET_PARAM_VALUE(msg_p, int, example_paramid_myint);
    printf("    myint = %d\n", myint_rec);
  }
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_mystring))
  {
    mystring_rec = strdup(MPL_MSG_GET_PARAM_VALUE_REF(msg_p, char*, example_paramid_mystring));
    printf("    mystring = '%s'\n", mystring_rec);
    free(mystring_rec);
  }
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_myenum))
  {
    myenum_rec = MPL_MSG_GET_PARAM_VALUE(msg_p, example_my_enum_t, example_paramid_myenum);
    printf("    myenum = %d (%s)\n", myenum_rec, mpl_param_value_get_string(example_paramid_myenum, &myenum_rec));
  }
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_myuint8))
  {
    myuint8_rec = MPL_MSG_GET_PARAM_VALUE(msg_p, uint8_t, example_paramid_myuint8);
    printf("    myuint8 = 0x%x\n", myuint8_rec);
  }
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_myuint32))
  {
    myuint32_rec = MPL_MSG_GET_PARAM_VALUE(msg_p, uint32_t, example_paramid_myuint32);
    printf("    myuint32 = 0x%x\n", myuint32_rec);
  }
  if (MPL_MSG_PARAM_PRESENT(msg_p, example_paramid_mybool))
  {
    mybool_rec = MPL_MSG_GET_PARAM_VALUE(msg_p, bool, example_paramid_mybool);
    printf("    mybool = %d (%s)\n", mybool_rec, mpl_param_value_get_string(example_paramid_mybool, &mybool_rec));
  }
}


void mylog_func(void* user_p, int level, const char* file, int line, const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  switch (level)
  {
  case LOG_EMERG:
  case LOG_ALERT:
  case LOG_CRIT:
  case LOG_ERR:
    fprintf(stderr, "ERROR [%s:%d]: ", file, line);
    break;
        
  case LOG_WARNING:
    fprintf(stderr, "WARN  [%s:%d]: ", file, line);
    break;
        
  case LOG_NOTICE:     
  case LOG_INFO:
    fprintf(stderr, "INFO  [%s:%d]: ", file, line);
    break;
        
  case LOG_DEBUG:
  default:
    fprintf(stderr, "DEBUG [%s:%d]: ", file, line);
    break;
  }
    
  vfprintf(stderr, format, ap);
}
