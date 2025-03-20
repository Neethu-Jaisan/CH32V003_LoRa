#include "lorawan.h"
#include "rfm95w.h"
#include "debug.h"

void LoRaWAN_Send(uint8_t *data, uint8_t length) {
    RFM95W_SetMode(1);  // Set to standby mode
    RFM95W_Send(data, length);
    printf("LoRaWAN: Packet Sent\r\n");
}

void LoRaWAN_Receive(void) {
    uint8_t buffer[64];
    if (RFM95W_Receive(buffer)) {
        printf("LoRaWAN: Received %s\r\n", buffer);
    }
}
