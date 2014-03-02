/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifndef X86
  #define RUNNING_DIR "/data/tmp"	// The default working directory of the server. Can be changed from command line.
  #define MMPROBE_DIR "/data/tmp"
  #define MMPROBETXT  "/data/tmp/mmprobe.txt"
  #define FIFO_NAME_PATH "/data/tmp/mmprobed_fifo"
  #define LIST_NAME_PATH "/data/tmp/mmprobed_list"
#else
  #define RUNNING_DIR "/tmp"	// The default working directory of the server. Can be changed from command line.
  #define MMPROBE_DIR "/tmp"
  #define MMPROBETXT  "/tmp/mmprobe.txt"
  #define FIFO_NAME_PATH "/tmp/mmprobed_fifo"
  #define LIST_NAME_PATH "/tmp/mmprobed_list"
#endif
#define FIFO_NAME "mmprobed_fifo" // The FIFO name
#define LIST_NAME "mmprobed_list" // The probe list name

#endif /* COMMON_H_ */
