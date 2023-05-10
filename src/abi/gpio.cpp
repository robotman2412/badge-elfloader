/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "gpio.hpp"
#include "abi_gpio.h"

#include <driver/gpio.h>
#include <driver/i2c.h>

// TODO: Support for MCUs other than ESP32-C6.



#define IO_CAP_COMMON		\
		IO_CAP_HIGH_Z		|\
		IO_CAP_INPUT		|\
		IO_CAP_OUTPUT		|\
		IO_CAP_UART_RX	|\
		IO_CAP_I2C_CLOCK	|\
		IO_CAP_I2C_DATA	|\
		IO_CAP_PWM_OUT	|\
		IO_CAP_SPI_CLOCK	|\
		IO_CAP_SPI_SEND	|\
		IO_CAP_SPI_RECV

static uint32_t io_pin_cap_arr[] = {
	/* 0 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 1 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 2 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 3 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 4 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 5 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 6 */ IO_CAP_COMMON | IO_CAP_ANALOG_IN,
	/* 7 */ IO_CAP_COMMON,
	/* 8 */ IO_CAP_COMMON,
	/* 9 */ IO_CAP_COMMON,
	/* 10 */ IO_CAP_COMMON,
	/* 11 */ IO_CAP_COMMON,
	/* 12 */ IO_CAP_COMMON,
	/* 13 */ IO_CAP_COMMON,
	/* 14 */ IO_CAP_COMMON,
	/* 15 */ IO_CAP_COMMON,
	/* 16 */ IO_CAP_COMMON,
	/* 17 */ IO_CAP_COMMON,
	/* 18 */ IO_CAP_COMMON,
	/* 19 */ IO_CAP_COMMON,
	/* 20 */ IO_CAP_COMMON,
	/* 21 */ IO_CAP_COMMON,
	/* 22 */ IO_CAP_COMMON,
	/* 23 */ IO_CAP_COMMON,
	/* 24 */ IO_CAP_COMMON,
	/* 25 */ IO_CAP_COMMON,
	/* 26 */ IO_CAP_COMMON,
	/* 27 */ IO_CAP_COMMON,
	/* 28 */ IO_CAP_COMMON,
	/* 29 */ IO_CAP_COMMON,
	/* 30 */ IO_CAP_COMMON,
};

static __thread i2c_cmd_handle_t host_cmd[2];

// Get the number of present GPIO pins.
int io_cap_pin_count() {
	return 31;
}

// Get the capabilities of a GPIO pin.
// Returns 0 if the pin does not exist.
uint32_t io_cap_pin(int pin) {
	if (pin < 0 || pin >= io_cap_pin_count()) return 0;
	return io_pin_cap_arr[pin];
}


// Get the number of hardware I2C interfaces.
int io_cap_i2c_count() {
	return 2;
}

// Get the capabilities of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c(int interface) {
	switch (interface) {
		case 0: // I2C
			return I2C_CAP_HOST | I2C_CAP_DEV | I2C_CAP_10BIT;
		case 1: // LP I2C
			return I2C_CAP_HOST | I2C_CAP_10BIT;
		default: return 0;
	}
}

// Get the highest clock speed in Hertz of an I2C interface.
// Returns 0 if the interface does not exist.
uint32_t io_cap_i2c_speed(int interface) {
	if (interface != 0 && interface != 1) {
		return 0;
	} else {
		return 800000; // 800 Kbit/s.
	}
}


// TODO: SPI capabilities.


// Read value from pin.
// If set to output, the resulting value is UNSPECIFIED.
bool io_read(int pin) {
	return gpio_get_level((gpio_num_t) pin);
}

// Write value to pin.
// If set to input, the resulting behaviour is UNSPECIFIED.
void io_write(int pin, bool value) {
	gpio_set_level((gpio_num_t) pin, value);
}

// Set mode of pin.
// Returns whether the operation was successful.
bool io_mode(int pin, io_mode_t mode) {
	switch (mode) {
		default: return false;
			
		case IO_MODE_HIGH_Z:
		case IO_MODE_INPUT:
			return gpio_set_direction((gpio_num_t) pin, GPIO_MODE_INPUT) == 0;
			
		case IO_MODE_OUTPUT:
			return gpio_set_direction((gpio_num_t) pin, GPIO_MODE_INPUT) == 0;
	}
}

// Set pull direction of an input pin.
// If not set to IO_MODE_INPUT, the resulting value is UNSPECIFIED.
// Returns whether the operation was successful.
bool io_pull(int pin, io_pull_t dir) {
	switch (dir) {
		default: return false;
			
		case IO_NO_PULL:
			return gpio_set_pull_mode((gpio_num_t) pin, GPIO_FLOATING) == 0;
			
		case IO_PULLUP:
			return gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY) == 0;
			
		case IO_PULLDOWN:
			return gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLDOWN_ONLY) == 0;
	}
}


// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be NULL.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr(int pin, io_change_t when, io_isr_t isr) {
	return false; // TODO.
}

// Attach a pin change handler to a digital input pin.
// The cookie value passed to the ISR will be the same as COOKIE.
// If a handler already exists, it will be replaced.
// Returns whether the operation was successful.
bool io_attach_isr_cookie(int pin, io_change_t when, io_isr_t isr, void *cookie) {
	return false; // TODO.
}

// Detach a pin change handler.
// Returns whether the operation was successful and there was a handler present.
bool io_detach_isr(int pin) {
	return false; // TODO.
}



// Set up an I2C interface as I2C host.
// Returns whether the operation was successful.
bool i2c_host_init(int interface, int sda, int scl){
	return false; // TODO.
}

// Start an I2C transaction (I2C host only).
// Returns whether the operation was successful.
bool i2c_host_start(int interface) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	// Add start command to the queue.
	return i2c_master_start(host_cmd[interface]) == 0;
}

// Stop an I2C transaction (I2C host only).
// Returns whether the operation was successful.
bool i2c_host_stop(int interface) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	
	// Add stop command to queue.
	int res = i2c_master_stop(host_cmd[interface]);
	// Perform the actions.
	res |= i2c_master_cmd_begin((i2c_port_t) interface, host_cmd[interface], pdMS_TO_TICKS(50));
	
	// Clean up.
	i2c_cmd_link_delete(host_cmd[interface]);
	host_cmd[interface] = nullptr;
	
	return res == 0;
}


// Write an I2C address (either 7-bit or 10-bit; I2C host only).
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_addr(int interface, int device, bool read_bit) {
	if (device & ~0x3ff) return false;
	
	if (device & 0x380) {
		// 10-bit addressing.
		return i2c_host_write_byte(interface, 0xf0 | ((device >> 7) & 0x06) | read_bit)
			&& i2c_host_write_byte(interface, device);
		
	} else {
		// 7-bit addressing.
		return i2c_host_write_byte(interface, (device << 1) | read_bit);
	}
}

// Write an I2C byte (I2C host only).
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_byte(int interface, uint8_t send_byte) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	// Add write command to the queue.
	return i2c_master_write_byte(host_cmd[interface], send_byte, 1) == 0;
}

// Read an I2C byte (I2C host only) and acknowledge it.
// Returns whether the operation was successful.
bool i2c_host_read_byte(int interface, uint8_t *recv_byte) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	// Add read command to the queue.
	return i2c_master_read_byte(host_cmd[interface], recv_byte, I2C_MASTER_ACK) == 0;
}

// Write an I2C byte (I2C host only).
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful and acknowledged.
bool i2c_host_write_bytes(int interface, const uint8_t *send_buf, size_t send_len) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	// Add write command to the queue.
	return i2c_master_write(host_cmd[interface], send_buf, send_len, 1) == 0;
}

// Read an I2C byte (I2C host only) and acknowledge it.
// This operation may be delayed until i2c_host_stop is called.
// Returns whether the operation was successful.
bool i2c_host_read_bytes(int interface, uint8_t *recv_buf, size_t recv_len) {
	// Bounds check.
	if (interface != 0 && interface != 1) return false;
	// Create the context.
	if (!host_cmd[interface]) host_cmd[interface] = i2c_cmd_link_create();
	// Add read command to the queue.
	return i2c_master_read(host_cmd[interface], recv_buf, recv_len, I2C_MASTER_ACK) == 0;
}


// Write to an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns the number of acknowledged data bytes or -1 on failure.
int i2c_host_write_to(int interface, int device, const uint8_t *send_buf, size_t send_len) {
	bool res = i2c_host_start(interface)
		&& i2c_host_write_addr(interface, device, 0)
		&& i2c_host_write_bytes(interface, send_buf, send_len);
	return res & i2c_host_stop(interface);
}
// Read from an I2C device at a certain address (I2C host only).
// Automatically chooses 7-bit or 10-bit I2C device addressing.
// Returns whether the operation was successful.
bool i2c_host_read_from(int interface, int device, uint8_t *recv_buf, size_t recv_len) {
	bool res = i2c_host_start(interface)
		&& i2c_host_write_addr(interface, device, 1)
		&& i2c_host_read_bytes(interface, recv_buf, recv_len);
	return res & i2c_host_stop(interface);
}


// Exports ABI symbols into `map` (no wrapper).
void abi::gpio::exportSymbolsUnwrapped(elf::SymMap &map) {
	map["io_cap_pin_count"]		= (size_t) &io_cap_pin_count;
	map["io_cap_pin"]			= (size_t) &io_cap_pin;
	map["io_cap_i2c_count"]		= (size_t) &io_cap_i2c_count;
	map["io_cap_i2c"]			= (size_t) &io_cap_i2c;
	map["io_cap_i2c_speed"]		= (size_t) &io_cap_i2c_speed;
	map["io_read"]				= (size_t) &io_read;
	map["io_write"]				= (size_t) &io_write;
	map["io_mode"]				= (size_t) &io_mode;
	map["io_pull"]				= (size_t) &io_pull;
	map["io_attach_isr"]		= (size_t) &io_attach_isr;
	map["io_attach_isr_cookie"]	= (size_t) &io_attach_isr_cookie;
	map["io_detach_isr"]		= (size_t) &io_detach_isr;
	map["i2c_host_init"]		= (size_t) &i2c_host_init;
	map["i2c_host_start"]		= (size_t) &i2c_host_start;
	map["i2c_host_stop"]		= (size_t) &i2c_host_stop;
	map["i2c_host_write_addr"]	= (size_t) &i2c_host_write_addr;
	map["i2c_host_write_byte"]	= (size_t) &i2c_host_write_byte;
	map["i2c_host_read_byte"]	= (size_t) &i2c_host_read_byte;
	map["i2c_host_write_bytes"]	= (size_t) &i2c_host_write_bytes;
	map["i2c_host_read_bytes"]	= (size_t) &i2c_host_read_bytes;
	map["i2c_host_write_to"]	= (size_t) &i2c_host_write_to;
	map["i2c_host_read_from"]	= (size_t) &i2c_host_read_from;
}

