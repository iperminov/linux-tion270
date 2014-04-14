#ifndef _TION270_H_
#define _TION270_H_

#include <mach/mfp.h>

/* Flash region */
#define TION270_FLASH_PHYS		PXA_CS0_PHYS
/* Maximum flash size 64 MB, actual size is auto-detected */
#define TION270_FLASH_SIZE		SZ_64M

/* Ethernet DM9000 region */
#define TION270_DM9000_PHYS				PXA_CS2_PHYS
/* Ethernet DM9000 IRQ GPIO */
#define TION270_DM9000_IRQ_GPIO_NUM		114
#define TION270_DM9000_IRQ_GPIO_MFP		GPIO114_GPIO

/* MMC card detect GPIO */
#define TION270_MMC_CARD_DETECT_GPIO_NUM		0
#define TION270_MMC_CARD_DETECT_GPIO_MFP		GPIO0_GPIO

/* USB device controller host detect GPIO */
#define TION270_UDC_HOST_DETECT_GPIO_NUM		41
#define TION270_UDC_HOST_DETECT_GPIO_MFP		GPIO41_GPIO

/* AC97 reset GPIO */
#define TION270_AC97_RESET_GPIO_NUM		95

/* Touchscreen UCB1400 IRQ GPIO */
#define TION270_TOUCHSCREEN_IRQ_GPIO_NUM		113
#define TION270_TOUCHSCREEN_IRQ_GPIO_MFP		GPIO113_GPIO

/* PWM backlight */
#define TION270_PWM_BACKLIGHT_GPIO_NUM		16
#define TION270_PWM_BACKLIGHT_ON_GPIO_MFP	GPIO16_PWM0_OUT
#define TION270_PWM_BACKLIGHT_OFF_GPIO_MFP	MFP_CFG_OUT(GPIO16, AF0, DRIVE_LOW)

/* LCD power enable GPIO (Tion-Pro270 rev. 2) */
#if defined(CONFIG_MACH_TION_PRO270)
#	define TION270_LCD_POWER_GPIO
#	define TION270_LCD_POWER_GPIO_NUM				44
#	define TION270_LCD_POWER_GPIO_MFP				GPIO44_GPIO
#endif /* defined(CONFIG_MACH_TION_PRO270) */

/* VGA DAC power enable GPIO */
#define TION270_VGA_DAC_ENABLE_GPIO_NUM			81
#define TION270_VGA_DAC_ENABLE_GPIO_MFP			GPIO81_GPIO

/* LEDs GPIO (Tion-Pro270 rev. 2) */
#if defined(CONFIG_MACH_TION_PRO270)
#	define TION270_RED_LED_GPIO_NUM		54
#	define TION270_RED_LED_GPIO_MFP		GPIO54_GPIO
#	define TION270_GREEN_LED_GPIO_NUM	107
#	define TION270_GREEN_LED_GPIO_MFP	GPIO107_GPIO
#endif /* defined(CONFIG_MACH_TION_PRO270) */

#if defined(CONFIG_TION270_FFUART)
void tion270_ffuart_init(void);
#else
static inline void tion270_ffuart_init(void) { }
#endif /* defined(CONFIG_TION270_FFUART) */

#if defined(CONFIG_TION270_BTUART)
void tion270_btuart_init(void);
#else
static inline void tion270_btuart_init(void) { }
#endif /* defined(CONFIG_TION270_BTUART) */

#if defined(CONFIG_TION270_STUART)
void tion270_stuart_init(void);
#else
static inline void tion270_stuart_init(void) { }
#endif /* defined(CONFIG_TION270_STUART) */

struct mtd_partition;

#if defined(CONFIG_TION270_FLASH)
void tion270_flash_init(struct mtd_partition* partitions, size_t nr_partitions);
#else
static inline void tion270_flash_init(struct mtd_partition* partitions, size_t nr_partitions) { }
#endif /* defined(CONFIG_TION270_FLASH) */

#if defined(CONFIG_TION270_DM9000)
void tion270_ether_init(void);
#else
static inline void tion270_ether_init(void) { }
#endif

struct i2c_pxa_platform_data;
struct i2c_board_info;

#if defined(CONFIG_TION270_I2C)
void tion270_i2c_init(struct i2c_pxa_platform_data *i2c_platform_info,
		struct i2c_board_info *i2c_devices, size_t nr_i2c_devices);
#else
static inline void tion270_i2c_init(struct i2c_pxa_platform_data *i2c_platform_info,
		struct i2c_board_info *i2c_devices, size_t nr_i2c_devices)
{ }
#endif /* defined(CONFIG_TION270_I2C) */

#if defined(CONFIG_TION270_I2C_POWER)
void tion270_i2c_power_init(struct i2c_pxa_platform_data *i2c_power_platform_info,
		struct i2c_board_info *i2c_power_devices, size_t nr_i2c_power_devices);
#else
static inline void tion270_i2c_power_init(struct i2c_pxa_platform_data *i2c_power_platform_info,
		struct i2c_board_info *i2c_power_devices, size_t nr_i2c_power_devices)
{ }
#endif /* defined(CONFIG_TION270_I2C_POWER) */

struct pxa2xx_spi_master;
struct spi_board_info;

#if defined(CONFIG_TION270_SPI)
void tion270_spi_init(struct pxa2xx_spi_master *spi_master_info,
		struct spi_board_info *spi_devices, size_t nr_spi_devices);
#else
static inline void tion270_spi_init(struct pxa2xx_spi_master *spi_master_info,
		struct spi_board_info *spi_devices, size_t nr_spi_devices)
{ }
#endif /* defined(CONFIG_TION270_SPI) */

#if defined(CONFIG_TION270_MMC)
void tion270_mmc_init(void);
#else
static inline void tion270_mmc_init(void) { }
#endif /* defined(CONFIG_TION270_MMC) */

#if defined(CONFIG_TION270_USB_HOST)
void tion270_ohci_init(void);
#else
static inline void tion270_ohci_init(void) { }
#endif /* defined(CONFIG_TION270_USB_HOST) */

#if defined(CONFIG_TION270_USB_P2_GADGET)
void tion270_udc_init(void);
#else
static inline void tion270_udc_init(void) { }
#endif /* defined(CONFIG_TION270_USB_P2_GADGET) */

#if defined(CONFIG_TION270_AC97)
void tion270_ac97_init(void);
#else
static inline void tion270_ac97_init() { }
#endif /* defined(CONFIG_TION270_AC97) */

#if defined(CONFIG_TION270_UCB1400)
void tion270_touchscreen_init(void);
#else
static inline void tion270_touchscreen_init(void) { }
#endif /* defined(CONFIG_TION270_UCB1400) */

struct platform_pwm_backlight_data;

#if defined(CONFIG_TION270_BACKLIGHT_PWM)
void tion270_pwm_backlight_power(int on);
void tion270_pwm_backlight_init(struct platform_pwm_backlight_data *pwm_backlight_data);
#else
#	define tion270_pwm_backlight_power NULL
static inline void tion270_pwm_backlight_init(struct platform_pwm_backlight_data *pwm_backlight_data)
{ }
#endif /* defined(CONFIG_TION270_BACKLIGHT_PWM) */

struct pxafb_mode_info;
struct pxafb_mach_info;

#if defined(CONFIG_TION270_FB)

extern struct pxafb_mode_info tion270_pxafb_mode_info_320x200;
extern struct pxafb_mode_info tion270_pxafb_mode_info_640x480;
extern struct pxafb_mode_info tion270_pxafb_mode_info_800x600;

void tion270_pxafb_lcd_power(int on, struct fb_var_screeninfo *info);
void tion270_pxafb_vga_power(int on, struct fb_var_screeninfo *info);
void tion270_pxafb_lcd_and_vga_power(int on, struct fb_var_screeninfo *info);

void tion270_pxafb_init(struct pxafb_mach_info *pxafb_info,
		struct pxafb_mode_info *modes, size_t num_modes, int is_vga);

#else

static inline void tion270_pxafb_init(struct pxafb_mach_info *pxafb_info,
		struct pxafb_mode_info *modes, size_t num_modes, int is_vga)
{ }

#endif /* defined(CONFIG_TION270_FB) */

#if defined(CONFIG_TION270_LEDS)
void tion270_gpio_leds_init(void);
#else
static inline void tion270_gpio_leds_init(void) { }
#endif /* defined(CONFIG_TION270_LEDS) */

#if defined(CONFIG_TION270_PWM)
void tion270_pwm_init(void);
#else
static inline void tion270_pwm_init(void) { }
#endif /* defined(CONFIG_TION270_PWM) */

#endif /* _TION270_H_ */
