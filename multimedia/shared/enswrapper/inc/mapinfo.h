/*
 * Copyright 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 *
 *     http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 */

#ifndef _MAPINFO_H_
#define _MAPINFO_H_

typedef struct mapinfo {
    struct mapinfo *next;
    unsigned start;
    unsigned end;
    unsigned exidx_start;
    unsigned exidx_end;
    char name[];
} mapinfo;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

mapinfo *parse_maps_line(char *line);
const char *map_to_name(mapinfo *mi, unsigned pc, const char* def, unsigned *offset);
mapinfo *get_process_mapinfo();
void free_mapinfo(mapinfo *mi);
const mapinfo *pc_to_mapinfo(mapinfo *mi, unsigned pc, unsigned *rel_pc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MAPINFO_H_ */


