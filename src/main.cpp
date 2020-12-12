#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

const int stepPin = 3; 
const int dirPin = 4; 
const int ledPin = 7;
boolean isBusy = false;
boolean isUp = true;

void setup() {
  Serial.begin(9600);

  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.println("Toilet lid controller");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  Serial.println(F("Toilet seat controller is ready\n\n"));
}

void setBusy() {
  isBusy = true;
  digitalWrite(ledPin, HIGH);
}

void setNotBusy() {
  isBusy = false;
  digitalWrite(ledPin, LOW);
}

int heavy_duration = 500;
int start_duration = 200;
int end_duration = 80;
int total_steps = 11000;

void up() {
  digitalWrite(dirPin, LOW);

  int duration_diff = start_duration - end_duration;

  for(int x = 0; x < total_steps; x++) {
    int steps_left = total_steps - x;
    double left_ratio = steps_left / (double) total_steps;

    int duration = end_duration + left_ratio * (double) duration_diff;
    if (left_ratio > 0.95) {
      duration = heavy_duration;
    } 

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(duration);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(duration);
  }
}

void down() {
  digitalWrite(dirPin, HIGH);

  int duration_diff = start_duration - end_duration;

  for(int x = 0; x < total_steps; x++) {
    int steps_left = total_steps - x;
    double left_ratio = steps_left / (double) total_steps;

    int duration = end_duration + (1.0 - left_ratio) * (double) duration_diff;
    if (left_ratio < 0.05) {
      duration = heavy_duration;
    }

    digitalWrite(stepPin,HIGH);
    delayMicroseconds(duration);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(duration);
  }
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
 // Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4 && measure.RangeMilliMeter < 1500 && !isBusy) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);

    setBusy();
    if (isUp) {
      Serial.println(F("GOING UP"));
      up();  
    } else {
      Serial.println(F("GOING DOWN"));
      down();
    }
    setNotBusy();
    Serial.println(F("FINISHED"));

    isUp = !isUp;
  }
    
  delay(50);
}