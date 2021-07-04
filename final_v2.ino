#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include <User_Setup.h>
#include <User_Setups/Setup25_TTGO_T_Display.h> //her defe tft_eSPI yukledikce mutleq get commentden cixart
#include<stdio.h>
#include<string.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <SimpleTimer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library


#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 27

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (RGB+W NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(60,32, PIN,
  NEO_MATRIX_LEFT     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

String sarray="";

using namespace std;
#include "DHT.h"

#define DHTPIN 22 //dht pin
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define led 13//relay pin
#define mot 17 //motion sensor pin
const int buzzer = 15; //buzzer pin
////////////////////////////////////////bluetooth///////////////////
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c3319154" 
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b2669"

SimpleTimer Timer;
long f2 = 0;
bool releStatus = false ;                                                ///true --> LOW -->yandi    //false--> HIGH --> sondu
bool timeStatus = 0;

////////////////////////////dht11 with relayStatus method/////////
void dht11(bool Status) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  else {
    String str = "";
    str += t;
    str += ",";
    str += h;
    str += ",";
    str += Status;
    pCharacteristic->setValue((char*)str.c_str());
    pCharacteristic->notify();

    Serial.println(h);
    Serial.println(F(" %  Temperature: "));
    Serial.println(t);
  }

}
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks:
  public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("**********  ");
        Serial.print("New value: ");
        if (value.length() > 0) {
          string s = value;

          //       gelen datani ekrana cxartmaqcun
         

          int n = s.length();
          char char_array[n + 1];
          strcpy(char_array, s.c_str());
          for (int i = 0; i < n; i++) {
            Serial.print(char_array[i]);
            sarray = sarray + char_array[i];
          }
           for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);

          } sarray=char_array;
          Serial.print(String("arrayyyyyyyyyyy")+sarray);
          //  printf("float value : %1.0f\n" , (float) atoll(char_array));
          f2 = atoll(char_array);
          Serial.print("long  ");
          Serial.println(f2);

          //turnOff(f2);
        }
        if (value == "a") {
          Serial.println ("Yandi");
          releStatus = true;
          digitalWrite(led, LOW);                                                         //+++++++++++++++++++++++++++++++++++++++++++++++++++                                                  
        }
        if (value == "b") {
          Serial.println ("Sondu");
          digitalWrite(led, HIGH);                                                       // ***************************************************
          releStatus = false;
        }
       
        Serial.println();
        Serial.println("**********");
        dht11(releStatus);

      }
      dht11(releStatus);
    }

};

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(mot, INPUT);
  digitalWrite(led, HIGH);                                                      // ***************************************************

  //////////////////////tft screen/////////////////////////
  
  tft.init();
  tft.setRotation(0);
      //////////////////////////////////////////////////////////////////tft screen//////////////////
    
 tft.fillScreen(TFT_WHITE);
// tft.setFreeFont(&Orbitron_Light_32);
   tft.setFreeFont(&FreeSans12pt7b);
  tft.setCursor(55, 100);//1UZUNLUQ , 2HUNDURLUK
  tft.setTextColor(TFT_BLUE); 
    tft.print("System started"); 
  //////////////////////buzzer/////////////////////////
  ledcSetup(0, 1E5, 12);
  ledcAttachPin(15, 0);
  // Create the BLE Device
  BLEDevice::init("GANJA FOOD");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");


  Serial.println(F("DHTxx test!"));

  dht.begin();


  
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(120);
  matrix.setTextColor(colors[0]);
}

int x    = matrix.width();
int pass = 0;

void buzzerr() {
  ledcWriteTone(0, 800);
  delay(1000);
  uint8_t octave = 1;
  for (octave; octave < 255; octave ++) {
    ledcWriteNote(0, NOTE_C, octave);

  }
}

bool statuSender() {

}
void loop() {
      //////////////////////////////////////////////////////////////////tft screen//////////////////
    
 tft.fillScreen(TFT_WHITE);
 //tft.setFreeFont(&Orbitron_Light_32);
 
    tft.setFreeFont(&FreeSans12pt7b);
  tft.setCursor(55, 100);//1UZUNLUQ , 2HUNDURLUK
  tft.setTextColor(TFT_BLUE); 
    tft.print("System running"); 
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    //////////////////////////////////////////////////////////////////tft screen//////////////////
    
 tft.fillScreen(TFT_WHITE);
 //tft.setFreeFont(&Orbitron_Light_32);
    tft.setFreeFont(&FreeSans12pt7b);
  tft.setCursor(55, 100);//1UZUNLUQ , 2HUNDURLUK
  tft.setTextColor(TFT_BLUE); 
    tft.print("Device Connected"); 
  
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
    dht11(releStatus);
  }
    ///////////////////////////timer/////////////////////
    Timer.setInterval(f2);
  Serial.print("timer f2 : ");

  Serial.println(f2);

  if (f2 > 1000) {
    timeStatus == 1;
    releStatus = true;
    digitalWrite(led, LOW);                                                          //+++++++++++++++++++++++++++++++++++++++++++++++++++
    Serial.print("ikinci f    // *****************  ");
   if (Timer.isReady()) {
      Serial.print("vaxt tamam oldu ");
      releStatus = false;
      digitalWrite(led, HIGH);                                                       // ***************************************************
      Timer.reset();
      timeStatus == 0;//
      f2=0;
    }
    
//    if (millis() % f2 == 0) {
//
//      Serial.print("Led high low : ");
//      releStatus = false;
//      digitalWrite(led, HIGH);   //SO:N
//      delay(1000);
//
//    }
  }
/////////////////////////////////   motion sensors  ///////////////////////////
  if (digitalRead(mot)) {
    Serial.print("hereket statusu: ");
    Serial.println(digitalRead(mot));
    releStatus = false;
  digitalWrite(led, HIGH);                                                      // ***************************************************
    buzzerr();
  }
  else {
    ledcWriteTone(0, 0); ///buzzer sondurmek
    if (timeStatus == 1) {
      releStatus = true;
      digitalWrite(led, LOW);                                                          //+++++++++++++++++++++++++++++++++++++++++++++++++++
     timeStatus == 0;
    }
    else {
    //  releStatus = false;
       Serial.print("Bleeeeeeeeeeeeet ");
     // digitalWrite(led, HIGH);                                                      // ***************************************************
    }
  }

  Serial.print("releStatus: ");
  Serial.println(releStatus);

   //delay(2000);
  dht11(releStatus);

    matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(sarray);
  
          Serial.print(String("arrayyyyyyyyyyy:  ")+sarray+String("   \n"));
  if(--x < -390) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);
}
