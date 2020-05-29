#include "stm32f1xx_hal.h"
void all_led_off(void);
uint8_t calculate_test_params(uint8_t start);
void check_for_next_step(void);
float adc_to_bar(uint16_t val);
uint16_t bar_to_adc_diff(uint16_t bar);
uint16_t bar_to_adc(uint16_t bar);

