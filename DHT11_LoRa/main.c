#include "ch32v00x.h"
#include "debug.h"
#include "rfm95w.h"
#include "lorawan.h"
#include "dht11.h"

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++);
}

int main(void) {
    SystemInit();
    USART_Printf_Init(115200);
    SPI1_Init();
    
    printf("CH32V003 LoRaWAN + DHT11 Start\r\n");
    
    RFM95W_Init();
    DHT11_Init(GPIOC, GPIO_Pin_3);

    uint8_t version = RFM95W_ReadReg(0x42);
    printf("RFM95W Version: 0x%02X\r\n", version);
    
    if (version != 0x12) {
        printf("ERROR: No response from RFM95W! Check wiring.\r\n");
        while (1);
    }

    DHT11_Data sensorData;
    uint8_t payload[5];

    while (1) {
        if (DHT11_Read(&sensorData)) {
            printf("DHT11: Temp = %dC, Humidity = %d%%\r\n", sensorData.temperature, sensorData.humidity);
            
            // Prepare LoRaWAN payload
            payload[0] = 0x01;  // Sensor ID
            payload[1] = sensorData.temperature;
            payload[2] = sensorData.humidity;
            payload[3] = 0x00;  // Reserved
            payload[4] = 0x00;  // Reserved
            
            LoRaWAN_Send(payload, sizeof(payload));
            printf("LoRaWAN: Sent Temp = %dC, Hum = %d%%\r\n", sensorData.temperature, sensorData.humidity);
        } else {
            printf("ERROR: DHT11 Read Failed!\r\n");
        }
        
        delay_ms(5000);
    }
}
