#ifndef LORAWAN_H
#define LORAWAN_H

#include <stdint.h>

void LoRaWAN_Send(uint8_t *data, uint8_t length);
void LoRaWAN_Receive(void);

#endif
