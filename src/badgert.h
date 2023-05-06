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

// Simple C wrapper for the C++ runtime API.

#pragma once

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Go from path straight to running a program.
bool badgert_start(const char *path);
// Go from file descriptor straight to running a program.
// File descriptor is closed when finished.
bool badgert_start_fd(const char *filename, FILE *fd);

// Register a dynamic library from a buffer.
// The buffer must exist until a matching `badgert_unregister` call is made.
void badgert_register_buf(const char *filename, const void *buf, size_t buf_len);
// Register a dynamic library from a path.
void badgert_register_file(const char *filename, const char *fullpath);
// Unregister a dynamic library.
void badgert_unregister(const char *filename);

// Add a dynamic library search directory.
void badgert_add_search_dir(const char *path);
// Remove a dynamic library search directory.
void badgert_remove_search_dir(const char *path);

#ifdef __cplusplus
} // extern "C"
#endif
