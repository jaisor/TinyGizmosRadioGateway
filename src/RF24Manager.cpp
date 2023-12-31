#if !( defined(ESP32) ) && !( defined(ESP8266) )
  #error This code is intended to run on ESP8266 or ESP32 platform!
#endif

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <ArduinoLog.h>

#define CE_PIN  GPIO_NUM_22
#define CSN_PIN GPIO_NUM_21

#include <Arduino.h>
#include <Time.h>

#include "RF24Manager.h"
#include "Configuration.h"
#include "BaseMessage.h"

const uint8_t address[7] = "mynode";

CRF24Manager::CRF24Manager() {  
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  Log.infoln("Radio begin successfully: %t", _radio->begin());
  _radio->setDataRate(RF24_250KBPS);
  _radio->setPALevel(RF24_PA_LOW);
  _radio->setAddressWidth(7);
  _radio->setAutoAck(false);
  _radio->setPayloadSize(sizeof(float));
  _radio->disableDynamicPayloads();
  _radio->openReadingPipe(0, address);
  _radio->startListening();

  Log.infoln("Radio listening...");
  Log.noticeln("  Channel: %i", _radio->getChannel());
  Log.noticeln("  PayloadSize: %i", _radio->getPayloadSize());
  Log.noticeln("  DataRate: %i", _radio->getDataRate());
  Log.noticeln("  isPVariant: %t", _radio->isPVariant());

  _radio->printDetails();
  
  _queue.push_back(new CBaseMessage(String("test")));
}

CRF24Manager::~CRF24Manager() { 
  delete _radio;
  Log.noticeln("CRF24Manager destroyed");
}

void CRF24Manager::loop() {
  if (_radio->available()) {
    digitalWrite(INTERNAL_LED_PIN, HIGH);
    _radio->read( &_data, sizeof(_data) );
    Log.infoln("Received: '%s'", _data);
    _queue.push_back(new CBaseMessage(String(_data)));
    delay(100);
    digitalWrite(INTERNAL_LED_PIN, LOW);
  }
}