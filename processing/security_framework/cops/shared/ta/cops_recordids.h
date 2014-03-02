/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_RECORDIDS_H
#define COPS_RECORDIDS_H


/**
 * @brief Defined records
 *
 * This defines the type of records that are known in the
 * bootpartition.
 *
 * Records with id 0xffffffff or 0x00000000 are special, those
 * records indicate that the end of the list have been reached.
 * These stop records do not need to include a valid length, this
 * makes an empty flash fullfill the requirment for those records.
 *
 * Note: Relevant id's must be keept in sync with x-loader
 *
 */
typedef enum record_id {
    RECORD_ID_STOP_READING = (int)0xffffffff,
    RECORD_ID_STOP_READING2 = 0x00000000,
    RECORD_ID_AUTHMAGIC = 0x574f5244,
    RECORD_ID_SECPROFILE = 0x45433142
} record_id_t;

#endif                          /*COPS_RECORDIDS_H */
