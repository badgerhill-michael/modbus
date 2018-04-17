// This #include statement was automatically added by the Particle IDE.
//#include <MQTT.h>

// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"
#include "modbus-post.h"
#include "modbus_functions.h"

STARTUP( System.enableFeature(FEATURE_RETAINED_MEMORY) );

/////////////////////////////////////////////////////////////////////////////////////
// Gas mass flow meter
int gas_flow_meter_id = 255;
ModbusMaster    nodeGF( gas_flow_meter_id );

// CO2 Watchdog
int co2_watchdog_modbus_id = 1;
ModbusMaster    nodeCO2watchdog( co2_watchdog_modbus_id ); 

// Emerson Smart Gateway
int smart_gateway_modbus_id = 2;
ModbusMaster    nodeSG( smart_gateway_modbus_id ); 

// pH Meter
int ph_modbus_id_03 = 3;
ModbusMaster    nodeSO03( ph_modbus_id_03 ); 

// Humidity Sensor
int humidity_modbus_id = 4;
ModbusMaster    nodeHumidity( humidity_modbus_id ); 

// Humidity Sensor 2
ModbusMaster    nodeHum2( 5 ); 

// Temp sensor
ModbusMaster    nodeTEMP( 7 );

// Solo temp controllers
int tempc_modbus_id_10 = 10;
ModbusMaster    nodeSO10( tempc_modbus_id_10 ); 

int tempc_modbus_id_11 = 11;
ModbusMaster    nodeSO11( tempc_modbus_id_11 ); 

int tempc_modbus_id_12 = 12;
ModbusMaster    nodeSO12( tempc_modbus_id_12 ); 

int tempc_modbus_id_13 = 13;
ModbusMaster    nodeSO13( tempc_modbus_id_13 ); 

int tempc_modbus_id_14 = 14;
ModbusMaster    nodeSO14( tempc_modbus_id_14 ); 

int tempc_modbus_id_15 = 15;
ModbusMaster    nodeSO15( tempc_modbus_id_15 ); 

int tempc_modbus_id_16 = 16;
ModbusMaster    nodeSO16( tempc_modbus_id_16 ); 

int tempc_modbus_id_17 = 17;
ModbusMaster    nodeSO17( tempc_modbus_id_17 ); 

int tempc_modbus_id_18 = 18;
ModbusMaster    nodeSO18( tempc_modbus_id_18 ); 

int tempc_modbus_id_19 = 19;
ModbusMaster    nodeSO19( tempc_modbus_id_19 ); 

int tempc_modbus_id_20 = 20;
ModbusMaster    nodeSO20( tempc_modbus_id_20 ); 

int tempc_modbus_id_21 = 21;
ModbusMaster    nodeSO21( tempc_modbus_id_21 ); 

/////////////////////////////////////////////////////////////////////////////////////

String          response = "";
char            msg[512];
int             err_count=0;
long int        err_timer=0;
char            endpoint[128];
char            url[256];
String          version;
char            MQTT_TOPIC[512];
int             txdata;

retained int    delay_time          = 500;
retained int    temp_enabled        = 1;
retained int    co2_enabled         = 1;
retained int    gateway_enabled     = 1;
retained int    humidity_enabled    = 1;
retained int    density_enabled     = 0;
retained int    ph_enabled          = 0;
//retained double  humidity_setpoint   = 32.0;

int cloudTempEnabled( String set_point );
int cloudCO2Enabled( String set_point );
int cloudGatewayEnabled( String set_point );
int cloudHumidEnabled( String set_point );
int cloudDensEnabled( String set_point );
int cloudpHEnabled( String set_point );
int cloudModbusRead( String params );
int cloudModbusWrite( String params );

//char* mqtt_server = "test.mosquitto.org";
//void callback(char* topic, byte* payload, unsigned int length);
uint32_t convert_ints_to_int( uint16_t* w );

/*
MQTT mqtt_client( mqtt_server, 1883, callback );

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    log_msg( p );
    delay(1000);
}
*/

void setup() {
#if ( PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION )
//    Cellular.on();
    int txdata = get_cell_usage();
#endif
 //   sprintf( MQTT_TOPIC, "badgerhillbrewing-com/%i/messages", device_number(System.deviceID()) );

//    mqtt_client.setBroker( mqtt_server, 1883 );
//    mqtt_client.connect( System.deviceID() );

    version = String(System.deviceID()) + "_" + Time.format( Time.now(), TIME_FORMAT_ISO8601_FULL );

    nodeGF.begin(9600);
    nodeSG.begin(9600);
    nodeHumidity.begin(9600);
    nodeHum2.begin(9600);
    nodeTEMP.begin(9600);
    nodeSO10.begin(9600);
    nodeSO11.begin(9600);
    nodeSO12.begin(9600);
    nodeSO13.begin(9600);
    nodeSO14.begin(9600);
    nodeSO15.begin(9600);
    nodeSO16.begin(9600);
    nodeSO17.begin(9600);
    nodeSO18.begin(9600);
    nodeSO19.begin(9600);
    nodeSO20.begin(9600);
    nodeSO21.begin(9600);
  
    sprintf( endpoint, "%s", "www.badgerhillbrewing.com" );
    
//    char *aKey = "Unmapped";
//    char* bKey = "Beerpeople";

//    char *aKey = "SOCIABLE";
//    char *bKey = "CIDERWERKS";

    char *aKey = "Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6";
    char* bKey = "KTYPDWRBG8S8";
    
    sprintf( url, "GET /modbus/post.php?accessKey=%s&bucketKey=%s", aKey, bKey );
    
    Particle.function( "setDelayTime", cloudSetDelayTime );
    Particle.variable( "delay_time", delay_time );
    
    Particle.function( "setTempCSP", cloudSetTempSetPoint );
    Particle.function( "modbusRead", cloudModbusRead );
    Particle.function( "modbusWrite", cloudModbusWrite );

    Particle.function( "TempON",    cloudTempEnabled );
    Particle.variable( "temp_enabled", temp_enabled );

    Particle.function( "CO2ON",     cloudCO2Enabled );
    Particle.variable( "co2_enabled", co2_enabled );

    Particle.function( "GatewayON", cloudGatewayEnabled );
    Particle.variable( "SG_enabled", gateway_enabled );

    Particle.function( "HumidON", cloudHumidEnabled );
    Particle.variable( "humidity_on", humidity_enabled );

    Particle.function( "DensON", cloudDensEnabled );
    Particle.variable( "density_on", density_enabled );

    Particle.function( "pHON", cloudpHEnabled );
    Particle.variable( "ph_enabled", ph_enabled );

    Particle.variable( "err_count", err_count );

	report_connection_stats( msg );
    log_msg( msg );
    
/*    Particle.function( "setHumSP", cloudSetHumSetPoint );
    Particle.variable( "humiditySP", humidity_setpoint );
    pinMode( D2, OUTPUT );
    digitalWrite( D2, LOW );
 */
}

void loop() {
    sprintf( msg, "" );
#if ( PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION )
    int data = get_cell_usage();
    sprintf( msg, ",%d", data-txdata );
    txdata=data;
#endif 
    log_msg( String(Time.now()) + String(msg)  );
    
	String get_data_string = "&reported_by=" + version;
    
/*    float new_temp = get_temp( nodeTEMP );
    if( new_temp > -999.9 ) {
        get_data_string += "&date_recorded=" + String(Time.now()) + "&system_name=tempc&system_id=7&T=" + String::format( "%0.2f", new_temp);
        post_to_server( get_data_string );
    }
 */

    if( co2_enabled ) {
        gas_flow co2_meter = get_gas_flow( nodeGF );
        if( co2_meter.v > -999.9 ) {
            float temporary_float = co2_meter.v;
            String values = "system_name=co2_flow&system_id=" + String(gas_flow_meter_id) + "&v=" + String::format( "%07.4f", temporary_float/1000.0 ) + "&dv=" + String::format( "%06.4f", co2_meter.dv );
            post_to_server( get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values );
//            log_msg( values );
        }
    }

//    if( density_enabled ) {
//        devil_info density_sensor = get_devil_info( nodeDEVIL );
//        if( density_sensor.T > -999.9 ) {
//            String values = "system_name=devil&system_id=1&T=" + String(density_sensor.T) + "&density=" + String(density_sensor.rho);
//            post_to_server( get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values );
//            log_msg( values );
//        }
//    }
    
    if( gateway_enabled ) {
        emerson_data *edata = (emerson_data *)calloc( 1, sizeof(emerson_data) );

        int rtn = get_emerson_data( edata );
        if( rtn == 0 ) {
            String values = "system_name=SmartGateway&system_id=2&cltp=" + String::format( "%0.2f", edata->cltp) + 
                "&cltt=" + String::format( "%0.2f", edata->cltt) +
                "&hltp=" + String::format( "%0.2f", edata->hltp) +
                "&hltt=" + String::format( "%0.2f", edata->hltt) +
                "&fvp=" + String::format( "%0.2f", edata->fvp) +
                "&fvt=" + String::format( "%0.2f", edata->fvt) +
                "&mflowdv=" + String::format( "%0.2f", edata->mflowdv) +
                "&mflowv=" + String::format( "%0.2f", edata->mflowv) +
                "&mflowt=" + String::format( "%0.2f", edata->mflowt) +
                "&phyg1p=" + String::format( "%0.3f", edata->phyg1p) +
                "&phyg1t=" + String::format( "%0.2f", edata->phyg1t) +
                "&forkwet=" + String::format( "%0.2f", edata->forkwet) +
                "&forkhz=" + String::format( "%0.2f", edata->forkhz) +
                "&forkt=" + String::format( "%0.2f", edata->forkt ); 
            post_to_server( get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values );
//            log_msg( values );
        }
        free(edata);
    }
    
    if( humidity_enabled ) {
        humidity humidity_meter = get_humidity( nodeHumidity );
        if( humidity_meter.T > -999.9 ) {
            get_data_string += "&date_recorded=" + String(Time.now()) + "&system_name=humidity&system_id=" + String(humidity_modbus_id) + "&humidity="
                + String::format( "%0.1f", humidity_meter.humidity) + "&T=" + String::format( "%0.2f", humidity_meter.T );
            post_to_server( get_data_string );
 //         log_msg( get_data_string );
        }
        humidity_meter = get_humidity( nodeHum2 );
        if( humidity_meter.T > -999.9 ) {
            get_data_string += "&date_recorded=" + String(Time.now()) + "&system_name=humidity&system_id=" + String(5) + "&humidity="
                + String::format( "%0.1f", humidity_meter.humidity) + "&T=" + String::format( "%0.2f", humidity_meter.T );
            post_to_server( get_data_string );
 //         log_msg( get_data_string );
        }

    }

    if( ph_enabled ) {
        ph ph_meter = get_ph( nodeSO03 );
        if( ph_meter.T > -999.9 ) {
            get_data_string += "&date_recorded=" + String(Time.now()) + "&system_name=ph&system_id=3&ph="
                + String(ph_meter.ph) + "&T=" + String(ph_meter.T);
            post_to_server( get_data_string );
//            log_msg( "ph=" + String(ph_meter.ph) + "&T=" + String(ph_meter.T) );
        }
    }

    if( temp_enabled ) {
        read_and_post_tempc(  co2_watchdog_modbus_id, nodeCO2watchdog, get_data_string );
        read_and_post_tempc( tempc_modbus_id_10, nodeSO10, get_data_string );
        read_and_post_tempc( tempc_modbus_id_11, nodeSO11, get_data_string );
        read_and_post_tempc( tempc_modbus_id_12, nodeSO12, get_data_string );
        read_and_post_tempc( tempc_modbus_id_13, nodeSO13, get_data_string );
        read_and_post_tempc( tempc_modbus_id_14, nodeSO14, get_data_string );
        read_and_post_tempc( tempc_modbus_id_15, nodeSO15, get_data_string );
        read_and_post_tempc( tempc_modbus_id_16, nodeSO16, get_data_string );
        read_and_post_tempc( tempc_modbus_id_17, nodeSO17, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_18, nodeSO18, get_data_string );
        read_and_post_tempc( tempc_modbus_id_19, nodeSO19, get_data_string );
        read_and_post_tempc( tempc_modbus_id_20, nodeSO20, get_data_string );
        read_and_post_tempc( tempc_modbus_id_21, nodeSO21, get_data_string );
    }
    check_error_state();
    delay_with_process( delay_time );
}

void delay_with_process( int d ) {
    int t0 = millis();
    while( millis()-t0 < d ) {
        Particle.process();
        delay( 100 );
//        if ( mqtt_client.isConnected() )
//            mqtt_client.loop();
    }
}

void read_and_post_tempc( int id, ModbusMaster node, String get_data_string ) {
    temp_info   temp_ctl1 = get_temp_info( node );
    String      system_name = "tempc";
    String      parameter_name = "T";
 
    if( temp_ctl1.T > -999.9 ) {
        if( id == co2_watchdog_modbus_id ) {
            system_name = "co2c";
            parameter_name = "P";
        }
        get_data_string += "&date_recorded=" + String(Time.now()) + "&system_name=" + system_name + "&system_id=" + String(id) 
            +"&" + parameter_name + "=" + String(temp_ctl1.T) + "&SP=" + String(temp_ctl1.SP) + "&OUT1=" + 
            String(temp_ctl1.OUT1) + "&Alarm1=" + String(temp_ctl1.Alarm1);
        post_to_server( get_data_string );
//        log_msg( "system_id=" + String::format( "%02i", id) + "&" +parameter_name + "=" + String::format( "%0.1f", temp_ctl1.T ) + "&OUT1=" + 
//            String::format( "%04.1f", temp_ctl1.OUT1) + "&Alarm1=" + String(temp_ctl1.Alarm1) );
    }
    else
        log_msg( "Failed for id=" + String(id) );
}

int get_emerson_data( emerson_data *edata ) {
    uint8_t         result;
    uint16_t        data[28];
    char            err[256];
    
    int qty  = 28;
    int addr = 0;
    result = get_modbus_data3( nodeSG, data, addr, qty );
    if ( result == nodeSG.ku8MBSuccess ) {
//        get_gateway_time( edata->datestring );
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
    edata->mflowt   = convert_ints_to_float( &data[16] );
    edata->phyg1p   = convert_ints_to_float( &data[18] )*27.7076;
    edata->phyg1t   = convert_ints_to_float( &data[20] );
    edata->forkwet = convert_ints_to_float( &data[22] );
    edata->forkhz  = convert_ints_to_float( &data[24] );
    edata->forkt   = convert_ints_to_float( &data[26] );
}

int cloudSetDelayTime( String set_point ) {
    report_connection_stats( msg );
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

int cloudHumidEnabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        humidity_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
}

int cloudDensEnabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        density_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
}

int cloudpHEnabled( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 && new_value <= 1) {
        ph_enabled = new_value;
        return( 1 );
    }
    else
        return(-1);
}

int cloudModbusRead( String params ) {
    String s = params.substring( 0, params.indexOf( ":" ) );
    String a = params.substring( params.indexOf( ":" )+1, params.indexOf( ":", s.length()+2 )+1 );
    String q = params.substring( params.lastIndexOf( ":")+1 );
    
    int system_id = s.toInt();
    int address = a.toInt();
    int qnty    = q.toInt();
    
    ModbusMaster node = systemID2node( system_id );

    uint8_t         result;
    uint16_t        addr, qty, data[qnty];
    char            err[256];
    uint16_t        new_reading;
    
    new_reading = -999.9;

    addr = (uint16_t)address;
    qty = (uint16_t)qnty;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        for( int n = 0; n < qnty; ++n ) {
            new_reading  = data[n];
            sprintf( msg, "system_id %i, addr %i, qty %i = %i", system_id, address+n, qnty, new_reading );
            log_msg( msg );
        }
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i,%i): %s", result, system_id, addr, err );
        log_msg( msg );
    }
    return(1);
}

int cloudModbusWrite( String params ) {
    String s = params.substring( 0, params.indexOf( ":" ) );
    String a = params.substring( params.indexOf( ":" )+1, params.indexOf( ":", s.length()+2 )+1 );
    String q = params.substring( params.lastIndexOf( ":")+1 );
    
    int system_id = s.toInt();
    int address = a.toInt();
    int value    = q.toInt();
    
    ModbusMaster node = systemID2node( system_id );

    uint8_t         result;
    uint16_t        addr, qty, data;
    char            err[256];

    addr = (uint16_t)address;
    data = (uint16_t)value;

    result = put_modbus_datum( node, addr, data );
    if ( result == node.ku8MBSuccess ) {
        sprintf( msg, "Address %i set to %i on system_id %i", addr, data, system_id );
        log_msg( msg );
    }
    else {
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
        return(-1);
    }
    return(1);
}

bool isBitISet( uint16_t data, int i ) {
   uint16_t mask = (uint16_t)( 1 << i ) ;
   return( mask & data );   
}

int cloudSetTempSetPoint( String command ) {
    String sid = command.substring( 0, command.indexOf( ":" ) );
    String sp  = command.substring( command.indexOf( ":" )+1 );
    int system_id = sid.toInt();
    float new_value = sp.toFloat();
    if( new_value >= 0 ) {
        set_temp_setpoint( system_id, new_value );
        return( 1 );
    }
    else
        return(-1);
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

ModbusMaster systemID2node( int system_id ) {
    ModbusMaster    node;

    if( system_id == co2_watchdog_modbus_id )
        node = nodeCO2watchdog;
    else if( system_id == smart_gateway_modbus_id )
        node = nodeSG;
    else if( system_id == ph_modbus_id_03 )
        node = nodeSO03;
    else if( system_id == humidity_modbus_id )
        node = nodeHumidity;
    else if( system_id == 5 )
        node = nodeHum2;
    else if( system_id == 7 )
        node = nodeTEMP;
    else if( system_id == tempc_modbus_id_10 )
        node = nodeSO10;
    else if( system_id == tempc_modbus_id_11 )
        node = nodeSO11;
    else if( system_id == tempc_modbus_id_12 )
        node = nodeSO12;
    else if( system_id == tempc_modbus_id_13 )
        node = nodeSO13;
    else if( system_id == tempc_modbus_id_14 )
        node = nodeSO14;
    else if( system_id == tempc_modbus_id_15 )
        node = nodeSO15;
    else if( system_id == tempc_modbus_id_16 )
        node = nodeSO16;
    else if( system_id == tempc_modbus_id_17 )
        node = nodeSO17;
    else if( system_id == tempc_modbus_id_18 )
        node = nodeSO18;
    else if( system_id == tempc_modbus_id_19 )
        node = nodeSO19;
    else if( system_id == tempc_modbus_id_20 )
        node = nodeSO20;
    else if( system_id == tempc_modbus_id_21 )
        node = nodeSO21;
    else if( system_id == gas_flow_meter_id )
        node = nodeGF;
        
    return(node);
}

void set_temp_setpoint( int system_id, float new_setpoint ) {   
    uint8_t         result;
    char            err[256];
    uint16_t        new_SP;
    ModbusMaster    node;

    node = systemID2node( system_id );
        
    new_SP = (uint16_t) ( round(new_setpoint * 10.0) );

    result = put_modbus_datum( node, 4098-1, new_SP );
    if ( result == node.ku8MBSuccess )
        sprintf( msg, "Setpoint set to %0.2f on system_id %i", new_setpoint, system_id );
    else {
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
    }
    log_msg( msg );
}

void report_connection_stats( char* s ){
#if ( PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION )
    int signal_strength = WiFi.RSSI();
    char* ssid          = (char *)WiFi.SSID();
    IPAddress ip        = WiFi.localIP();
#endif

#if ( PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION )
    CellularSignal sig  = Cellular.RSSI();
    int signal_strength = sig.rssi;
    char* ssid          = "cellular";
    IPAddress ip        = Cellular.localIP();
#endif

    sprintf( s, "%s %idb %i.%i.%i.%i", ssid, signal_strength, ip[0], ip[1], ip[2], ip[3] );
    return;
}

void check_error_state() {
    if( err_count > 0 ) {
        if( err_timer == 0 )
            err_timer = Time.now();
        else if ( Time.now() - err_timer < 300 ) {
            if( err_count > 100 ) {
                sprintf( msg, "Too many errors (%i), rebooting...", err_count );
                log_msg( msg );
                err_count = 0;
                err_timer = 0;
                delay(1000);
                System.reset();
            }
            normal.setActive(false);
            error.setActive(true);
            fail.setActive(false);
        }
        else { // if we haven't had an error in 5 minutes, reset error state to green
            err_count = 0;
            err_timer = 0;
            normal.setActive(true);
            error.setActive(false);
            fail.setActive(false);
        }
    }
    else {
        err_timer = 0;
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
    else  if ( result == node.ku8MBSuccess )
        sprintf( err, "No error, dude!" );
    else
        sprintf( err, "Unidentified Error" );   
}

temp_info get_temp_info( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    temp_info       new_reading;
    
    new_reading.T = -999.9;
    new_reading.SP = -999.9;
    new_reading.OUT1 = -999.9;


    addr = 4097;// Current temp
    qty = 2;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
        if( data[0] > 32769 ) {
            log_msg( "Temp or temp probe not available: " + String::format( "%0x", data[0] ) + "H" );
            delay(1000);
            return( new_reading );
        }
        new_reading.T  = data[0]/10.0;
        if( new_reading.T > 1000.0 )
            new_reading.T   = (float)(data[0]-65536)/10.0;
        new_reading.SP = data[1]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "'%s (%i)' getting T/SP", err, result );
        log_msg( msg );
    }
    delay_with_process(250);
    
    addr = 4115; // Output 1 in %
    qty = 1;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading.OUT1  = data[0]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "'%s (%i)' getting OUT1", err, result );
        log_msg( msg );
    }
    delay_with_process(250);
    
    addr = 4139; // Alarm1 Status
    qty = 1;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess )
        new_reading.Alarm1 = isBitISet( data[0], 4 );
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "'%s (%i)' getting Alarm1", err, result );
        log_msg( msg );
    }
    delay_with_process(250);
    return( new_reading );
}

humidity get_humidity( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    humidity        new_reading;
    
    new_reading.T   = -999.9;
    new_reading.humidity = -999.9;

    addr = 0;
    qty  = 2;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading.humidity = (float)data[0]/10.0;
        new_reading.T   = ((float)data[1]/10.0*1.8)+32.0;
        if( new_reading.T > 1000.0 )
            new_reading.T   = ((float)(data[1]-65536)/10.0*1.8)+32.0;
     }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

float get_temp( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    float           new_reading;
    
    new_reading   = -999.9;

    addr = 98;
    qty  = 2;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading = (float)data[0];
        new_reading += (float)data[1]/65536.0;
     }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

ph get_ph( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[2];
    char            err[256];
    ph        new_reading;
    
    new_reading.T   = -999.9;
    new_reading.ph = -999.9;

    addr = 0;
    qty  = 2;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading.ph = (float)data[0]/100.0;
        new_reading.T   = ((float)data[1]/10.0*1.8)+32.0;
        if( new_reading.T > 1000.0 )
            new_reading.T   = ((float)(data[1]-65536)/10.0*1.8)+32.0;
     }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
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

gas_flow get_gas_flow( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[4];
    char            err[256];
    gas_flow        new_reading;

    new_reading.v = -999.9;
    new_reading.dv = -999.9;
    
    addr = 2;
    qty = 4;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading.dv = ( data[0] * 65536 + data[1] ) /1000.0;
        float v1 = (float)(data[2] * 65536 + data[3]);
        new_reading.v = (v1 *1000.0 + (float)data[4])/1000.0;
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

uint32_t convert_ints_to_int( uint16_t* w ) {
    uint32_t new_int;
    memcpy( &new_int, w, sizeof(uint32_t) );
    return( new_int );
}

float convert_ints_to_float( uint16_t* w ) {
    float new_float;
    memcpy( &new_float, w, sizeof(float) );
    return( new_float );
}

void post_to_server( String get_parameters ) {
    TCPClient client;
    if ( client.connect( endpoint, 80) ) {
        String get_data_string = String( url );
        get_data_string.concat( get_parameters );
        get_data_string.concat( " HTTP/1.1\r\n" );
        get_data_string.concat( "Host: " + String(endpoint) + "\r\n" );
        get_data_string.concat( "User-Agent: Arduino\r\n" );
        get_data_string.concat( "Accept-Version: */*\r\n\r\n" );
        client.println( get_data_string );
//        log_msg( String( get_data_string ) );
        get_server_response( client );
    }
    else {
        err_count++;
        sprintf( msg, "%s", "Connection failed." );
        log_msg( msg );
    }
}

void get_server_response( TCPClient connection ) {
    char        *buffer;
    size_t      sz = 512;

    long t0 = millis();
    buffer = (char *) calloc( 1, sz );
    while( connection.connected() && (millis()-t0 < 5000) ) {
        int new_bytes = connection.available();
        if( new_bytes > 0 ) {
            connection.read( (uint8_t *)buffer, new_bytes );
            String s = String( buffer );
            response.concat( s );
        }
        Particle.process();
    }
//    int pos = response.indexOf( "system_name" )-3;
//    if( pos > 0 )
//        log_msg( "OK: " + response.substring( pos+1 ) );
//    if( pos == 0 )
//        log_msg( "NOT OK: " + response );
    connection.stop();
    response = "";
    free(buffer);
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

void log_msg( char* m ) {
//    Serial.println( msg );
//    if ( mqtt_client.isConnected() )
//        mqtt_client.publish( MQTT_TOPIC, m );
//    else {
        Particle.publish( "modbus", m, 300, PRIVATE );
//        mqtt_client.connect( System.deviceID() );
//        delay( 5000 );
//        if ( mqtt_client.isConnected() )
//            mqtt_client.publish( MQTT_TOPIC, m );
//    }
}

void log_msg( String m ) {
    m.toCharArray( msg, 512 );
    log_msg( msg );
}

int device_number( String device_id ) {
    int dev_number = 0;
    
    if( device_id == "280022000851353531343431" )
        dev_number = 1;
    else if( device_id == "31003e001247353136383631" )
        dev_number = 2;
    else if( device_id == "2a005b000d51353532343635" )
        dev_number = 6;
    else if( device_id == "3e002d001147343438323536" )
        dev_number = 7;
    else if( device_id == "260032001147373036343935" )
        dev_number = 8;
    else if( device_id == "4e0048000351353530373132" )
        dev_number = 9;
    return( dev_number );
}

#if ( PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION )
int get_cell_usage() { 
    CellularData data;
    if (!Cellular.getDataUsage(data))
        log_msg( "Error! Not able to get cell usage data." );
    return( data.tx_session + data.rx_session );
}
#endif
