/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */


#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#include "compiler.h"
#include "system_sam4s.h"


/*----------------------------------------------------------------------------*/

#define BOARD_NUM_OF_LED 3

/** PWM LED0 pin definitions. */
//#define PIN_PWM_LED0_GPIO    PIO_PB13_IDX // periph A
#define PIN_PWM_LED0_GPIO    PIO_PA12_IDX // PAN
#define PIN_PWM_LED0_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED0_CHANNEL PWM_CHANNEL_0

/** PWM LED1 pin definitions.*/
//#define PIN_PWM_LED1_GPIO    PIO_PA20_IDX // periph B
#define PIN_PWM_LED1_GPIO    PIO_PA11_IDX // TILT
#define PIN_PWM_LED1_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED1_CHANNEL PWM_CHANNEL_1

/** PWM LED2 pin definitions. */
//#define PIN_PWM_LED2_GPIO    PIO_PA19_IDX // periph B
#define PIN_PWM_LED2_GPIO    PIO_PB4_IDX // ZOOM // Doesn't work since PA9 is a PWMFL line, which is input only
#define PIN_PWM_LED2_FLAGS   (PIO_PERIPH_B | PIO_DEFAULT)
#define PIN_PWM_LED2_CHANNEL PWM_CHANNEL_2

#endif // USER_BOARD_H

