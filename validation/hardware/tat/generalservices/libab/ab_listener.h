#define AB_DEBUGFS_PATH_SUBSCRIBE "/sys/kernel/debug/ab8500/irq-subscribe"
#define AB_DEBUGFS_PATH_UNSUBSCRIBE "/sys/kernel/debug/ab8500/irq-unsubscribe"
#define AB_DEBUGFS_PATH_INTERRUPT "/sys/devices/platform/ab8500-i2c.0/ab8500-debug.0/irq-"
#define AB_DEBUGFS_BASE_INTERRUPT 450
#define AB_EVENT_BUF_LEN 64

typedef struct {
	int interrupt;
	int signal;
} ab_listener;

struct pid_to_fd {
	int fd;
	pid_t pid;
	int sig;
	int it;
};

