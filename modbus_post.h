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
} temp_info;

gas_flow get_gas_flow();
fork_info get_fork_info();
temp_info get_temp_info();


LEDStatus normal( RGB_COLOR_GREEN,  LED_PATTERN_FADE,  LED_SPEED_NORMAL );
LEDStatus error(  RGB_COLOR_YELLOW, LED_PATTERN_BLINK, LED_SPEED_SLOW   );
LEDStatus fail(   RGB_COLOR_RED,    LED_PATTERN_BLINK, LED_SPEED_FAST   );

/*

float get_density() {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    float           new_reading;

    addr = 2;
    qty = 2;

    result = get_modbus_data4( nodeGF, data, addr, qty );
    if ( result == nodeGF.ku8MBSuccess ) {
        new_reading = convert_ints_to_float( &data[0] );
    }
    else {
        err_count++;
        get_modbus_error( nodeGF, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    return( new_reading );
}

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

void get_gateway_time( char* date_string ) {
    uint8_t         result;
    uint16_t        data[32];
    char            err[256];

    int qty = 8;
    result = get_modbus_data4( nodeSG, data, 9000, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
        sprintf( date_string, "%02i/%02i/%i, %02i:%02i:%02i",
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
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    return( new_reading );
}
 */
/*
int cloudSetMeter( String set_point ) {
    float new_value = set_point.toFloat();
    if( new_value >= 0 ) {
        set_meter_value( meter, new_value );
        return( 1 );
    }
    else
        return(-1);
}

void set_meter_value( int meter, float value ) {
    int     new_value;

    float b = -1.734;
//    float m =  3.213;
    float m =  3.0;
 
    new_value = (int)round( (value*m)+b);
    if( new_value < 0 )
        new_value = 0;
    if( new_value > 64 )
        new_value = 64;
    analogWrite( meter, new_value, 2000 );
//    sprintf( msg, "Setting to %f, input %i (%0.2f)", value, new_value, (value*m)+b );
//    log_msg( msg );
}
 */
 
 
    
 /* density_fork = get_fork_info();
    sprintf( msg, "DENSITY FORK, wet, %0.0f, f, %0.1f, T, %0.1f, t, %li", density_fork.wet, density_fork.f, density_fork.T, Time.now() );
    if( density_fork.T > 0 )
        log_msg( msg );
    delay( delay_time );
 */
 
 /*  get_gateway_time( datestring );
    sprintf( msg, "GATEWAY, t, %i, Date, %s", Time.now(), datestring );
    if( datestring != "" )
        log_msg( msg );
    delay( delay_time );
  
    temp_ctl1 = get_temp_info();
    post_to_server( temp_ctl1.T );
    get_server_response();
    delay( delay_time );
 
    sprintf( msg, "TEMP, T, %0.1f, SP, %0.1f, OUT1, %0.1f, t, %li", temp_ctl1.T, temp_ctl1.SP, temp_ctl1.OUT1, Time.now() );
    if( temp_ctl1.T > 0 )
        log_msg( msg );

    if( temp_ctl1.SP != 75.0 ) {
        temp_ctl1.SP = 75.0;
        set_temp_setpoint( temp_ctl1.SP );
        delay( delay_time );
    }
   
    sprintf( msg, "DENSITY, rho, %0.1f, %li", get_density(), Time.now() );
    log_msg( msg );
    delay( delay_time );

    get_msg_stats();
    delay( delay_time );
 */ 
