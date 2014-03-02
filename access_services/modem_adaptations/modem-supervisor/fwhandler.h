/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted to the requirements of the ST-Ericsson U8500 product.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef fwhandler_h
#define fwhandler_h


#ifdef fwhandler_c
#define PUBLIC
#else
#define PUBLIC extern
#endif

PUBLIC int init_firmware_upload_handler(void);
PUBLIC int firmware_upload(char *mloader_path);

#undef PUBLIC

#endif /* fwhandler_h */
