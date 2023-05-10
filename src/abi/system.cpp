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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>

// Exports ABI symbols into `map` (no wrapper).
void abi::system::exportSymbolsUnwrapped(elf::SymMap &map) {
	// From system.h:
	map["yield"]    = (size_t) +[]{
		#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
			kernel::ctx_t *ctx = kernel::getCtx();
			vPortYield();
			if (ctx) kernel::setCtx(ctx);
		#else
			vPortYield();
		#endif
	};
	map["delay_ms"] = (size_t) +[](int64_t millis){
		#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
			kernel::ctx_t *ctx = kernel::getCtx();
			vTaskDelay(pdMS_TO_TICKS(millis));
			if (ctx) kernel::setCtx(ctx);
		#else
			vTaskDelay(pdMS_TO_TICKS(millis));
		#endif
	};
	map["delay_us"] = (size_t) +[](int64_t micros){
		#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
			kernel::ctx_t *ctx = kernel::getCtx();
		#endif
		int64_t millis = micros / 1000;
		if (millis >= 4) {
			micros -= millis * 1000;
			int64_t t1 = esp_timer_get_time();
			vTaskDelay(pdMS_TO_TICKS(millis));
			int64_t t2 = esp_timer_get_time();
			int64_t took_us = t2 - t1;
			micros -= took_us;
		}
		esp_rom_delay_us(micros);
		#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
			if (ctx) kernel::setCtx(ctx);
		#endif
	};
	map["sched_yield"] = map["yield"];
	map["usleep"]      = map["delay_us"];
}
