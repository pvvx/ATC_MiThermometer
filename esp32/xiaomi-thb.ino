#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>

int scanTime = 30; // seconds
BLEScan* pBLEScan;

void printBuffer(uint8_t* buf, int len) {
  for (int i = 0; i < len; i++) {
    Serial.printf("%02x", buf[i]);
  }
  Serial.print("\n");
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    uint8_t* findServiceData(uint8_t* data, size_t length, uint8_t* foundBlockLength) {
      uint8_t* rightBorder = data + length;
      while (data < rightBorder) {
        uint8_t blockLength = *data;
        //Serial.printf("blockLength: 0x%02x\n",blockLength);
        if (blockLength < 5) {
          data += (blockLength + 1);
          continue;
        }
        uint8_t blockType = *(data + 1);
        uint16_t serviceType = *(uint16_t*)(data + 2);
        //Serial.printf("blockType: 0x%02x, 0x%04x\n", blockType, serviceType);
        if (blockType == 0x16) { // https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
          // Serial.printf("blockType: 0x%02x, 0x%04x\n", blockType, serviceType);
          /* 16-bit UUID for Members 0xFE95 Xiaomi Inc. https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf */
          if (serviceType == 0xfe95 || serviceType == 0x181a) { // mi or custom service
            //Serial.printf("blockLength: 0x%02x\n",blockLength);
            //Serial.printf("blockType: 0x%02x, 0x%04x\n", blockType, serviceType);
            *foundBlockLength = blockLength;
            return data;
          }
        }
        data += (blockLength + 1);
      }
      return nullptr;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      uint8_t* payload = advertisedDevice.getPayload();
      size_t payloadLength = advertisedDevice.getPayloadLength();
      uint8_t serviceDataLength = 0;
      uint8_t* serviceData = findServiceData(payload, payloadLength, &serviceDataLength);
      if (serviceData == nullptr || serviceDataLength < 15)
        return;
      uint16_t serviceType = *(uint16_t*)(serviceData + 2);
      //Serial.printf("Found service '%04x' data len: %d, ", serviceType, serviceDataLength);
      //printBuffer(serviceData, serviceDataLength);
      if (serviceType == 0xfe95) {
        if (serviceData[5] & 0x10) {
          Serial.printf("MAC: "); printBuffer(serviceData + 9, 6);
        }
        if ((serviceData[5] & 0x08) == 0) { // not encrypted
          switch (serviceData[15]) {
            case 0x0D: {
                float temp = *(uint16_t*)(serviceData + 15 + 3) / 10.0;
                float humidity = *(uint16_t*)(serviceData + 15 + 5) / 10.0;
                Serial.printf("Temp: %.1f°, Humidity: %.1f %%, cout: %d\n", temp, humidity, serviceData[8]);
              }
              break;
            case 0x04: {
                float temp = *(uint16_t*)(serviceData + 15 + 3) / 10.0;
                Serial.printf("Temp: %.1f°\n", temp);
              }
              break;
            case 0x06: {
                float humidity = *(uint16_t*)(serviceData + 15 + 3) / 10.0;
                Serial.printf("Humidity: %.1f%%\n", humidity);
              }
              break;
            case 0x0A: {
                int battery = *(serviceData + 15 + 3);
                if (serviceData[15]) {
                  Serial.printf("Battery: %d%%, %d mV, cout: %d\n", battery, *(uint16_t*)(serviceData + 11 + 5 + 4), serviceData[8]);
                } else
                  Serial.printf("Battery: %d%%\n", battery);
              }
              break;
            default:
              Serial.printf("Type: 0x%02x ", serviceData[15]);
              printBuffer(serviceData, serviceDataLength);
              break;
          }
        } else {
          if (serviceDataLength > 17) {
            Serial.printf("Crypted data[%d]! ", serviceDataLength - 15); // https://github.com/Magalex2x14/LYWSD03MMC-info
          }
          Serial.printf("count: %d\n", serviceData[8]); 
        }
      } else { // serviceType == 0x181a
        Serial.printf("MAC: ");
        printBuffer(serviceData + 4, 6);
        float temp = *(uint16_t*)(serviceData + 10) / 100.0;
        float humidity = *(uint16_t*)(serviceData + 12) / 100.0;
        uint16_t vbat = *(uint16_t*)(serviceData + 14);
        Serial.printf("Temp: %.1f°, Humidity: %.1f%%, Vbatt: %d, Battery: %d%%, cout: %d\n", temp, humidity, vbat, serviceData[16], serviceData[17]);
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setInterval(625); // default 100
  pBLEScan->setWindow(625);  // default 100, less or equal setInterval value
  pBLEScan->setActiveScan(true);
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  pBLEScan->stop();
  pBLEScan->clearResults();
  //  delay(5000);
}
