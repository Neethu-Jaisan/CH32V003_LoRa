#ifndef RFM95W_H
#define RFM95W_H

#include "ch32v00x.h"

#define RFM95W_NSS_PORT GPIOD
#define RFM95W_NSS_PIN  GPIO_Pin_0
#define RFM95W_RESET_PORT GPIOD
#define RFM95W_RESET_PIN  GPIO_Pin_4
#define RFM95W_DIO0_PORT GPIOC
#define RFM95W_DIO0_PIN  GPIO_Pin_2

void RFM95W_Init(void);
void RFM95W_Reset(void);
uint8_t RFM95W_ReadReg(uint8_t addr);
void RFM95W_WriteReg(uint8_t addr, uint8_t value);
void RFM95W_SetFrequency(uint32_t frequency);
void RFM95W_Send(uint8_t *data, uint8_t length);
uint8_t RFM95W_Receive(uint8_t *buffer);
void RFM95W_SetTxPower(uint8_t power);
void RFM95W_SetMode(uint8_t mode);

#endif
