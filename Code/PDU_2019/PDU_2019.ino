#include "mcp_can.h"

#include "Pin_Defines.h"

MCP_CAN CAN(SPI_CS_PIN);

void set_outputs(byte len, byte* buf)
{
  
  // Set each digital
  if(buf[BRAKE_LIGHT_BYTE]>>BRAKE_LIGHT_BIT&MASK_1) Serial.println("Brake Activated!");
  digitalWrite(BRAKE_LIGHT_PIN, buf[BRAKE_LIGHT_BYTE]>>BRAKE_LIGHT_BIT&MASK_1);

  if(buf[FUEL_PUMP_BYTE]>>FUEL_PUMP_BIT&MASK_1) Serial.println("Fuel Pump Activated!");
  digitalWrite(FUEL_PUMP_PIN, buf[FUEL_PUMP_BYTE]>>FUEL_PUMP_BIT&MASK_1);

  if(buf[MS_BYTE]>>MS_BIT&MASK_1) Serial.println("Microsquirt Activated!");
  digitalWrite(MS_PIN, buf[MS_BYTE]>>MS_BIT&MASK_1);
  
  if(buf[BRAKE_SOL_BYTE]>>BRAKE_SOL_BIT&MASK_1) Serial.println("Brake Solenoid Activated!");
  digitalWrite(BRAKE_SOL_PIN, buf[BRAKE_SOL_BYTE]>>BRAKE_SOL_BIT&MASK_1);

  if(buf[STARTER_BYTE]>>STARTER_BIT&MASK_1) Serial.println("Starter Activated!");
  digitalWrite(STARTER_PIN, buf[STARTER_BYTE]>>STARTER_BIT&MASK_1);

  //if(buf[SHIFT_UP_BYTE]>>SHIFT_UP_BIT&MASK_1) Serial.println("Upshift Activated!");
  //digitalWrite(SHIFT_UP_PIN, buf[SHIFT_UP_BYTE]>>SHIFT_UP_BIT&MASK_1);

  //if(buf[SHIFT_DN_BYTE]>>SHIFT_DN_BIT&MASK_1) Serial.println("Downshift Activated!");
  //digitalWrite(SHIFT_DN_PIN, buf[SHIFT_DN_BYTE]>>SHIFT_DN_BIT&MASK_1);

  if(buf[SPARE_CPBRB_BYTE]>>SPARE_CPBRB_BIT&MASK_1) Serial.println("Spare CPBRB Activated!");
  digitalWrite(SPARE_CPBRB_PIN, buf[SPARE_CPBRB_BYTE]>>SPARE_CPBRB_BIT&MASK_1);

  // Set the engine fan
  if(buf[2] > 0) {
    Serial.println("Fan Activated");
    Serial.println(buf[2]);
    int fan_pwm = 0.70*buf[2]; // reduce given value to 80 percent
    analogWrite(ENGINE_FAN_PIN, fan_pwm);
  } else {
    digitalWrite(ENGINE_FAN_PIN, 0); //For pin 6, analog write may fully turn off fan
  }
  
}

void setup(){

  // Uncomment below line if you are debugging and need to see what is being received. Note this may keep the downshift and brake light on beacuse they're on the TX/RX pins respectively
  // Serial.begin(9600);
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
  CAN.init_Filt(1, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
  CAN.init_Filt(2, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
  CAN.init_Filt(3, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
  CAN.init_Filt(4, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
  CAN.init_Filt(5, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
  CAN.init_Filt(0, 0, CAN_REAR_POWER_DIST_MSG_ADDRESS);
}

void loop(){

  // If there is a message available in the buffer
  while (CAN_MSGAVAIL == CAN.checkReceive())
  {
    
    // Create a buffer and insert the three byte message into it
    byte buf[8];
    byte len = 0;
    CAN.readMsgBuf(&len, buf);

    // Set the outputs of the board given the CAN message
    set_outputs(len, buf);
    
  }
  
}
