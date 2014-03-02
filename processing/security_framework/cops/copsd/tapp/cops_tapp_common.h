/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_TAPP_COMMON_H
#define COPS_TAPP_COMMON_H

#include <cops_shared.h>
#include <cops_state.h>

cops_return_code_t cops_tapp_setup(cops_tapp_io_t **tapp_io);

void cops_tapp_destroy(cops_tapp_io_t **tapp_io);

cops_return_code_t cops_tapp_init_io(struct cops_state *state,
                                     cops_tapp_io_t *tapp_io);

cops_return_code_t cops_tapp_invoke(struct cops_state *state,
                                    cops_tapp_io_t **tapp_io,
                                    cops_taf_id_t taf_id);

cops_return_code_t cops_tapp_save_cops_data(struct cops_state *state,
                                            cops_tapp_io_t *tapp_io,
                                            enum cops_data_type data_type);

#endif /* COPS_TAPP_COMMON_H */
