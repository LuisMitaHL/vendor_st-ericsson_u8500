#include <stdint.h>
#include <unistd.h>
#include "ab_listener.h"

pid_t abxxxx_listener_id(void);
int abxxxx_interrupt_registering(int interrupt_id, int signal_id);
int abxxxx_read(uint16_t addr, uint8_t *val);
int abxxxx_write(uint16_t addr, uint8_t val);
ab_listener *abxxxx_shm_install(int *shmid);

#define AddrtoBank(x) ((x & 0xFF00) >> 8)
#define AddrtoAddr(x) (x  & 0x00FF)

#define AB_DEBUGFS_PATH_ADDRESS "/sys/kernel/debug/ab8500/register-address"
#define AB_DEBUGFS_PATH_BANK	"/sys/kernel/debug/ab8500/register-bank"
#define AB_DEBUGFS_PATH_VALUE	"/sys/kernel/debug/ab8500/register-value"
