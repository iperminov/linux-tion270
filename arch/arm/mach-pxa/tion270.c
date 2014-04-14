/*
 *  linux/arch/arm/mach-pxa/tion270.c
 *
 *  Support for ZAO "ZEO" PXA270 based Tion270/Tion-Pro270 board.
 *
 *  Copyright (C) Igor A. Perminov <igor.a.perminov@gmail.com>, 2009, 2014
 *
 *  Based on linux/arch/arm/mach-pxa/colibri.c by Daniel Mack, Toradex AG,
 *  and linux/arch/arm/mach-pxa/tion270.c (kernels 2.6.22.19, 2.6.35.14)
 *  by ZAO "ZEO".
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/platform_data/video-pxafb.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/pxa27x.h>
#include <mach/tion270.h>

#include "generic.h"

#if defined(CONFIG_TION270_EXT)

#	if defined(CONFIG_TION270_EXT_KEYBOARD)
#		include <linux/platform_data/keypad-pxa27x.h>
#	endif /* defined(CONFIG_TION270_EXT_KEYBOARD) */

#	if defined(CONFIG_TION270_EXT_MCP2515)
#		define TION270_MCP2515_CS_GPIO_NUM 108
#		define TION270_MCP2515_CS_GPIO_MFP MFP_CFG_OUT(GPIO108, AF0, DRIVE_HIGH)

#		define TION270_MCP2515_IRQ_GPIO_NUM 105
#		define TION270_MCP2515_IRQ_GPIO_MFP GPIO105_GPIO

#		include <linux/can/platform/mcp251x.h>
#		include <linux/spi/spi.h>
#		include <linux/spi/pxa2xx_spi.h>
#	endif /* defined(CONFIG_TION270_EXT_MCP2515) */

#endif /* defined(CONFIG_TION270_EXT) */

/*
 * Framebuffer (LCD/VGA).
 */
#if defined(CONFIG_TION270_FB)

static char* tion270_video_mode = NULL;
core_param(tion270_video_mode, tion270_video_mode, charp, 0444);

static bool tion270_vga_dac_enable = 1;
core_param(tion270_vga_dac_enable, tion270_vga_dac_enable, bool, 0444);

struct tion270_video_mode_def {
	const char* name;
	struct pxafb_mode_info* mode;
};

static struct tion270_video_mode_def __initdata tion270_video_modes[] = {
		[0] = {
				.name			= "320x200",
				.mode			= &tion270_pxafb_mode_info_320x200,
		},
		[1] = {
				.name			= "640x480",
				.mode			= &tion270_pxafb_mode_info_640x480,
		},
		[2] = {
				.name			= "800x600",
				.mode			= &tion270_pxafb_mode_info_800x600,
		},
		[3] = {
				.name			= "none",
				.mode			= NULL,
		},
};

/* 640x480 by default */
#define DEFAULT_VIDEO_MODE 1

static void __init tion270_video_init(void)
{
	/*
	 * Predefined video modes, to specify it in kernel parameters use one of:
	 *   tion270_video_mode=320x200,
	 *   tion270_video_mode=640x480,
	 *   tion270_video_mode=800x600 or
	 *   tion270_video_mode=none to left the framebuffer uninitialized.
	 */

	struct tion270_video_mode_def* mode_def = NULL;

	if (tion270_video_mode) {
		int i;

		for (i = 0; i < ARRAY_SIZE(tion270_video_modes); i++) {
			if (!strcmp(tion270_video_mode, tion270_video_modes[i].name)) {
				mode_def = &tion270_video_modes[i];
				break;
			}
		}

		if (!mode_def) {
			mode_def = &tion270_video_modes[DEFAULT_VIDEO_MODE];

			printk(KERN_WARNING "Unknown video mode \"%s\", \"%s\" will be used instead\n",
				tion270_video_mode, mode_def->name);
		}
	}
	else {
		mode_def = &tion270_video_modes[DEFAULT_VIDEO_MODE];
	}

	if (!mode_def->mode) {
		/* Don't initialize the framebuffer at all. */
		return;
	}

	printk(KERN_INFO "%s video mode is being initialized, VGA DAC is %s\n",
			mode_def->name, tion270_vga_dac_enable ? "on" : "off");

	tion270_pxafb_init(NULL, mode_def->mode, 1, tion270_vga_dac_enable);
}

#else
static inline void tion270_video_init(void) { }
#endif /* defined(CONFIG_TION270_FB) */

/*
 * Direct keys keyboard.
 */
#if defined(CONFIG_TION270_EXT_KEYBOARD)

static mfp_cfg_t tion270_keyboard_pin_config[] __initdata = {
	GPIO93_KP_DKIN_0	| WAKEUP_ON_LEVEL_HIGH,
	GPIO94_KP_DKIN_1	| WAKEUP_ON_LEVEL_HIGH,
	GPIO96_KP_DKIN_3	| WAKEUP_ON_LEVEL_HIGH,
};

static struct matrix_keymap_data tion270_matrix_keymap_data = {
	.keymap		= NULL,
	.keymap_size	= 0,
};

static struct pxa27x_keypad_platform_data tion270_keypad_platform_data = {
	/* .matrix_keymap_data should be not NULL regardless of both rows and cols == 0 */
	.matrix_keymap_data	= &tion270_matrix_keymap_data,
	.matrix_key_rows	= 0,
	.matrix_key_cols	= 0,
	.direct_key_num		= 4,
	.direct_key_map = {
		KEY_A, KEY_B, KEY_RESERVED, KEY_C
	},
	.debounce_interval	= 30,
};

static void __init tion270_keyboard_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_keyboard_pin_config));
	pxa_set_keypad_info(&tion270_keypad_platform_data);
}

#else
static inline void tion270_keyboard_init(void) { }
#endif /* defined(CONFIG_TION270_EXT_KEYBOARD) */

/*
 * MCP2515 CAN controller on SPI bus.
 */
#if defined(CONFIG_TION270_EXT_MCP2515)

static mfp_cfg_t tion270_mcp2515_pin_config[] __initdata = {
	TION270_MCP2515_CS_GPIO_MFP,
	TION270_MCP2515_IRQ_GPIO_MFP,
};

/* What about tion270_spi_master_info.enable_dma? */

static struct mcp251x_platform_data tion270_mcp251x_platform_data = {
	.oscillator_frequency = CONFIG_TION270_EXT_MCP2515_OSC_FREQ,
};

static struct pxa2xx_spi_chip tion270_mcp2515_controller_data = {
	.tx_threshold	= 8,
	.rx_threshold	= 8,
	.dma_burst_size	= 4,
	.timeout		= 235,	/* From original Tion270 source: "Check timeout" */
	.gpio_cs		= TION270_MCP2515_CS_GPIO_NUM,
};

static struct spi_board_info tion270_spi_board_info[] = {
	[0] = {
		.modalias = "mcp2515",
		.platform_data = &tion270_mcp251x_platform_data,
		.controller_data = &tion270_mcp2515_controller_data,
		.irq = PXA_GPIO_TO_IRQ(TION270_MCP2515_IRQ_GPIO_NUM),
		.max_speed_hz = 10000000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_3,
	},
};

#define tion270_spi_board_info_size (ARRAY_SIZE(tion270_spi_board_info))

static void __init tion270_mcp2515_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_mcp2515_pin_config));
	/* SPI device list is initialized by tion270_spi_init */
}

#else

#	define tion270_spi_board_info		NULL
#	define tion270_spi_board_info_size	0

static inline void tion270_mcp2515_init(void) { }

#endif /* defined(CONFIG_TION270_EXT_MCP2515) */

static void __init tion270_init(void) {
	tion270_ffuart_init();
	tion270_btuart_init();
	tion270_stuart_init();

	tion270_flash_init(NULL, 0); /* Default MTD partitioning */
	tion270_ether_init();
	tion270_i2c_init(NULL, NULL, 0); /* Default I2C device list */
	tion270_i2c_power_init(NULL, NULL, 0); /* Default I2C power device list */
	tion270_mcp2515_init();
	tion270_spi_init(NULL, tion270_spi_board_info, tion270_spi_board_info_size); /* Default SPI master and specified device list */
	tion270_mmc_init();
	tion270_ohci_init();
	tion270_udc_init();
	tion270_ac97_init();
	tion270_touchscreen_init();
	tion270_pwm_backlight_init(NULL);
	tion270_video_init();
	tion270_gpio_leds_init();
	tion270_pwm_init();
	tion270_keyboard_init();
}

MACHINE_START(TION270,
#if defined(CONFIG_MACH_TION270)
		"Tion270 Board"
#elif defined(CONFIG_MACH_TION_PRO270)
		"Tion-Pro270 Board"
#else
# warning "Unknown Tion270 board"
		"Unknown Tion270 Board"
#endif
		)
	.atag_offset	= 0x100,
	.init_machine	= tion270_init,
	.map_io		= pxa27x_map_io,
	.nr_irqs	= PXA_NR_IRQS,
	.init_irq	= pxa27x_init_irq,
	.handle_irq	= pxa27x_handle_irq,
	.init_time	= pxa_timer_init,
	.restart	= pxa_restart,
MACHINE_END
