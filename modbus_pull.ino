// This #include statement was automatically added by the Particle IDE.
#include "ModbusMaster.h"
// instantiate ModbusMaster object as slave ID 1
int             slave_id = 2;
ModbusMaster    node( slave_id );
char            msg[512];

#define MYFLOAT(hi, lo) ((double) ((hi) << 16 | (lo)))
#define MYDBL(hi,lo) ((double)((hi << 16 ) | (lo)))

void setup() {
	// initialize Modbus communication baud rate
	node.begin(9600);
    node.enableTXpin(D7); //D7 is the pin used to control the TX enable pin of RS485 driver
    node.enableDebug();  //Print TX and RX frames out on Serial. Beware, enabling this messes up the timings for RS485 Transactions, causing them to fail.
	
//	Serial.begin(9600);
//	while(!Serial.available()) Particle.process();
//	Serial.println("Starting Modbus Interface");
	log_msg( "Starting Modbus Interface" );
}


void loop() {
//	static uint32_t i;
	uint8_t         j, result;
	uint16_t        data[16];
	char            err[56];
	char            datestring[256];

    delay(2000);
    get_gateway_time( datestring );
    if( strlen(datestring) > 0 ) {
        sprintf( msg, "The date on slave %i is %s", slave_id, datestring );
        log_msg( msg );
    }
    delay(2000);
/*    int qty = 8;
    result = get_modbus_data3( data, 0, qty );//1000 44097 V10000
    if ( result == node.ku8MBSuccess ) {
        for ( int j = 0; j < qty; j++ ) {
			sprintf( msg, "%X = %X + %X (%li)", data[j], highByte(data[j]), lowByte(data[j]), data[j] );
			log_msg( msg );
		}
    }
    else {
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

        sprintf( msg, "Error: %li (%X): %s", result, result, err );
        log_msg( msg );
    }
    delay(2000);
 */
}

uint8_t get_modbus_data3( uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     j,
                result;

//    sprintf( msg, "%li Input Registers at %i from %i\n", mqty, maddress, slave_id );
//    log_msg( msg );

    result = node.readInputRegisters( maddress, mqty );

    if ( result == node.ku8MBSuccess ) {
		for ( j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	return( result );
}

uint8_t get_modbus_data4( uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     j,
                result;

//    sprintf( msg, "%li Holding Registers at %i from %i\n", mqty, maddress, slave_id );
//    log_msg( msg );

    result = node.readHoldingRegisters( maddress, mqty );

    if ( result == node.ku8MBSuccess ) {
		for ( j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	return( result );
}

void get_gateway_time( char* date_string ) {
    uint8_t         j, result;
    uint16_t        data[32];

    int qty = 7;
    result = get_modbus_data4( data, 9000, qty );//1000 44097 V10000
    if ( result == node.ku8MBSuccess ) {
        sprintf( date_string, "%02i/%02i/%i, %02i:%02i:%02i",
            data[1],
            data[2],
            data[0],
            data[3]-5,
            data[4],
            data[5]
        );
    }
    else
        sprintf( date_string, "%s", "" );
}

void log_msg( char* msg ) {
//    Serial.println( msg );
    Particle.publish( "modbus", msg );
    delay(1000);
}
