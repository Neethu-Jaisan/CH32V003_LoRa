#include "dht11.h"
#include "debug.h"

void DHT11_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Configure the DHT11 data pin as output
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    GPIO_SetBits(DHT11_PORT, DHT11_PIN); // Set pin high initially
}

// Simple delay function (use accurate timing if needed)
void delay_us(uint32_t us) {
    volatile uint32_t count = us * 8;  // Approximate delay for CH32V003 at 48MHz
    while (count--) __NOP();
}

// Function to read DHT11 data
uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity) {
    uint8_t i, j, data[5] = {0};

    // Start signal: Pull data pin low for at least 18ms
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_us(18000);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_us(30);

    // Configure the pin as input (to read data from DHT11)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    // Wait for DHT11 response
    uint32_t timeout = 10000;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--) delay_us(1);
    if (timeout == 0) return 1;

    timeout = 10000;
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--) delay_us(1);
    if (timeout == 0) return 1;

    timeout = 10000;
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--) delay_us(1);
    if (timeout == 0) return 1;

    // Read 5 bytes (40 bits) from DHT11
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {
            while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)); // Wait for start of bit
            delay_us(30); // Wait for 30us
            if (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)) // If still high, it's a 1
                data[j] |= (1 << (7 - i));

            timeout = 10000;
            while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && timeout--) delay_us(1);
            if (timeout == 0) return 1;
        }
    }

    // Validate checksum
    if ((data[0] + data[1] + data[2] + data[3]) == data[4]) {
        *humidity = data[0];
        *temperature = data[2];
        return 0;
    } else {
        return 1; // Checksum error
    }
}
