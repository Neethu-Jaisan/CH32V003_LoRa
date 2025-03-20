# LoRaWAN-Based DHT11 Sensor Data Transmission with CH32V003 & RFM95W

## **1. Overview**
This experiment demonstrates the integration of a **DHT11 temperature & humidity sensor** with the **CH32V003** microcontroller to send sensor data over **LoRaWAN** using the **RFM95W LoRa module**. The transmitted data is received by a **ChirpStack LoRaWAN server**, which decodes and displays the sensor values.

## **2. Hardware Requirements**
- CH32V003F4P6 microcontroller
- RFM95W LoRa module (SX1276/1278)
- DHT11 temperature & humidity sensor
- Jumper wires & breadboard
- 3.3V power supply

## **3. Circuit Connections**

### **RFM95W (LoRa Module) Wiring**
| RFM95W Pin | CH32V003 Pin |
|------------|-------------|
| NSS        | PD0         |
| MOSI       | PA1         |
| MISO       | PA2         |
| SCK        | PC5         |
| DIO0       | PC4         |
| RESET      | PC3         |

### **DHT11 Sensor Wiring**
| DHT11 Pin | CH32V003 Pin |
|-----------|-------------|
| VCC       | 3.3V        |
| GND       | GND         |
| Data      | PC3         |

## **4. Software Setup**
### **4.1 Dependencies**
Ensure you have the following:
- **MounRiver Studio** for compiling and flashing CH32V003
- **LoRaWAN Gateway + ChirpStack** configured

### **4.2 Firmware Files**
The following files are used in this project:
- `main.c`: Initializes LoRa & DHT11, reads sensor data, and transmits over LoRaWAN.
- `dht11.c / dht11.h`: DHT11 driver for reading temperature and humidity.
- `rfm95w.c / rfm95w.h`: LoRa module driver for sending packets.
- `lorawan.c / lorawan.h`: LoRaWAN transmission logic.
- `README.md`: This documentation.

## **5. Sample Output**
Upon successful execution, the CH32V003 UART debug prints the following messages:
```
CH32V003 LoRaWAN + DHT11 Start
RFM95W Version: 0x12
DHT11: Temp = 27C, Humidity = 60%
LoRaWAN: Sent Temp = 27C, Hum = 60%
```

On ChirpStack, the decoded payload appears as:
```json
{
  "sensor_id": 1,
  "temperature": 27,
  "humidity": 60
}
```

## **6. ChirpStack Payload Decoder**
To decode the incoming LoRa payload in ChirpStack, use the following **JavaScript decoder**:
```js
function decodeUplink(input) {
    return {
        data: {
            sensor_id: input.bytes[0],
            temperature: input.bytes[1],
            humidity: input.bytes[2]
        }
    };
}
```

## **7. Troubleshooting**
### **Issue: RFM95W Not Responding**
- Check SPI connections and ensure `NSS`, `SCK`, `MISO`, and `MOSI` are correctly wired.
- Verify power supply (3.3V) to RFM95W.
- Confirm SPI communication using the loopback test.

### **Issue: DHT11 Read Fails**
- Ensure correct **pull-up resistor (4.7KΩ - 10KΩ)** on the Data pin.
- Check DHT11 wiring and power connections.

### **Issue: No Data on ChirpStack**
- Ensure your **LoRa gateway** is online and forwarding packets.
- Verify that **LoRaWAN keys (DevEUI, AppKey, AppEUI)** are correctly configured in ChirpStack.

---

## **8. Next Steps**
- Optimize power consumption using **sleep modes**.
- Implement **adaptive data rate (ADR)** for dynamic transmission power control.
- Expand the project with **multiple sensors**.

This experiment successfully demonstrates **DHT11 sensor integration with LoRaWAN on CH32V003**, enabling real-time **IoT data transmission**. 🚀

