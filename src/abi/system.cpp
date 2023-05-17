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

#include "system.hpp"

#include <abi.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>

// Yield to scheduler.
static void abi_yield() {
#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
	kernel::ctx_t *ctx = kernel::getCtx();
	vPortYield();
	if (ctx) kernel::setCtx(ctx);
#else
	vPortYield();
#endif
}

// Delay in milliseconds.
static void abi_delay_ms(int64_t millis) {
	if (millis <= 0) return;
#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
	kernel::ctx_t *ctx = kernel::getCtx();
	vTaskDelay(pdMS_TO_TICKS(millis));
	if (ctx) kernel::setCtx(ctx);
#else
	vTaskDelay(pdMS_TO_TICKS(millis));
#endif
}

// Delay in microseconds.
static void abi_delay_us(int64_t micros) {
	if (micros <= 0) return;
	esp_rom_delay_us(micros);
}

// Map in new memory.
static void *abi_mem_map(size_t len, size_t min_align, bool allow_exec) {
	auto ctx = abi::getContext();
	return (void *) ctx->map(len, 1, allow_exec, min_align);
}

// Unmap memory.
static void abi_mem_unmap(void *addr) {
	auto ctx = abi::getContext();
	ctx->unmap((size_t) addr);
}

// Exports ABI symbols into `map` (no wrapper).
void abi::system::exportSymbolsUnwrapped(elf::SymMap &map) {
	// From system.h:
	map["yield"]       = (size_t) &abi_yield;
	map["delay_ms"]    = (size_t) &abi_delay_ms;
	map["delay_us"]    = (size_t) &abi_delay_us;
	map["uptime_ms"]   = (size_t) +[]() -> int64_t { return esp_timer_get_time() / 1000; };
	map["uptime_us"]   = (size_t) &esp_timer_get_time;
	map["sched_yield"] = (size_t) &abi_yield;
	map["usleep"]      = (size_t) &abi_delay_us;
	map["__mem_map"]   = (size_t) &abi_mem_map;
	map["__mem_unmap"] = (size_t) &abi_mem_unmap;
}
