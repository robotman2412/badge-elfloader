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

#include "abi.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>
#include <esp_system.h>
#include <esp_log.h>
static const char *TAG = "badgeabi";

#include <malloc.h>
#include <stdio.h>
#include <string.h>

extern "C" const char abiCallTemplate[];
extern "C" const char abiCallTemplateEnd[];

namespace abi {

static elf::SymMap cache;
#ifdef CONFIG_BADGEABI_ENABLE_MPU
static std::vector<fptr_t> abiTable;
#endif
static std::unordered_map<int, Context> contextMap;
static int nextPID = 1;
static __thread int threadPID = 0;



// Map of ABI contexts.
const std::unordered_map<int, Context> &getContexts() {
	return contextMap;
}

// Create a new numbered ABI context.
// You should set the PID from the kernel context to equal the context's PID.
Context &newContext() {
	int pid = nextPID++;
	auto &ref = contextMap[pid];
	ref.pid = pid;
	return ref;
}

// Get a context given a PID.
Context *getContext(int pid) {
	auto iter = contextMap.find(pid);
	if (iter == contextMap.end()) {
		return nullptr;
	} else {
		return &iter->second;
	}
}

// Get the current context.
Context *getContext() {
	return getContext(threadPID);
}

// Set the context for this thread.
void setContext(int pid) {
	threadPID = pid;
}

// Destroy an ABI context.
bool deleteContext(Context &context) {
	return contextMap.erase(context.getPID());
}

// Destroy an ABI context.
bool deleteContext(int pid) {
	return contextMap.erase(pid);
}



Context::Context() {
	ESP_LOGD(TAG, "Context()");
}

Context::~Context() {
	ESP_LOGD(TAG, "~Context()");
	for (auto mem: actual) {
		free((void *) mem.base);
	}
}

/* // Merge touching entries of available ranges.
void Context::mergeAvailable() {
	// Sort available ranges by base address.
	// std::sort(available.begin(), available.end(), [](MemRange a, MemRange b) {
	// 	return a.base < b.base;
	// });
	
	for (size_t i = 0; i < available.size() - 1;) {
		MemRange &r0 = available[i].promise;
		MemRange &r1 = available[i+1].promise;
		
		// Test whether ranges touch.
		if (r0.base + r0.length == r1.base) {
			// They touch; merge ranges an check again.
			r0.length += r1.length;
			available.erase(available.begin() + i + 1);
		} else {
			// They don't touch; check next ranges.
			i++;
		}
	}
} */

/* // Insert a range into available such that available remains sorted.
void Context::insertAvailable(MemMapped add) {
	#if 0
	// Binary search the position to insert in to.
	ssize_t begin = 0, end = available.size();
	
	while (begin < end) {
		// Compare against midpoint.
		ssize_t midpoint = (begin + end) / 2;
		if (add < available[midpoint]) {
			// Go lower.
			end = midpoint;
		} else {
			// Go higher.
			begin = midpoint + 1;
		}
	}
	
	// Final insertion.
	available.insert(available.begin() + begin, add);
	#else
	// Linear search the position to insert in to.
	size_t i;
	for (i = 0; i < available.size() && add > available[i]; i++);
	
	// Final insertion.
	available.insert(available.begin() + i, add);
	#endif
} */

// Map a new range of a minimum size.
// Returns pointer on success, 0 otherwise.
// The minimum provided alignment shall be `sizeof(size_t)`.
size_t Context::map(size_t min_length, bool allow_write, bool allow_exec, size_t min_align) {
	// Enforce alignment is a power of two.
	if (min_align & (min_align - 1) || !min_length) {
		return 0;
	}
	
	// Set minimum alignment.
	if (min_align <= sizeof(size_t)) {
		min_align = sizeof(size_t);
	}
	if (min_length % min_align) {
		min_length += min_align - min_length % min_align;
	}
	
	size_t base;
	MemRange mem;
	if (min_align <= sizeof(size_t)) {
		// No need for additional alignment of output memory.
		mem  = allocator(min_length, allow_write, allow_exec);
		if (!mem.base) return 0;
		base = mem.base;
		
	} else {
		// Possible need for additional alignment of output memory.
		mem  = allocator(min_length + min_align, allow_write, allow_exec);
		if (!mem.base) return 0;
		if (mem.base % min_align) {
			base = mem.base + min_align - mem.base % min_align;
		} else {
			base = mem.base;
		}
	}
	
	// Actual allocation.
	size_t actual_i = actual.size();
	actual.push_back(mem);
	
	// Used space.
	mapped.push_back({{base, min_length}, actual_i});
	
	return base;
}

// Actual implementation of unmap.
// Index is into mapped list.
void Context::unmapIdx(size_t index) {
	// Free the memory.
	deallocator(actual[index]);
	// Remove it from the list.
	actual.erase(actual.begin() + index);
	mapped.erase(mapped.begin() + index);
	
	/* // Move a block from mapped to available.
	auto range = mapped[index];
	mapped.erase(mapped.begin() + index);
	insertAvailable(range);
	
	// Check for the possibility to release resources.
	mergeAvailable();
	for (size_t i = 0; i < available.size();) {
		size_t j   = available[i].actual;
		auto sub   = available[i].promise;
		auto super = actual[j];
		
		// If ranges match then the memory can be released.
		if (sub == super) {
			// Remove memory ranges.
			actual.erase(actual.begin() + available[i].actual);
			available.erase(available.begin() + i);
			
			// Correct indices.
			for (size_t x = 0; x < available.size(); x++) {
				if (available[x].actual >= j) {
					available[x].actual --;
				}
			}
			for (size_t x = 0; x < mapped.size(); x++) {
				if (mapped[x].actual >= j) {
					mapped[x].actual --;
				}
			}
			
			// Release memory.
			deallocator(super);
			
		} else {
			// Check next range.
			i++;
		}
	} */
}

// Unmap a range of memory.
bool Context::unmap(size_t base) {
	// Locate matching range.
	for (size_t i = 0; i < mapped.size(); i++) {
		if (base == mapped[i].promise.base) {
			// Match found.
			unmapIdx(i);
			return true;
		}
	}
	// Match not found.
	return false;
}



// An overridable allocator used for Context.
// The minimum provided alignment shall be `sizeof(size_t)`.
MemRange allocator(size_t min_length, bool allow_write, bool allow_exec) __attribute__((weak));
MemRange allocator(size_t min_length, bool allow_write, bool allow_exec) {
	void *mem = malloc(min_length);
	ESP_LOGD(TAG, "allocator(%zu, %d, %d) = %p", min_length, allow_write, allow_exec, mem);
	return { (size_t) mem, min_length };
}

// An overridable allocator used for Context.
void deallocator(MemRange range) __attribute__((weak));
void deallocator(MemRange range) {
	ESP_LOGD(TAG, "deallocator(%p)", (void*) range.base);
	free((void *) range.base);
}



// Generates an ABI call wrapper function.
static void *writeABIWrapper(int abiIndex) {
	// Allocate an memories.
	size_t len = abiCallTemplateEnd - abiCallTemplate;
	size_t cap = len + sizeof(int);
	void  *mem = malloc(cap);
	if (!mem) return nullptr;
	
	// Copy in the generic.
	memcpy(mem, (const void *) abiCallTemplate, len);
	// Copy in the INDEX WORD.
	uint32_t *indexPtr = (uint32_t *) (len + (size_t) mem);
	*indexPtr = abiIndex;
	
	// Done!
	return mem;
}

static void initCache() {
	exportSymbolsUnwrapped(cache);
	#ifdef CONFIG_BADGEABI_ENABLE_MPU
	for (auto &entry: cache) {
		auto fptr = entry.second;
		entry.second = (size_t) writeABIWrapper(abiTable.size());
		abiTable.push_back((fptr_t) fptr);
	}
	#endif
}

// Exports ABI symbols into `map`.
void exportSymbolsUnwrapped(elf::SymMap &map) {
	gpio::exportSymbolsUnwrapped(map);
	libc::exportSymbolsUnwrapped(map);
	system::exportSymbolsUnwrapped(map);
	math::exportSymbolsUnwrapped(map);
	implicitops::exportSymbolsUnwrapped(map);
	display::exportSymbolsUnwrapped(map);
}



#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
// Get the ABI TABLE.
fptr_t *getAbiTable() {
	if (!cache.size()) initCache();
	return abiTable.data();
}

// Get the size of the ABI TABLE.
size_t getAbiTableSize() {
	if (!cache.size()) initCache();
	return abiTable.size();
}
#endif

// Exports ABI symbols into `map`.
void exportSymbols(elf::SymMap &map) {
	if (!cache.size()) initCache();
	map.merge(cache);
}

} // namespace abi
