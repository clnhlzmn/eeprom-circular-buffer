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

static uint8_t ee_cb_get_previous_index(struct ee_cb *self, uint8_t index) {
    int previous_index = index - 1;
    if (previous_index < 0) {
        previous_index = self->buffer_size - 1;
    }
    return (uint8_t)previous_index;
}

static uint8_t ee_cb_get_next_index(struct ee_cb *self, uint8_t index) {
    int next_index = index + 1;
    if (next_index >= self->buffer_size) {
        next_index = 0;
    }
    return (uint8_t)next_index;
}

struct status_pair {
    uint8_t previous;
    uint8_t current;
};

static struct status_pair ee_cb_get_previous_and_current_status(
    struct ee_cb *self, uint8_t current_index) {
    uint8_t *current_address = ee_cb_get_status_address(self, current_index);
    uint8_t previous_index = ee_cb_get_previous_index(self, current_index);
    uint8_t *previous_address = ee_cb_get_status_address(self, previous_index);
    uint8_t current_status = ee_cb_read_byte(self, current_address);
    uint8_t previous_status = ee_cb_read_byte(self, previous_address);
    return (struct status_pair) {
        .previous = previous_status, 
        .current = current_status
    };
}

static uint8_t ee_cb_get_write_index(struct ee_cb *self) {
    uint8_t current_index = 0;
    while (1) {
        struct status_pair status = 
            ee_cb_get_previous_and_current_status(self, current_index);
        if ((uint8_t)(status.previous + 1) != status.current) {
            break;
        }
        current_index = ee_cb_get_next_index(self, current_index);
    }
    return current_index;
}

static uint8_t ee_cb_get_read_index(struct ee_cb *self) {
    uint8_t write_index = ee_cb_get_write_index(self);
    return ee_cb_get_previous_index(self, write_index);
}

static uint8_t ee_cb_get_status_to_write(struct ee_cb *self, uint8_t write_index) {
    uint8_t previous_index = ee_cb_get_previous_index(self, write_index);
    uint8_t *previous_status_address = ee_cb_get_status_address(self, previous_index);
    uint8_t previous_status = ee_cb_read_byte(self, previous_status_address);
    return previous_status + 1;
}

int ee_cb_init(struct ee_cb *self, uint8_t *base_address, size_t data_size, 
    size_t buffer_size, ee_cb_writer writer, ee_cb_reader reader) {
    if (!writer || !reader || buffer_size > EE_CB_MAX_BUFFER_SIZE) return -1;
    self->writer = writer;
    self->reader = reader;
    self->base_address = base_address;
    self->data_size = data_size;
    self->buffer_size = buffer_size;
    return 0;
}

int ee_cb_write(struct ee_cb *self, const uint8_t *data) {
    if (!self || !data) return -1;
    uint8_t write_index = ee_cb_get_write_index(self);
    /*write data*/
    uint8_t *data_address = ee_cb_get_data_address(self, write_index);
    self->writer(data_address, data, self->data_size);
    /*write status*/
    uint8_t next_status = ee_cb_get_status_to_write(self, write_index);
    uint8_t *status_address = ee_cb_get_status_address(self, write_index);
    ee_cb_write_byte(self, status_address, next_status);
    return 0;
}

int ee_cb_read(struct ee_cb *self, uint8_t *data) {
    if (!self || !data) return -1;
    uint8_t read_index = ee_cb_get_read_index(self);
    uint8_t *read_address = ee_cb_get_data_address(self, read_index);
    self->reader(read_address, data, self->data_size);
    return 0;
}