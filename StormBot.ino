/*
Storm Drain Robot
Copyright 2015  David Perkinson and Kyle Perkinson
 */
#include <XBee.h>

int leftMotorProtocolId = 101;
int leftMotorEnablePin = 8;
int leftMotorFirstPin = 11;
int leftMotorSecondPin = 3;
int leftMotorCurrentSpeed = 0;
int leftMotorTargetSpeed = 0;

int rightMotorProtocolId = 102;
int rightMotorEnablePin = 12;
int rightMotorFirstPin = 9;
int rightMotorSecondPin = 10;
int rightMotorCurrentSpeed = 0;
int rightMotorTargetSpeed = 0;

XBee xbee;
XBeeResponse response;
// create reusable response objects for responses we expect to handle 
Rx16Response rx16;
Rx64Response rx64;
uint8_t option = 0;
uint8_t data = 0;

void setup() {                
  xbee = XBee();
  response = XBeeResponse();
  // create reusable response objects for responses we expect to handle 
  rx16 = Rx16Response();
  rx64 = Rx64Response();
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(leftMotorEnablePin, OUTPUT);     
  pinMode(leftMotorFirstPin, OUTPUT);
  pinMode(leftMotorSecondPin, OUTPUT);
  pinMode(rightMotorEnablePin, OUTPUT);     
  pinMode(rightMotorFirstPin, OUTPUT);
  pinMode(rightMotorSecondPin, OUTPUT);
  setPwmFrequency(leftMotorFirstPin, 8);  // change Timer2 divisor to 8 gives 3.9kHz PWM freq
  setPwmFrequency(rightMotorFirstPin, 8);  // change Timer2 divisor to 8 gives 3.9kHz PWM freq
  enableLeftMotor();
  enableRightMotor();
  leftMotorSetTargetSpeed(255);
  rightMotorSetTargetSpeed(255);
  //printf("init complete\n");
  //Serial.begin(9600);
  delay(5000);
}

void loop() 
{
  getMotorDesiredSpeedsFromXBee();
  checkMotorSpeedLimits();
  leftMotorCurrentSpeed = adjustLeftMotorSpeed(leftMotorCurrentSpeed,leftMotorTargetSpeed);
  rightMotorCurrentSpeed = adjustRightMotorSpeed(rightMotorCurrentSpeed,rightMotorTargetSpeed);
  //printMotorDebug();
  delay(10);
}

void checkMotorSpeedLimits()
{
    if ((leftMotorTargetSpeed <= -255) || (leftMotorTargetSpeed > 255))
    {
      leftMotorTargetSpeed = 0;
    }
    if ((rightMotorTargetSpeed <= -255) || (rightMotorTargetSpeed > 255))
    {
      rightMotorTargetSpeed = 0;
    }
}

void printMotorDebug()
{
  if (leftMotorCurrentSpeed != leftMotorTargetSpeed)
  {
    //Serial.println(leftMotorCurrentSpeed);
  }
  if (rightMotorCurrentSpeed != rightMotorTargetSpeed)
  {
    //Serial.println(rightMotorCurrentSpeed);
  }
}

void getMotorDesiredSpeedsFromXBee() 
{
    uint8_t payload[4]; 
    bool result = false;
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) 
    {
      // got something
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) 
      {
        // got a rx packet
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
        {
                xbee.getResponse().getRx16Response(rx16);
                option = rx16.getOption();
                payload[0] = rx16.getData(0);
                payload[1] = rx16.getData(1);
                payload[2] = rx16.getData(2);
                payload[3] = rx16.getData(3);
        } 
        else 
        {
                xbee.getResponse().getRx64Response(rx64);
                payload[0] = rx64.getData(0);
                payload[1] = rx64.getData(1);
                payload[2] = rx64.getData(2);
                payload[3] = rx64.getData(3);
        }
        uint8_t *payloadPointer = payload;
        leftMotorTargetSpeed = *(int *)payloadPointer;
        payloadPointer += 2;
        rightMotorTargetSpeed = *(int *)payloadPointer;
      } 
      else 
      {
        // the response was not RX16 or RX64
        //Serial.println("XBee API error");
      }
    } 
    else if (xbee.getResponse().isError()) 
    {
      // possibly print error here
      //Serial.println("XBee Response error");
    } 
    else
    {
      //Serial.println("XBee no message");
    }
}
 

void leftMotorSetTargetSpeed(int speed)
{
  leftMotorTargetSpeed = speed;
}
void rightMotorSetTargetSpeed(int speed)
{
  rightMotorTargetSpeed = speed;
}
void enableLeftMotor()
{
    digitalWrite(leftMotorEnablePin, HIGH);
}
void enableRightMotor()
{
    digitalWrite(rightMotorEnablePin, HIGH);
}
void disableLeftMotor()
{
    digitalWrite(leftMotorEnablePin, LOW);
}
int adjustLeftMotorSpeed(int currentSpeed, int targetSpeed)
{
  currentSpeed = calculateNextSpeed(currentSpeed,targetSpeed);
  setLeftMotorSpeed(currentSpeed);
  return currentSpeed;
}
int adjustRightMotorSpeed(int currentSpeed, int targetSpeed)
{
  currentSpeed = calculateNextSpeed(currentSpeed,targetSpeed);
  setRightMotorSpeed(currentSpeed);
  return currentSpeed;
}
int calculateNextSpeed(int currentSpeed, int targetSpeed)
{
  if (currentSpeed < targetSpeed)
  {
    currentSpeed = currentSpeed + 1;
  }
  if (currentSpeed > targetSpeed)
  {
    currentSpeed = currentSpeed - 1;
  }
  return currentSpeed;
}
void setLeftMotorSpeed(int currentSpeed)
{
  if (currentSpeed > 0)
  {
    leftMotorForwardSpeed(currentSpeed);
  }
  else
  {
    leftMotorBackwardSpeed(-currentSpeed);
  }
}
void setRightMotorSpeed(int currentSpeed)
{
  if (currentSpeed > 0)
  {
    rightMotorForwardSpeed(currentSpeed);
  }
  else
  {
    rightMotorBackwardSpeed(-currentSpeed);
  }
}

// To drive the motor in H-bridge mode
// the power chip inputs must be opposite polarity
// and the Enable input must be HIGH
void leftMotorForwardSpeed(int speed)
{
  analogWrite(leftMotorFirstPin, speed);
  analogWrite(leftMotorSecondPin, 0);
}
void rightMotorForwardSpeed(int speed)
{
  analogWrite(rightMotorFirstPin, speed);
  analogWrite(rightMotorSecondPin, 0);
}
void leftMotorBackwardSpeed(int speed)
{
    analogWrite(leftMotorFirstPin, 0);
    analogWrite(leftMotorSecondPin, speed);
}
void rightMotorBackwardSpeed(int speed)
{
    analogWrite(rightMotorFirstPin, 0);
    analogWrite(rightMotorSecondPin, speed);
}


/*
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired (Timer0)
 *   - Pins 9 and 10 are paired (Timer1)
 *   - Pins 3 and 11 are paired (Timer2)
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 5, 6 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
 
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) { // Timer0 or Timer1
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) { 
      TCCR0B = TCCR0B & 0b11111000 | mode; // Timer0
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode; // Timer1
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode; // Timer2
  }
}

