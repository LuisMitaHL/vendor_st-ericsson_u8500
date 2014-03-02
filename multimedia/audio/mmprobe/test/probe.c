/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>

#include "../api/r_mm_probe.h"
#include "../include/utils.h"

int main(int argc, char *argv[]) {
  char ia[320];

  int i;

  for(i = 0; i < 320; i++){
          ia[i] = i;
  }

  for (i=0; i<100; i++) {
    mmprobe_probe(10, ia, 320);
    mmprobe_probe(19, ia, 320);
    usleep(5000);
  }

  return 0;
}
