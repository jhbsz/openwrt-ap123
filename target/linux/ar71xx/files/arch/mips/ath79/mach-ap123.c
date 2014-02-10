/*
 *  TP-LINK TL-WR841N/ND v8/TL-MR3420 v2 board support
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define AP123_GPIO_LED_WLAN	14
#define AP123_GPIO_LED_WAN	18
#define AP123_GPIO_LED_LAN1	20
#define AP123_GPIO_LED_LAN2	22
#define AP123_GPIO_LED_USB	11

#define AP123_GPIO_BTN_RESET	17
#define AP123_GPIO_SW_RFKILL	16

#define AP123_KEYS_POLL_INTERVAL	20	/* msecs */
#define AP123_KEYS_DEBOUNCE_INTERVAL (3 * AP123_KEYS_POLL_INTERVAL)

static const char *ap123_part_probes[] = {
	"cmdlinepart",
	NULL,
};

static struct flash_platform_data ap123_flash_data = {
	.part_probes	= ap123_part_probes,
	.name = "ath-nor0",
};

static struct gpio_led ap123_leds_gpio[] __initdata = {
	{
		.name		= "ap123:green:lan1",
		.gpio		= AP123_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "ap123:green:lan2",
		.gpio		= AP123_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "ap123:green:wan",
		.gpio		= AP123_GPIO_LED_WAN,
		.active_low	= 1,
	}, {
		.name		= "ap123:green:wlan",
		.gpio		= AP123_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "ap123:green:usb",
		.gpio		= AP123_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ap123_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AP123_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP123_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "RFKILL switch",
		.type		= EV_SW,
		.code		= KEY_RFKILL,
		.debounce_interval = AP123_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP123_GPIO_SW_RFKILL,
		.active_low	= 0,
	}
};

static void __init ap123_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	/* Disable JTAG, enabling GPIOs 0-3 */
	/* Configure OBS4 line, for GPIO 4*/
	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE,
				 AR934X_GPIO_FUNC_CLK_OBS4_EN);

	ath79_register_m25p80(&ap123_flash_data);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_PHY_SWAP);

	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, -1);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);

	/* GMAC0 is connected to the PHY0 of the internal switch */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_register_eth(0);

	/* GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_register_eth(1);

	ath79_register_wmac(ee, mac);
}

static void __init ap123_init(void)
{
	ap123_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(ap123_leds_gpio) - 1,
				 ap123_leds_gpio);

	ath79_register_gpio_keys_polled(1, AP123_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(ap123_gpio_keys),
					ap123_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_AP123, "AP123", "AP123",
	     ap123_init);

