
#include "eeprom_circular_buffer.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

struct ee_cb cb;

typedef uint16_t data_type;

#define DATA_SIZE sizeof(data_type)
#define BUFFER_SIZE 255
#define MEM_SIZE ((DATA_SIZE) + 1) * BUFFER_SIZE
static uint8_t ee_mem[MEM_SIZE];

void ee_read(uint8_t *addr, uint8_t *dst, size_t size) {
    assert(addr >= ee_mem && addr < ee_mem + MEM_SIZE);
    memcpy(dst, addr, size);
}

void ee_write(uint8_t *addr, const uint8_t *src, size_t size) {
    assert(addr >= ee_mem && addr < ee_mem + MEM_SIZE);
    memcpy(addr, src, size);
}

int main(void) {
    assert(ee_cb_init(&cb, ee_mem, DATA_SIZE, BUFFER_SIZE, ee_write, ee_read) == 0);
    data_type my_data = 0xFF;
    assert(ee_cb_read(&cb, (uint8_t*)&my_data) == 0);
    assert(my_data == 0);
    my_data = 0x55;
    assert(ee_cb_write(&cb, (uint8_t*)&my_data) == 0);
    my_data = 0;
    ee_cb_read(&cb, (uint8_t*)&my_data);
    assert(my_data == 0x55);
    for (int i = 0; i < BUFFER_SIZE * 5; ++i) {
        my_data = i;
        ee_cb_write(&cb, (uint8_t*)&my_data);
        my_data = 0xFF;
        ee_cb_read(&cb, (uint8_t*)&my_data);
        assert(my_data == i);
    }
    printf("tests pass\r\n");
    return 0;
}