#include "dht11.h"
#include "debug.h"

#define DHT11_PORT GPIOC
#define DHT11_PIN  GPIO_Pin_3

void DHT11_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOx, GPIO_Pin);
}

uint8_t DHT11_Read(DHT11_Data *data) {
    uint8_t bits[5] = {0};
    uint32_t timeout = 0;
    
    // Send start signal
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(18);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_us(40);

    // Change to input
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    // Wait for response
    timeout = 10000;
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)) {
        if (--timeout == 0) return 0;
    }
    
    timeout = 10000;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)) {
        if (--timeout == 0) return 0;
    }

    // Read 40 bits (5 bytes)
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN));
            delay_us(30);
            if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)) bits[i] |= (1 << (7 - j));
            while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN));
        }
    }

    // Verify checksum
    if (bits[4] != (bits[0] + bits[1] + bits[2] + bits[3])) return 0;

    data->humidity = bits[0];
    data->temperature = bits[2];
    
    return 1;
}
