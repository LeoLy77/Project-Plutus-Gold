/** ***************************************************************
* @file myoslib/src/s4433912_os_led.h
* @author Leo Ly - 44339124
* @date 12/03/2021
* @brief rgb led os
* REFERENCE:  
***************************************************************
* EXTERNAL FUNCTIONS 
*************************************************************** 
* void s4433912_cli_led_set(uint8_t led, bool mode);
* void s4433912_cli_led_toggle(uint8_t led);
* void s4433912_cli_led_init();
*************************************************************** */

#ifndef S4433912_OS_RGB_LED_H
#define S4433912_OS_RGB_LED_H

#include <drivers/gpio.h>
#include <ctype.h>

#define SLEEP_TIME_MS   1000

#define RED_LED_NODE DT_ALIAS(led1)
#define GREEN_LED_NODE DT_ALIAS(led2)
#define BLUE_LED_NODE DT_ALIAS(led3)

#if DT_NODE_HAS_STATUS(RED_LED_NODE, okay)
#define RED_RGB 0
#define RED_LED	DT_GPIO_LABEL(RED_LED_NODE, gpios)
#define RED_LED_PIN	DT_GPIO_PIN(RED_LED_NODE, gpios)
#define RED_LED_FLAGS	DT_GPIO_FLAGS(RED_LED_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#define RED_RGB 0
#define RED_LED	""
#define RED_LED_PIN	0
#define RED_LED_FLAGS	0
#endif

#if DT_NODE_HAS_STATUS(BLUE_LED_NODE, okay)
#define BLUE_RGB 2
#define BLUE_LED	DT_GPIO_LABEL(BLUE_LED_NODE, gpios)
#define BLUE_LED_PIN	DT_GPIO_PIN(BLUE_LED_NODE, gpios)
#define BLUE_LED_FLAGS	DT_GPIO_FLAGS(BLUE_LED_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#define BLUE_RGB 0
#define BLUE_LED	""
#define BLUE_LED_PIN	0
#define BLUE_LED_FLAGS	0
#endif

#if DT_NODE_HAS_STATUS(GREEN_LED_NODE, okay)
#define GREEN_RGB 1
#define GREEN_LED	DT_GPIO_LABEL(GREEN_LED_NODE, gpios)
#define GREEN_LED_PIN	DT_GPIO_PIN(GREEN_LED_NODE, gpios)
#define GREEN_LED_FLAGS	DT_GPIO_FLAGS(GREEN_LED_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#define GREEN_RGB 0
#define GREEN_LED	""
#define GREEN_LED_PIN	0
#define GREEN_LED_FLAGS	0
#endif


extern struct device* board_led_devs[3];

void s4433912_cli_led_set(uint8_t led, bool mode);
void s4433912_cli_led_toggle(uint8_t led);
void s4433912_cli_led_init();

#endif