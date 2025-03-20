#ifndef DHT11_H
#define DHT11_H

#include "ch32v00x.h"

#define DHT11_PIN GPIO_Pin_1 // Define the GPIO pin for DHT11 (Change if needed)
#define DHT11_PORT GPIOA

void DHT11_Init(void);
uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity);

#endif
