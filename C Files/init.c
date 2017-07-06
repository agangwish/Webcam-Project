/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "compiler.h"
#include "gpio.h"
#include "ioport.h"

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	// From PWM code. May be unnecessary.
	#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
		/* Disable the watchdog */
		WDT->WDT_MR = WDT_MR_WDDIS;
	#endif
	
	
	ioport_init();
	
	// Start by resetting camera
	ioport_set_pin_dir(CAM_RST_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CAM_RST_PIN, false);
	
	// Start by not resetting WiFi
	ioport_set_pin_dir(WIFI_RESET_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(WIFI_RESET_PIN, true);
	
	// Pull CTS pin of WiFi low to enable communication
	ioport_set_pin_dir(WIFI_CTS_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(WIFI_CTS_PIN, false);
	
	// Set other pins as inputs
	ioport_set_pin_dir(WIFI_COMM_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(WIFI_WEB_SETUP_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(WIFI_NETWORK_PIN, IOPORT_DIR_INPUT);
	
	
	// From PWM code
	#ifdef CONF_BOARD_PWM_LED0
		/* Configure PWM LED0 pin */
		gpio_configure_pin(PIN_PWM_LED0_GPIO, PIN_PWM_LED0_FLAGS);
	#endif
		
	#ifdef CONF_BOARD_PWM_LED1
		/* Configure PWM LED1 pin */
		gpio_configure_pin(PIN_PWM_LED1_GPIO, PIN_PWM_LED1_FLAGS);
	#endif
		
	#ifdef CONF_BOARD_PWM_LED2
		/* Configure PWM LED2 pin */
		gpio_configure_pin(PIN_PWM_LED2_GPIO, PIN_PWM_LED2_FLAGS);
	#endif
}
