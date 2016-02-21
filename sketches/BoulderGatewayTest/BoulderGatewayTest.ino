#include "BoulderManager.h"

#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>

#include <SPI.h>
#include <MySensor.h>

//Boulder Manager Vars
BoulderManager mBoulderManager;

//Hardware Vars
MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW); // NRFRF24L01 radio driver
MyHwATMega328 hw; // Hardware profile

//Gateway Vars
MySensor gw(transport, hw);

void onIncomingMessageEvent(const MyMessage &message) {

  //Num Sensor Armed Event
  if (message.type == V_ARMED) {

    Serial.println();
    Serial.print("Boulder: ");
    Serial.print(message.sender);
    Serial.print(" Sensor: ");
    Serial.print(message.sensor);
    Serial.print(" Armed: ");
    Serial.println(String(message.getInt()));

    mBoulderManager.setBoulderSensorArmed(message.sender, message.sensor, message.getInt());

    Serial.print("Num Armed Sensors: ");
    Serial.println(mBoulderManager.getNumArmedSensors(message.sender));
  }

  //Num Sensor Motion Detected Event
  if (message.type == V_TRIPPED) {

    Serial.println();
    Serial.print("Boulder: ");
    Serial.print(message.sender);
    Serial.print(" Sensor: ");
    Serial.print(message.sensor);
    Serial.print(" Tripped: ");
    Serial.println(String(message.getInt()));

    mBoulderManager.setBoulderSensorTripped(message.sender, message.sensor, message.getInt());

    Serial.print("Num Tripped Sensors: ");
    Serial.println(mBoulderManager.getNumTrippedSensors(message.sender));

    //Send Color
    mBoulderManager.setBoulderColorActivity(message.sender);
   
    String color = mBoulderManager.getBoulderHexColor(message.sender);

    MyMessage colorMsg(0, V_RGB);
    colorMsg.setDestination(message.sender);
    gw.send(colorMsg.set(color.c_str()));
  }
}

void setup()
{
  //Config Gateway
  gw.begin(onIncomingMessageEvent, 0, true, 0);
}

void loop()
{
  //Update Gateway
  gw.process();
}





