# An implementation of Microchip app note AVR101

See [AVR101](http://ww1.microchip.com/downloads/en/AppNotes/doc2526.pdf) for details.

This library implements a circular buffer for eeprom memory. The API exposes an interface to read and write a chunk of bytes. Every write is performed in a new location in the buffer to avoid wearing out the eeprom. For example, if I call `ee_cb_init` with `buffer_size` set to 32 then the first 32 calls to `ee_cb_write` will use new locations for the writes. After the 32nd call the write location will wrap to the beginning of the buffer again. To make this work the location of the last written data must also be saved to the eeprom on every write. To avoid wearing that location it also uses a circular buffer.

# API

## `struct ee_cb`

To use this library you must create an instance of `struct ee_cb` and initialize it with `ee_cb_init`

## `ee_cb_init`

Use this function to initialize `struct ee_cb`.

* param `base_address`

    This is the eeprom address where you would like the buffer to be located. The buffer must be large enough to hold `(data_size + 1) * buffer_size` bytes.

* param `data_size`

    This is the size of the data you would like to read and write to the buffer. For example: if you wanted to store an `uint32_t` then `data_size` would be 4. 

* param `buffer_size` 
    
    This is the number of eeprom locations to use for storage. For example: if you your eeprom has an endurance of 100000 writes and you set `buffer_size` to 8, then you will be able to perform at least 800000 writes before you experience eeprom corruption. 
    
* params `reader` and `writer`
    
    These are pointers to functions that implement the actual reading and writing of the eeprom. They should match the prototypes given by `ee_cb_reader` and `ee_cb_writer`. 

## `ee_cb_read` and `ee_cb_write`

After successful initialization (`ee_cb_init` returned 0) the functions `ee_cb_read` and `ee_cb_write` can be used to read and write `data_size` bytes. 

Please note that the behavior of `ee_cb_write` and `ee_cb_read` is undefined if the eeprom is not initialized such that all locations have the same value. This is typically performed during programming. For example: an AVR microcontroller eeprom gets all bytes initialized to `0xFF` after a chip erase.