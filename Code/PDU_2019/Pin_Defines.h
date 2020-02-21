#define SPI_CS_PIN 9
#define PDU_IN_ID 80
#define PDU_OUT_ID 81
#define canDelay 100

#define AMS_STATUS_PIN A6
#define IMD_STATUS_PIN A7
 
#define BRAKE_LIGHT_BYTE 0
#define BRAKE_LIGHT_BIT 0
#define BRAKE_LIGHT_PIN 7

#define FUEL_PUMP_BYTE 0
#define FUEL_PUMP_BIT 1
#define FUEL_PUMP_PIN 5

#define STARTER_BYTE 0
#define STARTER_BIT 5
#define STARTER_PIN 8

#define SHIFT_UP_BYTE 0
#define SHIFT_UP_BIT 6
#define SHIFT_UP_PIN 4

#define SHIFT_DN_BYTE 0
#define SHIFT_DN_BIT 7
#define SHIFT_DN_PIN 3

#define FAN_BYTE 2
#define FAN_BIT 0
#define FAN_LEN 8 //8 bits for fan pwm
#define FAN_PIN 6

#define COCKPIT_BRB_STATUS_BYTE 1
#define COCKPIT_BRB_STATUS_BIT 3

// not currently used
#define SPARE_RBRB_BYTE 1
#define SPARE_RBRB_BIT 2
#define SPARE_RBRB_PIN 10

#define MASK_1 0x1
#define BIT_2_MASK 0x2
#define BIT_3_MASK 0x4
#define BIT_4_MASK 0x8
#define BIT_5_MASK 0x10
#define BIT_6_MASK 0x20
#define BIT_7_MASK 0x40
#define BIT_8_MASK 0x80
