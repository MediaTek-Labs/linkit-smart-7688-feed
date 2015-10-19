#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MMAP_PATH			"/dev/mem"

enum {
	CLK_XTAL = 0,
	CLK_12,
	CLK_25,
	CLK_40,
	CLK_48,
	__CLK_MAX,
};

static unsigned int refclk_rates[] = { 40, 12, 25, 40, 48 };
static char *refclk_names[] = { "xtal", "12", "25", "40", "48" };
static uint8_t* refclk_mmap_reg = NULL;
static int refclk_mmap_fd = 0;

static void __refclk_set(unsigned int val)
{
	unsigned int reg = *(volatile uint32_t*) (refclk_mmap_reg + 0x2c);

	reg &= ~(0x7 << 9);
	reg |= (val << 9);
	*(volatile uint32_t*) (refclk_mmap_reg + 0x2c) = reg;
}

static int refclk_set(unsigned int rate)
{
	int id;

	for (id = 1; id < __CLK_MAX; id ++) {
		if (refclk_rates[id] != rate)
			continue;
		__refclk_set(id);
		fprintf(stderr, "set reclk to %uMHz\n", rate);
		return 0;
	}
	__refclk_set(CLK_XTAL);
	fprintf(stderr, "failed to set reclk to %uMHz using xtal instead\n", rate);
	return -1;
}

static int refclk_get(void)
{
	unsigned int reg = *(volatile uint32_t*) (refclk_mmap_reg + 0x2c);
	int id;

	reg >>= 9;
	reg &= 7;

	fprintf(stderr, "refclk rates in MHz: ");
        for (id = 0; id < __CLK_MAX; id ++) {
		if (id == reg)
			fprintf(stderr, "[%s] ", refclk_names[id]);
		else
			fprintf(stderr, "%s ", refclk_names[id]);
	}
	fprintf(stderr, "\n");
	return 0;
}

static int refclk_mmap(void)
{
	if ((refclk_mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
		fprintf(stderr, "unable to open mmap file");
		return -1;
	}

	refclk_mmap_reg = (uint8_t*) mmap(NULL, 1024, PROT_READ | PROT_WRITE,
		MAP_FILE | MAP_SHARED, refclk_mmap_fd, 0x10000000);
	if (refclk_mmap_reg == MAP_FAILED) {
		perror("foo");
		fprintf(stderr, "failed to mmap");
		refclk_mmap_reg = NULL;
		close(refclk_mmap_fd);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int ret = -1;

	if (refclk_mmap())
		return -1;

	if (argc >= 3 && !strcmp(argv[1], "set")) {
		ret = refclk_set(atoi(argv[2]));
	} else if (argc >= 2 && !strcmp(argv[1], "get")) {
		ret = refclk_get();
	} else {
		fprintf(stderr, "%s set <rate>\n", *argv);
		fprintf(stderr, "%s get\n", *argv);
	}

	close(refclk_mmap_fd);

	return ret;
}
