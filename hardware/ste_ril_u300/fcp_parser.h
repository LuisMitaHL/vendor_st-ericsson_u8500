#ifndef FCP_PARSER_H
#define FCP_PARSER_H

#include <stdint.h>
#include <endian.h>

struct ts_51011_921_resp {
    uint8_t   rfu_1[2];
    uint16_t  file_size; /* be16 */
    uint16_t  file_id;   /* be16 */
    uint8_t   file_type;
    uint8_t   rfu_2;
    uint8_t   file_acc[3];
    uint8_t   file_status;
    uint8_t   data_size;
    uint8_t   file_structure;
    uint8_t   record_size;
} __attribute__((packed));

int fcp_to_ts_51011(const char *stream,
                    size_t len,
                    struct ts_51011_921_resp *out);

#endif
