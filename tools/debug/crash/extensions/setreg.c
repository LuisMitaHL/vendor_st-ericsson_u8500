/* setreg - set registers for crashing task(s) */

#include "defs.h"
#include <string.h>

#define STRSTR(s1, s2) ((s1) ? strstr((s1), (s2)) : NULL)

#define ISSW_SAVED_REGS_START 0x17fe0000
#define ISSW_SAVED_REGS_SIZE  0x00020000
#define REGSTR_BUF_SIZE 256

int _init(void);
int _fini(void);

void cmd_setreg(void);
char *help_setreg[];

static struct command_table_entry command_table[] = {
	{ "setreg", cmd_setreg, help_setreg, 0 },
	{ NULL }
};


char *help_setreg[] = {
	"setreg",                        /* command name */
	"sets and prints crash registers",   /* short description */
	"[-c <cpu>] [-z|-w [-a <addr>]|-l|<reg> <val>]",                     /* argument synopsis, or " " if none */
	"  This command prints and sets crash registers",
	"  -c <cpu>     not needed if there's only one cpu or if getting the register values from the log",
	"  -z           zeros all registers for the specified cpu",
	"  -w           for a watchdog dump, get all registers for the specified cpu from where the ISSW stored them,",
	"               use -a <hexval> to set the physical address of the location to look in, defaults to 0x17fe0000",
	"  -l           get all registers from the kernel log for the crashing cpu",
	"  <reg> <val>  set register <reg> to <val>"
	"\nEXAMPLE",
	"  Print all crash registers for CPU 0:\n",
	"    crash> setreg -c 0",
	"crash> setreg -c0",
	"pc : [<0xc0439cdc>]    lr : [<0xc07c4cbc>]    cpsr : 0x60000093",
	"sp : 0xde243ec0  ip : 0xde243e90  fp : 0xde243ecc",
	"r10: 0x00000007  r9 : 0x00000000  r8 : 0x60000013",
	"r7 : 0xc0aacfd0  r6 : 0xc0aad13c  r5 : 0x00000063  r4 : 0xc0a997d0",
	"r3 : 0x00000000  r2 : 0x00000001  r1 : 0x0025166c  r0 : 0xc0a8a6a4",
	"orig_r0: 0x00000000",
	NULL
};

struct issw_saved_regs {
	unsigned int usr_r0;
	unsigned int usr_r1;
	unsigned int usr_r2;
	unsigned int usr_r3;
	unsigned int usr_r4;
	unsigned int usr_r5;
	unsigned int usr_r6;
	unsigned int usr_r7;
	unsigned int usr_r8;
	unsigned int usr_r9;
	unsigned int usr_r10;
	unsigned int usr_r11;
	unsigned int usr_r12;
	unsigned int usr_r13;
	unsigned int usr_r14;

	unsigned int svc_r13;
	unsigned int svc_r14;
	unsigned int svc_spsr;

	unsigned int abt_r13;
	unsigned int abt_r14;
	unsigned int abt_spsr;

	unsigned int und_r13;
	unsigned int und_r14;
	unsigned int und_spsr;

	unsigned int irq_r13;
	unsigned int irq_r14;
	unsigned int irq_spsr;

	unsigned int mon_spsr;
	unsigned int mon_cpsr;
	unsigned int mon_r14;
};
typedef struct issw_saved_regs issw_saved_regs_t;

struct reginfo {
	char *name;
	unsigned int no;
} regmap[] = {
	{"r0", 0},
	{"r1", 1},
	{"r2", 2},
	{"r3", 3},
	{"r4", 4},
	{"r5", 5},
	{"r6", 6},
	{"r7", 7},
	{"r8", 8},
	{"r9", 9},
	{"r10", 10},
	{"r11", 11},
	{"fp", 11},
	{"r12", 12},
	{"ip", 12},
	{"r13", 13},
	{"sp", 13},
	{"r14", 14},
	{"lr", 14},
	{"r15", 15},
	{"pc", 15},
	{"cpsr", 16},
	{"orig_r0", 17},
	{NULL, 0}
};


int reg_name_to_no(const char *name) {
	int i = 0;
	while (regmap[i].name != NULL) {
		if (strcmp(name, regmap[i].name) == 0) {
			return regmap[i].no;
		}
		i++;
	}
	return -1;
}


void print_all_regs(int cpu) {
	struct arm_pt_regs *panic_task_regs = machdep->machspec->crash_task_regs;
	if (panic_task_regs != NULL) {
		printf("pc : [<0x%08lx>]    lr : [<0x%08lx>]    cpsr : 0x%08lx\n",
		       panic_task_regs[cpu].ARM_pc,
		       panic_task_regs[cpu].ARM_lr,
		       panic_task_regs[cpu].ARM_cpsr);
		printf("sp : 0x%08lx  ip : 0x%08lx  fp : 0x%08lx\n",
		       panic_task_regs[cpu].ARM_sp,
		       panic_task_regs[cpu].ARM_ip,
		       panic_task_regs[cpu].ARM_fp);
		printf("r10: 0x%08lx  r9 : 0x%08lx  r8 : 0x%08lx\n",
		       panic_task_regs[cpu].ARM_r10,
		       panic_task_regs[cpu].ARM_r9,
		       panic_task_regs[cpu].ARM_r8);
		printf("r7 : 0x%08lx  r6 : 0x%08lx  r5 : 0x%08lx  r4 : 0x%08lx\n",
		       panic_task_regs[cpu].ARM_r7,
		       panic_task_regs[cpu].ARM_r6,
		       panic_task_regs[cpu].ARM_r5,
		       panic_task_regs[cpu].ARM_r4);
		printf("r3 : 0x%08lx  r2 : 0x%08lx  r1 : 0x%08lx  r0 : 0x%08lx\n",
		       panic_task_regs[cpu].ARM_r3,
		       panic_task_regs[cpu].ARM_r2,
		       panic_task_regs[cpu].ARM_r1,
		       panic_task_regs[cpu].ARM_r0);
		printf("orig_r0: 0x%08lx\n", panic_task_regs[cpu].ARM_ORIG_r0);

	}
	else {
		printf("no regs\n");
	}
}


void setreg(int cpu, int regno, unsigned long regval) {
	struct arm_pt_regs *panic_task_regs = machdep->machspec->crash_task_regs;
	if (machdep->machspec->crash_task_regs == NULL) {
		if (!(panic_task_regs = calloc(kt->cpus, sizeof(*panic_task_regs)))) {
			error(INFO, "cannot malloc panic_task_regs space\n");
			return;
		}
		machdep->machspec->crash_task_regs = panic_task_regs;

	}
	panic_task_regs[cpu].uregs[regno] = regval;
}

void zero_regs(int cpu) {
	setreg(cpu, 0, 0);
	setreg(cpu, 1, 0);
	setreg(cpu, 2, 0);
	setreg(cpu, 3, 0);
	setreg(cpu, 4, 0);
	setreg(cpu, 5, 0);
	setreg(cpu, 6, 0);
	setreg(cpu, 7, 0);
	setreg(cpu, 8, 0);
	setreg(cpu, 9, 0);
	setreg(cpu, 10, 0);
	setreg(cpu, 11, 0);
	setreg(cpu, 12, 0);
	setreg(cpu, 13, 0);
	setreg(cpu, 14, 0);
	setreg(cpu, 15, 0);
	setreg(cpu, 16, 0);
	setreg(cpu, 17, 0);
}

char *get_prev_log(char *buf, unsigned long int pos) {
	static unsigned long int curr_pos = 0;

	if (pos) {
		curr_pos = pos;
		buf[curr_pos] = '\0';
	}

	if (!curr_pos) {
		return NULL;
	}

	while (buf[curr_pos-1]!='\n') {
		if (!--curr_pos) {
			return NULL;
		}
	}
	buf[curr_pos-1] = '\0';
	return &buf[curr_pos];

}

char *get_prefixed_hex_regval(char *str, const char *prefix, unsigned long int *val) {
	if (!str) {return NULL;}
	if (!(str = strstr(str, prefix))) {return NULL;}
	str += strlen(prefix);
	*val = strtoul(str, NULL, 16);
	return str;
}

void get_regs_from_log() {
	int log_len;
	char *buf;
	unsigned long int log_addr;
	unsigned long int index_start;
	char *str;
	unsigned int log_end;
	unsigned long int r3_val, r2_val, r1_val, r0_val;
	unsigned long int r7_val, r6_val, r5_val, r4_val;
	unsigned long int r10_val, r9_val, r8_val;
	unsigned long int sp_val, ip_val, fp_val;
	unsigned long int pc_val, lr_val, cpsr_val;
	long int cpu;

	get_symbol_data("log_buf_len", sizeof(int), &log_len);
	if (!(buf = malloc(log_len+1))) {
		error(INFO, "Cannot allocate buffer for log contents");
		return;
	}
	get_symbol_data("log_buf", sizeof(log_addr), &log_addr);
	get_symbol_data("log_end", sizeof(log_end), &log_end);

	if (log_end < log_len) {
		if (!readmem(log_addr, KVADDR, buf,
			     log_end, "log_buf contents", RETURN_ON_ERROR)) {
			error(INFO, "Cannot read log contents");
			free(buf);
			return;
		}
	}
	else {
		index_start = log_end & (log_len - 1);
		if(!readmem(log_addr + index_start,KVADDR, buf,
			    log_len - index_start, "log_buf contents", RETURN_ON_ERROR)) {
			error(INFO, "Cannot read log contents");
			free(buf);
			return;
		}
		if (index_start) {
			if (!readmem(log_addr, KVADDR, buf + log_len - index_start,
				     index_start, "log_buf contents", RETURN_ON_ERROR)) {
				error(INFO, "Cannot read log contents");
				free(buf);
				return;
			}
		}
	}

	/*
	  We go through the log from the end.  In case there are several matches
	  for the register print-outs, the last ones should be used.
	 */
	str = get_prev_log(buf, MIN(log_end,log_len));
	while (str) {
		if (STRSTR(str, "Flags:")) {
			str = get_prev_log(buf, 0);
			if (!(str = get_prefixed_hex_regval(str, "r3 : ", &r3_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r2 : ", &r2_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r1 : ", &r1_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r0 : ", &r0_val))) {continue;}

			str = get_prev_log(buf, 0);
			if (!(str = get_prefixed_hex_regval(str, "r7 : ", &r7_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r6 : ", &r6_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r5 : ", &r5_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r4 : ", &r4_val))) {continue;}

			str = get_prev_log(buf, 0);
			if (!(str = get_prefixed_hex_regval(str, "r10: ", &r10_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r9 : ", &r9_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "r8 : ", &r8_val))) {continue;}

			str = get_prev_log(buf, 0);
			if (!(str = get_prefixed_hex_regval(str, "sp : ", &sp_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "ip : ", &ip_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "fp : ", &fp_val))) {continue;}

			str = get_prev_log(buf, 0);
			if (!(str = get_prefixed_hex_regval(str, "pc : [<", &pc_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "lr : [<", &lr_val))) {continue;}
			if (!(str = get_prefixed_hex_regval(str, "psr: ", &cpsr_val))) {continue;}

			str = get_prev_log(buf, 0);
			if (!(str = STRSTR(str, "LR is at "))) {continue;}
			str = get_prev_log(buf, 0);
			if (!(str = STRSTR(str, "PC is at "))) {continue;}
			str = get_prev_log(buf, 0);

			if (!(str = get_prefixed_hex_regval(str, "CPU:", (unsigned long *)&cpu))) {continue;}
			if (cpu >= kt->cpus) {
				error(INFO, "no valid cpu number found in log\n");
				continue;
			}

			setreg(cpu, 3, r3_val); setreg(cpu, 2, r2_val); setreg(cpu, 1, r1_val); setreg(cpu, 0, r0_val);
			setreg(cpu, 7, r7_val); setreg(cpu, 6, r6_val); setreg(cpu, 5, r5_val); setreg(cpu, 4, r4_val);
			setreg(cpu, 10, r10_val); setreg(cpu, 9, r9_val); setreg(cpu, 8, r8_val);
			setreg(cpu, 13, sp_val); setreg(cpu, 12, ip_val); setreg(cpu, 11, fp_val);
			setreg(cpu, 16, cpsr_val); setreg(cpu, 15, pc_val); setreg(cpu, 14, lr_val);

			/* Done with log */
			free(buf);
			return;
		}
		str = get_prev_log(buf, 0);
	}

	error(INFO, "no registers found in log\n");
	free(buf);
}

void set_all_regs_from_prebark(int cpu, issw_saved_regs_t *issw_saved_regs) {
	struct arm_pt_regs *panic_task_regs = machdep->machspec->crash_task_regs;

	if (machdep->machspec->crash_task_regs == NULL) {
		if (!(panic_task_regs = calloc(kt->cpus, sizeof(*panic_task_regs)))) {
			error(INFO, "cannot malloc panic_task_regs space\n");
			return;
		}
		machdep->machspec->crash_task_regs = panic_task_regs;
	}

	panic_task_regs[cpu].ARM_r0 = issw_saved_regs->usr_r0;
	panic_task_regs[cpu].ARM_r1 = issw_saved_regs->usr_r1;
	panic_task_regs[cpu].ARM_r2 = issw_saved_regs->usr_r2;
	panic_task_regs[cpu].ARM_r3 = issw_saved_regs->usr_r3;
	panic_task_regs[cpu].ARM_r4 = issw_saved_regs->usr_r4;
	panic_task_regs[cpu].ARM_r5 = issw_saved_regs->usr_r5;
	panic_task_regs[cpu].ARM_r6 = issw_saved_regs->usr_r6;
	panic_task_regs[cpu].ARM_r7 = issw_saved_regs->usr_r7;
	panic_task_regs[cpu].ARM_r8 = issw_saved_regs->usr_r8;
	panic_task_regs[cpu].ARM_r9 = issw_saved_regs->usr_r9;
	panic_task_regs[cpu].ARM_r10 = issw_saved_regs->usr_r10;
	panic_task_regs[cpu].ARM_fp = issw_saved_regs->usr_r11;
	panic_task_regs[cpu].ARM_ip = issw_saved_regs->usr_r12;
	panic_task_regs[cpu].ARM_pc = issw_saved_regs->mon_r14 - 0x4;
	panic_task_regs[cpu].ARM_cpsr = issw_saved_regs->mon_spsr;

	if (IS_KVADDR(panic_task_regs[cpu].ARM_pc)) {
		panic_task_regs[cpu].ARM_sp = issw_saved_regs->svc_r13;
		panic_task_regs[cpu].ARM_lr = issw_saved_regs->svc_r14;
	}
	else {
		panic_task_regs[cpu].ARM_sp = issw_saved_regs->usr_r13;
		panic_task_regs[cpu].ARM_lr = issw_saved_regs->usr_r14;
	}

}

void get_regs_from_prebark(int cpu, ulonglong addr) {
	char *buf;
	char *search_ptr;
	char reg_str[REGSTR_BUF_SIZE];
	size_t reg_str_len;
	issw_saved_regs_t *issw_saved_regs = NULL;

	snprintf(reg_str, REGSTR_BUF_SIZE, "CPU%d registers  ", cpu);
	reg_str_len = strlen(reg_str);

	if (!(search_ptr = buf = malloc(ISSW_SAVED_REGS_SIZE))) {
		error(INFO, "issw saved regs malloc\n");
		return;
	}

	if (!readmem(addr, PHYSADDR, buf,
		     ISSW_SAVED_REGS_SIZE, "issw saved regs", RETURN_ON_ERROR)) {
		error(INFO, "issw saved regs readmem\n");
		goto out;
	}
	while (search_ptr < (buf+ISSW_SAVED_REGS_SIZE)) {
		search_ptr = memchr(search_ptr, reg_str[0], buf+ISSW_SAVED_REGS_SIZE-search_ptr);
		if (search_ptr == NULL) {
			break;
		}
		if (search_ptr + reg_str_len+sizeof(issw_saved_regs_t) > buf + ISSW_SAVED_REGS_SIZE) {
			search_ptr = NULL;
			break;
		}
		if (strncmp(search_ptr, reg_str, reg_str_len) == 0) {
			issw_saved_regs = (issw_saved_regs_t*)(search_ptr + reg_str_len);
			break;
		}
		search_ptr++;
	}
	if (issw_saved_regs != NULL) {
		set_all_regs_from_prebark(cpu, issw_saved_regs);
	}
	else {
		fprintf(fp, "ISSW saved registers not found\n");
	}

out:
	free(buf);
	return;
}

void cmd_setreg(void) {
	int c;
	int regno;
	unsigned long regval;
	int cpu = -1;
	int wd_regs = 0;
	int log_regs = 0;
	int z_regs = 0;
	int nbr_reg_srcs = 0;
	char *conv_ptr;
	ulonglong addr = ISSW_SAVED_REGS_START;
	int modified_prebark_addr = 0;

	while ((c = getopt(argcnt, args, "zwlc:a:")) != EOF) {
		switch(c)
		{
		case 'z':
			z_regs = 1;
			nbr_reg_srcs++;
			break;
		case 'w':
			wd_regs = 1;
			nbr_reg_srcs++;
			break;
		case 'l':
			log_regs = 1;
			nbr_reg_srcs++;
			break;
		case 'c':
			cpu = dtoi(optarg, FAULT_ON_ERROR, NULL);
			break;
		case 'a':
			modified_prebark_addr = 1;
			addr = htoll(optarg, FAULT_ON_ERROR, NULL);
			break;
		default:
			argerrs++;
			break;
		}
	}

	if (argerrs) {
		cmd_usage(pc->curcmd, SYNOPSIS);
	}

	// This is the <reg> <val> case
	if (argcnt - optind == 2) {
		nbr_reg_srcs++;
	}

	if (nbr_reg_srcs > 1) {
		cmd_usage(pc->curcmd, SYNOPSIS);
	}

	if (cpu != -1 && log_regs) {
		/* CPU figured out from log. Should not be specified on commandline */
		cmd_usage(pc->curcmd, SYNOPSIS);
	}

	if (cpu == -1 && !log_regs) {
		/* No CPU chosen. This is the OK if getting registers from the log or if there's
		 * only one CPU to choose from. */
		if (kt->cpus == 1) {
			cpu = 0;
		}
		else {
			cmd_usage(pc->curcmd, SYNOPSIS);
		}
	}

	if (modified_prebark_addr) {
		if (!wd_regs) {
			cmd_usage(pc->curcmd, SYNOPSIS);
		}
	}

	if (log_regs) {
		get_regs_from_log();
		return;
	}

	if (cpu >= kt->cpus) {
		cmd_usage(pc->curcmd, SYNOPSIS);
	}

	if (z_regs) {
		zero_regs(cpu);
		return;
	}

	if (wd_regs) {
		get_regs_from_prebark(cpu, addr);
		return;
	}

	if (argcnt == optind) {
		print_all_regs(cpu);
		return;
	}

	if (argcnt - optind != 2) {
		cmd_usage(pc->curcmd, SYNOPSIS);
	}

	regno = reg_name_to_no(args[optind]);
	if (regno == -1) {
		error(INFO, "not a valid register name\n");
		return;
	}
	regval = strtoul(args[optind+1], &conv_ptr, 0);
	if (args[optind+1] != conv_ptr) {
		setreg(cpu, regno, regval);
		return;
	}
	cmd_usage(pc->curcmd, SYNOPSIS);
}

int _init(void)
{
	register_extension(command_table);
	return 1;
}

int _fini(void)
{
	return 1;
}
