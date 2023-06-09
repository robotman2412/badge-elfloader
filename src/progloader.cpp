/*
	MIT License

	Copyright (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
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

#include "progloader.hpp"

#include <esp_log.h>
static const char *TAG = "badgeloader";

#ifdef CONFIG_BADGEABI_ENABLE_MPU
#include <mpu.hpp>
#endif

namespace loader {


Linkage::Linkage(int _pid):
	entryFunc(nullptr),
	pid(_pid),
	hasExecutable(0),
	linkAttempted(0),
	linkSuccessful(0) {}
Linkage::Linkage(const abi::Context &actx):
	entryFunc(nullptr),
	pid(actx.getPID()),
	hasExecutable(0),
	linkAttempted(0),
	linkSuccessful(0) {}
Linkage::~Linkage() {}

// Discard unused information (mostly linkage information after `linkAttempted` is true).
void Linkage::garbageCollect() {
	if (linkAttempted) {
		files.clear();
	}
}

// Load a library from a file.
// Returns success status.
bool Linkage::loadLibrary(const std::string &filename, FILE *fd) {
	if (linkAttempted) { return false; }
	auto actx = abi::getContext(pid);
	if (!actx) {
		ESP_LOGE(TAG, "Cannot load without valid ABI context");
		return false;
	}
	
	// Create reading context.
	auto elf = elf::ELFFile(fd);
	
	// Try to read data.
	if (!elf.readDyn()) {
		return false;
	}
	
	// Try to load progbits.
	auto prog = elf.load([&](size_t vaddr, size_t len, size_t align) {
		size_t mem = actx->map(len, 1, 1, align);
		return std::pair(mem, mem);
	});
	if (!prog) return false;
	
	// Export the symbols.
	if (!elf::exportSymbols(elf, prog, symbols)) {
		free(prog.memory_cookie);
		return false;
	}
	
	// Add to loaded things list.
	loaded.push_back(prog);
	files.push_back(std::move(elf));
	filenames.push_back(filename);
	
	#ifdef CONFIG_BADGEABI_ENABLE_MPU
	// Apply MPU settings.
	if (!mpu::applyPH(elf, prog)) {
		ESP_LOGW(TAG, "Applying MPU settings failed, ignoring.");
	}
	#endif
	
	ESP_LOGI(TAG, "%s loaded to 0x%08zx (offset 0x%08zx)", filename.c_str(), (size_t) prog.vaddr_real, (size_t) prog.vaddr_offset());
	return true;
}

// Load the executable from a file.
// Returns success status.
bool Linkage::loadExecutable(const std::string &filename, FILE *fd) {
	if (linkAttempted || hasExecutable) { return false; }
	// Yep this can be easily forwarded.
	if (loadLibrary(filename, fd)) {
		hasExecutable = true;
		entryFunc = loaded.back().entry;
		return true;
	}
	return false;
}

// Perform final dynamic linking before code execution can begin.
// Returns success status.
bool Linkage::link() {
	if (linkAttempted) { return linkSuccessful; }
	linkAttempted = true;
	
	// Link all the things.
	for (size_t i = 0; i < loaded.size(); i++) {
		ESP_LOGD(TAG, "Applying relocations %zu/%zu", i+1, loaded.size());
		if (!elf::relocate(files[i], loaded[i], symbols)) {
			ESP_LOGE(TAG, "Dynamic linking failed");
			return false;
		}
	}
	
	linkSuccessful = true;
	return true;
}

};
