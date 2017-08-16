// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"

int             slave_id = 2;
ModbusMaster    node( slave_id );
char            msg[512];
int             delay_time = 28000;

#define MYFLOAT(hi, lo) ((double) ((hi) << 16 | (lo)))
#define MYINT(hi,lo) ( (uint16_t)((hi << 8 ) + lo ) )
#define MYDBL(hi,lo) ( (double)((hi << 16 ) + lo ) )

void setup() {
//    Particle.variable("msg", msg );

// initialize Modbus communication baud rate
	node.begin(9600);
	node.enableTXpin(D7);
//  node.enableDebug();  //Print TX and RX frames out on Serial. Beware, enabling this messes up the timings for RS485 Transactions, causing them to fail.
	sprintf( msg, "%s", "Starting Modbus Interface" );
	log_msg( msg );
}


void loop() {
	uint8_t         result;
	uint16_t        data[16];
	char            datestring[256];
    char            err[256];

    delay( delay_time );
    get_gateway_time( datestring );
    if( strlen(datestring) > 0 )
        sprintf( msg, "Smart Gateway Time: %s", datestring );
    else
        sprintf( msg, "Error talking to slave %i. delay_time = %i", slave_id, delay_time );
    log_msg( msg );

    get_constant_float();
    delay( delay_time );
/*    
    int qty = 8;
    result = get_modbus_data3( data, 0, qty );
    if ( result == node.ku8MBSuccess ) {
        for ( int j = 0; j < qty; j++ ) {
			sprintf( msg, "%X = %X + %X (%i)", data[j], highByte(data[j]), lowByte(data[j]), data[j] );
			log_msg( msg );
		}
    }
    else {
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
 */
 
//	sprintf( msg, "%s", "Done!" );
//	log_msg( msg );
//    delay( delay_time );
}

void get_modbus_error( uint8_t result, char* err ) {
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
	return( result );
}

uint8_t get_modbus_data4( uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     result;

    result = node.readHoldingRegisters( maddress, mqty );
    if ( result == node.ku8MBSuccess ) {
		for ( int j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	return( result );
}

/*
    49001	Current Year		
	49002	Current Month		
	49003	Current Day		
	49004	Current Hour		
	49005	Current Minute		
	49006	Current Second		
	49007	Messages Received		
	49008	Corrupt Messages Received		
	49009	Messages Sent with Exception		
	49010	Messages Sent Count		
	49011	Valid Messages Ignored		
	49012	Constant Float 12345.0
 */
 
void get_constant_float() {
    uint8_t         result;
    uint16_t        addr, qty, data[32];
    char            err[256];

    addr = 9007;
    qty = 8;
    result = get_modbus_data4( data, addr-1, qty );
    if ( result == node.ku8MBSuccess ) {
/*
        sprintf( msg, "%i %s %i", 49001, "Current Year",      data[0] );
     	log_msg( msg );

 	    sprintf( msg, "%i %s %i", 49002, "Current Month",     data[1] );
     	log_msg( msg );

        sprintf( msg, "%i %s %i", 49003, "Current Day",       data[2] );
     	log_msg( msg );

        sprintf( msg, "%i %s %i", 49004, "Current Hour",      data[3] );
     	log_msg( msg );

        sprintf( msg, "%i %s %i", 49005, "Current Minute",    data[4] );
     	log_msg( msg );

        sprintf( msg, "%i %s %i", 49006, "Current Second",    data[5] );
     	log_msg( msg );
 */
        sprintf( msg, "\t%i %s %i", 49007, "Messages Received", data[0] );
     	log_msg( msg );

        sprintf( msg, "\t%i %s %i", 49008, "Corrupt Messages Received", data[1] );
     	log_msg( msg );

        sprintf( msg, "\t%i %s %i", 49010, "Messages Sent",     data[3] );
     	log_msg( msg );

//  sprintf( msg, "\t%i %s %i", 49012, "Constant Float 12345.0",     data[5] );
//  log_msg( msg );
    }
    else {
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
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
        sprintf( date_string, "%s", "" );
        get_modbus_error( result, err );
        sprintf( msg, "Error: %i (%X): %s", result, result, err );
        log_msg( msg );
    }
}

void log_msg( char* msg ) {
    Serial.println( msg );
    Particle.publish( "modbus", msg );
    delay( 1000 );
}
