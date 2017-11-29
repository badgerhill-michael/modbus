typedef struct gas_flow {
  float dv;
  float v;
} gas_flow;

typedef struct fork_info {
  float wet;
  float f;
  float T;
} fork_info;

typedef struct temp_info {
  float T;
  float SP;
  float OUT1;
//  int   Alarm1;
  int   modbus_id;
} temp_info;

typedef struct emerson_data {
    long    t;              // time in seconds since the epoch
    float   cltp;           // volume of cold liquor tank in InchesOfH2O
    float   cltt;           // temp of cold liquor tank in ˚F
    float   hltp;           // volume of hot liquor tank in InchesOfH2O
    float   hltt;           // temp of hot liquor tank in ˚F
    float   fvp;            // 2" pressure gauge
    float   fvt;            // 2" pressure gauge
    float   mflowdv;        // Mag flow meter flow rate
    float   mflowv;         // Mag flow meter totalized flow
    char    datestring[64]; //Time from the gateway
} emerson_data;

gas_flow get_gas_flow();
fork_info get_fork_info();
temp_info get_temp_info();

LEDStatus normal( RGB_COLOR_GREEN,  LED_PATTERN_FADE,  LED_SPEED_NORMAL );
LEDStatus error(  RGB_COLOR_YELLOW, LED_PATTERN_BLINK, LED_SPEED_SLOW   );
LEDStatus fail(   RGB_COLOR_RED,    LED_PATTERN_BLINK, LED_SPEED_FAST   );
