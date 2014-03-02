#ifndef INCLUSION_GUARD_STE_ADM_INTERNAL_H
#define INCLUSION_GUARD_STE_ADM_INTERNAL_H

// ADM semaphore. This semaphore is a bit inconvenient (requires time
// in absolute value), so we might want to change it in the future.
#include <semaphore.h>
#include "ste_adm_client.h"
#include <linux_utils.h>
typedef sem_t adm_sem_t;

int adm_sem_init(adm_sem_t* sem, unsigned int init_val);

// returns 0 if take, <0 if timeout, _signal_ or error.
// timeout is in seconds, might wait longer
// returns 0 if semaphore ok, ETIMEDOUT if the call timed out.
int adm_sem_wait(adm_sem_t* sem, unsigned short timeout);
void  adm_sem_post(adm_sem_t* sem);
void adm_sem_destroy(adm_sem_t* sem);
void adm_sem_wait_and_printf(adm_sem_t* sem, const char* file, int line, const char* arg, ...);
int adm_sem_get_count(adm_sem_t* sem);


// Return a file descriptor to a shared memory segment of size bytes.
// When all file descriptors and mappings to this area are destroyed,
// the shared memory is removed as well. In case shm_fd_to_reuse != -1, it will
// use that file descriptor to create the shared memory segment.
// Returns -1 if the shared memory region could not be created.
int util_shm_create(int shm_fd_to_reuse, int bytes);

// Transmit the specified file descriptor over the socket
ste_adm_res_t util_fd_send_to_socket(int socket, int fd);

ste_adm_modem_type_t adm_get_modem_type();

#endif // INCLUSION_GUARD_STE_ADM_INTERNAL_H

