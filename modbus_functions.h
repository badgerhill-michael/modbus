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

uint8_t get_modbus_coils( ModbusMaster node, uint16_t data_buffer[], uint16_t maddress, uint16_t mqty ) {
    uint8_t     result;

    result = node.readCoils( maddress, mqty );
    if ( result == node.ku8MBSuccess ) {
		for ( int j = 0; j < mqty; j++ )
			data_buffer[j] = node.getResponseBuffer( j );
	}
	node.clearResponseBuffer();
    node.clearTransmitBuffer();
	return( result );
}

uint8_t put_modbus_datum( ModbusMaster node, uint16_t maddress, uint16_t mdata ) {
    uint8_t     result;
    
    result = node.writeSingleRegister( maddress, mdata );
    return( result );
}

uint8_t  put_modbus_coil( ModbusMaster node, uint16_t maddress, uint16_t mdata ) {
    uint8_t     result;
    result = node.writeSingleCoil( maddress, mdata );
    return( result );
}
