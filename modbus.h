//
//  main.h
//  modbus_post
//
//  Created by Michael Koppelman on 2/12/17.
//  Copyright © 2017 Badger Hill Brewing. All rights reserved.
//

#ifndef main_h
#define main_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "mreadfile.h"
#include "sql.h"

extern sqlite3 *db;
extern char    *db_name;

typedef struct emerson_data {
    long    t;          // time in seconds since the epoch
    float   cltp;       // volume of cold liquor tank in InchesOfH2O
    float   cltt;       // temp of cold liquor tank in ˚F
    float   hltp;       // volume of hot liquor tank in InchesOfH2O
    float   hltt;       // temp of hot liquor tank in ˚F
    float   mflowdv;    // Mag flow meter flow rate
    float   mflowv;     // Mag flow meter totalized flow
    float   mflowt;     // Mag flow meter temp
} emerson_data;

int read_and_insert_file(void);
int get_emerson_data( emerson_data *edata );
void parse_emerson_data( uint16_t data[32], emerson_data *edata );
void create_sqlite_table(void);
int insert_emerson_data( emerson_data *edata );
void print_emerson_data ( emerson_data *edata );
//int read_from_serial();
void print_modbus_data( uint16_t data[32], int nnum );
void get_gateway_time( modbus_t *mb, char* date_string );

#endif /* main_h */

