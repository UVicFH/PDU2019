#include "mcp_can.h"

#include "Pin_Defines.h"

MCP_CAN CAN(SPI_CS_PIN);

int AMS_STATUS = 0;
int IMD_STATUS = 0;

//Unsigned char acts as byte for message sending
unsigned char CANOut[8] = {0,0,0,0,0,0,0,0};

void set_outputs(byte len, byte* buf)
{
  
  // Set each digital
  if(buf[BRAKE_LIGHT_BYTE]>>BRAKE_LIGHT_BIT&MASK_1) Serial.println("Brake Activated!");
  digitalWrite(BRAKE_LIGHT_PIN, buf[BRAKE_LIGHT_BYTE]>>BRAKE_LIGHT_BIT&MASK_1);

  //Checks both fuel pump status and CBRB status to make sure fuel pump shuts off when CBRB is off
  if((buf[FUEL_PUMP_BYTE]>>FUEL_PUMP_BIT&MASK_1)&&(buf[COCKPIT_BRB_STATUS_BYTE]>>COCKPIT_BRB_STATUS_BIT&MASK_1)) Serial.println("Fuel Pump Activated!");
  digitalWrite(FUEL_PUMP_PIN, ((buf[FUEL_PUMP_BYTE]>>FUEL_PUMP_BIT&MASK_1)&&(buf[COCKPIT_BRB_STATUS_BYTE]>>COCKPIT_BRB_STATUS_BIT&MASK_1)));

  if(buf[STARTER_BYTE]>>STARTER_BIT&MASK_1) Serial.println("Starter Activated!");
  digitalWrite(STARTER_PIN, buf[STARTER_BYTE]>>STARTER_BIT&MASK_1);

  if(buf[SHIFT_UP_BYTE]>>SHIFT_UP_BIT&MASK_1) Serial.println("Upshift Activated!");
  digitalWrite(SHIFT_UP_PIN, buf[SHIFT_UP_BYTE]>>SHIFT_UP_BIT&MASK_1);

  if(buf[SHIFT_DN_BYTE]>>SHIFT_DN_BIT&MASK_1) Serial.println("Downshift Activated!");
  digitalWrite(SHIFT_DN_PIN, buf[SHIFT_DN_BYTE]>>SHIFT_DN_BIT&MASK_1);

//*******SPARE SET TO ALWAYS BE ON, TO CHANGE UNCOMMENT LINES BELOW (SEE LINE 88 ASWELL)*******
//  if(buf[SPARE_RBRB_BYTE]>>SPARE_RBRB_BIT&MASK_1) Serial.println("Spare RBRB Activated!");
//  digitalWrite(SPARE_RBRB_PIN, buf[SPARE_RBRB_BYTE]>>SPARE_RBRB_BIT&MASK_1);


//Non PWM Fan activation for testing purposes
  if(buf[FAN_BYTE]>30) {
    Serial.println("Fan Activated!");
    digitalWrite(FAN_PIN, 1);
  } else if (buf[FAN_BYTE] < 1)  {
    digitalWrite(FAN_PIN, 0);
  }

//  // Set the engine fan with PWM
//  if(buf[FAN_BYTE]>0) {
//    Serial.println("Fan Activated");
//    Serial.println(buf[2]);
//    int fan_pwm = buf[FAN_BYTE]; // reduce given value to 80 percent
//    analogWrite(FAN_PIN, fan_pwm);
//  } else {
//    digitalWrite(FAN_PIN, 0); //For pin 6, analog write may fully turn off fan
//  }
  
}

//returns IMD status
int getIMDStatus(){
  if(analogRead(IMD_STATUS_PIN) >= 600){
      return 1;
    }
  return 0;
}

//returns AMS status
int getAMSStatus(){
  if(analogRead(AMS_STATUS_PIN) >= 600){
    return 1;
    }
  return 0;
}

//Creates message to send based on AMS/IMD status
void createMessage(int AMS, int IMD, unsigned char (&message)[8]){
  //set AMS IMD Flag bytes based on their current status
    if(AMS&&IMD){
      message[6] = 1;
      message[7] = 1;
    }
    else if(AMS){
      message[6] = 1;
      message[7] = 0;
    }
    else if(IMD){
      message[7] = 1;
      message [6] = 0; 
    }
    else{
      message[6] = 0;
      message[7] = 0;
    }
  }

void setup(){

  // Uncomment below line if you are debugging and need to see what is being received. Note this may keep the downshift and brake light on beacuse they're on the TX/RX pins respectively
  Serial.begin(9600);
  SPI.begin();
  
  // Create an infinite loop to prevent the program from starting before CAN is established
  for(;;)
  {
    if(CAN_OK == CAN.begin(CAN_500KBPS))
    {
      Serial.println("CAN BUS INIT GOOD");
      break;
    }
    else
    {
      Serial.println("CAN BUS INIT FAIL, RETRY");
      delay(100);
    }
  }

  // Set both masks to check all digits (compare the entire ID to the filter) of the arbitration IDs
  CAN.init_Mask(0, 0, 0xFFF);
  CAN.init_Mask(1, 0, 0xFFF);

  // Set all filters to only accept the desired arbitration ID
  CAN.init_Filt(1, 0, PDU_IN_ID);
  CAN.init_Filt(2, 0, PDU_IN_ID);
  CAN.init_Filt(3, 0, PDU_IN_ID);
  CAN.init_Filt(4, 0, PDU_IN_ID);
  CAN.init_Filt(5, 0, PDU_IN_ID);
  CAN.init_Filt(0, 0, PDU_IN_ID);
  Serial.println("setup done");

  //*******SPARE ALWAYS SET TO ON, TO CHANGE DELETE LINE BELOW (SEE LINE 33 ASWELL)*******
  digitalWrite(SPARE_RBRB_PIN, HIGH);

  pinMode(AMS_STATUS_PIN, INPUT);
  pinMode(IMD_STATUS_PIN, INPUT);
}

void loop(){

  //check AMS/IMD status and set message
  AMS_STATUS = getAMSStatus();
  IMD_STATUS = getIMDStatus();
  
  //set message
  createMessage(AMS_STATUS, IMD_STATUS, CANOut);
  //send out AMS/IMD status as 8 bit CANOut char array
  CAN.sendMsgBuf(PDU_OUT_ID, 0, 8, CANOut);
  
  // If there is a message available in the buffer
  if (CAN_MSGAVAIL == CAN.checkReceive())
  {
     Serial.println("message available");
    // Create a buffer and insert the three byte message into it
    byte buf[8];
    byte len = 0;
    CAN.readMsgBuf(&len, buf);

    // Set the outputs of the board given the CAN message
    set_outputs(len, buf);
  }
  
}
