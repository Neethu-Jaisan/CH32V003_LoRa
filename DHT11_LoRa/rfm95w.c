#include "rfm95w.h"
#include "debug.h"

void SPI1_Init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
                      RCC_APB2Periph_GPIOD | RCC_APB2Periph_SPI1;

    GPIO_InitTypeDef gpioInit;

    // Configure PA1 (MOSI), PA2 (MISO), PC5 (SCK)
    gpioInit.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    gpioInit.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &gpioInit);

    // Configure NSS (PD0) as output
    gpioInit.GPIO_Pin = GPIO_Pin_0;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &gpioInit);
    GPIO_SetBits(GPIOD, GPIO_Pin_0);

    SPI_InitTypeDef spiInit;
    spiInit.SPI_Mode = SPI_Mode_Master;
    spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spiInit.SPI_DataSize = SPI_DataSize_8b;
    spiInit.SPI_CPOL = SPI_CPOL_Low;
    spiInit.SPI_CPHA = SPI_CPHA_1Edge;
    spiInit.SPI_NSS = SPI_NSS_Soft;
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &spiInit);
    
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Transmit(uint8_t data) {
    while (!(SPI1->STATR & SPI_I2S_FLAG_TXE));
    SPI1->DATAR = data;
    while (!(SPI1->STATR & SPI_I2S_FLAG_RXNE));
    return SPI1->DATAR;
}

void RFM95W_WriteReg(uint8_t addr, uint8_t value) {
    GPIO_ResetBits(RFM95W_NSS_PORT, RFM95W_NSS_PIN);
    SPI1_Transmit(addr | 0x80);
    SPI1_Transmit(value);
    GPIO_SetBits(RFM95W_NSS_PORT, RFM95W_NSS_PIN);
}

uint8_t RFM95W_ReadReg(uint8_t addr) {
    GPIO_ResetBits(RFM95W_NSS_PORT, RFM95W_NSS_PIN);
    SPI1_Transmit(addr & 0x7F);
    uint8_t value = SPI1_Transmit(0x00);
    GPIO_SetBits(RFM95W_NSS_PORT, RFM95W_NSS_PIN);
    return value;
}

void RFM95W_Reset(void) {
    GPIO_ResetBits(RFM95W_RESET_PORT, RFM95W_RESET_PIN);
    delay_ms(10);
    GPIO_SetBits(RFM95W_RESET_PORT, RFM95W_RESET_PIN);
    delay_ms(10);
}

void RFM95W_Init(void) {
    RFM95W_Reset();
    uint8_t version = RFM95W_ReadReg(0x42);
    if (version == 0x12) {
        printf("RFM95W detected!\r\n");
    } else {
        printf("ERROR: RFM95W not detected.\r\n");
    }
}
