/*
 * eeprom_circular_buffer.c
 *
 * Created: 7/14/2020 8:50:43 AM
 *  Author: Colin
 */ 

#include "eeprom_circular_buffer.h"

static uint8_t *ee_cb_get_data_base_address(struct ee_cb *self) {
    return self->base_address;
}

static uint8_t *ee_cb_get_data_address(struct ee_cb *self, uint8_t index) {
    return ee_cb_get_data_base_address(self) + index * self->data_size;
}

static uint8_t *ee_cb_get_status_base_address(struct ee_cb *self) {
    return self->base_address + self->buffer_size * self->data_size;
}

static uint8_t *ee_cb_get_status_address(struct ee_cb *self, uint8_t index) {
    return ee_cb_get_status_base_address(self) + index;
}

static uint8_t ee_cb_read_byte(struct ee_cb *self, uint8_t *addr) {
    uint8_t ret = 0;
    self->reader(addr, &ret, 1);
    return ret;
}

static void ee_cb_write_byte(struct ee_cb* self, uint8_t* addr, uint8_t value) {
    self->writer(addr, &value, 1);
}

/*returns the current parameter index*/
static uint8_t ee_cb_get_write_index(struct ee_cb *self) {
    uint8_t *current_address = ee_cb_get_status_base_address(self);
    while (1) {
        uint8_t *previous_address = current_address - 1;
        if (previous_address < ee_cb_get_status_base_address(self)) {
            previous_address = ee_cb_get_status_base_address(self) + self->buffer_size - 1;
        }
        uint8_t status_at_addr = ee_cb_read_byte(self, current_address);
        uint8_t status_at_previous_addr = ee_cb_read_byte(self, previous_address);
        if ((uint8_t)(status_at_previous_addr + 1) != status_at_addr) {
            break;
        }
        current_address++;
        if (current_address >= ee_cb_get_status_base_address(self) + self->buffer_size) {
            current_address = ee_cb_get_status_base_address(self);
        }
    }
    return (uint8_t)(current_address - ee_cb_get_status_base_address(self));
}

int ee_cb_init(struct ee_cb *self, uint8_t *base_address, size_t data_size, 
    size_t buffer_size, ee_cb_writer writer, ee_cb_reader reader) {
    if (!writer || !reader || buffer_size > 255) return -1;
    self->writer = writer;
    self->reader = reader;
    self->base_address = base_address;
    self->data_size = data_size;
    self->buffer_size = buffer_size;
    return 0;
}

int ee_cb_write(struct ee_cb *self, const uint8_t *data) {
    if (!self || !data) return -1;
    /*get the status index*/
    uint8_t write_index = ee_cb_get_write_index(self);
    /*write data*/
    uint8_t *data_address = ee_cb_get_data_address(self, write_index);
    self->writer(data_address, data, self->data_size);
    /*write status*/
    int previous_index = write_index - 1;
    if (previous_index < 0) {
        previous_index = self->buffer_size - 1;
    }
    uint8_t next_status_value = 
        ee_cb_read_byte(self, 
            ee_cb_get_status_address(self, (uint8_t)previous_index)) + 1;
    ee_cb_write_byte(self, ee_cb_get_status_address(self, write_index), next_status_value);
    return 0;
}

int ee_cb_read(struct ee_cb *self, uint8_t *data) {
    if (!self || !data) return -1;
    int current_index = ee_cb_get_write_index(self);
    current_index--;
    if (current_index < 0) {
        current_index = self->buffer_size - 1;
    }
    uint8_t *data_address = ee_cb_get_data_address(self, (uint8_t)current_index);
    self->reader(data_address, data, self->data_size);
    return 0;
}