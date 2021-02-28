/**
  An example of a BLE intermediary.
  Accepts advertisements from one thermometer and transmits to another device LCD.
*/

#include "BLEDevice.h"

BLEScan* pBLEScan;

// The remote data device MAC
BLEAddress inMacAddress = BLEAddress("a4:c1:38:0b:5e:ed");
// The remote LCD device MAC
BLEAddress outMacAddress = BLEAddress("a4:c1:38:56:58:70");
// The remote service we wish to connect to.
static BLEUUID serviceUUID("00001f10-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("00001f1f-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static struct {
  uint8_t mac[8];
  float temp;
  float humidity;
  uint16_t ctrl;
  uint16_t dev_id;
  uint16_t vbat;
  uint8_t cnt;
  uint8_t old_cnt;
  uint8_t bat;
  uint8_t en;
} srvdata;

void printBuffer(uint8_t* buf, int len) {
  for (int i = 0; i < len; i++) {
    Serial.printf("%02x", buf[i]);
  }
  Serial.print("\n");
}

void parse_value(uint8_t* buf, int len) {
  if (len < buf[2] + 3) // len data
    return;
  int16_t id = buf[0] + (buf[1] << 8);
  int16_t value = buf[3];
  if (len > 1)
    value |=  buf[4] << 8;
  switch (id) {
    case 0x100D:
      if (buf[2] == 4) {
        srvdata.temp = value / 10.0;
        value =  buf[5] | (buf[6] << 8);
        srvdata.humidity = value / 10.0;
        srvdata.en = true;
        Serial.printf("Temp: %.1f°, Humidity: %.1f %%\n", srvdata.temp, srvdata.humidity);
      }
      break;
    case 0x1004:
      if (buf[2] == 2) {
        srvdata.temp = value / 10.0;
        srvdata.en = true;
        Serial.printf("Temp: %.1f°\n", srvdata.temp);
      }
      break;
    case 0x1006:
      if (buf[2] == 2) {
        srvdata.humidity = value / 10.0;
        srvdata.en = true;
        Serial.printf("Humidity: %.1f%%\n", srvdata.humidity);
      }
      break;
    case 0x100A:
      if (buf[2] == 1) {
        srvdata.bat = value;
        Serial.printf("Battery: %d%%", srvdata.bat);
        if (len > 5 && buf[4] == 2) {
          srvdata.vbat = buf[5] | (buf[6] << 8);
          Serial.printf(", %d mV", srvdata.vbat);
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
      doConnect = false;
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
          uint8_t* payload = advertisedDevice.getPayload();
          size_t payloadLength = advertisedDevice.getPayloadLength();
          uint8_t serviceDataLength = 0;
          uint8_t* serviceData = findServiceData(payload, payloadLength, &serviceDataLength);
          if (serviceData == nullptr || serviceDataLength < 17 || serviceData[1] != 0x16)
            return;
          uint16_t serviceType = *(uint16_t*)(serviceData + 2);
          Serial.printf("Found service '%04x' data len: %d, ", serviceType, serviceDataLength);
          printBuffer(serviceData, serviceDataLength);
          if (serviceType == 0xfe95) {
            // 0  1  2    4    6    8  9            15   17
            // 15 16 95fe 5030 5b05 06 ed5e0b38c1a4 0d10 04 2300 9d02
            uint32_t i = 4;
            srvdata.ctrl = *(uint16_t*)(serviceData + i);
            i += 2; // = 6
            Serial.printf("CTRL: %04x ", srvdata.ctrl);
            srvdata.dev_id = *(uint16_t*)(serviceData + i);
            i += 2; // = 8
            Serial.printf("DEVID: %04x ", srvdata.dev_id);
            srvdata.cnt = serviceData[i++]; // i = 9
            if (srvdata.ctrl & 0x10) {
              if (serviceDataLength < i + 6) {
                Serial.printf("Error format! count: %d\n", srvdata.cnt);
                return;
              }
              srvdata.mac[5] = serviceData[i++];
              srvdata.mac[4] = serviceData[i++];
              srvdata.mac[3] = serviceData[i++];
              srvdata.mac[2] = serviceData[i++];
              srvdata.mac[1] = serviceData[i++];
              srvdata.mac[0] = serviceData[i++]; // i = 15
              Serial.printf("MAC: "); printBuffer(srvdata.mac, 6);
            }
            if (srvdata.ctrl & 0x20) { // Capability
              if (serviceDataLength < i) {
                Serial.printf("count: %d\n", srvdata.cnt);
                return;
              }
              if (serviceData[i++] & 0x20) { // IO
                if (serviceDataLength < i + 2) {
                  Serial.printf("count: %d\n", srvdata.cnt);
                  return;
                }
                i += 2;
              }
            }
            if (srvdata.ctrl & 0x40) { // Data
              if ((srvdata.ctrl & 0x08) == 0) { // not encrypted
                serviceDataLength -= i;
                payload = &serviceData[i];
                if (serviceDataLength > 3) {
                  parse_value(payload, serviceDataLength);
                  serviceDataLength -= payload[2] + 3;
                  payload += payload[2] + 3;
                }
              } else {
                if (serviceDataLength > 19) { // aes-ccm  bindkey
                  Serial.printf("Crypted data[%d]! ", serviceDataLength - 15);
                }
              }
            }
            Serial.printf("count: %d\n", srvdata.cnt);
          } else if (serviceType == 0x181a) { //
            if (serviceDataLength > 18) { // custom format
              srvdata.mac[5] = serviceData[4];
              srvdata.mac[4] = serviceData[5];
              srvdata.mac[3] = serviceData[6];
              srvdata.mac[2] = serviceData[7];
              srvdata.mac[1] = serviceData[8];
              srvdata.mac[0] = serviceData[9];
              Serial.printf("MAC: ");
              printBuffer(srvdata.mac, 6);
              srvdata.temp = *(int16_t*)(serviceData + 10) / 100.0;
              srvdata.humidity = *(uint16_t*)(serviceData + 12) / 100.0;
              srvdata.en = true;
              srvdata.vbat = *(uint16_t*)(serviceData + 14);
              srvdata.bat = serviceData[16];
              srvdata.cnt = serviceData[17];
              Serial.printf("Temp: %.2f°, Humidity: %.2f%%, Vbatt: %d, Battery: %d%%, flg: 0x%02x, cout: %d\n", srvdata.temp, srvdata.humidity, srvdata.vbat, srvdata.bat, serviceData[18], srvdata.cnt);
            } else if (serviceDataLength == 17) { // format atc1441
              memcpy(&srvdata.mac, &serviceData[4], 6);
              Serial.printf("MAC: ");
              printBuffer(srvdata.mac, 6);
              int16_t x = (serviceData[10] << 8) | serviceData[11];
              srvdata.temp = x / 10.0;
              srvdata.humidity = serviceData[12];
              srvdata.en = true;
              srvdata.bat = serviceData[13];
              srvdata.vbat = (serviceData[14] << 8) | serviceData[15];
              srvdata.cnt = serviceData[16];
              Serial.printf("Temp: %.1f°, Humidity: %.0f%%, Vbatt: %d, Battery: %d%%, cout: %d\n", srvdata.temp, srvdata.humidity, srvdata.vbat, srvdata.bat, srvdata.cnt);
            }
          }
          pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
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

void Scan() {
  pBLEScan->setInterval(125);
  pBLEScan->setWindow(125);
  pBLEScan->setActiveScan(false);
  pBLEScan->start(5, false);
}
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 15 seconds.
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  Serial.println("Start scan (5 sec).");
  Scan();
}

void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect) {
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
    Serial.printf("Time since boot: %u. New scan.\n", (uint32_t)(millis() / 1000));
    Scan();
    if (srvdata.en) {
      srvdata.en = false;
      if (srvdata.old_cnt != srvdata.cnt) {
        srvdata.old_cnt = srvdata.cnt;
        uint8_t blk[7];
        int16_t tm = srvdata.temp * 10.0;
        uint16_t hm = srvdata.humidity;
        blk[0] = 0x22;
        blk[1] = tm;
        blk[2] = tm >> 8;
        blk[3] = hm;
        blk[4] = hm >> 8;
        blk[5] = 60;
        blk[6] = 0xA0;
        Serial.printf("New Data to LCD: Temp: %.1f°, Humidity: %.0f%% : ", srvdata.temp, srvdata.humidity);
        printBuffer(blk, sizeof(blk));
        pRemoteCharacteristic->writeValue(blk, sizeof(blk));
      }
    }
    delay(10);
  } else {
    delay(1000); // Delay a second between loops.
    Serial.println("Start new scan 5 sec.");
    Scan();
  }
}
