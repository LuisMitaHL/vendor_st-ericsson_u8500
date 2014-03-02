/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_common.h>
#include <cops_bootpartitionhandler.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
#define LOCKSTATUS_BUFFERSIZE       10
#define LOCKSTATUS_PERM_RO          "perm_ro"
#define LOCKSTATUS_PWR_RO           "pwr_ro"
#define LOCKSTATUS_RW               "rw"

#define BOOTPART_SYSFS_RW_VALUE     '0'
#define BOOTPART_SYSFS_RO_VALUE     '1'

#define RECORD_MIN_SIZE (sizeof(uint32_t) * 2)

typedef struct records {
    record_id_t id;
    uint32_t size;
    uint8_t data[];
} records_t;

/**
 * Bootpartition lock status
 */
typedef enum cops_bootpart_lockstatus {
    /** bootpartition is open (read/write) */
    COPS_BOOTPART_LOCKSTATUS_RW,
    /** bootpartition is locked until powered off */
    COPS_BOOTPART_LOCKSTATUS_PWR_RO,
    /** bootpartition is permanently locked */
    COPS_BOOTPART_LOCKSTATUS_PERM_RO
} cops_bootpart_lockstatus_t;

static uint8_t *recorddata = NULL;
static ssize_t recorddatasize = 0;
static off_t rpos = 0;
static uint8_t fillvalue = 0;

static int bootpart_sysfs_ro_fd = -1;
static int bootpart_dev_fd = -1;
static int bootpart_sysfs_rolock_fd = -1;

static cops_return_code_t
cops_bootpart_get_lock_status(cops_bootpart_lockstatus_t *lockstatus);

static cops_return_code_t readallrecords(void);

static cops_return_code_t writeallrecords(void);

static cops_return_code_t remove_record(record_id_t id, bool *modified);

static void freerecords(void);

static cops_return_code_t allocrecord(records_t **record, record_id_t recordid,
                                      uint32_t recordsize);

static size_t getdatasize(uint32_t recordsize);

static size_t getrecordsize(uint32_t datasize);
#endif

cops_return_code_t cops_bootpart_open(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
    char rw_value = BOOTPART_SYSFS_RW_VALUE;
    cops_bootpart_lockstatus_t rostatus = COPS_BOOTPART_LOCKSTATUS_RW;

    bootpart_sysfs_rolock_fd = open(STR(BOOTPARTITION_ROLOCK_SYSFSNODE),
                                    O_RDWR);

    if (bootpart_sysfs_rolock_fd == -1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Could not open %s\n", STR(BOOTPARTITION_ROLOCK_SYSFSNODE));
    }

    /* check permanent lock as opening the bootpartition in read-write mode
     * fails if it is permanently locked */
    COPS_CHK_RC(cops_bootpart_get_lock_status(&rostatus));

    if (rostatus == COPS_BOOTPART_LOCKSTATUS_RW) {
        bootpart_sysfs_ro_fd = open(STR(BOOTPARTITION_RO_SYSFSNODE), O_RDWR);

        if (bootpart_sysfs_ro_fd == -1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not open %s\n", STR(BOOTPARTITION_RO_SYSFSNODE));
        }

        if (write(bootpart_sysfs_ro_fd, &rw_value, sizeof(rw_value))
                != sizeof(rw_value)) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not clear readonlyflag\n");
        }

        bootpart_dev_fd = open(STR(BOOTPARTITION_DEV), O_RDWR);

        if (bootpart_dev_fd == -1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not open %s\n", STR(BOOTPARTITION_DEV));
        }
    }

function_exit:
#endif
    return ret_code;
}

cops_return_code_t cops_bootpart_close(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
    char ro_value = BOOTPART_SYSFS_RO_VALUE;

    if (bootpart_dev_fd != -1) {
        if (close(bootpart_dev_fd) != 0) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Error when closing %s\n", STR(BOOTPARTITION_DEV));
        }

        bootpart_dev_fd = -1;
    }

    if (bootpart_sysfs_ro_fd != -1) {
        if (lseek(bootpart_sysfs_ro_fd, 0, SEEK_SET) == (off_t)-1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Unable to set offset %s\n", strerror(errno));
        }

        if (write(bootpart_sysfs_ro_fd, &ro_value, sizeof(ro_value))
                != sizeof(ro_value)) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not setreadonlyflag\n");
        }

        if (close(bootpart_sysfs_ro_fd) != 0) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Error when closing %s\n", STR(BOOTPARTITION_RO_SYSFSNODE));
        }

        bootpart_sysfs_ro_fd = -1;
    }

    if (bootpart_sysfs_rolock_fd != -1) {
        if (close(bootpart_sysfs_rolock_fd) != 0) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
               "Error when closing %s\n", STR(BOOTPARTITION_ROLOCK_SYSFSNODE));
        }

        bootpart_sysfs_rolock_fd = -1;
    }

function_exit:
#endif
    return ret_code;
}

cops_return_code_t cops_bootpart_lock_partition(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
    cops_bootpart_lockstatus_t rostatus = COPS_BOOTPART_LOCKSTATUS_RW;

    if (bootpart_sysfs_rolock_fd == -1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "Bootpartition lock not open\n");
    }

    COPS_CHK_RC(cops_bootpart_get_lock_status(&rostatus));

    if (rostatus == COPS_BOOTPART_LOCKSTATUS_RW) {
        if (lseek(bootpart_sysfs_rolock_fd, 0, SEEK_SET) == (off_t)-1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Unable to set offset %s\n", strerror(errno));
        }

        if (write(bootpart_sysfs_rolock_fd, LOCKSTATUS_PERM_RO,
                  strlen(LOCKSTATUS_PERM_RO)) != strlen(LOCKSTATUS_PERM_RO)) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not permanently lock the bootpartition\n");
        }
    } else if (rostatus == COPS_BOOTPART_LOCKSTATUS_PWR_RO) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Bootpartition is temporarily locked\n");
    }

function_exit:
#endif
    return ret_code;
}

cops_return_code_t cops_bootpart_remove_record(record_id_t id)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
    cops_bootpart_lockstatus_t rostatus = COPS_BOOTPART_LOCKSTATUS_RW;

    COPS_CHK_RC(cops_bootpart_get_lock_status(&rostatus));

    if (rostatus == COPS_BOOTPART_LOCKSTATUS_RW) {
        bool modified = false;

        COPS_CHK_RC(readallrecords());

        COPS_CHK_RC(remove_record(id, &modified));

        if (modified == true) {
            COPS_CHK_RC(writeallrecords());
        }
    } else {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Bootpartition is locked\n");
    }

function_exit:
    freerecords();
#else
    (void)id;
#endif
    return ret_code;
}

cops_return_code_t cops_bootpart_write_record(record_id_t id,
        uint32_t datasize,
        uint8_t *data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
    cops_bootpart_lockstatus_t rostatus = COPS_BOOTPART_LOCKSTATUS_RW;

    if (datasize & 0x3) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Datasize is not even 4 bytes");
    }

    COPS_CHK_RC(cops_bootpart_get_lock_status(&rostatus));

    if (rostatus == COPS_BOOTPART_LOCKSTATUS_RW) {
        bool modified = false;
        records_t *current = 0;

        COPS_CHK_RC(readallrecords());

        COPS_CHK_RC(remove_record(id, &modified));

        COPS_CHK_RC(allocrecord(&current, id, getrecordsize(datasize)));

        if (datasize > 0) {
            memcpy(current->data, data, datasize);
        }

        COPS_CHK_RC(writeallrecords());
    } else {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
            "Bootpartition is locked (temporarily or permanent)");
    }

function_exit:
    freerecords();
#else
    (void)id;
    (void)datasize;
    (void)data;
#endif
    return ret_code;
}

#if !defined COPS_IN_LOADERS && defined COPS_BOOTPART_SUPPORT
static cops_return_code_t
cops_bootpart_get_lock_status(cops_bootpart_lockstatus_t *lockstatus)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    ssize_t readlength = 0;
    char buffer[LOCKSTATUS_BUFFERSIZE];

    if (bootpart_sysfs_rolock_fd == -1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "Bootpartition lock not open\n");
    }

    if (lseek(bootpart_sysfs_rolock_fd, 0, SEEK_SET) == (off_t)-1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Unable to set offset %s\n", strerror(errno));
    }

    readlength = read(bootpart_sysfs_rolock_fd, buffer, LOCKSTATUS_BUFFERSIZE);
    buffer[readlength] = 0;

    if (!strncmp(buffer, LOCKSTATUS_PERM_RO, strlen(LOCKSTATUS_PERM_RO)) &&
        readlength >= (ssize_t)strlen(LOCKSTATUS_PERM_RO)) {
        *lockstatus = COPS_BOOTPART_LOCKSTATUS_PERM_RO;
    } else if (!strncmp(buffer, LOCKSTATUS_PWR_RO, strlen(LOCKSTATUS_PWR_RO)) &&
                readlength >= (ssize_t)strlen(LOCKSTATUS_PWR_RO)) {
        *lockstatus = COPS_BOOTPART_LOCKSTATUS_PWR_RO;
    } else if (!strncmp(buffer, LOCKSTATUS_RW, strlen(LOCKSTATUS_RW)) &&
                readlength >= (ssize_t)strlen(LOCKSTATUS_RW)) {
        *lockstatus = COPS_BOOTPART_LOCKSTATUS_RW;
    } else {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Could not get proper status %s\n", strerror(errno));
    }

function_exit:

    return ret_code;
}

static cops_return_code_t remove_record(record_id_t id, bool *modified)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    records_t *record = 0;

    *modified = false;

    if (recorddata != NULL) {
        record = (records_t *)recorddata;

        while ((uint8_t *)record < (recorddata + recorddatasize)) {
            size_t recordsize = record->size;

            if (record->id == id) {
                uint8_t *tmprecorddata = recorddata;
                void *data = (uint8_t *)record + recordsize;
                size_t dataleft = recorddatasize -
                                  ((uint8_t *)record - recorddata) - recordsize;
                size_t newrecordsize = recorddatasize - recordsize;

                memmove(record, data, dataleft);

                tmprecorddata = realloc(recorddata, newrecordsize);

                if (newrecordsize > 0) {
                    COPS_CHK_ALLOC(tmprecorddata);
                }

                recorddata = tmprecorddata;
                recorddatasize = newrecordsize;

                *modified = true;
                break;
            }

            record = (records_t *)(((uint8_t *)record) + recordsize);
        }
    }

function_exit:

    return ret_code;
}

static cops_return_code_t writeallrecords(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    off_t wpos = 0;
    uint8_t *buf = NULL;

    if (recorddata != NULL) {
        if (bootpart_dev_fd == -1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR, "Bootpartition not open\n");
        }

        if (lseek(bootpart_dev_fd, 0, SEEK_SET) == (off_t)-1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Unable to set offset %s\n", strerror(errno));
        }

        if (write(bootpart_dev_fd, recorddata, recorddatasize)
                != recorddatasize) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not write record size, %s\n", strerror(errno));
        }

        wpos = lseek(bootpart_dev_fd, 0, SEEK_CUR);

        if (wpos == (off_t)-1) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Unable to get offset %s\n", strerror(errno));
        }

        if (wpos < rpos) {
            ssize_t bufsize = rpos - wpos;

            buf = malloc(bufsize);
            COPS_CHK_ALLOC(buf);

            memset(buf, fillvalue, bufsize);

            if (write(bootpart_dev_fd, buf, bufsize) != bufsize) {
                COPS_SET_RC(COPS_RC_STORAGE_ERROR, "Could not fill data, %s\n",
                            strerror(errno));
            }
        }
    }

function_exit:
    if (buf != NULL) {
        free(buf);
    }

    return ret_code;
}

static cops_return_code_t readallrecords(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    record_id_t recordid = RECORD_ID_STOP_READING;

    if (bootpart_dev_fd == -1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "Bootpartition not open\n");
    }

    if (lseek(bootpart_dev_fd, 0, SEEK_SET) == (off_t)-1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Unable to set offset %s\n", strerror(errno));
    }

    do {
        if (read(bootpart_dev_fd, &recordid, sizeof(recordid)) !=
            sizeof(recordid)) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                        "Could not read record id %s\n", strerror(errno));
        }

        if (!(recordid == RECORD_ID_STOP_READING ||
            recordid == RECORD_ID_STOP_READING2)) {
            uint32_t recordsize;
            records_t *current = 0;
            ssize_t datasize = 0;

            if (read(bootpart_dev_fd, &recordsize, sizeof(recordsize)) !=
                sizeof(recordsize)) {
                COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                            "Could not read record size %s\n", strerror(errno));
            }

            COPS_CHK_RC(allocrecord(&current, recordid, recordsize));
            datasize = getdatasize(current->size);

            if (datasize & 0x3) {
                COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                            "Datasize is not even 4 bytes\n");
            }

            if (datasize > 0) {
                if (read(bootpart_dev_fd, current->data, datasize)
                    != datasize) {
                    COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                            "Could not read record data %s\n", strerror(errno));
                }
            }
        }

    } while (!(recordid == RECORD_ID_STOP_READING ||
               recordid == RECORD_ID_STOP_READING2));

    fillvalue = recordid;
    rpos = lseek(bootpart_dev_fd, 0, SEEK_CUR);

    if (rpos == (off_t)-1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Unable to get offset %s\n", strerror(errno));
    }

function_exit:

    return ret_code;
}

static void freerecords(void)
{
    if (recorddata != NULL) {
        free(recorddata);
        recorddata = NULL;
        recorddatasize = 0;
    }
}

static cops_return_code_t allocrecord(records_t **record, record_id_t recordid,
                                      uint32_t recordsize)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t *tmprecorddata = recorddata;

    if (record == NULL || recordsize < RECORD_MIN_SIZE) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "No where to store resulting "
           "addrees or too small size, id %d size %d\n", recordid, recordsize);
    }

    tmprecorddata = realloc(recorddata, recorddatasize + recordsize);
    COPS_CHK_ALLOC(tmprecorddata);

    *record = (records_t *)(tmprecorddata + recorddatasize);
    (*record)->id = recordid;
    (*record)->size = recordsize;

    recorddata = tmprecorddata;
    recorddatasize = recorddatasize + recordsize;

function_exit:
    return ret_code;
}

static size_t getdatasize(uint32_t recordsize)
{
    return recordsize - RECORD_MIN_SIZE;
}

static size_t getrecordsize(uint32_t datasize)
{
    return datasize + RECORD_MIN_SIZE;
}
#endif
