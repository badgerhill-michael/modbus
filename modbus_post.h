typedef struct gas_flow {
  float dv;
  float v;
} gas_flow;

typedef struct ph {
  float T;
  float ph;
} ph;

typedef struct humidity {
  float T;
  float humidity;
} humidity;

typedef struct fork_info {
  float wet;
  float f;
  float T;
} fork_info;

typedef struct temp_info {
  float T;
  float SP;
  float OUT1;
  bool   Alarm1;
  int   modbus_id;
} temp_info;

typedef struct devil_info {
  float T;
  float rho;
  float d_viscosity;
  float k_viscosity;
  float humidity;
} devil_info;

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

/*
void get_msg_stats() {
    uint8_t         result;
    uint16_t        addr, qty, data[4];
    char            err[256];

    addr = 9007;
    qty = 4;
    result = get_modbus_data4( nodeSG, data, addr-1, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
        sprintf( msg, "GATEWAY, %s, %i, %s, %i, %s, %i, t, %li", "Received", data[0], "Corrupt", data[1], "Sent", data[3], Time.now() );
     	log_msg( msg );
    }
    else {
        err_count++;
        get_modbus_error( nodeSG, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}
 */
