/*
 *  linux/arch/arm/mach-pxa/tion270lib.c
 *
 *  Support library for ZAO "ZEO" PXA270 based Tion270/Tion-Pro270 board.
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
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/etherdevice.h>
#include <linux/dm9000.h>
#include <linux/i2c.h>
#include <linux/i2c/pxa-i2c.h>
#include <linux/leds.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/platform_device.h>
#include <linux/platform_data/mmc-pxamci.h>
#include <linux/platform_data/usb-ohci-pxa27x.h>
#include <linux/platform_data/pxa2xx_udc.h>
#include <linux/platform_data/video-pxafb.h>
#include <linux/pwm.h>
#include <linux/spi/spi.h>
#include <linux/spi/pxa2xx_spi.h>
#include <linux/ucb1400.h>
#include <linux/pwm_backlight.h>

#include <mach/audio.h>
#include <mach/pxa27x.h>
#include <mach/pxa27x-udc.h>
#include <mach/udc.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/sizes.h>
#include <asm/system_info.h>

#include <mach/tion270.h>

#include "generic.h"
#include "devices.h"

/*
 * FFUART.
 */
#if defined(CONFIG_TION270_FFUART)

static mfp_cfg_t tion270_ffuart_pin_config[] __initdata = {
	GPIO10_FFUART_DCD,
	GPIO27_FFUART_RTS,
	GPIO33_FFUART_DSR,
	GPIO34_FFUART_RXD,
	GPIO38_FFUART_RI,
	GPIO39_FFUART_TXD,
	GPIO40_FFUART_DTR,
	GPIO100_FFUART_CTS,
};

void __init tion270_ffuart_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_ffuart_pin_config));
	pxa_set_ffuart_info(NULL);
}

#endif /* defined(CONFIG_TION270_FFUART) */

/*
 * BTUART.
 */
#if defined(CONFIG_TION270_BTUART)

static mfp_cfg_t tion270_btuart_pin_config[] __initdata = {
	GPIO42_BTUART_RXD,
	GPIO43_BTUART_TXD,
#	if !defined(CONFIG_MACH_TION_PRO270)
	GPIO44_BTUART_CTS,
#	endif /* !defined(CONFIG_MACH_TION_PRO270) */
	GPIO45_BTUART_RTS,
};

void __init tion270_btuart_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_btuart_pin_config));
	pxa_set_btuart_info(NULL);
}

#endif /* defined(CONFIG_TION270_BTUART) */

/*
 * STUART.
 */
#if defined(CONFIG_TION270_STUART)

static mfp_cfg_t tion270_stuart_pin_config[] __initdata = {
	GPIO46_STUART_RXD,
	GPIO47_STUART_TXD,
};

void __init tion270_stuart_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_stuart_pin_config));
	pxa_set_stuart_info(NULL);
}

#endif /* defined(CONFIG_TION270_STUART) */

/*
 * NOR flash.
 */
#if defined(CONFIG_TION270_FLASH)

static struct mtd_partition tion270_flash_partitions[] = {
	[0] = {
		.name		= "Bootloader",
		.offset		= 0x00000000,
		.size		= 0x00040000,
		.mask_flags	= MTD_WRITEABLE  /* Force read-only to prevent bootloader corruption. */
	},
	[1] = {
		.name		= "Environment",
		.offset		= 0x00040000,
		.size		= 0x00040000,
		.mask_flags	= 0
	},
	[2] = {
		.name		= "Kernel",
		.offset		= 0x00080000,
		.size		= 0x00400000,
		.mask_flags	= 0
	},
	[3] = {
		.name		= "RootFS",
		.offset		= 0x00480000,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= 0
	},
};

static struct physmap_flash_data tion270_flash_physmap[] = {
	[0] = {
		.width		= 4,			/* Bank width in bytes. */
		.parts		= tion270_flash_partitions,
		.nr_parts	= ARRAY_SIZE(tion270_flash_partitions)
	},
};

static struct resource tion270_flash_resources[] = {
	[0] = {
		.start		= TION270_FLASH_PHYS,
		.end		= TION270_FLASH_PHYS + TION270_FLASH_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
};

static struct platform_device tion270_flash_device = {
	.name			= "physmap-flash",
	.id				= 0,
	.dev 			= {
		.platform_data	= tion270_flash_physmap,
	},
	.resource		= tion270_flash_resources,
	.num_resources	= ARRAY_SIZE(tion270_flash_resources),
};

void __init tion270_flash_init(struct mtd_partition* partitions, size_t nr_partitions)
{
	if (partitions) {
		tion270_flash_physmap[0].parts = partitions;
		tion270_flash_physmap[0].nr_parts = nr_partitions;
	}

	platform_device_register(&tion270_flash_device);
}

#endif /* defined(CONFIG_TION270_FLASH) */

/*
 * Ethernet (Davicom DM9000).
 */
#if defined(CONFIG_TION270_DM9000)

static mfp_cfg_t tion270_dm9000_pin_config[] __initdata = {
	GPIO78_nCS_2,	/* Ethernet CS */
	TION270_DM9000_IRQ_GPIO_MFP,	/* Ethernet IRQ on GPIO */
};

static struct resource tion270_dm9000_resources[] = {
	[0] = {
		.start		= TION270_DM9000_PHYS,
		.end		= TION270_DM9000_PHYS + 3,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start		= TION270_DM9000_PHYS + 4,
		.end		= TION270_DM9000_PHYS + 4 + 500,
		.flags		= IORESOURCE_MEM,
	},
	[2] = {
		.start		= PXA_GPIO_TO_IRQ(TION270_DM9000_IRQ_GPIO_NUM),
		.end		= PXA_GPIO_TO_IRQ(TION270_DM9000_IRQ_GPIO_NUM),
		.flags		= IORESOURCE_IRQ | IRQF_TRIGGER_RISING,
	},
};

static struct dm9000_plat_data tion270_dm9000_platform_data = {
	.flags		= DM9000_PLATF_NO_EEPROM,
	/* .dev_addr is filled in below */
};

static struct platform_device tion270_dm9000_device = {
	.name			= "dm9000",
	.id				= -1,
	.dev			= {
		.platform_data	= &tion270_dm9000_platform_data,
	},
	.resource		= tion270_dm9000_resources,
	.num_resources	= ARRAY_SIZE(tion270_dm9000_resources),
};

static inline int parse_hex_digit(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 0x0a;
	}
	else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 0x0a;
	}
	else {
		return -1;
	}
}

static int __init parse_ether_addr(const char* s, u8 ether_addr[/* 6 */]) {
	int i;

	for (i = 0; i < 6 && s[0] && s[1]; i++) {
		u8 b;

		int d = parse_hex_digit(s[0]);

		if (d == -1) {
			return -1;
		}

		b = d << 4;

		d = parse_hex_digit(s[1]);

		if (d == -1) {
			return -1;
		}

		b |= d;

		*ether_addr++ = b;

		s += 2;

		if (!(*s)) {
			/* Increment i before ending loop */
			continue;
		}

		if (*s == ':') {
			if (*(s + 1)) {
				s++;
			}
			else {
				/* Trailing ':' is not allowed */
				return -1;
			}
		}
	}

	return i == 6 ? 0 : -1;
}

static char* tion270_ether_addr = NULL;
core_param(tion270_ether_addr, tion270_ether_addr, charp, 0444);

void __init tion270_ether_init(void)
{
	u8 ether_addr[sizeof(tion270_dm9000_platform_data.dev_addr)];
	const char *ether_addr_source = NULL;

	if (tion270_ether_addr) {
		if (!parse_ether_addr(tion270_ether_addr, ether_addr) &&
				is_valid_ether_addr(ether_addr))
		{
			ether_addr_source = "boot parameter";
		}
		else {
			printk(KERN_ERR "%s specified, but contains invalid address \"%s\"\n",
					"tion270_ether_addr", tion270_ether_addr);
		}
	}

	if (!ether_addr_source) {
		u64 serial = (((u64) system_serial_high) << 32) | system_serial_low;
		u8 *eth = ether_addr;
		int n = sizeof(ether_addr);

		while (n--) {
			*eth++ = serial & 0xff;
			serial >>= 8;
		}

		if (is_valid_ether_addr(ether_addr)) {
			ether_addr_source = "board serial number";
		}
	}

	if (ether_addr_source) {
		printk(KERN_INFO "Taking MAC address from %s\n", ether_addr_source);
		memcpy(tion270_dm9000_platform_data.dev_addr, ether_addr, sizeof(tion270_dm9000_platform_data.dev_addr));
	}
	else {
		printk(KERN_INFO "No valid MAC address specified, it will be taken from device if any\n");
	}

	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_dm9000_pin_config));
	platform_device_register(&tion270_dm9000_device);
}

#endif /* defined(CONFIG_TION270_DM9000) */

/*
 * I2C bus and devices.
 */
#if defined(CONFIG_TION270_I2C)

static mfp_cfg_t tion270_i2c_pin_config[] __initdata = {
	GPIO117_I2C_SCL,	/* I2C clock */
	GPIO118_I2C_SDA,	/* I2C data */
};

static struct i2c_board_info __initdata tion270_i2c_devices[] = {
#	if defined(CONFIG_TION270_I2C_RTC_M41T00)
	{
		/* RTC */
		I2C_BOARD_INFO("m41t00", 0x68),
	},
#	endif /* defined(CONFIG_TION270_I2C_RTC_M41T00) */
};

void __init tion270_i2c_init(struct i2c_pxa_platform_data *i2c_platform_info,
		struct i2c_board_info *i2c_devices, size_t nr_i2c_devices)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_i2c_pin_config));

	pxa_set_i2c_info(i2c_platform_info);

	if (i2c_devices) {
		i2c_register_board_info(0, i2c_devices, nr_i2c_devices);
	}
	else {
		i2c_register_board_info(0, ARRAY_AND_SIZE(tion270_i2c_devices));
	}
}

#endif /* defined(CONFIG_TION270_I2C) */

/*
 * Power I2C bus and devices.
 */
#if defined(CONFIG_TION270_I2C_POWER)

void __init tion270_i2c_power_init(struct i2c_pxa_platform_data *i2c_power_platform_info,
		struct i2c_board_info *i2c_power_devices, size_t nr_i2c_power_devices)
{
	pxa27x_set_i2c_power_info(i2c_power_platform_info);

	if (i2c_power_devices) {
		i2c_register_board_info(1, i2c_power_devices, nr_i2c_power_devices);
	}
}

#endif /* defined(CONFIG_TION270_I2C_POWER) */

/*
 * SPI bus and devices.
 */
#if defined(CONFIG_TION270_SPI)

static mfp_cfg_t tion270_spi_pin_config[] __initdata = {
	GPIO23_SSP1_SCLK,
	GPIO24_SSP1_SFRM,
	GPIO25_SSP1_TXD,
	GPIO26_SSP1_RXD,
};

static struct pxa2xx_spi_master tion270_spi_master_info = {
	.num_chipselect	= 1,
	.enable_dma		= 0,
};

void __init tion270_spi_init(struct pxa2xx_spi_master *spi_master_info,
		struct spi_board_info *spi_devices, size_t nr_spi_devices)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_spi_pin_config));

	if (spi_master_info) {
		pxa2xx_set_spi_info(1, spi_master_info);
	}
	else {
		pxa2xx_set_spi_info(1, &tion270_spi_master_info);
	}

	if (spi_devices) {
		spi_register_board_info(spi_devices, nr_spi_devices);
	}
}

#endif /* defined(CONFIG_TION270_SPI) */

/*
 * MMC/SD controller.
 */
#if defined(CONFIG_TION270_MMC)

static mfp_cfg_t tion270_mci_pin_config[] __initdata = {
	GPIO32_MMC_CLK,
	GPIO112_MMC_CMD,
	GPIO92_MMC_DAT_0,
	GPIO109_MMC_DAT_1,
	GPIO110_MMC_DAT_2,
	GPIO111_MMC_DAT_3,
	TION270_MMC_CARD_DETECT_GPIO_MFP,	/* MMC card detect */
};

static struct pxamci_platform_data tion270_mci_platform_data = {
	.ocr_mask			= MMC_VDD_32_33 | MMC_VDD_33_34,
	.gpio_power			= -1,
	.gpio_card_ro		= -1,
	.gpio_card_detect	= TION270_MMC_CARD_DETECT_GPIO_NUM,
	.detect_delay_ms	= 200,
};

void __init tion270_mmc_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_mci_pin_config));
	pxa_set_mci_info(&tion270_mci_platform_data);
}

#endif /* defined(CONFIG_TION270_MMC) */

/*
 * PXA27x OHCI (USB host) controller.
 */
#if defined(CONFIG_TION270_USB_HOST)

static mfp_cfg_t tion270_ohci_pin_config[] __initdata = {
#	if !defined(CONFIG_TION270_USB_P2_HOST)
	/*
	 * Tion's original source notes: "NOTE: for 1 and 2 USB host port
	 * do not GPIO88_USBH1_PWR".
	 * We can't use over-current protection with the port 2, because its PWR pin
	 * isn't connected. And with over-current protection enabled it constantly reports
	 * the over-current condition, because the active level of the over-current signal
	 * has to be configured low because of the schematic of the USB power circuit.
	 */
	GPIO88_USBH1_PWR,	/* Over-current indicator */
#	else
	GPIO88_GPIO,		/* Setup as GPIO (override bootloader settings if any) */
#	endif
	GPIO89_USBH1_PEN,	/* Power enable */
	/* GPIO41_USB_P2_7,	*/ /* Port 2 OTG ID */
};

static int tion270_ohci_enter(struct device *dev)
{
#	if defined(CONFIG_TION270_USB_P2_HOST)
	printk(KERN_DEBUG "%s: setting up USB port 2 as the host port\n", __func__);

	/* UP2OCR = UP2OCR_IDON | UP2OCR_HXS | UP2OCR_HXOE | UP2OCR_DPPDE | UP2OCR_DMPDE; */
	UP2OCR = UP2OCR_HXOE | UP2OCR_HXS | UP2OCR_DPPDE | UP2OCR_DMPDE;
#	else
	/* Do nothing. */
	printk(KERN_DEBUG "%s: skipping USB port 2 setup, so as it isn't a host port\n", __func__);
#	endif /* defined(CONFIG_TION270_USB_P2_HOST) */
	return 0;
}

static void tion270_ohci_exit(struct device *dev)
{
	/*
	 * Original Tion's 2.6.22.19 source notes that in this place USB port power
	 * is still on and because of that they do UHCRHS = 0x1.
	 * Currently USB port power is turned off by the driver with no extra effort.
	 */

#	if defined(CONFIG_TION270_USB_P2_HOST)
	printk(KERN_DEBUG "%s: disabling port 2\n", __func__);

	UP2OCR = 0;
#	else
	/* Do nothing. */
	printk(KERN_DEBUG "%s: skipping port 2\n", __func__);
#	endif /* if defined(CONFIG_TION270_USB_P2_HOST) */
}

static struct pxaohci_platform_data tion270_ohci_platform_data = {
	.port_mode	= PMM_GLOBAL_MODE,
	.flags		=
#	if defined(CONFIG_TION270_USB_P1_HOST)
		ENABLE_PORT1 |
#	endif /* defined(CONFIG_TION270_USB_P1_HOST) */
#	if defined(CONFIG_TION270_USB_P2_HOST)
		/* We can't use over-current protection with port 2 - see the comment
		   in the USB MFP configuration. */
		ENABLE_PORT2 | NO_OC_PROTECTION |
#	else
		POWER_SENSE_LOW | OC_MODE_GLOBAL |
#	endif /* defined(CONFIG_TION270_USB_P2_HOST) */
		POWER_CONTROL_LOW,
	.power_on_delay	= 250,
	.init		= tion270_ohci_enter,
	.exit		= tion270_ohci_exit,
};

void __init tion270_ohci_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_ohci_pin_config));
	pxa_set_ohci_info(&tion270_ohci_platform_data);
}

#endif /* defined(CONFIG_TION270_USB_HOST) */

/*
 * PXA27x UDC (USB device) controller.
 */
#if defined(CONFIG_TION270_USB_P2_GADGET)

static mfp_cfg_t tion270_udc_pin_config[] __initdata = {
	TION270_UDC_HOST_DETECT_GPIO_MFP,		/* USB host detect */
};

static void tion270_udc_command(int cmd)
{
	switch(cmd) {
	case PXA2XX_UDC_CMD_CONNECT:
		printk(KERN_DEBUG "%s: connect\n", __func__);
		UP2OCR = UP2OCR_HXOE | UP2OCR_DPPUE;
		break;
	case PXA2XX_UDC_CMD_DISCONNECT:
		printk(KERN_DEBUG "%s: disconnect\n", __func__);
		UP2OCR = 0;
		break;
	default:
		printk(KERN_ERR "%s: unknown command %d\n", __func__, cmd);
		break;
	}
}

static struct pxa2xx_udc_mach_info udc_info __initdata = {
	.gpio_pullup = TION270_UDC_HOST_DETECT_GPIO_NUM,
	.udc_command = tion270_udc_command,
};

void __init tion270_udc_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_udc_pin_config));
	pxa_set_udc_info(&udc_info);
}

#endif /* defined(CONFIG_TION270_USB_P2_GADGET) */

/*
 * AC97 (sound & touchscreen).
 */
#if defined(CONFIG_TION270_AC97)

static mfp_cfg_t tion270_ac97_pin_config[] __initdata = {
	GPIO28_AC97_BITCLK,
	GPIO29_AC97_SDATA_IN_0,
	GPIO30_AC97_SDATA_OUT,
	GPIO31_AC97_SYNC,
	GPIO95_AC97_nRESET,
	GPIO98_AC97_SYSCLK,
};

static pxa2xx_audio_ops_t tion270_ac97_audio_ops = {
	.reset_gpio = TION270_AC97_RESET_GPIO_NUM,
};

void __init tion270_ac97_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_ac97_pin_config));
	pxa_set_ac97_info(&tion270_ac97_audio_ops);
}

#endif /* defined(CONFIG_TION270_AC97) */

/*
 * UCB1400 touchscreen.
 */
#if defined(CONFIG_TION270_UCB1400)

static mfp_cfg_t tion270_ucb1400_pin_config[] __initdata = {
	TION270_TOUCHSCREEN_IRQ_GPIO_MFP,	/* Touchscreen IRQ */
};

static struct ucb1400_pdata tion270_ucb1400_pdata = {
	.irq		= PXA_GPIO_TO_IRQ(TION270_TOUCHSCREEN_IRQ_GPIO_NUM),
};

static struct platform_device tion270_ucb1400_device = {
	.name		= "ucb1400_core",
	.id			= -1,
	.dev		= {
		.platform_data	= &tion270_ucb1400_pdata,
	},
};

void __init tion270_touchscreen_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_ucb1400_pin_config));
	platform_device_register(&tion270_ucb1400_device);
}

#endif /* defined(CONFIG_TION270_UCB1400) */

/*
 * PWM backlight
 */
#if defined(CONFIG_TION270_BACKLIGHT_PWM)

/* Following MFP configurations are used at run-time, so no __initdata! */

static mfp_cfg_t tion270_pwm_backlight_on_pin_config[] /* __initdata */ = {
		TION270_PWM_BACKLIGHT_ON_GPIO_MFP,
};

static mfp_cfg_t tion270_pwm_backlight_off_pin_config[] /* __initdata */ = {
		TION270_PWM_BACKLIGHT_OFF_GPIO_MFP,
};

static struct gpio tion270_pwm_backlight_gpios[] = {
	{
		.gpio	= TION270_PWM_BACKLIGHT_GPIO_NUM,
		.flags	= GPIOF_OUT_INIT_LOW,
		.label	= "PWM backlight power",
	},
};

static int tion270_pwm_backlight_notify(struct device *dev, int brightness) {
	printk(KERN_DEBUG "%s: brightness=%d\n", __func__, brightness);

	// Note: original Tion270 code sets PWM to 1/2 duty cycle to switch the backlight on. Why?

	return brightness;
}

static struct platform_pwm_backlight_data tion270_pwm_backlight_data = {
	.pwm_id				=  -1,	/* Superseded by pwm_lookup */
	.max_brightness		= 100,
	.dft_brightness		=  75,
	.lth_brightness		=   0,
	/* .levels = { }, */
	.pwm_period_ns		= 1000000,	/* 1 kHz by default */
	.notify				= tion270_pwm_backlight_notify,
};

static struct platform_device tion270_pwm_backlight_device = {
	.name	= "pwm-backlight",
	.id		= -1,
	.dev 	= {
		.parent			= &pxa27x_device_pwm0.dev,
		.platform_data	= &tion270_pwm_backlight_data,
	},
};

static struct pwm_lookup tion270_pwm_lookup_backlight[] = {
		PWM_LOOKUP("pxa27x-pwm.0", 0, "pwm-backlight", NULL),
};

static ushort tion270_backlight_power_on_delay = 250;	/* 250 ms delay by default */
core_param(tion270_backlight_power_on_delay, tion270_backlight_power_on_delay, ushort, 0644);

static void tion270_backlight_power_on(struct work_struct *work)
{
	printk(KERN_DEBUG "%s\n", __func__);

	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pwm_backlight_on_pin_config));
}

static DECLARE_DELAYED_WORK(tion270_backlight_power_delayed_work, tion270_backlight_power_on);

void tion270_pwm_backlight_power(int on)
{
	printk(KERN_DEBUG "%s: on=%d\n", __func__, on);

	/* Switch GPIO16 to PWM mode after LCD power on with some delay to avoid screen flashing. */

	if (on) {
		ushort delay;

		kparam_block_sysfs_write(tion270_backlight_power_on_delay);
		delay = tion270_backlight_power_on_delay;
		kparam_unblock_sysfs_write(tion270_backlight_power_on_delay);

		schedule_delayed_work(&tion270_backlight_power_delayed_work, msecs_to_jiffies(delay));
	}
	else {
		cancel_delayed_work(&tion270_backlight_power_delayed_work);
		pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pwm_backlight_off_pin_config));
	}
}

void __init tion270_pwm_backlight_init(struct platform_pwm_backlight_data *pwm_backlight_data) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pwm_backlight_off_pin_config));

	if (gpio_request_array(ARRAY_AND_SIZE(tion270_pwm_backlight_gpios))) {
		printk(KERN_ERR "Can't configure GPIO for PWM backlight\n");
	}

	if (pwm_backlight_data) {
		tion270_pwm_backlight_device.dev.platform_data = pwm_backlight_data;
	}

	platform_device_register(&tion270_pwm_backlight_device);
	pwm_add_table(ARRAY_AND_SIZE(tion270_pwm_lookup_backlight));
}

#endif /* defined(CONFIG_TION270_BACKLIGHT_PWM) */

/* Framebuffer (LCD/VGA) */
#if defined(CONFIG_TION270_FB)

static mfp_cfg_t tion270_pxafb_pin_config[] __initdata = {
	GPIOxx_LCD_TFT_16BPP,
	GPIO86_LCD_LDD_16,	/* 18BPP = 16BPP + LDD 16 and 17 */
	GPIO87_LCD_LDD_17,
#	if defined(TION270_LCD_POWER_GPIO)
	TION270_LCD_POWER_GPIO_MFP,			/* LCD power enable GPIO (Tion-Pro270 rev. 2) */
#	endif /* defined(TION270_LCD_POWER_GPIO) */
	TION270_VGA_DAC_ENABLE_GPIO_MFP,	/* VGA DAC power enable */
};

static struct gpio tion270_pxafb_gpios[] = {
#	if defined(TION270_LCD_POWER_GPIO)
	/* LCD power enable GPIO (Tion-Pro270 rev. 2) */
	{
		.gpio	= TION270_LCD_POWER_GPIO_NUM,
		.flags	= GPIOF_OUT_INIT_LOW,
		.label	= "LCD power enable",
	},
#	endif /* defined(TION270_LCD_POWER_GPIO) */
	/* VGA DAC power enable GPIO */
	{
		.gpio	= TION270_VGA_DAC_ENABLE_GPIO_NUM,
		.flags	= GPIOF_OUT_INIT_LOW,
		.label	= "VGA DAC enable",
	},
};

/* 320x240 @ 72.2Hz: Fpix=8.67MHz (104MHz / (2 * (5 + 1)), Fline=18.06kHz */
struct pxafb_mode_info tion270_pxafb_mode_info_320x200 = {
	.pixclock		= 115380,	/* Fpix period in picoseconds */
	.xres			= 320,
	.yres			= 240,
	.bpp			= 24,
	.depth			= 18,

	.left_margin	= 48,		/* 1 - 256 pixels */
	.right_margin	= 48,		/* 1 - 256 pixels */
	.upper_margin	= 3,		/* 0 - 255 lines  */
	.lower_margin	= 3,		/* 0 - 255 lines  */

	.hsync_len		= 64,		/* 1 - 64 pixels */
	.vsync_len		= 4,		/* 1 - 64 lines  */
};

/* 640x480 @ 60Hz: Fpix=26MHz (104MHz / (2 * (1 + 1)), Fline=31.33kHz */
struct pxafb_mode_info tion270_pxafb_mode_info_640x480 = {
	.pixclock		= 38460,	/* Fpix period in picoseconds */
	.xres			= 640,
	.yres			= 480,
	.bpp			= 24,
	.depth			= 18,

	.left_margin	= 100,		/* 1 - 256 pixels */
	.right_margin	= 50,		/* 1 - 256 pixels */
	.upper_margin	= 30,		/* 0 - 255 lines  */
	.lower_margin	= 10,		/* 0 - 255 lines  */

	.hsync_len		= 40,		/* 1 - 64 pixels */
	.vsync_len		= 2,		/* 1 - 64 lines  */
};

/* 800x600 @ 72.2Hz: Fpix=52MHz (104MHz / (2 * (0 + 1)), Fline=48.06kHz */
struct pxafb_mode_info tion270_pxafb_mode_info_800x600 = {
	.pixclock		= 19230,	/* Fpix period in picoseconds */
	.xres			= 800,
	.yres			= 600,
	.bpp			= 24,
	.depth			= 18,

	.left_margin	= 114,		/* 1 - 256 pixels */
	.right_margin	= 106,		/* 1 - 256 pixels */
	.upper_margin	= 23,		/* 0 - 255 lines  */
	.lower_margin	= 37,		/* 0 - 255 lines  */

	.hsync_len		= 62,		/* 1 - 64 pixels */
	.vsync_len		= 6,		/* 1 - 64 lines  */
};

static struct pxafb_mach_info tion270_pxafb_mach_info = {
	.lcd_conn	= LCD_COLOR_TFT_18BPP | LCD_ALTERNATE_MAPPING
					| LCD_PCLK_EDGE_FALL | LCD_BIAS_ACTIVE_HIGH,
	/* .modes, .num_modes, .pxafb_lcd_power and .pxafb_backlight_power
	   are set by tion270_pxafb_init() */
};

void tion270_pxafb_lcd_power(int on, struct fb_var_screeninfo *info)
{
#	if defined(TION270_LCD_POWER_GPIO)

	printk(KERN_DEBUG "%s: on=%d\n", __func__, on);

	/* Note: LCD power is turned on-off-on at startup for some reason,
	   but it seems working with no fix/workaround. */

	gpio_set_value(TION270_LCD_POWER_GPIO_NUM, on);

#	else

	printk(KERN_DEBUG "%s: on=%d (no-op)\n", __func__, on);

#	endif /* defined(TION270_LCD_POWER_GPIO) */
}

void tion270_pxafb_vga_power(int on, struct fb_var_screeninfo *info)
{
	printk(KERN_DEBUG "%s: on=%d\n", __func__, on);

	gpio_set_value(TION270_VGA_DAC_ENABLE_GPIO_NUM, on);
}

void tion270_pxafb_lcd_and_vga_power(int on, struct fb_var_screeninfo *info)
{
	tion270_pxafb_lcd_power(on, info);
	tion270_pxafb_vga_power(on, info);
}

void __init tion270_pxafb_init(struct pxafb_mach_info *pxafb_info,
		struct pxafb_mode_info *modes, size_t num_modes, int is_vga_dac_enable)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pxafb_pin_config));

	if (!pxafb_info) {
		pxafb_info = &tion270_pxafb_mach_info;

		pxafb_info->modes = modes;
		pxafb_info->num_modes = num_modes;

		if (!pxafb_info->pxafb_lcd_power) {
			if (gpio_request_array(ARRAY_AND_SIZE(tion270_pxafb_gpios))) {
				printk(KERN_ERR "Can't configure GPIO for LCD/VGA power\n");
			}

			pxafb_info->pxafb_lcd_power =
					is_vga_dac_enable ? tion270_pxafb_lcd_and_vga_power : tion270_pxafb_lcd_power;
		}

		if (!pxafb_info->pxafb_backlight_power) {
			pxafb_info->pxafb_backlight_power = tion270_pwm_backlight_power;
		}
	}

	pxa_set_fb_info(NULL, pxafb_info);
}

#endif /* defined(CONFIG_TION270_FB) */

/*
 * GPIO LEDs
 */
#if defined(CONFIG_TION270_LEDS)

static mfp_cfg_t tion270_gpio_leds_pin_config[] __initdata = {
	TION270_RED_LED_GPIO_MFP,
	TION270_GREEN_LED_GPIO_MFP,
};

static struct gpio_led tion270_gpio_leds[] = {
	[0] = {
		.name = "tion-pro270:red:1",
		.gpio = TION270_RED_LED_GPIO_NUM,
		.default_trigger = "none",
		.active_low = 1,	/* Low level on GPIO pin activates LED */
	},
	[1] = {
		.name = "tion-pro270:green:1",
		.gpio = TION270_GREEN_LED_GPIO_NUM,
		.default_trigger = "heartbeat",
		.active_low = 1,	/* Low level on GPIO pin activates LED */
	},
};

static struct gpio_led_platform_data tion270_gpio_led_platform_data = {
	.num_leds = ARRAY_SIZE(tion270_gpio_leds),
	.leds = tion270_gpio_leds,
};

static struct platform_device tion270_gpio_leds_device = {
	.name	= "leds-gpio",
	.id		= -1,
	.dev 	= {
		.platform_data	= &tion270_gpio_led_platform_data,
	},
};

void __init tion270_gpio_leds_init(void) {
	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_gpio_leds_pin_config));
	platform_device_register(&tion270_gpio_leds_device);
}

#endif /* defined(CONFIG_TION270_LEDS) */

#if defined(CONFIG_TION270_PWM)

static mfp_cfg_t tion270_pwm_pin_config[] __initdata = {
	/* Except for PWM0 - it is used for backlight */
	GPIO17_PWM1_OUT,
	GPIO11_PWM2_OUT,
	GPIO12_PWM3_OUT,
};

void __init tion270_pwm_init(void) {
	/*
	 * PWM hardware is initialized by the generic PXA code,
	 * we have to initialize PWM pins only.
	 */

	printk(KERN_DEBUG "%s: configuring PWM outputs\n", __func__);

	pxa2xx_mfp_config(ARRAY_AND_SIZE(tion270_pwm_pin_config));
}

#endif /* defined(CONFIG_TION270_PWM) */
