// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"
#include "modbus-post.h"
#include <time.h>

/////////////////////////////////////////////////////////////////////////////////////
// Slave ID = 2 refers to the Emerson Smart Gateway in our particular configuration
ModbusMaster    nodeSG( 2 ); 
// Slave ID = 255 refers to the gas mass flow meter
ModbusMaster    nodeGF( 255 );  
// Slave ID = 10 refers to a SOLO temp controller
ModbusMaster    nodeSO( 10 ); 
// Slave ID = 1 refers to the DEVIL simulator
//ModbusMaster    nodeGF( 1 );  
//char*  endpoint = "groker.initialstate.com";
//char*  endpoint = "insecure-groker.initialstate.com";

char * endpoint = "192.168.1.5";

//
// Delay between data requests. Sort of.
int             delay_time = 5000;
TCPClient       client;
String  response = "";
const int       meter = D0;
char            msg[1024];
int             err_count=0;

void setup() {
    nodeSG.begin(9600);
    nodeSG.enableTXpin(D7);

    nodeGF.begin(9600);
    nodeGF.enableTXpin(D7);

    nodeSO.begin(9600);
    nodeSO.enableTXpin(D7);
    
 //   set_autotune();
 //   delay(1000);
    
    Particle.function( "setSetPoint", cloudSetTempSetPoint );
	sprintf( msg, "Starting %s version %i", "Modbus", 0.1 );
	log_msg( msg );
	
//	sprintf( msg, "Autotune is %i", get_autotune() );
//	log_msg( msg );
}

void init_modbus() {
    System.reset();
}

void loop() {
	char            datestring[256];
    gas_flow        co2_meter;
    fork_info       density_fork;
    temp_info       temp_ctl1;
    char            get_data[512];

    check_error_state( err_count );
    
//// Get temperature controller data
    temp_ctl1 = get_temp_info();
    String get_data_string = String( "&sys=tempc10&T=" + String(temp_ctl1.T) + "&SP=" + String(temp_ctl1.SP) + "&OUT1=" + String(temp_ctl1.OUT1) );
    post_to_server( get_data_string );
//// End get temperature controller data

//// Get co2 flow meter data
    co2_meter = get_gas_flow();
    get_data_string = String( "&sys=co2_flow255&v=" + String(co2_meter.v/1000.0) + "&dv=" + String(co2_meter.dv) );
    post_to_server( get_data_string );
//// End get co2 flow meter data
    delay( delay_time );
}

int cloudSetTempSetPoint( String set_point ) {
    float new_value = set_point.toFloat();
    if( new_value >= 0 ) {
        set_temp_setpoint( new_value );
        return( 1 );
    }
    else
        return(-1);
}

void set_temp_setpoint( float new_setpoint ) {   
    uint8_t         result;
    char            err[256];
    uint16_t        new_SP;
    
    new_SP = (uint16_t) ( round(new_setpoint * 10.0) );

    result = put_modbus_datum( nodeSO, 4098, new_SP );
    if ( result == nodeSO.ku8MBSuccess )
        sprintf( msg, "TEMP CONTROLLER, Setpoint set to %0.2f", new_setpoint );
    else {
        get_modbus_error( nodeSO, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
    }
    log_msg( msg );
}

int get_autotune() {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    int             value;

    addr = 2068;
    qty = 1;

    result = get_modbus_coils( nodeSO, data, addr, qty );
    if ( result == nodeSO.ku8MBSuccess )
        value = data[0];
    else {
        err_count++;
        get_modbus_error( nodeSO, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    return( value );
}

void set_autotune() {   
    uint8_t         result;
    char            err[256];

    result = put_modbus_coil( nodeSO, 2068, 1 );
    if ( result == nodeSO.ku8MBSuccess )
        sprintf( msg, "TEMP CONTROLLER, Set autotune to %i", 1 );
    else {
        get_modbus_error( nodeSO, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
    }
    log_msg( msg );
}

void check_error_state( int err_count ) {
    if( err_count > 10 ) {
//        set_meter_value( meter, 20.0 );
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
//        set_meter_value( meter, 8.0 );
    }
}

void get_modbus_error( ModbusMaster node, uint8_t result, char* err ) {
    normal.setActive(false);
    error.setActive(true);
    fail.setActive(false);

    if( result == node.ku8MBInvalidSlaveID )
        sprintf( err, "Invalid slave id or comm error" );
    else if( result == node.ku8MBInvalidFunction )
        sprintf( err, "Invalid function" );
    else if( result == node.ku8MBResponseTimedOut )
        sprintf( err, "Response Timed Out" );
    else if( result == node.ku8MBInvalidCRC )
        sprintf( err, "Invalid CRC" );
    else  if ( result == nodeSO.ku8MBSuccess )
        sprintf( err, "No error, dude!" );
    else
        sprintf( err, "Unidentified Error" );   
}

uint8_t put_modbus_datum( ModbusMaster node, uint16_t maddress, uint16_t mdata ) {
    uint8_t     result;
    
    result = node.writeSingleRegister( maddress-1, mdata );
    return( result );
}

uint8_t  put_modbus_coil( ModbusMaster node, uint16_t maddress, uint16_t mdata ) {
    uint8_t     result;
    result = node.writeSingleCoil( maddress-1, mdata );
    return( result );
}

uint8_t get_modbus_data3( ModbusMaster node, uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
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

uint8_t get_modbus_coils( ModbusMaster node, uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     result;

    result = node.readCoils( maddress-1, mqty );
    if ( result == node.ku8MBSuccess ) {
		for ( int j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	node.clearResponseBuffer();
    node.clearTransmitBuffer();
	return( result );
}

uint8_t get_modbus_data4( ModbusMaster node, uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
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

temp_info get_temp_info() {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    float           t  = 0.0,
                    sp = 0.0;
    temp_info       new_reading;

    addr = 4097;// Current temp
    qty = 2;

    result = get_modbus_data4( nodeSO, data, addr-1, qty );
    if ( result == nodeSO.ku8MBSuccess ) {
        new_reading.T  = data[0]/10.0;
        new_reading.SP = data[1]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( nodeGF, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    delay(delay_time);
    
    addr = 4115; // Output 1 in %
    qty = 1;

    result = get_modbus_data4( nodeSO, data, addr-1, qty );
    if ( result == nodeSO.ku8MBSuccess ) {
        new_reading.OUT1  = data[0]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( nodeGF, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    return( new_reading );
}

gas_flow get_gas_flow() {
    uint8_t         result;
    uint16_t        addr, qty, data[5];
    char            err[256];
    gas_flow        new_reading;

    addr = 2;
    qty = 5;

    result = get_modbus_data4( nodeGF, data, addr, qty );
    if ( result == nodeGF.ku8MBSuccess ) {
        new_reading.dv = ( data[0] * 65536 + data[1] ) /1000.0;
        float v1 = data[2] * 65536 + data[3];
        new_reading.v = (v1 *1000 + data[4])/1000.0;
    }
    else {
        err_count++;
        get_modbus_error( nodeGF, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    return( new_reading );
}

float convert_ints_to_float( uint16_t* w ) {
    float new_float;
    memcpy( &new_float, w, sizeof(float) );
    return( new_float );
}

void post_to_server( String get_parameters ) {
    if ( client.connect( endpoint, 80) ) {
        sprintf( msg, "Posting to %s:%s", endpoint, get_parameters );
        log_msg( msg );
        
//        String get_data_string = String( "GET /api/events?accessKey=Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6&bucketKey=KTYPDWRBG8S8" );
        String get_data_string = String( "GET /index.php?accessKey=Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6&bucketKey=KTYPDWRBG8S8" );
        get_data_string.concat( get_parameters );
        get_data_string.concat( " HTTP/1.0" );
        client.println( get_data_string );
        
        client.println( String( "Date: " + Time.timeStr() ) );
        client.println( "User-Agent: Arduino" );
        client.println( "Accept-Version: ~0" );
        client.println( "Content-Type: application/x-www-form-urlencoded" );
//        client.println( "Date: Thu, 28 Sep 2017 19:58:14 GMT" );
//      client.println( "Retry-After: 10" );
        client.println( "Content-Length: 0" );
        client.println(  );
        get_server_response();
    }
    else {
        sprintf( msg, "%s", "Connection failed." );
        log_msg( msg );
    }
}

void get_server_response() {
    char        *buffer;
    size_t      size = 1024;

    buffer = (char *) calloc( 1, size );
    while( client.connected() ) {
        int new_bytes = client.available();
        if( new_bytes > 0 ) {
            client.read( (uint8_t *)buffer, new_bytes );
            String s = String( buffer );
            response.concat( s );
        }
        Particle.process();
    }
    response.toCharArray( msg, response.length() );
    log_msg(msg);
    client.stop();
    response = "";
    free(buffer);
}

void log_msg( char* msg ) {
//    Serial.println( msg );
    Particle.publish("modbus", msg, 300, PRIVATE);
    delay( 1000 );
}
