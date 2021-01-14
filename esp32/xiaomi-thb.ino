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

void parse_value(uint8_t* buf, int len) {
  int16_t x = buf[3];
  if (buf[2] > 1)
    x |=  buf[4] << 8;
  switch (buf[0]) {
    case 0x0D:
      if (buf[2] && len > 6) {
        float temp = x / 10.0;
        x =  buf[5] | (buf[6] << 8);
        float humidity = x / 10.0;
        Serial.printf("Temp: %.1f°, Humidity: %.1f %%\n", temp, humidity);
      }
      break;
    case 0x04: {
        float temp = x / 10.0;
        Serial.printf("Temp: %.1f°\n", temp);
      }
      break;
    case 0x06: {
        float humidity = x / 10.0;
        Serial.printf("Humidity: %.1f%%\n", humidity);
      }
      break;
    case 0x0A: {
        Serial.printf("Battery: %d%%", x);
        if (len > 5 && buf[4] == 2) {
          uint16_t battery_mv = buf[5] | (buf[6] << 8);
          Serial.printf(", %d mV", battery_mv);
        }
        Serial.printf("\n");
      }
      break;
    default:
      Serial.printf("Type: 0x%02x ", buf[0]);
      printBuffer(buf, len);
      break;
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    uint8_t* findServiceData(uint8_t* data, size_t length, uint8_t* foundBlockLength) {
      uint8_t* rightBorder = data + length;
      while (data < rightBorder) {
        uint8_t blockLength = *data + 1;
        //Serial.printf("blockLength: 0x%02x\n",blockLength);
        if (blockLength < 5) {
          data += blockLength;
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
        data += blockLength;
      }
      return nullptr;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      uint8_t mac[6];
      uint8_t* payload = advertisedDevice.getPayload();
      size_t payloadLength = advertisedDevice.getPayloadLength();
      uint8_t serviceDataLength = 0;
      uint8_t* serviceData = findServiceData(payload, payloadLength, &serviceDataLength);
      if (serviceData == nullptr || serviceDataLength < 15)
        return;
      uint16_t serviceType = *(uint16_t*)(serviceData + 2);
      Serial.printf("Found service '%04x' data len: %d, ", serviceType, serviceDataLength);
      printBuffer(serviceData, serviceDataLength);
      if (serviceType == 0xfe95) {
        if (serviceData[5] & 0x10) {
          mac[5] = serviceData[9];
          mac[4] = serviceData[10];
          mac[3] = serviceData[11];
          mac[2] = serviceData[12];
          mac[1] = serviceData[13];
          mac[0] = serviceData[14];
          Serial.printf("MAC: "); printBuffer(mac, 6);
        }
        if ((serviceData[5] & 0x08) == 0) { // not encrypted
          serviceDataLength -= 15;
          payload = &serviceData[15];
          while (serviceDataLength > 3) {
            parse_value(payload, serviceDataLength);
            serviceDataLength -= payload[2] + 3;
            payload += payload[2] + 3;
          }
          Serial.printf("count: %d\n", serviceData[8]);
        } else {
          if (serviceDataLength > 19) { // aes-ccm  bindkey
            // https://github.com/ahpohl/xiaomi_lywsd03mmc
            // https://github.com/Magalex2x14/LYWSD03MMC-info
            Serial.printf("Crypted data[%d]! ", serviceDataLength - 15);
          }
          Serial.printf("count: %d\n", serviceData[8]);
        }
      } else { // serviceType == 0x181a
        if(serviceDataLength > 18) { // custom format
          mac[5] = serviceData[4];
          mac[4] = serviceData[5];
          mac[3] = serviceData[6];
          mac[2] = serviceData[7];
          mac[1] = serviceData[8];
          mac[0] = serviceData[9];
          Serial.printf("MAC: ");
          printBuffer(mac, 6);
          float temp = *(int16_t*)(serviceData + 10) / 100.0;
          float humidity = *(uint16_t*)(serviceData + 12) / 100.0;
          uint16_t vbat = *(uint16_t*)(serviceData + 14);
          Serial.printf("Temp: %.2f°, Humidity: %.2f%%, Vbatt: %d, Battery: %d%%, flg: 0x%02x, cout: %d\n", temp, humidity, vbat, serviceData[16], serviceData[18], serviceData[17]);
        } else if(serviceDataLength == 17) { // format atc1441
          Serial.printf("MAC: "); printBuffer(serviceData + 4, 6);
          int16_t x = (serviceData[10]<<8) | serviceData[11];
          float temp = x / 10.0;
          uint16_t vbat = x = (serviceData[14]<<8) | serviceData[15];
          Serial.printf("Temp: %.1f°, Humidity: %d%%, Vbatt: %d, Battery: %d%%, cout: %d\n", temp, serviceData[12], vbat, serviceData[13], serviceData[16]);
        }
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
