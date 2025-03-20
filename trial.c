#include "ch32v00x.h"    // MCU-specific definitions
#include "rfm95w.h"      // RFM95W driver API (declares RFM95W_Init, RFM95W_Send, RFM95W_ReadReg, etc.)
#include "debug.h"       // Debug library (provides USART_Printf_Init to redirect printf)
#include <stdio.h>
#include <string.h>

// Simple busy-loop delay (in milliseconds)
void delay_ms(uint32_t ms) {
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 4000; j++);
    }
}

// UART1 Initialization for serial output (using PD6 for TX and PD5 for RX)
void UART1_Init(void) {
    // Enable clocks for GPIOD and USART1
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;
    
    GPIO_InitTypeDef gpioInit;
    // Configure PD6 as TX (Alternate Function Push-Pull)
    gpioInit.GPIO_Pin = GPIO_Pin_6;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &gpioInit);
    
    // Configure PD5 as RX (Floating Input)
    gpioInit.GPIO_Pin = GPIO_Pin_5;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &gpioInit);
    
    USART_InitTypeDef usartInit;
    usartInit.USART_BaudRate = 115200;
    usartInit.USART_WordLength = USART_WordLength_8b;
    usartInit.USART_StopBits = USART_StopBits_1;
    usartInit.USART_Parity = USART_Parity_No;
    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartInit.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usartInit);
    
    // Enable USART1
    USART_Cmd(USART1, USART_CTLR1_UE);
}

// Simple function to send a string over UART1 using printf (via debug.h redirection)
void UART1_SendString(const char *str) {
    while (*str) {
        while (!(USART1->STATR & USART_STATR_TXE));
        USART1->DATAR = *str++;
    }
}

// LED Initialization on PC1 (for visual feedback)
void LED_Init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
    GPIO_InitTypeDef gpioInit;
    gpioInit.GPIO_Pin = GPIO_Pin_1;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);
}

// Toggle LED on PC1
void LED_Toggle(void) {
    GPIOC->OUTDR ^= GPIO_Pin_1;
}

// SPI1 Initialization for RFM95W
// Uses: PA1 (MOSI) and PA2 (MISO) on GPIOA; PC5 (SCK) on GPIOC; PD0 (NSS) on GPIOD.
void SPI1_Init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
                      RCC_APB2Periph_GPIOD | RCC_APB2Periph_SPI1;
    
    GPIO_InitTypeDef gpioInit;
    // Configure PA1 (MOSI) and PA2 (MISO) on GPIOA
    gpioInit.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);
    
    // Configure PC5 (SCK) on GPIOC
    gpioInit.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &gpioInit);
    
    // Configure PD0 (NSS) on GPIOD as output push-pull
    gpioInit.GPIO_Pin = GPIO_Pin_0;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &gpioInit);
    GPIO_SetBits(GPIOD, GPIO_Pin_0);  // NSS high initially
    
    SPI_InitTypeDef spiInit;
    spiInit.SPI_Mode = SPI_Mode_Master;
    spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spiInit.SPI_DataSize = SPI_DataSize_8b;
    spiInit.SPI_CPOL = SPI_CPOL_Low;
    spiInit.SPI_CPHA = SPI_CPHA_1Edge;
    spiInit.SPI_NSS = SPI_NSS_Soft;
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &spiInit);
    
    SPI_Cmd(SPI1, SPI_CTLR1_SPE);
}

// SPI1_Transmit function used by the RFM95W driver (declared external in rfm95w.c)
uint8_t SPI1_Transmit(uint8_t data) {
    while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
    SPI1->DATAR = data;
    while (!(SPI1->STATR & SPI_I2S_FLAG_RXNE));
    return SPI1->DATAR;
}

int main(void) {
    SystemInit();
    delay_ms(10);
    
    SPI1_Init();
    UART1_Init();
    LED_Init();
    
    // Initialize UART printf redirection via debug library
    USART_Printf_Init(115200);
    
    printf("CH32V003 LoRaWAN ABP Start\r\n");
    printf("Device EUI: 30 24 02 5F 78 4F 5C 50\r\n");
    printf("Device Addr: FC 00 95 F4\r\n");
    printf("NwkSKey: F4 67 D2 A9 B5 F1 33 6C 61 FD D2 08 E3 CF 62 99\r\n");
    printf("AppSKey: 9A F3 61 74 3E F7 4C 9C 90 E5 9F 53 1D 88 85 80\r\n");
    printf("Frequency: 865.4025 MHz\r\n");
    
    // Check RFM95W version register (address 0x42)
    char buf[50];
    uint8_t version = RFM95W_ReadReg(0x42);
    sprintf(buf, "RFM95W Version: 0x%02X\r\n", version);
    printf("%s", buf);
    
    // Initialize the RFM95W radio
    RFM95W_Init();
    
    // Message to send ("Hello World")
    uint8_t message[] = "Hello World";
    
    while (1) {
        printf("Sending LoRa uplink: Hello World\r\n");
        RFM95W_Send(message, strlen((char *)message));
        LED_Toggle();
        delay_ms(1000);
    }
    
    return 0;
}
