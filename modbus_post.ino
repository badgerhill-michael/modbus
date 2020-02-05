// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"
#include "modbus-post.h"
#include "modbus_functions.h"

//STARTUP( System.enableFeature(FEATURE_RETAINED_MEMORY) );

/////////////////////////////////////////////////////////////////////////////////////
// Gas mass flow meter
int gas_flow_meter_id = 255;
ModbusMaster    nodeGF( gas_flow_meter_id );

// Gas mass flow meter
int gas_flow_meter2_id = 254;
ModbusMaster    nodeGF2( gas_flow_meter2_id );

// CO2 Watchdog
int co2_watchdog_modbus_id = 1;
ModbusMaster    nodeCO2watchdog( co2_watchdog_modbus_id ); 

// Emerson Smart Gateway
int smart_gateway_modbus_id = 2;
ModbusMaster    nodeSG( smart_gateway_modbus_id ); 

// CO2 Watchdog 2
int co2_watchdog_modbus_id2 = 3;
ModbusMaster    nodeSO03( co2_watchdog_modbus_id2 ); 

// Humidity Sensor 2
ModbusMaster    nodeHum2( 4 ); 

// Humidity Sensor
int humidity_modbus_id = 5;
ModbusMaster    nodeHumidity( humidity_modbus_id ); 

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

int tempc_modbus_id_22 = 22;
ModbusMaster    nodeSO22( tempc_modbus_id_22 ); 

int tempc_modbus_id_23 = 23;
ModbusMaster    nodeSO23( tempc_modbus_id_23 ); 

int tempc_modbus_id_24 = 24;
ModbusMaster    nodeSO24( tempc_modbus_id_24 ); 

int tempc_modbus_id_25 = 25;
ModbusMaster    nodeSO25( tempc_modbus_id_25 ); 

int tempc_modbus_id_34 = 34;
ModbusMaster    nodeSO34( tempc_modbus_id_34 ); 

int tempc_modbus_id_35 = 35;
ModbusMaster    nodeSO35( tempc_modbus_id_35 ); 

int tempc_modbus_id_36 = 36;
ModbusMaster    nodeSO36( tempc_modbus_id_36 ); 

int tempc_modbus_id_37 = 37;
ModbusMaster    nodeSO37( tempc_modbus_id_37 ); 

/////////////////////////////////////////////////////////////////////////////////////

String          response = "";
char            msg[512];
int             err_count=0;
long int        err_timer=0;
String          endpoint = "modbus.badgerhillbrewing.com";
String          url;
String          version;
int             loop_time = 0;
int             last_loop = 0;
int             ideal_loop_time = 100000;
TCPClient       client;

//BH-photon-09

retained int    delay_time          = 80000;
retained int    temp_enabled        = 1;
retained int    co2_enabled         = 0;
retained int    gateway_enabled     = 1;
retained int    humidity_enabled    = 0;
retained int    density_enabled     = 0;
retained int    ph_enabled          = 0;

//BH-argon-03
/*
int     delay_time          = 90000;
int     temp_enabled        = 0;
int     co2_enabled         = 0;
int     gateway_enabled     = 0;
int     humidity_enabled    = 0;
int     density_enabled     = 0;
int     ph_enabled          = 0;
 */
 
//BH-photon-15
/*
retained int    delay_time          = 30000;
retained int    temp_enabled        = 1;
retained int    co2_enabled         = 0;
retained int    gateway_enabled     = 0;
retained int    humidity_enabled    = 0;
retained int    density_enabled     = 0;
retained int    ph_enabled          = 0;
 */
//BH-photon-11
/*
retained int    delay_time          = 88000;
retained int    temp_enabled        = 0;
retained int    co2_enabled         = 1;
retained int    gateway_enabled     = 0;
retained int    humidity_enabled    = 0;
retained int    density_enabled     = 0;
retained int    ph_enabled          = 0;
 */
//BH-photon-14
//retained int    delay_time          = 90000;
//retained int    temp_enabled        = 1; // Only nodeSO37
//retained int    co2_enabled         = 0;
//retained int    gateway_enabled     = 0;
//retained int    humidity_enabled    = 0;
//retained int    density_enabled     = 0;
//retained int    ph_enabled          = 0;

//BH-electron-01
/*retained int    delay_time          = 300000;
retained int    temp_enabled        = 0;
retained int    co2_enabled         = 0;
retained int    gateway_enabled     = 0;
retained int    humidity_enabled    = 0;
retained int    density_enabled     = 0;
retained int    ph_enabled          = 0;
 */
 
/*int cloudTempEnabled( String set_point );
int cloudCO2Enabled( String set_point );
int cloudGatewayEnabled( String set_point );
int cloudHumidEnabled( String set_point );
int cloudDensEnabled( String set_point );
int cloudpHEnabled( String set_point );
int cloudModbusRead( String params );
int cloudModbusWrite( String params );

uint32_t convert_ints_to_int( uint16_t* w );
 */
void setup() {
    System.enableReset();
    Particle.syncTime();

#if ( PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION || PLATFORM_ID == PLATFORM_ARGON )
    int rtn = WiFi.selectAntenna(ANT_EXTERNAL);
    if( rtn == -1005 ) {
        log_msg( String("External antenna not found!" ) );
        WiFi.selectAntenna(ANT_INTERNAL);
    }
#endif

    version = String( System.deviceID() );

    int bps = 9600;

//    nodeSO10.begin(bps);
    nodeGF2.begin(bps);
/*
    nodeSO37.begin(bps);
    nodeTEMP.enableTXpin(D4);
    nodeGF.begin(bps);
    nodeGF2.begin(bps);
    nodeSG.begin(bps);
    nodeSO10.begin(bps);
    nodeHumidity.begin(bps);
 */    
    url = "GET /post.php?accessKey=Fu47tMl9H2FOsAfdHJ6RFeJi2PQR7Lm6&bucketKey=KTYPDWRBG8S8";

#if ( PLATFORM_ID == PLATFORM_XENON || PLATFORM_ID == PLATFORM_ARGON )
    if( ! Mesh.ready() ) {
        Mesh.connect();
    }
    log_msg( "Mesh IP: " + Mesh.localIP().toString() );
    Mesh.subscribe( "meshbus", subscriptionHandler );
    Mesh.subscribe( "pcloud", subscriptionHandler );
    Mesh.subscribe( "door_closed", subscriptionHandler );
#endif

    Particle.function( "setLoop", cloudSetLoopTime );
    Particle.variable( "loopTime", ideal_loop_time );

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

//    Particle.function( "DensON", cloudDensEnabled );
//    Particle.variable( "density_on", density_enabled );

    Particle.function( "pHON", cloudpHEnabled );
    Particle.variable( "ph_enabled", ph_enabled );

    Particle.variable( "err_count", err_count );

    loop_time = millis() - delay_time;
    
	report_connection_stats( msg );
    log_msg( String(msg) + " " + version );
}

void myHandler(const char *event, const char *data) {
    String d = data;
}

void loop() {
    String s;
    loop_time = tune_loop_time( loop_time );
    int signal_strength = report_connection_stats( msg );
#if ( PLATFORM_ID == PLATFORM_XENON )
    Mesh.publish( "meshbus", String::format( "%02i,%s,%is,%is", device_number(System.deviceID()), last_loop/1000, delay_time/1000 ) );
#else
    Particle.publish( "ModbusMonitor", String::format( "%02i,%is,%is,%idb", device_number(System.deviceID()), last_loop/1000, delay_time/1000, signal_strength  ) );
#endif

	String get_data_string = "&reported_by=" + version;
	String values = get_data_string + "&date_recorded=" + String(Time.now()) + "&system_name=particle&system_id="
	    + String(device_number(System.deviceID())) + "&WiFi=" + String( signal_strength );
	post_to_server( values );

    if( co2_enabled ) {
        gas_flow co2_meter = get_gas_flow( nodeGF );
        if( co2_meter.v > -999.9 ) {
            float temporary_float = co2_meter.v;
            values = "system_name=co2_flow&system_id=" + String(gas_flow_meter_id) + "&v="
                + String::format( "%0.3f", temporary_float/1000.0 ) + "&dv=" + String::format( "%0.3f", co2_meter.dv );
            float plato = get_density_from_gas_flow( nodeGF );
            if( plato > -999.9 )
                values += "&plato=" + String::format( "%0.3f", plato );
            s = get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values;
            post_to_server( s );
       }
    }

/*    if( density_enabled ) {
        devil_info density_sensor = get_devil_info( nodeDEVIL );
        if( density_sensor.T > -999.9 ) {
            String values = "system_name=devil&system_id=1&T=" + String(density_sensor.T) + "&density=" + String(density_sensor.rho);
            post_to_server( get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values );
//            log_msg( values );
        }
    }
 */
    
    if( gateway_enabled ) {
        emerson_data *edata = (emerson_data *)calloc( 1, sizeof(emerson_data) );

        int rtn = get_emerson_data( edata );
        if( rtn == 0 ) {
            values = "system_name=SmartGateway&system_id=2&cltp=" + String::format( "%0.2f", edata->cltp) + 
                "&cltt=" + String::format( "%0.2f", edata->cltt ) +
                "&hltp=" + String::format( "%0.2f", edata->hltp ) +
                "&hltt=" + String::format( "%0.2f", edata->hltt ) +
                "&fvp=" + String::format( "%0.2f", edata->fvp ) +
                "&fvt=" + String::format( "%0.2f", edata->fvt ) +
                "&mflowdv=" + String::format( "%0.2f", edata->mflowdv ) +
                "&mflowv=" + String::format( "%0.2f", edata->mflowv ) +
                "&mflowt=" + String::format( "%0.2f", edata->mflowt ) +
                "&mflow2dv=" + String::format( "%0.2f", edata->mflow2dv ) +
                "&mflow2v=" + String::format( "%0.2f", edata->mflow2v ) +
                "&mflow2t=" + String::format( "%0.2f", edata->mflow2t ) +
                "&phyg1p=" + String::format( "%0.3f", edata->phyg1p ) +
                "&phyg1t=" + String::format( "%0.2f", edata->phyg1t );
//                "&forkwet=" + String::format( "%0.2f", edata->forkwet) +
//                "&forkhz=" + String::format( "%0.2f", edata->forkhz) +
//                "&forkt=" + String::format( "%0.2f", edata->forkt ); 
            s = get_data_string + "&date_recorded=" + String(Time.now()) + "&" + values;
            post_to_server( s );
        }
//        delay(delay_time);
        free(edata);
    }
    
    if( humidity_enabled ) {
        humidity humidity_meter = get_humidity( nodeHumidity );
        if( humidity_meter.T > -999.9 ) {
            values = "&date_recorded=" + String(Time.now()) + "&system_name=humidity&system_id=" + String(humidity_modbus_id) + "&humidity="
                + String::format( "%0.1f", humidity_meter.humidity) + "&T=" + String::format( "%0.2f", humidity_meter.T );
            post_to_server( get_data_string + values );
        }
    }

    if( ph_enabled ) {
        ph ph_meter = get_ph( nodeSO03 );
        if( ph_meter.T > -999.9 ) {
            values = "&date_recorded=" + String(Time.now()) + "&system_name=ph&system_id=3&ph="
                + String(ph_meter.ph) + "&T=" + String(ph_meter.T);
            post_to_server( get_data_string + values );
        }
    }

    if( temp_enabled ) {
// Start BH-photon-09
        read_and_post_tempc(  co2_watchdog_modbus_id,   nodeCO2watchdog,    get_data_string );
        read_and_post_tempc(  co2_watchdog_modbus_id2,  nodeSO03,           get_data_string );
// End BH-photon-09
 // Start BH-photon-15
/*
        read_and_post_tempc( tempc_modbus_id_10, nodeSO10, get_data_string );
        read_and_post_tempc( tempc_modbus_id_11, nodeSO11, get_data_string );
        read_and_post_tempc( tempc_modbus_id_12, nodeSO12, get_data_string );
        read_and_post_tempc( tempc_modbus_id_13, nodeSO13, get_data_string );
        read_and_post_tempc( tempc_modbus_id_14, nodeSO14, get_data_string );
        read_and_post_tempc( tempc_modbus_id_15, nodeSO15, get_data_string );
        read_and_post_tempc( tempc_modbus_id_16, nodeSO16, get_data_string );
        read_and_post_tempc( tempc_modbus_id_19, nodeSO19, get_data_string );
        read_and_post_tempc( tempc_modbus_id_20, nodeSO20, get_data_string );
        read_and_post_tempc( tempc_modbus_id_25, nodeSO25, get_data_string );
 */
// Start BH-photon-15
//        read_and_post_tempc( tempc_modbus_id_17, nodeSO17, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_18, nodeSO18, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_21, nodeSO21, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_22, nodeSO22, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_23, nodeSO23, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_24, nodeSO24, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_34, nodeSO34, get_data_string );
//        read_and_post_tempc( tempc_modbus_id_35, nodeSO35, get_data_string );
//      read_and_post_tempc( tempc_modbus_id_36, nodeSO36, get_data_string );
//      read_and_post_tempc( tempc_modbus_id_37, nodeSO37, get_data_string );

    }
    // FIXME if $carbo
    /*
   float new_temp = get_temp( co2_watchdog_modbus_id2, 98 );
    if( new_temp > -999.9 )
        values = "&date_recorded=" + String(Time.now()) + "&system_name=carbo&system_id=" + String(device_number(System.deviceID())) + "&P=" + String::format( "%0.2f", new_temp*0.145038 );
    delay(1000);
    
    new_temp = get_temp( co2_watchdog_modbus_id2, 0 );
    if( new_temp > -999.9 )
        values += "&SP=" + String::format( "%0.2f", new_temp*0.145038 );
    post_to_server( get_data_string + values );
     */

    check_error_state();
    if( Time.minute() % 15 == 0 ) {
        log_msg( "Syncing Time" );
        Particle.syncTime();
    }
    delay_with_process( delay_time );
}

void delay_with_process( int d ) {
    int t0 = millis();
    while( millis()-t0 < d ) {
        Particle.process();
        delay( 1000 );
    }
}

void read_and_post_tempc( int id, ModbusMaster node, String get_data_string ) {
    temp_info   temp_ctl1 = get_temp_info( node );
    String      system_name = "tempc";
    String      parameter_name = "T";
    
    if( temp_ctl1.decimals > 1 ) {
        temp_ctl1.T = temp_ctl1.T / (float)(10^(temp_ctl1.decimals-1));
        temp_ctl1.SP = temp_ctl1.SP / (float)(10^(temp_ctl1.decimals-1));
    }
 
    if( temp_ctl1.T > 0 ) {
        if( id == co2_watchdog_modbus_id || id == co2_watchdog_modbus_id2 ) {
            system_name = "co2c";
            parameter_name = "P";
        }
        String values = "&date_recorded=" + String(Time.now()) +
                        "&system_name=" + system_name + "&system_id=" + String(id) +
                        "&" + parameter_name + "=" + String::format( "%0.1f", temp_ctl1.T ) +
                        "&SP=" + String::format( "%0.1f", temp_ctl1.SP );
        if( temp_ctl1.OUT1 >= 0.0 && temp_ctl1.OUT1 <= 100.0 )
            values += "&OUT1=" + String::format( "%0.0f", temp_ctl1.OUT1);
        if( temp_ctl1.Alarm1 == false || temp_ctl1.Alarm1 == true )
            values += "&Alarm1=" + String(temp_ctl1.Alarm1);
        String post_data = get_data_string + values;
        post_to_server( post_data );
    }
    else {
        log_msg( "Failed for id=" + String(id) );
        delay(1000);
    }
}

int get_emerson_data( emerson_data *edata ) {
    uint8_t         result;
    uint16_t        data[34];
    char            err[256];
    
    int qty  = 34;
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
        log_msg( String::format("SmartGateway Error: %i (%X): %s", result, result, err) );
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
//  edata->forkwet  = convert_ints_to_float( &data[22] );
//  edata->forkhz   = convert_ints_to_float( &data[24] );
//  edata->forkt    = convert_ints_to_float( &data[26] );
    edata->mflow2dv = convert_ints_to_float( &data[28] );
    edata->mflow2v  = convert_ints_to_float( &data[30] );
    edata->mflow2t  = convert_ints_to_float( &data[32] );
}

int tune_loop_time( int loop_time ) {
    last_loop = millis() - loop_time;
    loop_time = millis();

    int new_delay_time = 0;
    if( last_loop > ideal_loop_time*1.01 ) {
        new_delay_time = delay_time - ((last_loop - ideal_loop_time)/2);
        if( new_delay_time < 0 )
            new_delay_time = 0;
         cloudSetDelayTime( String( new_delay_time  ) );
    }
    else if( last_loop < ideal_loop_time*0.99 ) {
        new_delay_time = delay_time + ((ideal_loop_time - last_loop)/2);
        cloudSetDelayTime( String( new_delay_time  ) );
    }
    return( loop_time );
}

int cloudSetDelayTime( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 ) {
        delay_time = new_value;
//        log_msg( String::format( "Set delay_time to %i", delay_time ) );
        return( 1 );
    }
    else {
        log_msg( String::format( "Error: delay_time %i is not >= 0. Ignoring.",  new_value ) );
        return(-1);
    }
}

int cloudSetLoopTime( String set_point ) {
    int new_value = set_point.toInt();
    if( new_value >= 0 ) {
        ideal_loop_time = new_value;
        cloudSetDelayTime( String(ideal_loop_time * 0.9) );
        log_msg( String::format( "Set loop time to %i", ideal_loop_time ) );
        return( 1 );
    }
    else {
        log_msg( String::format( "Error: ideal_loop_time %i is not >= 0. Ignoring.",  new_value ) );
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
    int address = a.toInt()-1;
    int qnty    = q.toInt();
    
    ModbusMaster node = systemID2node( system_id );
    uint8_t         result;
    uint16_t        addr, qty, data[qnty];
    char            err[256];
    float        new_reading;
    
    new_reading = -999.9;

    addr = (uint16_t)address;
    qty = (uint16_t)qnty;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
//        new_reading = (float)data[0];
//        new_reading += (float)data[1]/65536.0;
//        sprintf( msg, "system_id %i, addr %i = %f", system_id, addr, new_reading );
//        log_msg( msg );
        for( int n = 0; n < qnty; ++n ) {
            new_reading  = (float)data[n];
            sprintf( msg, "system_id %i, addr %i = %f", system_id, addr+n, new_reading );
            log_msg( msg );
        }
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        log_msg( String::format( "Error: %i (%i,%i): %s", result, system_id, addr, err) );
    }
    return(1);
}

int cloudModbusWrite( String params ) {
    uint8_t         result, qty;
    uint16_t        addr;
    char            err[256];

    String s = params.substring( 0, params.indexOf( ":" ) );
    String a = params.substring( params.indexOf( ":" )+1, params.indexOf( ":", s.length()+2 )+1 );
    String v = params.substring( params.lastIndexOf( ":")+1 );
    
    int system_id = s.toInt();
    ModbusMaster node = systemID2node( system_id );

    int address = a.toInt()-1;
    addr = (uint16_t)address;

    if( v.lastIndexOf( "." ) > 0 )
        qty = 2;
    else
        qty = 1;

    uint16_t        data[qty];

    if( qty == 1 ) {        
        int value    = v.toInt();
        data[0] = (uint16_t)value;

        result = put_modbus_datum( node, addr, data[0] );
        if ( result == node.ku8MBSuccess ) {
            log_msg( String::format("Address %i set to %i on system_id %i", addr+1, data[0], system_id ) );
        }
        else {
            get_modbus_error( node, result, err );
            log_msg( String::format("Error: %i (%X): %s", result, result, err) );
            return(-1);
        }
    }
    else if( qty == 2 ) {        
        float value = v.toFloat();
        convert_float_to_ints( value, &data[0] );
        for( int n = 0; n < qty; ++n ) {
            result = put_modbus_datum( node, addr+n, data[n] );
            if ( result == node.ku8MBSuccess ) {
                log_msg( String::format("Address %i set to %i on system_id %i", addr+n, data[n], system_id) );
            }
            else {
                get_modbus_error( node, result, err );
                log_msg( String::format("Err system_id=%i:%i: %i: %s", system_id, addr+n, result, err) );
                return(-1);
            }
        }
    }
    return(1);
}

int cloudSetTempSetPoint( String command ) {
    String sid = command.substring( 0, command.indexOf( ":" ) );
    String sp  = command.substring( command.indexOf( ":" )+1 );
    int system_id = sid.toInt();
    float new_value = sp.toFloat();
    if( new_value >= -10 && new_value <=212 ) {
        set_temp_setpoint( system_id, new_value );
        return( 1 );
    }
    else
        return(-1);
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

ModbusMaster systemID2node( int system_id ) {
    ModbusMaster    node;

    if( system_id == co2_watchdog_modbus_id )
        node = nodeCO2watchdog;
    else if( system_id == smart_gateway_modbus_id )
        node = nodeSG;
    else if( system_id == co2_watchdog_modbus_id2 )
        node = nodeSO03;
    else if( system_id == humidity_modbus_id )
        node = nodeHumidity;
    else if( system_id == 4 )
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
    else if( system_id == tempc_modbus_id_22 )
        node = nodeSO22;
    else if( system_id == tempc_modbus_id_23 )
        node = nodeSO23;
    else if( system_id == tempc_modbus_id_24 )
        node = nodeSO24;
    else if( system_id == tempc_modbus_id_25 )
        node = nodeSO25;
    else if( system_id == tempc_modbus_id_34 )
        node = nodeSO34;
    else if( system_id == tempc_modbus_id_35 )
        node = nodeSO35;
    else if( system_id == tempc_modbus_id_36 )
        node = nodeSO36;
    else if( system_id == tempc_modbus_id_37 )
        node = nodeSO37;
    else if( system_id == gas_flow_meter_id )
        node = nodeGF;
    else if( system_id == gas_flow_meter2_id )
        node = nodeGF2;

    return(node);
}

int report_connection_stats( char* s ){
#if ( PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION || PLATFORM_ID == PLATFORM_ARGON )
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

#if ( PLATFORM_ID == PLATFORM_XENON )
    int signal_strength = 0;
    char* ssid          = "BlueTooth";
    IPAddress ip        = Mesh.localIP();
#endif

    sprintf( s, "%s %idb %i.%i.%i.%i", ssid, signal_strength, ip[0], ip[1], ip[2], ip[3] );
    return( signal_strength);
}

void check_error_state() {
    if( err_count > 0 ) {
        if( err_timer == 0 )
            err_timer = Time.now();
        else if ( Time.now() - err_timer < 300 ) {
            if( err_count > 100 ) {
                log_msg( String::format( "Too many errors (%i), rebooting...", err_count ) );
                err_count = 0;
                err_timer = 0;
                delay(10000);
                System.reset();
            }
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
            delay_with_process(1000);
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
    }
    delay_with_process(500);
    
    addr = 4115; // Output 1 in %
    qty = 1;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading.OUT1  = data[0]/10.0;
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
    }
    delay_with_process(500);

    addr = 4139; // Alarm1 Status
    qty = 1;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess )
        new_reading.Alarm1 = isBitISet( data[0], 4 );
    else {
        err_count++;
        get_modbus_error( node, result, err );
    }
    
    addr = 4120; // Decimals
    qty = 1;

    result = get_modbus_data4( node, data, addr-1, qty );
    if ( result == node.ku8MBSuccess )
        new_reading.decimals = (int)data[0];
    else {
        err_count++;
        get_modbus_error( node, result, err );
    }
    delay_with_process(500);

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
        sprintf( msg, "Humidity Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

float get_temp( ModbusMaster node, uint16_t addr ) {
    uint8_t         result;
    uint16_t        qty, data[2];
    char            err[256];
    float           new_reading;
    
    new_reading   = -999.9;

//    addr = 98;
    qty  = 2;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading = (float)data[0];
        new_reading += (float)data[1]/65536.0;
     }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        sprintf( msg, "Temp error: %i (%i): %s", result, addr, err );
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
        sprintf( msg, "Ph Error: %i (%i): %s", result, addr, err );
        log_msg( msg );
    }
    return( new_reading );
}

float get_density_from_gas_flow( ModbusMaster node ) {
    uint8_t         result;
    uint16_t        addr, qty, data[4];
    char            err[256];
    float           new_reading;

    new_reading = -999.9;
    
    addr = 32;
    qty = 2;

    result = get_modbus_data4( node, data, addr, qty );
    if ( result == node.ku8MBSuccess ) {
        new_reading = ( data[0] * 65536 + data[1] ) /1000.0;
    }
    else {
        err_count++;
        get_modbus_error( node, result, err );
        log_msg( String::format("Error: %i (%i): %s", result, addr, err ) );
    }
    delay_with_process(1000);
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
        log_msg( String::format("Error: %i (%i): %s", result, addr, err) );
    }
    delay_with_process(1000);
    return( new_reading );
}

bool isBitISet( uint16_t data, int i ) {
   uint16_t mask = (uint16_t)( 1 << i ) ;
   return( mask & data );   
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

void convert_float_to_ints( float w, uint16_t *ints ) {
    memcpy( ints, &w, sizeof(float) );
    return;
}

void post_to_server( String get_parameters ) {
#if ( PLATFORM_ID == PLATFORM_XENON )
    Mesh.publish( "meshbus", get_parameters );
#else
    Particle.publish( "modbus_post", get_parameters.substring(  1, get_parameters.length() ), PRIVATE );
    if ( client.connect( endpoint, 80) ) {
        String get_data_string = String( url );
        get_data_string.concat( get_parameters );
        get_data_string.concat( " HTTP/1.1\r\n" );
        get_data_string.concat( "Host: " + endpoint + "\r\n" );
        get_data_string.concat( "User-Agent: Arduino\r\n" );
        get_data_string.concat( "Accept-Version: */*\r\n\r\n" );
        client.println( get_data_string );
        get_server_response( client );
    }
    else {
        err_count++;
        log_msg( "Connection failed: " + get_parameters );
    }
#endif
}

void get_server_response( TCPClient connection ) {
    char        *buffer;
    size_t      sz = 512;

    long t0 = millis();
    buffer = (char *) calloc( 1, sz );
    while( connection.connected() && (millis()-t0 < 3000) ) {
        int new_bytes = connection.available();
        if( new_bytes > 0 ) {
            connection.read( (uint8_t *)buffer, new_bytes );
            String s = String( buffer );
            response.concat( s );
        }
        Particle.process();
    }
    connection.stop();
    response = "";
    free(buffer);
}

void log_msg( char* m ) {
    Particle.publish( "ModbusMonitor", m, 300, PRIVATE );
}

void log_msg( String m ) {
    Particle.publish( "ModbusMonitor", m, 300, PRIVATE );
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
    else if( device_id == "39001a000f47363333343437" )
        dev_number = 10;
    else if( device_id == "2f003f000b47363339343638" )
        dev_number = 11;
    else if( device_id == "340038000447363339343638" )
        dev_number = 12;
    else if( device_id == "2c0019000447363332363639" )
        dev_number = 13;
    else if( device_id == "310029001847343438323536" )
        dev_number = 14;
    else if( device_id == "31002c000a47363339343638" )
        dev_number = 15;
    else if( device_id == "2b0031000947373034353237" )
        dev_number = 16;
    else if( device_id == "e00fce684397d6403e234fe4" )
        dev_number = 17;
    else if( device_id == "e00fce680ddf8db402b192b6" )
        dev_number = 18;
    else if( device_id == "e00fce689565818273c68291" )
        dev_number = 22;
    else if( device_id == "e00fce6879cb62b64c26439d" )
        dev_number = 23;
    else if( device_id == "e00fce68c1e635f18c131eb4" )
        dev_number = 26;
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

void subscriptionHandler( const char *event, const char *data ) {
    String d = String(data);
    String e = String(event);
    
    if( e.equals( "meshbus" ) ) {
//        Particle.publish( "Debug", "'" +  d.substring( 0, 11) + "'" );
        if( d.substring( 0, 11).equals( "reported_by" ) ) {
            post_to_server( "&" + d );
//            Particle.publish( "Gateway", d.substring( d.indexOf( "system_name" ) ) );
        }
    }
    else if( e.equals( "door_closed" ) ) {
        Particle.publish( e, d );
//        pinMode( D0, OUTPUT );
//        pinMode( D1, OUTPUT );
        if( d.equals( "true" ) ) {
            RGB.control(true);
            RGB.color( 0, 127, 0 );
//            digitalWrite( D0, LOW );
//            digitalWrite( D1, HIGH );
        }
        else {
            RGB.control(true);
            RGB.color( 127, 0, 0 );
//            digitalWrite( D0, HIGH );
//            digitalWrite( D1, LOW );
        }
    }
    else
        Particle.publish( "ModbusMonitor", e + ":" + d );
}
