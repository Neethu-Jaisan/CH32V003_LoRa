#include "ch32v00x.h"   // MCU-specific definitions for CH32V003
#include "debug.h"      // Debug library that provides USART_Printf_Init
#include <stdio.h>

int main(void) {
    // Initialize system (clocks, etc.)
    SystemInit();

    // Initialize delay functions if needed (optional)
    // Delay_Init();  // Remove if using built-in delay

    // Initialize UART for printf output (baud rate 9600)
    USART_Printf_Init(115200);

    // Use printf to send a test message over UART
    printf("UART Test: Hello from CH32V003\r\n");

    // Loop forever
    while (1) {
        // Optionally, you can add a delay here if desired
    }

    return 0;
}
