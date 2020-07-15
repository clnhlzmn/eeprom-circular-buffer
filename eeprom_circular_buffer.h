/*
 * eeprom_circular_buffer.h
 * An implementation of AVR101 app note
 * Used to spread writes to eeprom over N
 * locations to wear the eeprom more slowly.
 * Use ee_cb_init to initialize a struct ee_cb
 * with reader and writer functions, the size
 * of data to be stored, and the number of eeprom
 * locations to use for storage.
 *
 * Created: 7/14/2020 8:50:33 AM
 *  Author: Colin
 */ 


#ifndef EEPROM_CIRCULAR_BUFFER_H_
#define EEPROM_CIRCULAR_BUFFER_H_

#include <stdint.h>
#include <stdlib.h>

#define EE_CB_MAX_BUFFER_SIZE (255)

/*Reader and writer function pointer types.
Functions matching these prototypes will be used to write and read to and from the eeprom*/
typedef void (*ee_cb_reader)(uint8_t *ee_addr, uint8_t *data_addr, size_t data_size);
typedef void (*ee_cb_writer)(uint8_t *ee_addr, const uint8_t *data_addr, size_t data_size);

/*eeprom circular buffer state*/
struct ee_cb {
    /*pointer to eeprom writer function*/
    ee_cb_writer writer;
    /*pointer to eeprom reader function*/
    ee_cb_reader reader;
    /*eeprom base address*/
    uint8_t *base_address;
    /*size of data elements in bytes*/
    size_t data_size;
    /*size of the buffer*/
    uint8_t buffer_size;
};

/*initialize a struct ee_cb
returns 0 on success and not zero otherwise
struct ee_cb must be initialized successfully before using with the other functions here
\param self pointer to struct ee_cb
\param base_address pointer to start of eeprom storage
\param data_size size of data to read and write
\param buffer_size number of eeprom locations to use
\param writer pointer to eeprom writer function
\param reader pointer to eeprom reader function
\note ee_cb uses (data_size + 1) * buffer_size bytes of eeprom
please ensure enough is available*/
int ee_cb_init(struct ee_cb *self, uint8_t *base_address, size_t data_size, 
    size_t buffer_size, ee_cb_writer writer, ee_cb_reader reader);

/*write the given parameter to the next position in the buffer*/
int ee_cb_write(struct ee_cb *self, const uint8_t *data);

/*read the currently stored parameter
returns 0 if no parameter has been written*/
int ee_cb_read(struct ee_cb *self, uint8_t *data);

#endif /* EEPROM_CIRCULAR_BUFFER_H_ */