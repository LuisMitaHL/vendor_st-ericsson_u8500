/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_RECORDHANDLER_H
#define COPS_RECORDHANDLER_H

#include <cops_data.h>
#include <cops_recordids.h>

cops_return_code_t cops_bootpart_open(void);

cops_return_code_t cops_bootpart_close(void);

cops_return_code_t cops_bootpart_lock_partition(void);

cops_return_code_t cops_bootpart_remove_record(record_id_t id);

cops_return_code_t cops_bootpart_write_record(record_id_t id,
        uint32_t length,
        uint8_t *data);

#endif                          /*COPS_RECORDHANDLER_H */
