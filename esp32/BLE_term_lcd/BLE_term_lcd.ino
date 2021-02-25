/**
  An example of a BLE intermediary.
  Accepts advertisements from one thermometer and transmits to another device LCD.
*/

#include "BLEDevice.h"

BLEScan* pBLEScan;

// The remote data device MAC
BLEAddress inMacAddress = BLEAddress("a4:c1:38:56:58:70");
// The remote LCD device MAC
BLEAddress outMacAddress = BLEAddress("a4:c1:38:0b:5e:ed");
// The remote service we wish to connect to.
static BLEUUID serviceUUID("00001f10-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("00001f1f-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

float temp, humidity;
static boolean doScan = false;


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
        temp = x / 10.0;
        x =  buf[5] | (buf[6] << 8);
        humidity = x / 10.0;
        doScan = true;
        Serial.printf("Temp: %.1f°, Humidity: %.1f %%\n", temp, humidity);
      }
      break;
    case 0x04: {
        temp = x / 10.0;
        doScan = true;
        Serial.printf("Temp: %.1f°\n", temp);
      }
      break;
    case 0x06: {
        humidity = x / 10.0;
        doScan = true;
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

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  printBuffer(pData, length);
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
      Serial.println("Connect");
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("onDisconnect");
    }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    printBuffer((uint8_t*)value.c_str(), value.length());
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  // pRemoteCharacteristic->writeValue("\x33\xff");

  connected = true;
  return true;
}
/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
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
      if (connected) {
        if (inMacAddress.equals(advertisedDevice.getAddress())) {
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
                Serial.printf("Crypted data[%d]! ", serviceDataLength - 15);
              }
              Serial.printf("count: %d\n", serviceData[8]);
            }
          } else { // serviceType == 0x181a
            if (serviceDataLength > 18) { // custom format
              mac[5] = serviceData[4];
              mac[4] = serviceData[5];
              mac[3] = serviceData[6];
              mac[2] = serviceData[7];
              mac[1] = serviceData[8];
              mac[0] = serviceData[9];
              Serial.printf("MAC: ");
              printBuffer(mac, 6);
              temp = *(int16_t*)(serviceData + 10) / 100.0;
              humidity = *(uint16_t*)(serviceData + 12) / 100.0;
              doScan = true;
              uint16_t vbat = *(uint16_t*)(serviceData + 14);
              Serial.printf("Temp: %.2f°, Humidity: %.2f%%, Vbatt: %d, Battery: %d%%, flg: 0x%02x, cout: %d\n", temp, humidity, vbat, serviceData[16], serviceData[18], serviceData[17]);
            } else if (serviceDataLength == 17) { // format atc1441
              Serial.printf("MAC: "); printBuffer(serviceData + 4, 6);
              int16_t x = (serviceData[10] << 8) | serviceData[11];
              temp = x / 10.0;
              doScan = true;
              uint16_t vbat = x = (serviceData[14] << 8) | serviceData[15];
              Serial.printf("Temp: %.1f°, Humidity: %d%%, Vbatt: %d, Battery: %d%%, cout: %d\n", temp, serviceData[12], vbat, serviceData[13], serviceData[16]);
            }
          }
        } else {
          Serial.print("Found device, MAC: ");
          Serial.println(advertisedDevice.getAddress().toString().c_str());
        }
      }
      else if (outMacAddress.equals(advertisedDevice.getAddress())) {
        Serial.println("Found output device!");
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
      } else {
        Serial.print("Found device, MAC: ");
        Serial.println(advertisedDevice.getAddress().toString().c_str());
      }
    }
};

#if 0
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if ((!connected) && devMacAddress.equals(advertisedDevice.getAddress())) {
        Serial.println("Found device!");
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
        return;
      } else {
        Serial.print("Found device, MAC: ");
        Serial.println(advertisedDevice.getAddress().toString().c_str());
      }
    }
};
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 15 seconds.
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(false);
  Serial.println("Start scan (15 sec).");
  pBLEScan->start(15, false);
}

uint32_t timer_tick;

void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    if (millis() - timer_tick > 10000) {
      timer_tick = millis();
      Serial.print("Time since boot: ");
      Serial.println(timer_tick / 1000);
      pBLEScan->start(10, false);
      if (doScan) {
        doScan = false;
        uint8_t blk[7];
        int16_t tm = temp * 10.0;
        uint16_t hm = humidity;
        blk[0] = 0x22;
        blk[1] = tm;
        blk[2] = tm >> 8;
        blk[3] = hm;
        blk[4] = hm >> 8;
        blk[5] = 30;
        blk[6] = 0;
        Serial.print("New Data to LCD: ");
        printBuffer(blk, sizeof(blk));
        pRemoteCharacteristic->writeValue(blk, sizeof(blk));
      }
    }
  } else {
    if (doScan) {
      pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
      doScan = false;
    } else {
      Serial.println("Start new scan 15 sec.");
      pBLEScan->start(15, false);
    }
  }
  delay(1000); // Delay a second between loops.
}
