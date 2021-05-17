/** ***************************************************************
* @file myoslib/src/s4433912_os_led.c
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

/* Includes ------------------------------------------------------------------*/
#include "s4433912_os_led.h"
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct device* board_led_devs[3];
gpio_pin_t LED_PINS[3] = {RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  general init for led gpio
  * @param  led - led name
  * @param 	pin - GPIO pin
  * @param 	flags - GPIO flags
  * @retval device - init device struct
  */
struct device* init_led(const char* led, gpio_pin_t pin, gpio_flags_t flags) {

    struct device* dev;
	int ret;

	dev = device_get_binding(led);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT_ACTIVE | flags);
	if (ret < 0) {
		return;
	}

    return dev;
}

/**
  * @brief  set GPIO pin
  * @param  led - led number
  * @param 	mode - GPIO mode
  * @retval void
  */
void s4433912_cli_led_set(uint8_t led, bool mode) {

    gpio_pin_set(board_led_devs[led], LED_PINS[led], (int) mode);
}

/**
  * @brief  set GPIO pin
  * @param  led - led number
  * @param 	mode - GPIO mode
  * @retval void
  */
void s4433912_cli_led_toggle(uint8_t led) {

    gpio_pin_toggle(board_led_devs[led], LED_PINS[led]);
}

/**
  * @brief  init rgb led
  * @retval void
  */
void s4433912_cli_led_init() {

	board_led_devs[RED_RGB] = init_led(RED_LED, RED_LED_PIN, RED_LED_FLAGS);
	board_led_devs[GREEN_RGB] = init_led(GREEN_LED, GREEN_LED_PIN, GREEN_LED_FLAGS);
	board_led_devs[BLUE_RGB] = init_led(BLUE_LED, BLUE_LED_PIN, BLUE_LED_FLAGS);
}
