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

#include "display.hpp"
#include <map>

// Simple struct with display update context.
struct Display {
	// Write callback function.
	display_write_func_t func;
	// Write callback cookie.
	void *cookie;
	// Display size in pixels.
	int width, height;
	
	// Flush the display.
	bool operator()(const void *buf, size_t len) const {
		return func(buf, len, 0, 0, width, height, cookie);
	}
	// Flush the display.
	bool operator()(const void *buf, size_t len, int x, int y, int w, int h) const {
		// Bounds check.
		if (x < 0 || y < 0
			|| x + w > width  || w > width  || w < 0
			|| y + h > height || h > height || h < 0) {
			return false;
		}
		return func(buf, len, x, y, w, h, cookie);
	}
};

// Stores the display contexts.
static std::map<int, Display> displays;
// Last added display's ID.
static int lastID = 0;

// Register a new display.
// The `cookie` parameter is passed to the display's write function.
// Returns an ID on success, 0 on error.
int display_add(display_write_func_t func, void *cookie, int width, int height) {
	// Bounds check.
	if (!func || width <= 1 || height <= 1) return false;
	
	// Emplace in the map.
	int id = ++lastID;
	displays[id] = Display{ func, cookie, width, height };
	return id;
}
// Remove a display.
// Returns success status.
bool display_remove(int display) {
	auto iter = displays.find(display);
	if (iter != displays.end()) {
		displays.erase(iter);
		return true;
	}
	return false;
}
// Get the amount of connected displays.
// May be zero on badges without displays.
int display_count() {
	return displays.size();
}
// Get the IDs of connected displays.
// Any ID zero means not present.
void display_get_ids(int *out_ids, size_t len) {
	size_t i = 0;
	for (auto iter = displays.begin(); i < len && iter != displays.end(); i++, iter++) {
		out_ids[i] = iter->first;
	}
	for (; i < len; i++) {
		out_ids[i] = 0;
	}
}

// Get the width in pixels of a display.
int display_width(int display) {
	auto iter = displays.find(display);
	if (iter != displays.end()) {
		return iter->second.width;
	}
	return 0;
}
// Get the height in pixels of a display.
int display_height(int display) {
	auto iter = displays.find(display);
	if (iter != displays.end()) {
		return iter->second.height;
	}
	return 0;
}

// Draw the full area of a display.
bool display_write(int display, const void *buf, size_t len) {
	printf("%p\n", __builtin_return_address(0));
	auto iter = displays.find(display);
	if (iter != displays.end()) {
		bool rv = iter->second(buf, len);
		printf("%p\n", __builtin_return_address(0));
		return rv;
	}
	printf("%p\n", __builtin_return_address(0));
	return false;
}
// Draw a part of the display.
bool display_write_partial(int display, const void *buf, size_t len, int x, int y, int width, int height) {
	auto iter = displays.find(display);
	if (iter != displays.end()) {
		return iter->second(buf, len, x, y, width, height);
	}
	return false;
}

void abi::display::exportSymbolsUnwrapped(elf::SymMap &map) {
	// From display.h:
	map["display_add"]				= (size_t) &display_add;
	map["display_remove"]			= (size_t) &display_remove;
	map["display_count"]			= (size_t) &display_count;
	map["display_get_ids"]			= (size_t) &display_get_ids;
	map["display_width"]			= (size_t) &display_width;
	map["display_height"]			= (size_t) &display_height;
	map["display_write"]			= (size_t) &display_write;
	map["display_write_partial"]	= (size_t) &display_write_partial;
}
