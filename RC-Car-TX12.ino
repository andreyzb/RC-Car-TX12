#include "sbus.h"
#include <Streaming.h>
#include <Servo.h>

#define FW 3
#define BW 5
#define SW 4
#define HL_PIN 12
#define BL_PIN 11


/* SbusRx object on Serial */
SbusRx sbus_rx(&Serial);
Servo servo1;

void setup() {
  servo1.attach(SW);
  pinMode(FW, OUTPUT);
  pinMode(BW, OUTPUT);
  pinMode(HL_PIN, OUTPUT);
  pinMode(BL_PIN, OUTPUT);
  stop();
  while (!Serial) {}
  /* Begin the SBUS communication */
  sbus_rx.Begin();
}

void loop() {
  if (sbus_rx.Read()) {
    if (sbus_rx.failsafe() == 0x08){
      stop();
      return;
    }
    int throttle = sbus_rx.rx_channels()[1];
    int steering = sbus_rx.rx_channels()[3];
    int HL = sbus_rx.rx_channels()[5];
    int BL = sbus_rx.rx_channels()[6];
    
    if (throttle > 1100) {
        digitalWrite(BW, LOW);
        analogWrite(FW, map(throttle, 1100, 1810, 0, 255));
    } else if (throttle < 900) {
        digitalWrite(FW, LOW);
        analogWrite(BW, map(throttle, 172, 900, 255, 0));
    }

    if (HL > 200) {
      digitalWrite(HL_PIN, HIGH);
    } else {
      digitalWrite(HL_PIN, LOW);
    }

    if (BL > 200) {
      digitalWrite(BL_PIN, HIGH);
    } else {
      digitalWrite(BL_PIN, LOW);
    }
   
    servo1.writeMicroseconds(steering + 580);
    
  }
}

void stop(){
  digitalWrite(BW, LOW);
  digitalWrite(FW, LOW);
}

long readVcc() { 
  long result; // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; 
  result = 1126400L / result; // Back-calculate AVcc in mV 
  return result;
}

//void loop() { Serial.println( readVcc(), DEC ); delay(1000); }
