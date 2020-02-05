#if ( PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION )
    #define THIS_DEVICE "electron"
#endif

#if ( PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION )
    #define THIS_DEVICE "photon"
#endif

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
  double SP;
  float OUT1;
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
  int   decimals;
} temp_info;

typedef struct devil_info {
  float T;
  float rho;
  float d_viscosity;
  float k_viscosity;
  float humidity;
} devil_info;

/*
    edata->t        = Time.now();
    edata->cltp     = convert_ints_to_float( &data[0] );
    edata->cltt     = convert_ints_to_float( &data[2] );
    edata->hltp     = convert_ints_to_float( &data[4] );
    edata->hltt     = convert_ints_to_float( &data[6] );
    edata->fvp      = convert_ints_to_float( &data[8] );
    edata->fvt      = convert_ints_to_float( &data[10] );
    edata->mflowdv  = convert_ints_to_float( &data[12] );
    edata->mflowv   = convert_ints_to_float( &data[14] );
    edata->mflowt   = convert_ints_to_float( &data[16] );
    edata->phyg1p   = convert_ints_to_float( &data[18] );
    edata->phyg1t   = convert_ints_to_float( &data[20] );
    edata->fork_wet = convert_ints_to_float( &data[22] );
    edata->fork_hz  = convert_ints_to_float( &data[24] );
    edata->fork_t   = convert_ints_to_float( &data[26] );
 */
 
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
    float   mflowt;         // Mag flow meter temp
    float   phyg1p;         // Mag flow meter temp
    float   phyg1t;         // Mag flow meter temp
    float   forkwet;        // Is the density fork wet?
    float   forkhz;         // Density fork frequency
    float   forkt;          // Density fork temp
    float   mflow2dv;       // Mag flow meter flow rate
    float   mflow2v;        // Mag flow meter totalized flow
    float   mflow2t;        // Mag flow meter temp
} emerson_data;

gas_flow get_gas_flow();
fork_info get_fork_info();
temp_info get_temp_info();

LEDStatus normal( 0x007D26CD,  LED_PATTERN_FADE,  LED_SPEED_NORMAL );
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


fork_info get_fork_info() {
	uint8_t         result;
	uint16_t        data[16];
    char            err[256];
    fork_info       new_reading;

    int qty = 6;
    result = get_modbus_data4( nodeSG, data, 0, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
        new_reading.wet = convert_ints_to_float(&data[0]);
        new_reading.f   = convert_ints_to_float(&data[2]);
        new_reading.T   = convert_ints_to_float(&data[4]);
    }
    else {
        err_count++;
        get_modbus_error( nodeSG, result, err );
        log_msg( String::format("Error: %i (%X): %s", result, result, err) );
    }
    return( new_reading );
}

devil_info get_devil_info( ModbusMaster node) {
    uint8_t         result;
    uint16_t        qty = 4,
                    addr;
    uint16_t        data[qty];
    char            err[256];
    devil_info      new_reading;

    new_reading.T           = -999.9;
    new_reading.rho         = -999.9;
    new_reading.d_viscosity = -999.9;
    new_reading.k_viscosity = -999.9;
    new_reading.humidity    = -999.9;
    
    addr = 104;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        sprintf( msg, "%i: %i (%0x), %i : %i (%0x)", addr, data[0], data[0], addr+2, data[1], data[1] );
        log_msg( msg );

        new_reading.T           = convert_ints_to_float(&data[0]);
//        new_reading.T   = (float)(data[0] * 65536 + data[1]);

           new_reading.rho         = convert_ints_to_float(&data[2]);
//        new_reading.d_viscosity = convert_ints_to_float(&data[6]);
//        new_reading.k_viscosity = convert_ints_to_float(&data[8]);
//        new_reading.humidity    = convert_ints_to_float(&data[10]);
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

void get_gateway_time( char* date_string ) {
    uint8_t         result;
    uint16_t        data[32];
    char            err[256];

    int qty = 8;
    result = get_modbus_data4( nodeSG, data, 9000, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
        sprintf( date_string, "%02i/%02i/%i:%02i:%02i:%02i",
            data[1],
            data[2],
            data[0],
            data[3],
            data[4],
            data[5]
        );
    }
    else {
        err_count++;
        sprintf( date_string, "%s", "" );
        get_modbus_error( nodeSG, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}
 */
