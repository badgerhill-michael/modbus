// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"
#include "modbus-post.h"
#include <time.h>

STARTUP( System.enableFeature(FEATURE_RETAINED_MEMORY) );

/////////////////////////////////////////////////////////////////////////////////////
// Slave ID = 255 refers to the gas mass flow meter
ModbusMaster    nodeGF( 255 );  
// Slave ID = 10 refers to a SOLO temp controller
ModbusMaster    nodeSO( 20 ); 
// Slave ID = 2 refers to the Emerson Smart Gateway in our particular configuration
ModbusMaster    nodeSG( 2 ); 
//
// Delay between data requests. Sort of.
TCPClient       client;
String          response = "";
char            msg[512];
int             err_count=0;
char            endpoint[128];
char            url[256];

retained int    delay_time      = 10000;
retained int    temp_enabled    = 1;
retained int    co2_enabled     = 0;
retained int    gateway_enabled = 0;

int cloudTempEnabled( String set_point );
int cloudCO2Enabled( String set_point );
int cloudGatewayEnabled( String set_point );

void setup() {
    System.enableUpdates();
    
    nodeSO.begin(9600);
    nodeGF.begin(9600);
    nodeSG.begin(9600);

    sprintf( endpoint, "%s", "www.badgerhillbrewing.com" );
    sprintf( url, "%s", "GET /modbus/index.php?accessKey=Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6&bucketKey=KTYPDWRBG8S8" );
    
    Particle.function( "setDelayTime", cloudSetDelayTime );
    Particle.variable( "delay_time", delay_time );
    
    Particle.function( "setTempCSP", cloudSetTempSetPoint );

    Particle.function( "setEndPoint", cloudSetEndPoint );
    Particle.variable( "endpoint", endpoint );
    Particle.function( "setURL", cloudSetURL );
    Particle.variable( "url", url );

    Particle.function( "TempON",    cloudTempEnabled );
    Particle.variable( "temp_enabled", temp_enabled );

    Particle.function( "CO2ON",     cloudCO2Enabled );
    Particle.variable( "co2_enabled", co2_enabled );

    Particle.function( "GatewayON", cloudGatewayEnabled );
    Particle.variable( "SG_enabled", gateway_enabled );

	sprintf( msg, "Starting %s version %0.1f", "Modbus", 0.2 );
	log_msg( msg );
	
	report_wifi_stats( msg );
    log_msg( msg );
}

void init_modbus() {
    System.reset();
}

void loop() {
    gas_flow        co2_meter;
    temp_info       temp_ctl1;
    String          get_data_string;

    if( temp_enabled ) {
        temp_ctl1 = get_temp_info();
        if( temp_ctl1.T > -999.9 ) {
            get_data_string = String( "&date_recorded=") + String(Time.now()) + String("&sys=tempc20&T=") + String(temp_ctl1.T) + "&SP=" + String(temp_ctl1.SP) + "&OUT1=" + String(temp_ctl1.OUT1);
            post_to_server( get_data_string );
//            log_msg( get_data_string );
        }
    }
   
    if( co2_enabled ) {
        co2_meter = get_gas_flow();
        if( co2_meter.v > -999.9 ) {
            float temporary_float = co2_meter.v;
            get_data_string = String( "&date_recorded=") + String(Time.now()) + String( "&sys=co2_flow255&v=") + String(temporary_float/1000.0) + "&dv=" + String(co2_meter.dv);
            post_to_server( get_data_string );
//            log_msg( get_data_string );
        }
    }
    
    if( gateway_enabled ) {
        emerson_data *edata = (emerson_data *)calloc( 1, sizeof(emerson_data) );

        int rtn = get_emerson_data( edata );
        if( rtn == 0 ) {
            get_data_string = "&date_recorded=" + String(Time.now()) + String( "&sys=SmartGateway2" ) +
                "&cltp=" + String(edata->cltp) + 
                "&cltt=" + String(edata->cltt) +
                "&hltp=" + String(edata->hltp) +
                "&hltt=" + String(edata->hltt) +
                "&fvp=" + String(edata->fvp) +
                "&fvt=" + String(edata->fvt) +
                "&mflowdv=" + String(edata->mflowdv) +
                "&mflowv=" + String(edata->mflowv);
            post_to_server( get_data_string );
//            log_msg( get_data_string );
        }
        free(edata);
    }
 
    for( int x = 0; x < delay_time; x+=500 ) {
        Particle.process();
        delay( 500 );
    }
    check_error_state( err_count );
}

int get_emerson_data( emerson_data *edata ) {
    uint8_t         result;
    uint16_t        data[16];
	char            datestring[64];
    char            err[256];
    
    int qty  = 16;
    int addr = 0;
    result = get_modbus_data3( nodeSG, data, addr, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
        get_gateway_time( edata->datestring );
        parse_emerson_data( data, edata );
        return(0);
    }
    else {
        err_count++;
        get_modbus_error( nodeSG, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
        return(-1);
    }
}

void parse_emerson_data( uint16_t data[16], emerson_data *edata ) {
    edata->t        = Time.now();
    edata->cltp     = convert_ints_to_float( &data[0] );
    edata->cltt     = convert_ints_to_float( &data[2] );
    edata->hltp     = convert_ints_to_float( &data[4] );
    edata->hltt     = convert_ints_to_float( &data[6] );
    edata->fvp      = convert_ints_to_float( &data[8] );
    edata->fvt      = convert_ints_to_float( &data[10] );
    edata->mflowdv  = convert_ints_to_float( &data[12] );
    edata->mflowv   = convert_ints_to_float( &data[14] );
}

int cloudSetDelayTime( String set_point ) {
    report_wifi_stats( msg );
    log_msg( msg );
    
    int new_value = set_point.toInt();
    if( new_value >= 0 ) {
        delay_time = new_value;
        sprintf( msg, "delay_time set to %i", delay_time );
        log_msg( msg );
        return( 1 );
    }
    else {
        sprintf( msg, "Error: delay_time %i is not >= 0. Ignoring.",  new_value );
        log_msg( msg );
        return(-1);
    }
}

int cloudTempEnabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        temp_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
}

int cloudCO2Enabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        co2_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
}

int cloudGatewayEnabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        gateway_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
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

int cloudSetEndPoint( String command ) {
    command.toCharArray( endpoint, 128 );
    return(1);
}

int cloudSetURL( String command ) {
    command.toCharArray( url, 128 );
    return(1);
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

void report_wifi_stats( char* s ){
    int signal_strength = WiFi.RSSI();
    char* ssid          = (char *)WiFi.SSID();
    IPAddress ip        = WiFi.localIP();
    bool connected      = WiFi.ready();
    sprintf( s, "%s %idb %i.%i.%i.%i", ssid, signal_strength, ip[0], ip[1], ip[2], ip[3] );
    return;
}
void check_error_state( int err_count ) {
    if( err_count > 25 ) {
        normal.setActive(false);
        error.setActive(false);
        fail.setActive(true);
        
        report_wifi_stats( msg );
        log_msg( msg );
        delay( delay_time );

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
    temp_info       new_reading;
    
    new_reading.T = -999.9;
    new_reading.SP = -999.9;
    new_reading.OUT1 = -999.9;

    addr = 4097;// Current temp
    qty = 2;

    result = get_modbus_data4( nodeSO, data, addr-1, qty );
    if ( result == nodeSO.ku8MBSuccess ) {
        new_reading.T  = data[0]/10.0;
        new_reading.SP = data[1]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( nodeSO, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
    delay(1000);
    
    addr = 4115; // Output 1 in %
    qty = 1;

    result = get_modbus_data4( nodeSO, data, addr-1, qty );
    if ( result == nodeSO.ku8MBSuccess ) {
        new_reading.OUT1  = data[0]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( nodeSO, result, err );
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

    new_reading.v = -999.9;
    new_reading.dv = -999.9;
    
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
        String get_data_string = String( url );
        get_data_string.concat( get_parameters );
        get_data_string.concat( " HTTP/1.1\r\n" );
        get_data_string.concat( "Host: " + String(endpoint) + "\r\n" );
        get_data_string.concat( "User-Agent: Arduino\r\n" );
        get_data_string.concat( "Accept-Version: */*\r\n\r\n" );
        client.println( get_data_string );
        log_msg( String( String(endpoint )  + " " + get_data_string ) );
        get_server_response();
    }
    else {
        sprintf( msg, "%s", "Connection failed." );
        log_msg( msg );
    }
}

void get_server_response() {
    char        *buffer;
    size_t      sz = 512;

    buffer = (char *) calloc( 1, sz );
    while( client.connected() ) {
        int new_bytes = client.available();
        if( new_bytes > 0 ) {
            client.read( (uint8_t *)buffer, new_bytes );
            String s = String( buffer );
            response.concat( s );
        }
        Particle.process();
    }
    log_msg( response );
    client.stop();
    response = "";
    free(buffer);
}

String urlencode( String iString ) {
    String oString = iString;
    oString.replace( " ", "+" );
    oString.replace( "/", "%2F" );
    oString.replace( ",", "%2C" );
    oString.replace( ":", "%3A" );
    
    return( oString );
}

void log_msg( char* m ) {
//    Serial.println( msg );
    Particle.publish( "modbus", m, 300, PRIVATE );
    delay( 1000 );
}

void log_msg( String m ) {
    m.toCharArray( msg, 512 );
//    Serial.println( msg );
    Particle.publish( "modbus", msg, 300, PRIVATE );
    delay( 1000 );
}
