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
	MUX_GPIO = 0,
	MUX_SPI_S,
	MUX_SPI_CS1,
	MUX_I2S,
	MUX_UART0,
	MUX_I2C,
	MUX_UART1,
	MUX_UART2,
	MUX_PWM0,
	MUX_PWM1,
	MUX_EPHY,
	MUX_WLED,
	__MUX_MAX,
};

static struct pinmux {
	char *name;
	char *func[4];
	unsigned int shift;
	unsigned int mask;
} mt7688_mux[] = {
	{
		.name = "i2c",
		.func = { "i2c", "gpio", NULL, NULL },
		.shift = 20,
		.mask = 0x3,
	}, {
		.name = "uart0",
		.func = { "uart", "gpio", NULL, NULL },
		.shift = 8,
		.mask = 0x3,
	}, {
		.name = "uart1",
		.func = { "uart", "gpio", NULL, NULL },
		.shift = 24,
		.mask = 0x3,
	}, {
		.name = "uart2",
		.func = { "uart", "gpio", "pwm", NULL },
		.shift = 26,
		.mask = 0x3,
	}, {
		.name = "pwm0",
		.func = { "pwm", "gpio", NULL, NULL },
		.shift = 28,
		.mask = 0x3,
	}, {
		.name = "pwm1",
		.func = { "pwm", "gpio", NULL, NULL },
		.shift = 30,
		.mask = 0x3,
	}, {
		.name = "refclk",
		.func = { "refclk", "gpio", NULL, NULL },
		.shift = 18,
		.mask = 0x1,
	}, {
		.name = "spi_s",
		.func = { "spi_s", "gpio", NULL, NULL },
		.shift = 2,
		.mask = 0x3,
	}, {
		.name = "spi_cs1",
		.func = { "spi_cs1", "gpio", NULL, "refclk" },
		.shift = 4,
		.mask = 0x3,
	}, {
		.name = "i2s",
		.func = { "i2s", "gpio", "pcm", NULL },
		.shift = 6,
		.mask = 0x3,
	}, {
		.name = "ephy",
		.func = { "ephy", "gpio", NULL, NULL },
		.shift = 34,
		.mask = 0x3,
	}, {
		.name = "wled",
		.func = { "wled", "gpio", NULL, NULL },
		.shift = 32,
		.mask = 0x3,
	}
};

static uint8_t* gpio_mmap_reg = NULL;
static int gpio_mmap_fd = 0;

static void __gpiomode_set(unsigned int mask, unsigned int shift, unsigned int val)
{
	volatile uint32_t *reg = (volatile uint32_t*) (gpio_mmap_reg + 0x60);
	unsigned int v;

	if (shift > 31) {
		shift -= 32;
		reg++;
	}

	v = *reg;
	v &= ~(mask << shift);
	v |= (val << shift);
	*(volatile uint32_t*) (reg) = v;
}

static int gpiomode_set(char *group, char *name)
{
	int i;
	int id;

	for (id = 0; id < __MUX_MAX; id ++)
		if (!strcmp(mt7688_mux[id].name, group))
			break;

	if (id < __MUX_MAX) for (i = 0; i < 4; i++) {
		if (!mt7688_mux[id].func[i] || strcmp(mt7688_mux[id].func[i], name))
			continue;
		__gpiomode_set(mt7688_mux[id].mask, mt7688_mux[id].shift, i);
		fprintf(stderr, "set pinmux %s -> %s\n", mt7688_mux[id].name, name);
		return 0;
	}
	fprintf(stderr, "unknown group/function combination\n");
	return -1;
}

static int gpiomode_get(void)
{
	unsigned int reg = *(volatile uint32_t*) (gpio_mmap_reg + 0x60);
	unsigned int reg2 = *(volatile uint32_t*) (gpio_mmap_reg + 0x64);
	int id;

	for (id = 0; id < __MUX_MAX; id ++) {
		unsigned int val;
		int i;

		if (mt7688_mux[id].shift < 32)
			val = (reg >> mt7688_mux[id].shift) & mt7688_mux[id].mask;
		else
			val = (reg2 >> (mt7688_mux[id].shift - 32)) & mt7688_mux[id].mask;

		fprintf(stderr, "Group %s - ", mt7688_mux[id].name);
		for (i = 0; i < 4; i++) {
			if (!mt7688_mux[id].func[i])
				continue;
			if (i == val)
				fprintf(stderr, "[%s] ", mt7688_mux[id].func[i]);
			else
				fprintf(stderr, "%s ", mt7688_mux[id].func[i]);
		}
		fprintf(stderr, "\n");
	}

	return 0;
}

static int gpiomode_mmap(void)
{
	if ((gpio_mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
		fprintf(stderr, "unable to open mmap file");
		return -1;
	}

	gpio_mmap_reg = (uint8_t*) mmap(NULL, 1024, PROT_READ | PROT_WRITE,
		MAP_FILE | MAP_SHARED, gpio_mmap_fd, 0x10000000);
	if (gpio_mmap_reg == MAP_FAILED) {
		perror("foo");
		fprintf(stderr, "failed to mmap");
		gpio_mmap_reg = NULL;
		close(gpio_mmap_fd);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int ret = -1;

	if (gpiomode_mmap())
		return -1;

	if (argc >= 4 && !strcmp(argv[1], "set")) {
		ret = gpiomode_set(argv[2], argv[3]);
	} else if (argc >= 2 && !strcmp(argv[1], "get")) {
		ret = gpiomode_get();
	} else {
		fprintf(stderr, "%s set <group> <function>\n", *argv);
		fprintf(stderr, "%s get\n", *argv);
	}

	close(gpio_mmap_fd);

	return ret;
}
