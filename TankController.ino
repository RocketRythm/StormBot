#include <XBee.h>

int leftTrackPotPin = 1;
int leftTrackPotValue = 0;
int leftTrackDesiredSpeed = 0;
int rightTrackPotPin = 0;
int rightTrackPotValue = 0;
int rightTrackDesiredSpeed = 0;
int leftMotorProtocolId = 101;
int rightMotorProtocolId = 102;
XBee xbee;
uint8_t payload[4];
Tx16Request tx;
TxStatusResponse txStatus;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  xbee = XBee();
  tx = Tx16Request(0x3377, payload, sizeof(payload));
  txStatus = TxStatusResponse();
}

void loop() {
  // put your main code here, to run repeatedly:
  leftTrackPotValue = analogRead(leftTrackPotPin);
  leftTrackDesiredSpeed = map(leftTrackPotValue,0,1023,-255,255);
  rightTrackPotValue = analogRead(rightTrackPotPin);
  rightTrackDesiredSpeed = map(rightTrackPotValue,0,1023,-255,255);
  sendSpeedsToTank();
  delay(100);  // sleep for 1 second (1000 milliseconds)
}

void printDebug()
{
  Serial.print(leftTrackPotValue);
  Serial.print(" ");
  Serial.print(leftTrackDesiredSpeed);
  Serial.print(" ");
  Serial.print(rightTrackPotValue);
  Serial.print(" ");
  Serial.println(rightTrackDesiredSpeed);
}

void sendSpeedsToTank()
{
  payload[0] = leftTrackDesiredSpeed >> 8 & 0xff;
  payload[1] = leftTrackDesiredSpeed & 0xff;
  payload[2] = rightTrackDesiredSpeed >> 8 & 0xff;
  payload[3] = rightTrackDesiredSpeed & 0xff;
  xbee.send(tx);
  // after sending a tx request, we expect a status response
  // wait up to 1 seconds for the status response
  if (xbee.readPacket(1000)) 
  {
    // got a response!
    // should be a znet tx status                   
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) 
    {
      xbee.getResponse().getTxStatusResponse(txStatus);
      // get the delivery status, the fifth byte
      if (txStatus.getStatus() == SUCCESS) 
      {
        // success.  time to celebrate
        //flashLed(statusLed, 5, 50);
      } 
      else 
      {
        // the remote XBee did not receive our packet. is it powered on?
        //flashLed(errorLed, 3, 500);
        //Serial.println("XBee tank did not rx pkt");
      }
    }      
  } 
  else if (xbee.getResponse().isError()) 
  {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
    // or flash error led
    //Serial.println("XBee getResponse error");
  } 
  else 
  {
    // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
    //flashLed(errorLed, 2, 50);
    //Serial.println("XBee timeout");
  }
}

