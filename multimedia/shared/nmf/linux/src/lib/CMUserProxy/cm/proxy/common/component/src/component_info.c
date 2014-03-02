/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/inc/cm_type.h>
#include <cm/proxy/inc/osal.h>
#include <cm/engine/elf/inc/elfabi.h>
#include <cm/proxy/common/repository/inc/repository.h>
#include <cm/proxy/common/component/inc/component_info.h>

#include <cm/engine/elf/inc/memory.h>
#include <cm/engine/memory/inc/memory_type.h>
#include <cm/engine/dsp/inc/dsp.h>

#include <cm/engine/utils/inc/swap.h>

/*
 * the following implementation is copied from the CM (ie kernel-side)
 * the code is duplicated (and slightly modified) in order to run user-side
 */

struct XXSection {
    Elf64_Word      sh_type;        /* Section type */
    t_uint32        sh_size;        /* Section size in bytes */
    Elf64_Word      sh_info;        /* Additional section information */
    Elf64_Word      sh_link;        /* Link to another section */

    const char      *data;
    const char      *sectionName;

    t_uint32            offsetInSegment;
    const t_elfmemory   *meminfo;
};

typedef struct
{
    t_dsp_memory_type_id         dspMemType;
    t_memory_property            property;
    t_cm_mpc_info_memory_purpose purpose;
    t_cm_mpc_info_memory_type    convertedMemType;
    t_uint8                      fileEntSize;
    t_uint8                      memEntSize;
    char*                        memoryName;
} t_elfmemory_info;

static const t_elfmemory_info mmdspMemories[] = {
    {SDRAM_CODE,       MEM_SHARABLE, CM_MM_CODE_TYPE, CM_MM_SDRAM_TYPE, 8, 8, "SDRAM_CODE"},  /* 0: Program memory */
    {INTERNAL_XRAM24,  MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_XRAM_TYPE,  3, 3, "XROM"},        /* 1: Internal X memory */
    {INTERNAL_YRAM24,  MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_YRAM_TYPE,  3, 3, "YROM"},        /* 2: Y memory */
    {SDRAM_EXT24,      MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_SDRAM_TYPE, 3, 4, "SDR0M24"},     /* 5: 24-bit external "X" memory */
    {SDRAM_EXT16,      MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_SDRAM_TYPE, 3, 2, "SDROM16"},     /* 6: 16-bit external "X" memory */
    {ESRAM_EXT24,      MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_ESRAM_TYPE, 3, 4, "ESROM24"},     /* 8: ESRAM24 */
    {ESRAM_EXT16,      MEM_SHARABLE, CM_MM_DATA_TYPE, CM_MM_ESRAM_TYPE, 3, 2, "ESROM16"},     /* 9: ESRAM16 */
    {ESRAM_CODE,       MEM_SHARABLE, CM_MM_CODE_TYPE, CM_MM_ESRAM_TYPE, 8, 8, "ESRAM_CODE"},  /*10: ESRAM code */
    {INTERNAL_XRAM24,  MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_XRAM_TYPE,  3, 3, "XRAM"},        /* 1: Internal X memory */
    {INTERNAL_YRAM24,  MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_YRAM_TYPE,  3, 3, "YRAM"},        /* 2: Y memory */
    {SDRAM_EXT24,      MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_SDRAM_TYPE, 3, 4, "SDRAM24"},     /* 5: 24-bit external "X" memory */
    {SDRAM_EXT16,      MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_SDRAM_TYPE, 3, 2, "SDRAM16"},     /* 6: 16-bit external "X" memory */
    {ESRAM_EXT24,      MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_ESRAM_TYPE, 3, 4, "ESRAM24"},     /* 8: ESRAM24 */
    {ESRAM_EXT16,      MEM_PRIVATE,  CM_MM_DATA_TYPE, CM_MM_ESRAM_TYPE, 3, 2, "ESRAM16"}      /* 9: ESRAM16 */
};

#define MAX_ELFSECTIONNAME  10
struct memoryMapping {
    char        *elfSectionName;
    t_uint32    memoryIndex[MEM_FOR_LAST]; // memoryIndex[t_instance_property]
};

static const struct memoryMapping mappingmem0[] = {
        {"mem0.0",  {0,  0}},
        {"mem0.1",  {0,  0}},
        {"mem0.2",  {0,  0}}
};
static const struct memoryMapping mappingmem10 =
        {"mem10",   {7,  7}};
static const struct memoryMapping mappingmem1[] = {
        {"",  {0xff,  0xff}},
        {"mem1.1",  {1,  1}},
        {"mem1.2",  {8,  1}},
        {"mem1.3",  {1,  1}},
        {"mem1.4",  {8,  1}},
        {"mem1.stack", {8, 1}}
};
static const struct memoryMapping mappingmem2[] = {
        {"",  {0xff,  0xff}},
        {"mem2.1",  {2,  2}},
        {"mem2.2",  {9,  2}},
        {"mem2.3",  {2,  2}},
        {"mem2.4",  {9,  2}}
};
static const struct memoryMapping mappingmem5[] = {
        {"",  {0xff,  0xff}},
        {"mem5.1",  {3,  3}},
        {"mem5.2",  {10, 3}},
        {"mem5.3",  {3,  3}},
        {"mem5.4",  {10, 3}}
};
static const struct memoryMapping mappingmem6[] = {
        {"",  {0xff,  0xff}},
        {"mem6.1",  {4,  4}},
        {"mem6.2",  {11, 4}},
        {"mem6.3",  {4,  4}},
        {"mem6.4",  {11, 4}}
};
static const struct memoryMapping mappingmem8[] = {
        {"",  {0xff,  0xff}},
        {"mem8.1",  {5,  5}},
        {"mem8.2",  {12, 5}},
        {"mem8.3",  {5,  5}},
        {"mem8.4",  {12, 5}}
};
static const struct memoryMapping mappingmem9[] = {
        {"",  {0xff,  0xff}},
        {"mem9.1",  {6,  6}},
        {"mem9.2",  {13, 6}},
        {"mem9.3",  {6,  6}},
        {"mem9.4",  {13, 6}}
};

static const struct {
    const struct memoryMapping* mapping;
    unsigned int        number;
} hashMappings[10] = {
        {mappingmem0, sizeof(mappingmem0) / sizeof(mappingmem0[0])},
        {mappingmem1, sizeof(mappingmem1) / sizeof(mappingmem1[0])},
        {mappingmem2, sizeof(mappingmem2) / sizeof(mappingmem2[0])},
        {0x0, 0},
        {0x0, 0},
        {mappingmem5, sizeof(mappingmem5) / sizeof(mappingmem5[0])},
        {mappingmem6, sizeof(mappingmem6) / sizeof(mappingmem6[0])},
        {0x0, 0},
        {mappingmem8, sizeof(mappingmem8) / sizeof(mappingmem8[0])},
        {mappingmem9, sizeof(mappingmem9) / sizeof(mappingmem9[0])},
};

static const t_elfmemory_info* MMDSP_getMappingByName(const char* sectionName, t_instance_property property)
{
    if(sectionName[0] == 'm' && sectionName[1] == 'e' && sectionName[2] == 'm')
    {
        if(sectionName[4] == '.')
        {
            if(sectionName[5] >= '0' && sectionName[5] <= '9')
            {
                if(sectionName[3] >= '0' && sectionName[3] <= '9')
                {
                    unsigned int m, sm;

                    m = sectionName[3] - '0';
                    sm = sectionName[5] - '0';
                    if(sm < hashMappings[m].number)
                        return &mmdspMemories[hashMappings[m].mapping[sm].memoryIndex[property]];
                }
            } else if(sectionName[3] == '1' && sectionName[5] == 's')
                return &mmdspMemories[mappingmem1[5].memoryIndex[property]];
        }
        else if(sectionName[3] == '1' && sectionName[4] == '0')
            return &mmdspMemories[mappingmem10.memoryIndex[property]];
    }

    return NULL;
}


// TODO Here we assume big endian (MMDSP !)
static Elf64_Half swapHalf(Elf64_Half half)
{
    return (Elf64_Half)swap16(half);
}

static Elf64_Word swapWord(Elf64_Word word)
{
    return (Elf64_Word)swap32(word);
}

static Elf64_Xword swapXword(Elf64_Xword xword)
{
    return (Elf64_Xword)swap64(xword);
}


static void calculateMemoryUsage(struct XXSection *section, const t_elfmemory_info *mem, t_cm_component_memory_info *pInfo)
{
    t_uint32 *statEntry;
    t_uint32 size;
    if (mem->property == MEM_SHARABLE) {
        statEntry = (t_uint32*)pInfo->firstInstantiate;
    } else {
        statEntry = (t_uint32*)pInfo->subsequentInstantiate;
    }

    /*
     * Convert from elf size to what will really be allocated.
     * CAUTION however, for X and Y this algo is not the same as the
     * calculations done in the CM allocator. The CM allocates memory
     * that is seen by the ARM core (32-bit), thus in case of X allocations we allocate
     * memory in the ARM @space "as if" the 4th byte was lost, actually it seems lost by
     * the virtue of hardware mapping, however the space is not wasted in X memory!
     *
     * the multiplicators and divisors are necessary for X 16 memory and external 24 or 16 memories
     *
     * example:
     *   - for 12 bytes in a SDRAM24 section, ie 4 words, must allocated 16 bytes in 32-bit memory
     *   - 3 bytes in XRAM24, ie one word, will allocate actually 3 bytes (! caution here)
     *   - for 4 bytes in SDRAM16 will actually allocate 4 bytes
     */
    size = section->sh_size / mem->fileEntSize * mem->memEntSize;
    statEntry[mem->purpose*CM_MM_NB_MEMTYPES + mem->convertedMemType] += size;
    //printf("%s(%s) %d: 0x%x(0x%x) -> 0x%x\n", mem->memoryName,
    //        section->sectionName, mem->memType, section->sh_size,
    //        section->sh_size / mem->fileEntSize, size);
}

t_cm_error cm_getComponentMemoryInfo(
        const char* templateName,
        t_cm_component_memory_info *pInfo)
{
    t_nmf_osal_fs_file_handle   file;
    const char        *elfdata;
    t_cm_size          size;
    const Elf64_Ehdr  *header;
    const Elf64_Shdr  *sections;
    const char        *strings;
    const t_uint16     e_machine = 0xa0;
    const t_uint8      e_class = ELFCLASS64; //only support MMDSP elf
    t_cm_error error = CM_OK;
    int i, nb;

    if((error = cm_openComponent(templateName, &file, (void**)&elfdata, &size, 1)) != CM_OK)
        return error;

    /* Sanity check */
    if (elfdata[EI_MAG0] != ELFMAG0 ||
            elfdata[EI_MAG1] != ELFMAG1 ||
            elfdata[EI_MAG2] != ELFMAG2 ||
            elfdata[EI_MAG3] != ELFMAG3 ||
            elfdata[EI_CLASS] != e_class) {
        OSAL_Log("This is not a MMDSP ELF file\n", 0, 0, 0);
	cm_closeComponent(file);
        return CM_INVALID_ELF_FILE;
    }

    header = (Elf64_Ehdr*)elfdata;
    /* Sanity check */
    if (swapHalf(header->e_machine) != e_machine) {
        OSAL_Log("This is not a executable for such MPC\n", 0, 0, 0);
	cm_closeComponent(file);
        return CM_INVALID_ELF_FILE;
    }

    nb = swapHalf(header->e_shnum);
    sections = (Elf64_Shdr*)&elfdata[swapXword(header->e_shoff)];
    strings = &elfdata[swapXword(sections[swapHalf(header->e_shstrndx)].sh_offset)];

    {
        int i = 0;
        int j = 0;
        for (i = 0; i < CM_MM_NB_PURPOSES; i++) {
            for (j = 0; j < CM_MM_NB_MEMTYPES; j++) {
                pInfo->firstInstantiate[i][j] = 0;
                pInfo->subsequentInstantiate[i][j] = 0;
            }
        }
    }

    for(i = 0; i < nb; i++) {
        struct XXSection section;
        const t_elfmemory_info *mem;
        section.sh_type     = swapWord(sections[i].sh_type);
        section.sh_info     = swapWord(sections[i].sh_info);
        section.sh_link     = swapWord(sections[i].sh_link);
        section.sh_size     = (t_uint32)swapXword(sections[i].sh_size);
        section.data        = &elfdata[(t_uint32)swapXword(sections[i].sh_offset)];
        section.sectionName = &strings[swapWord(sections[i].sh_name)];

        if (section.sh_size == 0x0)
            continue;

        mem = MMDSP_getMappingByName(section.sectionName, MEM_FOR_MULTIINSTANCE);
        if (mem)
            calculateMemoryUsage(&section, mem, pInfo);
    }

    cm_closeComponent(file);

    return error;
}
