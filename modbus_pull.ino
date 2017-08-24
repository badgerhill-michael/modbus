// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"

//int             slave_id = 2;   /* Emerson Smart Gateway */
int             slave_id = 255; /* Gas flow meter */

ModbusMaster    node( slave_id );
char            msg[1024];
int             delay_time = 4000;
int             err_count=0;

LEDStatus normal( RGB_COLOR_GREEN,  LED_PATTERN_FADE,  LED_SPEED_NORMAL );
LEDStatus error(  RGB_COLOR_YELLOW, LED_PATTERN_BLINK, LED_SPEED_SLOW   );
LEDStatus fail(   RGB_COLOR_RED,    LED_PATTERN_BLINK, LED_SPEED_FAST   );

void setup() {
//  Particle.variable("msg", msg );
// initialize Modbus communication baud rate
    node.begin(9600);
    node.enableTXpin(D7);
//    node.enableDebug();  //Print TX and RX frames out on Serial. Beware, enabling this messes up the timings for RS485 Transactions, causing them to fail.
	sprintf( msg, "%s", "Starting Modbus Interface" );
	log_msg( msg );
//  Subscribe to the integration response event
//    Particle.subscribe("hook-response/modbus", googleCloudHandler, MY_DEVICES);
}

//void googleCloudHandler( const char *event, const char *data ) {
//    normal.setActive(true);
//    Particle.publish( "modbus-log", "Google said OK" );
//}

void init_modbus() {
    System.reset();
}

void loop() {
	uint8_t         result;
	uint16_t        data[16];
	char            datestring[256];
    char            err[256];

    check_error_state( err_count );
    get_gas_flow();
    delay( delay_time );

/* The below is for the Emerson Smart Gateway */
/*
    get_gateway_time( datestring );
    delay( 4096 );

    int qty = 8;
    result = get_modbus_data3( data, 0, qty );
    if ( result == node.ku8MBSuccess ) {
        sprintf( msg, "%s, %0.0f, %0.2f, %0.2f, %0.2f, %i",
            datestring,
            convert_ints_to_float(&data[0]),
            convert_ints_to_float(&data[2]),
            convert_ints_to_float(&data[4]),
            convert_ints_to_float(&data[6]),
            Time.now()
        );
		log_msg( msg );
    }
    else {
        err_count++;
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
     delay( delay_time );
 */
}

void check_error_state( int err_count ) {
    if( err_count > 5 ) {
        normal.setActive(false);
        error.setActive(false);
        fail.setActive(true);
        sprintf( msg, "Too many errs (%i), rebooting.", err_count );
	    log_msg( msg );
        delay( delay_time );
        init_modbus();
        // The below will never execute...
        delay( delay_time );
        err_count = 0;
    }
    else {
        normal.setActive(true);
        error.setActive(false);
        fail.setActive(false);
    }
}

void get_modbus_error( uint8_t result, char* err ) {
    normal.setActive(false);
    error.setActive(true);
    fail.setActive(false);

    if( result == node.ku8MBInvalidSlaveID )
        sprintf( err, "Invalid slave id: %i", slave_id );
    else if( result == node.ku8MBInvalidFunction )
        sprintf( err, "Invalid function" );
    else if( result == node.ku8MBResponseTimedOut )
        sprintf( err, "Response Timed Out" );
    else if( result == node.ku8MBInvalidCRC )
        sprintf( err, "Invalid CRC" );
    else
        sprintf( err, "Unidentified Error" );   
}

uint8_t get_modbus_data3( uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     result;

    result = node.readInputRegisters( maddress, mqty );
    if ( result == node.ku8MBSuccess ) {
		for ( int j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	node.clearResponseBuffer();
    node.clearTransmitBuffer();
    return( result );
}

uint8_t get_modbus_data4( uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     result;

    result = node.readHoldingRegisters( maddress, mqty );
    if ( result == node.ku8MBSuccess ) {
		for ( int j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	node.clearResponseBuffer();
    node.clearTransmitBuffer();
	return( result );
}

void get_gas_flow() {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];

    addr = 2;
    qty = 5;

    result = get_modbus_data4( data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        float dv = ( data[0] * 65536 + data[1] ) /1000.0;
        float v1 = data[2] * 65536 + data[3];
        float v = (v1 *1000 + data[4])/1000.0;

        sprintf( msg, "%i, %7.3f, %8.3f", Time.now(), dv, v );
        log_msg( msg );
    }
    else {
        err_count++;
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}

void get_constant_float() {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];

    addr = 9012;
    qty = 2;
    result = get_modbus_data4( data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
        sprintf( msg, "%x %x %f", data[0], data[1], convert_ints_to_float(&data[0]) );
        log_msg( msg );
    }
    else {
        err_count++;
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}

void get_msg_stats() {
    uint8_t         result;
    uint16_t        addr, qty, data[4];
    char            err[256];

    addr = 9007;
    qty = 4;
    result = get_modbus_data4( data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
        sprintf( msg, "\t%i %s %i", 49007, "Messages Received", data[0] );
     	log_msg( msg );

        sprintf( msg, "\t%i %s %i", 49008, "Corrupt Messages Received", data[1] );
     	log_msg( msg );

        sprintf( msg, "\t%i %s %i", 49010, "Messages Sent",     data[3] );
     	log_msg( msg );
    }
    else {
        err_count++;
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}

float convert_ints_to_float( uint16_t* w ) {
    float new_float;
    memcpy( &new_float, w, sizeof(float) );
    return( new_float );
}

void get_gateway_time( char* date_string ) {
    uint8_t         result;
    uint16_t        data[32];
    char            err[256];

    int qty = 8;
    result = get_modbus_data4( data, 9000, qty );
    if ( result == node.ku8MBSuccess ) {
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
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}

void log_msg( char* msg ) {
//    Serial.println( msg );
    Particle.publish( "modbus", msg );
//    delay( 1000 );
}
