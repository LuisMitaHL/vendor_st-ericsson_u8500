/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* Handles generation of IL configuration data from the database
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ADM_LOG_FILENAME "db_cfg"

#undef NDEBUG
#include <ctype.h>
#include "ste_adm_db_cfg.h"
#include "ste_adm_config.h"
#include "ste_adm_db.h"
#include "ste_adm_dbg.h"
#include "OMX_Types.h" // the IL header for the structs, with nSize and nVersion

#define VALID_CFG_ITERATOR_MAGIC  0x11221144


///////////////////////////////////////////////////////////////////////////////
//
// struct_data cache
//
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    char cfg_set_name[ADM_MAX_DEVICE_NAME_LENGTH];
    char cfg_name[ADM_MAX_DEVICE_NAME_LENGTH];
    uint32_t hash;
    uint64_t last_used;
    int size;
    void *data;

} config_cache_entry_t;


#define MAX_CACHE_ENTRIES 500
#define MAX_CACHE_SIZE    200000     // The maximum amount of bytes to store in cache

static config_cache_entry_t config_cache[MAX_CACHE_ENTRIES];
static int config_cache_size = 0;
static int num_entries_in_cache = 0;


///////////////////////////////////////////////////////////////////////////////
//
// IL_Base_t
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    OMX_U32 nSize;                  /**< size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;       /**< OMX specification version information */
} IL_Base_t;





///////////////////////////////////////////////////////////////////////////////
//
// stmt_onecfg
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_onecfg
#define PID_ONECFG_STRUCT_DEF_NAME      1   // The name of the configuration set to use
#define PID_ONECFG_STRUCT_DATA_NAME     2   // The name of the configuration itself

// OUTPUT: column IDs for stmt_onecfg
#define CID_ONECFG_VALUE                    0   // The value of the parameter
#define CID_ONECFG_TYPE                    1   // The type of the value above
#define CID_ONECFG_SIZE                    2   // The size of the value in the struct, in bits
#define CID_ONECFG_OFFSET                3   // The offset of the value in the struct, in bits

static const char* stmt_onecfg_text =
    "SELECT StructData.Value, StructDef.Type,"
    " StructDef.Size, StructDef.Offset "
    " FROM StructData, StructDef"
    " WHERE StructDef.Param = StructData.Param AND"
    " StructDef.Name = ?1 AND"
    " StructData.Name = ?2 AND StructData.Value != 0";

static sqlite3_stmt* stmt_onecfg = NULL;





///////////////////////////////////////////////////////////////////////////////
//
// stmt_cfgiter
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_cfgiter
#define PID_CFGITER_NAME              1      // The component name

// OUTPUT: column IDs for stmt_cfgiter
#define CID_CFGITER_CONFIG_TYPE       0             // Configuration type, Config/Param
#define CID_CFGITER_CONFIG_INDEX_NAME 1          // String for OMX_GetExtensionIndex
#define CID_CFGITER_CONFIG_STRUCT_DEF_NAME   2   // The struct definition to use
#define CID_CFGITER_STRUCT_DATA_NAME         3   // The struct data to use

static const char* stmt_cfgiter_text =
    "SELECT ConfigType, IndexName, StructDef, StructData"
    " FROM Template_Config WHERE Name = ?1";

static sqlite3_stmt* stmt_cfgiter               = NULL;
static int             stmt_cfgiter_busy          = 0;





///////////////////////////////////////////////////////////////////////////////
//
// stmt_structsize
//
///////////////////////////////////////////////////////////////////////////////

// INPUT: parameter IDs for stmt_structsize
#define PID_STRUCTSIZE_STRUCT_DEF_NAME                1

// OUTPUT: column IDs for stmt_structsize
#define CID_STRUCTSIZE_BITSIZE                          0

static const char* stmt_structsize_text =
    "SELECT MAX(StructDef.Size + StructDef.Offset)"
    " FROM StructDef"
    " WHERE StructDef.Name = ?1";

static sqlite3_stmt* stmt_structsize            = NULL;




///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_is_busy
//
///////////////////////////////////////////////////////////////////////////////
int adm_db_cfg_is_busy()
{
    return stmt_cfgiter_busy;
}

///////////////////////////////////////////////////////////////////////////////
//
// get_error_message
//
///////////////////////////////////////////////////////////////////////////////
static const char* get_error_message()
{
    sqlite3* db;
    ste_adm_res_t res = adm_db_get_handle(&db);
    ADM_ASSERT(res == STE_ADM_RES_OK);
    return sqlite3_errmsg(db);
}



///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_init
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_cfg_close()
{


    ste_adm_res_t res = STE_ADM_RES_OK;

    // sqlite3_finalize(NULL) is a valid call
    int rc = sqlite3_finalize(stmt_onecfg);
    if (rc != SQLITE_OK) {
          ALOG_ERR("sqlite3_finalize failed with error %d in adm_db_cfg_close\n", rc);
        res = STE_ADM_RES_DB_FATAL;
    }
    stmt_onecfg = NULL;

    // If stmt_cfgiter exists, assert it is not busy
    ADM_ASSERT(stmt_cfgiter == NULL || !stmt_cfgiter_busy);
    rc = sqlite3_finalize(stmt_cfgiter);
    if (rc != SQLITE_OK) {
          ALOG_ERR("sqlite3_finalize failed with error %d in adm_db_cfg_close\n", rc);
        res = STE_ADM_RES_DB_FATAL;
    }
    stmt_cfgiter = NULL;

    rc = sqlite3_finalize(stmt_structsize);
    if (rc != SQLITE_OK) {
          ALOG_ERR("sqlite3_finalize failed with error %d in adm_db_cfg_close\n", rc);
        res = STE_ADM_RES_DB_FATAL;
    }
    stmt_structsize = NULL;

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_init_statements
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t adm_db_cfg_init_statements()
{
    sqlite3* db;
    ste_adm_res_t res = adm_db_get_handle(&db);
    int rc;

    // We are only allowed to be called if the database is open..
    ADM_ASSERT(res == STE_ADM_RES_OK);

    // Prepare the stmt_onecfg statement
    if (stmt_onecfg == NULL)
    {
        rc = sqlite3_prepare_v2(db, stmt_onecfg_text, -1, &stmt_onecfg, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(db));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_onecfg statement
    if (stmt_cfgiter == NULL)
    {
        rc = sqlite3_prepare_v2(db, stmt_cfgiter_text, -1, &stmt_cfgiter, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(db));
            return STE_ADM_RES_DB_FATAL;
        }
    }

    // Prepare the stmt_structsize statement
    if (stmt_structsize == NULL)
    {
        rc = sqlite3_prepare_v2(db, stmt_structsize_text, -1, &stmt_structsize, NULL);
        if (rc != SQLITE_OK) {
            ALOG_ERR("sqlite3_prepare_v2 failed with error %s\n", sqlite3_errmsg(db));
            return STE_ADM_RES_DB_FATAL;
        }
    }



    return STE_ADM_RES_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_init
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_cfg_init()
{
    // Not initialized? Due to dynamic libs?
    ADM_ASSERT(stmt_onecfg == 0);
    ADM_ASSERT(stmt_cfgiter == 0);
    ADM_ASSERT(stmt_cfgiter_busy == 0);
    ADM_ASSERT(stmt_structsize == 0);

    //ALOG_INFO("Bravely executing ADM_ASSERT(0)\n");
    //assert(0); // check if assert is active
    stmt_onecfg = 0;
    stmt_cfgiter = 0;
    stmt_cfgiter_busy = 0;
    stmt_structsize = 0;

    memset(config_cache, 0, MAX_CACHE_ENTRIES);


    if (adm_db_cfg_init_statements() != STE_ADM_RES_OK)
    {
        ALOG_ERR("adm_db_cfg_init_statements failed \n");
        return STE_ADM_RES_DB_FATAL;
    }

    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_reinitialize_statement
//
///////////////////////////////////////////////////////////////////////////////
static void adm_db_cfg_reinitialize_statement(sqlite3_stmt** statement)
{
    sqlite3_finalize(*statement);
    *statement = NULL;
    if (adm_db_cfg_init_statements() != STE_ADM_RES_OK)
    {
        ALOG_ERR("Failed to reinitialize statement\n");
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_create_iter
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_cfg_create_iter(adm_db_cfg_iter_t* iter, const char* comp_name)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(comp_name);

    // For now, we only allow a single instance of each iterator.
    // Not documented, so not an assert
    if (stmt_cfgiter_busy) {
        ALOG_ERR("cfg_iter_create - currently only one iterator instance is supported\n");
        return STE_ADM_RES_DB_FATAL;
    }

    // Reset the statement we want to use
    int rc = sqlite3_reset(stmt_cfgiter);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset(stmt_cfgiter) failed with error %s in cfg_iter_create\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    // Specify the device name to filter on
    // TODO: Check deallocation of SQLITE_TRANSIENT string memory
    // if the query fails (will be reset and re-bind, ok?)
    rc = sqlite3_bind_text(stmt_cfgiter, PID_CFGITER_NAME, comp_name,
                -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in cfg_iter_create\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    iter->cfg_bufsz   = 0;
    iter->cfg_buf     = NULL;
    stmt_cfgiter_busy = 1;
    iter->magic       = VALID_CFG_ITERATOR_MAGIC;
    iter->end_reached = 0;

    ALOG_INFO("DB CFG ITER: Create\n");

    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// val_to_hex
//
///////////////////////////////////////////////////////////////////////////////
static int val_to_hex(char c)
{
  int v = -1; // Conversion failed
  if (c >= '0' && c <= '9')
    v = c - '0';
  else {
    int c2 = toupper(c);
    if (c2 >= 'A' && c2 <= 'F')
        v = c2 - 'A' + 10;
    }
  return v;
}

///////////////////////////////////////////////////////////////////////////////
//
// get_hex
//
///////////////////////////////////////////////////////////////////////////////
static int get_hex(const char *buf)
{
  if (buf[0] == 0 || buf[1] == 0)
    return -1;

  int v1 = val_to_hex(buf[0]);
  int v2 = val_to_hex(buf[1]);

  if (v1 == -1 || v2 == -1)
    return -1;

  return v1 * 16 + v2;
}



///////////////////////////////////////////////////////////////////////////////
//
// extract_integer
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t extract_integer(char* dstbuf, unsigned int item_size)
{
    int v = sqlite3_column_int(stmt_onecfg, CID_ONECFG_VALUE);
    switch (item_size) {
        case 32:
            memcpy(dstbuf, &v, 4);
            break;
        case 16:  {
            short v16 = (short) v;
            memcpy(dstbuf, &v16, 2);
            break;
        }
        case 8: {
            char v8 = (char) v;
            memcpy(dstbuf, &v8, 1);
            break;
        }
        default: {
            ALOG_WARN("Unsupported size %d, ignoring\n", item_size);
            return STE_ADM_RES_DB_INCONSISTENT;
        }
    }

    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// extract_text
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t extract_text(char* dstbuf, unsigned int item_size)
{
    const char* v = (const char*) sqlite3_column_text(
                stmt_onecfg, CID_ONECFG_VALUE);
    if (v == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in extract_config_entry\n");
        return STE_ADM_RES_DB_FATAL;
    }

    unsigned int max_out = item_size/8;
    if (max_out >= 1) {
        // max_out instead of max_out-1 intended for warning below
        strncpy(dstbuf, v, max_out);

        if (dstbuf[max_out-1])
            ALOG_WARN("String '%s' truncated to '%.*s'\n", v, max_out-1, v);
        dstbuf[max_out-1] = 0;
    }

    return STE_ADM_RES_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// extract_rawhex
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t extract_rawhex(char* dstbuf, unsigned int item_size)
{
    ADM_ASSERT(item_size%8 == 0);
    ADM_ASSERT(item_size   >= 8);

    const char* v = (const char*) sqlite3_column_text(
                stmt_onecfg, CID_ONECFG_VALUE);
    if (v == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in extract_config_entry\n");
        return STE_ADM_RES_DB_FATAL;
    }

    unsigned int max_out = item_size/8;
    unsigned int pos; // TODO: Move into for loop once C99 compilation works
    for (pos=0 ; pos < max_out ; pos++) {
        int curval = get_hex(v + 2*pos); // -1 if failed, or [0,255] if OK.
        if (curval == -1) {
            ALOG_WARN("Hex string contained error around position %d (too short? invalid char? odd length?)\n", 2*pos);
            return STE_ADM_RES_DB_INCONSISTENT;
        }

        dstbuf[pos] = (char) curval; // TODO: cleaner if dstbuf is unsigned char
    }

    if (v[max_out*2] != 0) {
        ALOG_WARN("Hex string contained unparsed characters at end\n");
    }

    return STE_ADM_RES_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// extract_config_entry
//
// Caller guarantees that there is enough space in the output buffer (iter->cfg_buf)
// for (item_size+7)/8 bytes
//
///////////////////////////////////////////////////////////////////////////////

// TODO: Split into three functions
static ste_adm_res_t extract_config_entry(adm_db_cfg_iter_t* iter)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(iter->cfg_buf);

    unsigned int item_size = (unsigned int) sqlite3_column_int(
            stmt_onecfg, CID_ONECFG_SIZE);

    unsigned int item_offs = (unsigned int) sqlite3_column_int(
            stmt_onecfg, CID_ONECFG_OFFSET);

    // We don't want the customers to modify the first 32 bits of
    // the struct - the nSize.
    // FUTURE IMPROVEMENT: Or allow this, for empty fields in the end of the struct?
    if (item_offs <= 32) {
        ALOG_WARN("Tried to modify the size part of the struct, not allowed.\n");
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    if (item_offs%8 != 0) {
        ALOG_WARN("Unsupported offset %u, ignoring\n", item_offs);
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    if (item_size == 0 || item_size%8 != 0) {
        ALOG_WARN("Unsupported item size %u, ignoring\n", item_size);
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    char* dstbuf = iter->cfg_buf + item_offs/8;

    const char* type = (const char*) sqlite3_column_text(
                stmt_onecfg, CID_ONECFG_TYPE);
    if (type == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in extract_config_entry\n");
        return STE_ADM_RES_DB_FATAL;
    }

    if (strcmp(type, "int") == 0) {
        return extract_integer(dstbuf, item_size);
    } else if (strcmp(type, "text") == 0) {
        return extract_text(dstbuf, item_size);
    } else if (strcmp(type, "rawhex") == 0) {
        return extract_rawhex(dstbuf, item_size);
    } else {
        ALOG_ERR("Unsupported parameter type '%s'\n", type);
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    return STE_ADM_RES_OK;
}


//
//
//
static ste_adm_res_t allocate_struct(adm_db_cfg_iter_t* iter, const char* cfg_set_name)
{
    // Get size of struct. Since not all mebers necessarily have values,
    // the struct definition table needs to be consulted.
    // Reset the statement we want to use
    int rc = sqlite3_reset(stmt_structsize);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset(stmt_structsize) failed with error %s in cfg_iter_create\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    // Specify struct to get size of
    rc = sqlite3_bind_text(stmt_structsize, PID_STRUCTSIZE_STRUCT_DEF_NAME, cfg_set_name,
                -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in cfg_iter_create\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    rc = sqlite3_step(stmt_structsize);
    if (rc != SQLITE_ROW) {
        // If there are no elements found, the result will be 0 bytes, so SQLITE_DONE means some other fatal error.
        // While executing this for a step that does not match any lines, we get a valgrind warning
        // inside sqlite. ADM seems to provide correct input values.
        ALOG_ERR("sqlite3_step failed with error %d in allocate_struct\n", rc);
        adm_db_cfg_reinitialize_statement(&stmt_structsize);
        return STE_ADM_RES_DB_FATAL;
    }

    unsigned int last_bit_pos = (unsigned int) sqlite3_column_int(stmt_structsize, CID_STRUCTSIZE_BITSIZE);

    // Allow empty struct
    if (last_bit_pos == 0) {
        ALOG_INFO("The struct %s is empty; assuming IL-header only\n", cfg_set_name);
        last_bit_pos = 8 * sizeof(IL_Base_t);
    }


    // calculate last position in bytes
    unsigned int last_byte_pos = ((last_bit_pos + 7) / 8);

    // round to multiple of 4 bytes
    last_byte_pos = (last_byte_pos + 3) & ~3u;

    if (last_byte_pos < sizeof(IL_Base_t)) {
        ALOG_ERR("structure size (%d bytes) too small\n", last_byte_pos);
        return STE_ADM_RES_DB_FATAL;
    }


    // Allocate a buffer for the size of the current struct
    if (iter->cfg_buf != NULL)
        free(iter->cfg_buf);

    iter->cfg_bufsz = last_byte_pos;
    iter->cfg_buf = malloc(iter->cfg_bufsz);
    if (iter->cfg_buf == NULL) {
        ALOG_ERR("malloc failed for %u bytes\n", last_byte_pos);
        return STE_ADM_RES_DB_FATAL;
    }



    // Clear the config.
    (void) memset(iter->cfg_buf, 0, iter->cfg_bufsz);

    // Fill in the OMX version in the struct. Doing it before the while statement
    // below allows customer to override via the db.
    ((IL_Base_t*) iter->cfg_buf)->nVersion.nVersion = OMX_VERSION;

    // An empty struct is not very useful, but this allows
    // us to create such one.
    ((IL_Base_t*) iter->cfg_buf)->nSize = iter->cfg_bufsz; // FUTURE IMP: Now use nSize instead of cfg_bufsz?

    return STE_ADM_RES_OK;
}

static uint64_t get_time_in_millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (1000 * ((uint64_t)tv.tv_sec)) + tv.tv_usec/1000;
}

static uint32_t calc_hash(const char* cfg_set_name, const char* cfg_name)
{
    uint32_t hash  = 0;

    const char *ch = cfg_set_name;

    while(*ch != '\0') {
        hash = *ch + 31 * hash;
        ch++;
    }
    ch = cfg_name;
    while(*ch != '\0') {
        hash = *ch + 31 * hash;
        ch++;
    }

    return hash;
}



void clear_cached_config_data()
{
    int i;

    for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
        free(config_cache[i].data);
        config_cache[i].data = NULL;
    }
    ALOG_INFO("Config cache cleared.");
}

int get_cached_config_data(const char* cfg_set_name, const char* cfg_name,
                           const void** config_data)
{
    int i;
    uint32_t hash =  calc_hash(cfg_set_name, cfg_name);

    for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
        if (config_cache[i].data != NULL && config_cache[i].hash == hash) {
            if ((strncmp(cfg_set_name, config_cache[i].cfg_set_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0) &&
                (strncmp(cfg_name, config_cache[i].cfg_name, ADM_MAX_DEVICE_NAME_LENGTH) == 0)) {

                *config_data = config_cache[i].data;
                config_cache[i].last_used = get_time_in_millis();
                ALOG_INFO_VERBOSE("get_cached_config_data found (%s, %s) at %d\n", cfg_set_name, cfg_name, i);
                return 1;
            }
        }
    }
    ALOG_INFO_VERBOSE("get_cached_config_data (%s, %s) NOT found\n", cfg_set_name, cfg_name);

    return 0;
}

void add_config_data_to_cache(const char* cfg_set_name, const char* cfg_name,
                             const void* config_data, int size)
{
    int i;
    int oldest_entry = -1;
    uint64_t oldest_time = UINT64_MAX;

    if (config_cache_size >= MAX_CACHE_SIZE || num_entries_in_cache == MAX_CACHE_ENTRIES) {
        do {
            for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
                if (config_cache[i].data != NULL &&
                    config_cache[i].last_used < oldest_time) {
                    oldest_time = config_cache[i].last_used;
                    oldest_entry = i;
                }
            }

            i = oldest_entry;
            config_cache_size -= config_cache[i].size;
            num_entries_in_cache--;
            free (config_cache[i].data);
            config_cache[i].data = NULL;
        } while (config_cache_size >= MAX_CACHE_SIZE || num_entries_in_cache == MAX_CACHE_ENTRIES);
    }
    else {
        for (i = 0; i < MAX_CACHE_ENTRIES; i++) {
            if (config_cache[i].data == NULL) {
                break;
            }
        }
    }

    config_cache[i].data = malloc(size);
    if (config_cache[i].data == NULL) {
        ALOG_ERR("add_config_data_to_cache failed to allocate %d bytes of memory!", size);
        return;
    }
    strncpy(config_cache[i].cfg_set_name, cfg_set_name, ADM_MAX_DEVICE_NAME_LENGTH);
    strncpy(config_cache[i].cfg_name, cfg_name, ADM_MAX_DEVICE_NAME_LENGTH);
    config_cache[i].size = size;
    memcpy(config_cache[i].data, config_data, config_cache[i].size);
    config_cache[i].last_used = get_time_in_millis();
    config_cache[i].hash = calc_hash(cfg_set_name, cfg_name);
    config_cache_size += size;
    num_entries_in_cache++;
    ALOG_INFO_VERBOSE("add_config_data_to_cache (%s, %s) at %d\n", cfg_set_name, cfg_name, i);
}



///////////////////////////////////////////////////////////////////////////////
//
// create_config_data
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t create_config_data(adm_db_cfg_iter_t* iter,
                        const char* cfg_set_name, const char* cfg_name,
                        const void** config_data)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(cfg_set_name);
    ADM_ASSERT(cfg_name);
    ADM_ASSERT(stmt_onecfg);
    ADM_ASSERT(config_data);

    if (get_cached_config_data(cfg_set_name, cfg_name, config_data)) {
        return STE_ADM_RES_OK;
    }

    // Reset the statement we want to use
    int rc = sqlite3_reset(stmt_onecfg);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_reset(stmt_onecfg) failed with error %s in create_config_data\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    // Specify the comonent config name to filter on
    rc = sqlite3_bind_text(stmt_onecfg, PID_ONECFG_STRUCT_DEF_NAME, cfg_set_name,
                -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in create_config_data\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }

    // Specify the comonent config name to filter on
    rc = sqlite3_bind_text(stmt_onecfg, PID_ONECFG_STRUCT_DATA_NAME, cfg_name,
                -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        ALOG_ERR("sqlite3_bind_text failed with error %s in create_config_data\n", get_error_message());
        return STE_ADM_RES_DB_FATAL;
    }


    if (allocate_struct(iter, cfg_set_name) != STE_ADM_RES_OK) {
        ALOG_ERR("Error in create_config_data: allocate_struct failed\n");
        return STE_ADM_RES_DB_FATAL;
    }

    //
    // From this point, iter->cfg_buf is allocated and must be released
    // unless the iterator is successfully created
    //
    ADM_ASSERT(iter->cfg_buf != NULL);

    while( (rc=sqlite3_step(stmt_onecfg)) == SQLITE_ROW) {
        ste_adm_res_t res = extract_config_entry(iter);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("Error in config set '%s' / config_name '%s'\n", cfg_set_name, cfg_name);
            ADM_ASSERT(iter->cfg_buf != NULL);
            free(iter->cfg_buf);
            iter->cfg_buf = NULL;
            adm_db_cfg_reinitialize_statement(&stmt_onecfg);
            return res;
        }
    }

    if (rc != SQLITE_DONE) {
        ALOG_ERR("sqlite3_step failed in create_config_data, returncode %d", rc);
        ALOG_ERR("--> while parsing configset '%s', config_name '%s'\n", cfg_set_name, cfg_name);
        ADM_ASSERT(iter->cfg_buf != NULL);
        free(iter->cfg_buf);
        iter->cfg_buf = NULL;
        adm_db_cfg_reinitialize_statement(&stmt_onecfg);
        return STE_ADM_RES_DB_FATAL;
    }

    add_config_data_to_cache(cfg_set_name, cfg_name, iter->cfg_buf, iter->cfg_bufsz);

    *config_data = iter->cfg_buf;
    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_iter_get_next
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_db_cfg_iter_get_next(adm_db_cfg_iter_t* iter,
                                  int*                   is_param,
                                  const char**           index_name,
                                  const void**           config_data)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(is_param);
    ADM_ASSERT(index_name);
    ADM_ASSERT(config_data);
    ADM_ASSERT(stmt_cfgiter_busy);
    ADM_ASSERT(iter->magic == VALID_CFG_ITERATOR_MAGIC);

    int rc = SQLITE_DONE;
    if (!iter->end_reached)
        rc = sqlite3_step(stmt_cfgiter);

    if (rc == SQLITE_DONE) {
        // sqlite3_step must only return SQLITE_DONE at most once
        iter->end_reached = 1;
        return STE_ADM_RES_NO_MORE;
    }

    if (rc != SQLITE_ROW) {
        ALOG_ERR("sqlite3_step failed with '%s' (%u) in cfg_iter_get_next\n", get_error_message(), rc);
        adm_db_cfg_reinitialize_statement(&stmt_cfgiter);
        return STE_ADM_RES_DB_FATAL;
    }

    const char* cfg_type = (const char *) sqlite3_column_text(
                    stmt_cfgiter, CID_CFGITER_CONFIG_TYPE);
    if (!cfg_type) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in cfg_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }
    if (strcmp(cfg_type, "Param") == 0)
        *is_param = 1;
    else if (strcmp(cfg_type, "Config") == 0)
        *is_param = 0;
    else if (strcmp(cfg_type, "Design") == 0) {
	/* don't send Audio Tuning Tool design parameters to the effect */
        return STE_ADM_RES_OK;
    }
    else {
        ALOG_ERR("io_info - inconsistent db entry: cfg_type = '%s'\n",cfg_type);
        return STE_ADM_RES_DB_INCONSISTENT;
    }

    *index_name = (const char *) sqlite3_column_text(stmt_cfgiter,
                CID_CFGITER_CONFIG_INDEX_NAME);
    if (*index_name == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in cfg_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }

    const char* cfg_set_name = (const char *) sqlite3_column_text(stmt_cfgiter,
                CID_CFGITER_CONFIG_STRUCT_DEF_NAME);
    if (cfg_set_name == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in cfg_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }

    const char* cfg_name = (const char *) sqlite3_column_text(stmt_cfgiter,
                CID_CFGITER_STRUCT_DATA_NAME);
    if (cfg_name == NULL) { // out of memory in Linux == exhausted address space ==> fatal
        ALOG_ERR("memory allocation failed in cfg_iter_get_next\n");
        return STE_ADM_RES_DB_FATAL;
    }


    return create_config_data(iter, cfg_set_name, cfg_name, config_data);
}

///////////////////////////////////////////////////////////////////////////////
//
// adm_db_cfg_iter_destroy
//
///////////////////////////////////////////////////////////////////////////////
void adm_db_cfg_iter_destroy(adm_db_cfg_iter_t* iter)
{
    ADM_ASSERT(iter);
    ADM_ASSERT(stmt_cfgiter_busy);
    ADM_ASSERT(iter->magic == VALID_CFG_ITERATOR_MAGIC);

    ALOG_INFO("DB CFG ITER: Destroy\n");

    if (iter->cfg_buf) {
        // debug: overwrite it to prevent accidental successful use
        memset(iter->cfg_buf, 0, iter->cfg_bufsz);
        free(iter->cfg_buf);
    }

    stmt_cfgiter_busy = 0;
    memset(iter,0,sizeof(*iter));
}

