#include <linux/elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef DEBUG
#define debug_pr(str, ...) printf(str, ## __VA_ARGS__);
#else
#define debug_pr(str, ...)
#endif

#define IOMEM_FILE_NAME "/proc/iomem"
#define MAX_LINE_SIZE 128


static unsigned int iomem_total() {
	static FILE *iomemf;
	unsigned int total = 0;
	char buf[MAX_LINE_SIZE];
	unsigned int ent_start, ent_end;


	iomemf = fopen(IOMEM_FILE_NAME, "r");
	if (iomemf == NULL) {
		perror("Failed to open iomem file");
		exit(1);
	}

	while (1) {
		if (fgets(buf, MAX_LINE_SIZE, iomemf) != buf) {
			break;
		}
		if (strstr(buf, "System RAM") && sscanf(buf , "%08X-%08X : System RAM", &ent_start, &ent_end) == 2) {
			debug_pr("Matching System RAM line: %s\n", buf);
			total += ent_end - ent_start + 1;
		}
		else if (strstr(buf, "Crash kernel") && sscanf(buf , "%08X-%08X : Crash kernel", &ent_start, &ent_end) == 2) {
			debug_pr("Matching Crash kernel line: %s\n", buf);
			total -= ent_end - ent_start + 1;
		}
	}
	return total;
}

static unsigned int pt_load_total(const char *dump_name) {
	static FILE *df;

	struct elf32_hdr ehdr;
	struct elf32_phdr phdr;
	int i;
	unsigned int total_pt_load_size = 0;

	df = fopen(dump_name, "r");
	if (df == NULL) {
		perror("Failed to open core dump file");
		exit(1);
	}

	if (fread(&ehdr, sizeof(ehdr), 1, df) != 1) {
		perror("Failed to read ELF header");
		exit(1);
	}
	if (strncmp((char*)ehdr.e_ident, "\x7f" "ELF", 4)) {
		printf("Wrong ELF magic\n");
		exit(1);
	}
	debug_pr("ehdr.e_phentsize: %hu\n", ehdr.e_phentsize);

	if (fseek(df, ehdr.e_phoff, SEEK_SET) != 0) {
		perror("Failed to seek to program headers");
		exit(1);
	}

	for (i=0; i<ehdr.e_phnum; i++) {
		if (fread(&phdr, sizeof(phdr), 1, df) != 1) {
			perror("Failed to read program header");
			exit(1);
		}
		if (phdr.p_type == PT_LOAD) {
			total_pt_load_size += phdr.p_memsz;
		}
	}

	return total_pt_load_size;
}

int main(int argc, char *argv[]) {
	unsigned int elf_total;
	unsigned int proc_total;
	struct stat sb;

	if (argc!=2) {
		printf("Usage: %s <dump file>\n", argv[0]);
		exit(1);
	}

	elf_total = pt_load_total(argv[1]);
	debug_pr("elf_total: 0x%08x\n", elf_total);

	proc_total = iomem_total();
	debug_pr("proc_total: 0x%08x\n", proc_total);

	if (elf_total != proc_total) {
		debug_pr("Dump seems corrupt.\n");
		return 1;
	}

	/*
	 * The complete dump file, including headers and note segments, should
	 * be strictly larger than the dumped area or there is something wrong.
	 */
	stat(argv[1], &sb);
	if (sb.st_size <= elf_total) {
		debug_pr("Dump seems corrupt.\n");
		return 1;
	}

	debug_pr("Dump looks ok.\n");

	return 0;

}
