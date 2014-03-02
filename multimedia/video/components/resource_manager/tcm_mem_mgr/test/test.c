/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**
 * Name:         test.c
 * Author:       Maurizio Colombo
 * Date:         23/05/10 12:16
 * Description:  standalone unit test for memory manager
**/

#include <stdio.h>
#include <stdlib.h>
#include "standalone.h"
#include "memory_mgr.h"

#define HEAP_SIZE 2048
#define HEAP_BASE 0x1000

#define __EXIT_IF_FAILED(x)   if(x)NMF_PANIC("Test failed\n");
#define __ASSERT_NON_NULL(x)  if((x)==0)return -1;
#define __ASSERT_ALIGNED(x)   if((x)&(MM_ALGN-1))return -1; 
#define __ASSERT_ALLFREE()              \
{                                       \
  t_uint32 asfree, asused;              \
  MemMgr_GetStatus(&asfree,&asused);    \
  if(asfree!=HEAP_SIZE)return -1;       \
}

void NMF_PANIC(char * s)
{                                 
  printf("%s",s); 
  exit(-1);          
}
  
t_sint16 test1()
{
  t_uint32 a;
  printf("\n\n\nTEST 1: Basic malloc/free\n");
  
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(128));
  MemMgr_Free((void*)a);
  
  __ASSERT_ALLFREE();
  return 0;
}

t_sint16 test2()
{
  t_uint32 a;
  printf("\n\n\nTEST 2: Multiple malloc/free\n");
  
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(128));
  MemMgr_Free((void*)a);
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(64));
  MemMgr_Free((void*)a);
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(32));
  MemMgr_Free((void*)a);
  
  __ASSERT_ALLFREE();
  return 0;
}  

t_sint16 test3()
{
  t_uint32 a,b,c;
  printf("\n\n\nTEST 3: Multiple malloc/ multiple free\n");
  
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(128));
  __ASSERT_NON_NULL(b = (t_uint32)MemMgr_Malloc(64));
  __ASSERT_NON_NULL(c = (t_uint32)MemMgr_Malloc(32));
  MemMgr_Free((void*)b);
  MemMgr_Free((void*)c);
  MemMgr_Free((void*)a);
  
  __ASSERT_ALLFREE();
  return 0;
}  

t_sint16 test4()
{
  t_uint32 xxfree, xxused,ii;
  printf("\n\n\nTEST 4: Heap alignment\n");
  for(ii=1;ii<MM_ALGN;ii++)
  {
   MemMgr_Init((void*)(HEAP_BASE+ii), HEAP_SIZE);
   MemMgr_GetStatus(&xxfree,&xxused);
  
   if(xxfree != HEAP_SIZE-MM_ALGN)
     return -1;
  }
  for(ii=1;ii<MM_ALGN;ii++)
  {
   MemMgr_Init((void*)(HEAP_BASE+ii), HEAP_SIZE+ii);
   MemMgr_GetStatus(&xxfree,&xxused);
  
   if(xxfree != HEAP_SIZE-MM_ALGN)
     return -1;
  }
  MemMgr_Init((void*)HEAP_BASE, HEAP_SIZE);
  return 0;
}

t_sint16 test5()
{
  t_uint32 a, b, c;
  printf("\n\n\nTEST 5: Malloc Alignment\n");
  
  __ASSERT_NON_NULL(a = (t_uint32)MemMgr_Malloc(123));
  __ASSERT_ALIGNED(a);
  __ASSERT_NON_NULL(b = (t_uint32)MemMgr_Malloc(61));
  __ASSERT_ALIGNED(b);
  __ASSERT_NON_NULL(c = (t_uint32)MemMgr_Malloc(33));
  __ASSERT_ALIGNED(c);
  MemMgr_Free((void*)b);
  MemMgr_Free((void*)c);
  MemMgr_Free((void*)a);
  
  __ASSERT_ALLFREE();
  return 0;  
}

t_sint16 test6()
{
  t_uint32 a, b, c;
  printf("\n\n\nTEST 6: Out of memory\n");
  
  for(a=0;a<HEAP_SIZE/(MM_ALGN*128);a++)
  {
    __ASSERT_NON_NULL(b = (t_uint32)MemMgr_Malloc(MM_ALGN*128));
  }
  c = (t_uint32)MemMgr_Malloc(1);
  if(c!=0) return -1;
  MemMgr_Init((void*)HEAP_BASE, HEAP_SIZE);

  for(a=0;a<LST_MAX_ELM_NBR;a++)
  {
    __ASSERT_NON_NULL(b = (t_uint32)MemMgr_Malloc(1));
  }
  c = (t_uint32)MemMgr_Malloc(1);
  if(c!=0) return -1;
  MemMgr_Init((void*)HEAP_BASE, HEAP_SIZE);

  return 0; 
}

t_sint16 test7()
{ 
  t_uint32 a, c;
  t_uint32 arr[LST_MAX_ELM_NBR];
  
  printf("\n\n\nTEST 7: Out of memory and free\n");
  for(a=0;a<HEAP_SIZE/(MM_ALGN*128);a++)
  {
    __ASSERT_NON_NULL(arr[a] = (t_uint32)MemMgr_Malloc(MM_ALGN*128));
  }
  c = (t_uint32)MemMgr_Malloc(1);
  if(c!=0) return -1;
  for(a=0;a<HEAP_SIZE/(MM_ALGN*128);a++)
  {
    MemMgr_Free((void*)arr[a]);
  }
 
  __ASSERT_ALLFREE();

  for(a=0;a<LST_MAX_ELM_NBR;a++)
  {
    __ASSERT_NON_NULL(arr[a] = (t_uint32)MemMgr_Malloc(1));
  }
  c = (t_uint32)MemMgr_Malloc(1);
  if(c!=0) return -1;
  for(a=0;a<LST_MAX_ELM_NBR;a++)
  {
    MemMgr_Free((void*)arr[a]);
  }
 
  __ASSERT_ALLFREE();

  return 0; 
  
}
  
int main(int argc, char *argv[])
{
  MemMgr_Init((void*)HEAP_BASE, HEAP_SIZE);
  
  __EXIT_IF_FAILED(test1());
  __EXIT_IF_FAILED(test2());
  __EXIT_IF_FAILED(test3());
  __EXIT_IF_FAILED(test4());
  __EXIT_IF_FAILED(test5());
  __EXIT_IF_FAILED(test6());
  __EXIT_IF_FAILED(test7());

  printf("\n\nEND OF TESTS : SUCCESS\n\n\n");  	
  return 0;
}
