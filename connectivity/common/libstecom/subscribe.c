/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>

#include "subscribe_i.h"



struct subscribe_client_t
{
  struct sockaddr *addr;
  socklen_t addr_len;
  struct subscribe_client_t *previous;
  struct subscribe_client_t *next;
};

struct subscribe_t
{
  struct subscribe_client_t *first;
  struct subscribe_client_t *last;
};



struct subscribe_client_t * _subscribe_take_item(struct subscribe_t *context, struct subscribe_client_t *c)
{
  if(c->previous == NULL)
    context->first = c->next;
  else
    c->previous->next = c->next;

  if(c->next == NULL)
    context->last = c->previous;
  else
    c->next->previous = c->previous;

  return c;
}

struct subscribe_client_t * _subscribe_find_item(struct subscribe_t *context, struct sockaddr *addr, socklen_t addr_len)
{
  struct subscribe_client_t *iter;
  for(iter = context->first; iter != NULL; iter = iter->next)
  {
    if(iter->addr_len == addr_len && !memcmp(iter->addr, addr, addr_len))
    {
      return iter;
    }
  }

  return NULL;
}


struct subscribe_t * subscribe_init()
{
  struct subscribe_t *context = calloc(1, sizeof(struct subscribe_t));

  return context;
}

void subscribe_deinit(struct subscribe_t *context)
{
  if(!context)
    return;

  subscribe_clear(context);

  free(context);
  context = 0;
}


int subscribe_add(struct subscribe_t *context, struct sockaddr *addr, socklen_t addr_len)
{
  struct subscribe_client_t *c = calloc(1, sizeof(struct subscribe_client_t));
  c->addr = calloc(1, addr_len);

  c->addr_len = addr_len;
  memcpy(c->addr, addr, addr_len);
  c->next = NULL;

  if(context->last == NULL)
  {
    context->first = c;
  } else {
    c->previous = context->last;
    c->previous->next = c;
  }

  context->last = c;

  return 0;
}

int subscribe_remove(struct subscribe_t *context, struct sockaddr *addr, socklen_t addr_len)
{
  struct subscribe_client_t *c = _subscribe_find_item(context, addr, addr_len);

  if(c != NULL)
  {
    c = _subscribe_take_item(context, c);

    if(c != NULL)
    {
      free(c->addr);
      free(c);
    }
  }

  return 0;
}

void subscribe_clear(struct subscribe_t *context)
{
  struct subscribe_client_t *iter = context->first;
  struct subscribe_client_t *tmp;
  while (iter != NULL)
  {
    tmp = iter;
    iter = iter->next;
    free(tmp->addr);
    free(tmp);
  }

  context->first = NULL;
  context->last = NULL;
}

struct subscribe_client_t * subscribe_get_first(struct subscribe_t *context)
{
  return context->first;
}

struct subscribe_client_t * subscribe_get_last(struct subscribe_t *context)
{
  return context->last;
}

struct subscribe_client_t * subscribe_get_next(struct subscribe_client_t *client)
{
  return client->next;
}

struct subscribe_client_t * subscribe_get_previous(struct subscribe_client_t *client)
{
  return client->previous;
}

struct sockaddr * subscribe_get_addr(struct subscribe_client_t *client)
{
  return client->addr;
}

socklen_t subscribe_get_addr_len(struct subscribe_client_t *client)
{
  return client->addr_len;
}

