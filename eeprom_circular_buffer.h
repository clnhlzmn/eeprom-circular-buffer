/*
 * eeprom_circular_buffer.h
 * This is an implementation of Microchip app note AVR101.
 * It is used to spread writes to eeprom over N
 * locations to wear the eeprom more slowly.
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

/*ee_cb_write writes the given parameter to the next position in the buffer.
The eeprom must be initialized (all bytes equal value) 
before calling this function.*/
int ee_cb_write(struct ee_cb *self, const uint8_t *data);

/*
ee_cb_read reads the currently stored data.
If no parameter has been written the read data bytes will 
be equal to whatever value the eeprom was initialized with.
data_size bytes will be written to the location pointed to by data*/
int ee_cb_read(struct ee_cb *self, uint8_t *data);

#endif /* EEPROM_CIRCULAR_BUFFER_H_ */