#include <modbus/modbus.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main( int argc, char** argv ) {
	while( 1 ) {
		modbus_t	*mb;
		uint16_t	tab_reg[32];

		mb = modbus_new_tcp( "10.0.0.32", 502 );
		modbus_connect( mb );

		int rc = modbus_read_input_registers( mb, 0, 11, tab_reg );
		if ( rc == -1 ) {
   			fprintf( stderr, "Error: %i, %s\n", errno, modbus_strerror(errno) );
		}
		else {
   			fprintf( stdout, "%li, %f, %f\n", time(0), modbus_get_float_badc( &tab_reg[6] ), modbus_get_float_badc( &tab_reg[8] ) );
			fflush(0);
		}
		modbus_close( mb );
		modbus_free( mb );
		sleep(10);
	}
}