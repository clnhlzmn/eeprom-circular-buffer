# An implementation of Microchip app note AVR101

See [AVR101](http://ww1.microchip.com/downloads/en/AppNotes/doc2526.pdf) for details.

This library implements a circular buffer for eeprom memory. The API exposes an interface to read and write a chunk of bytes. Every write is performed in a new location in the buffer to avoid wearing out the eeprom. For example, if I call `ee_cb_init` with `buffer_size` set to 32 then the first 32 calls to `ee_cb_write` will use new locations for the writes. After the 32nd call the write location will wrap to the beginning of the buffer again. To make this work the location of the last written data must also be saved to the eeprom on every write. To avoid wearing that location it also uses a circular buffer.

### The algorithm

Assume `ee_cb_init` has been called with `data_size` set to 1 and `buffer_size` set to 2. A total of `(data_size + 1) * buffer_size` (4) bytes of eeprom is required.

We reserve 4 bytes in eeprom, and after a chip erase or programmatic initialization it looks like this: 

```
0: FF
1: FF
2: FF
3: FF
```

Addresses 0-1 will be used to store our data bytes, and addresses 2-3 will be used to store the read/write index.

... to be continued ...