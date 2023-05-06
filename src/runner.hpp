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

#pragma once

#include <progloader.hpp>
#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
#include <kernel.hpp>
#endif
#include <abi.hpp>

#include <functional>
#include <string>

namespace runtime {

enum setting {
	STACK_ENTRIES = 4096
};

// Function to call when process exits.
using Callback = std::function<void(int exitCode, abi::Context &ctx)>;

// Take a pre-loaded linkage and start it under a new thread.
bool startPreloaded(loader::Linkage &&linkage, abi::Context &ctx, Callback cb={});

// Go from file descriptor straight to running a program.
// File descriptor is closed when finished.
bool startFD(const std::string &filename, FILE *fd);

// Register a dynamic library from a buffer.
// The buffer must exist until a matching `badgert_unregister` call is made.
void registerBuf(const std::string &filename, const void *buf, size_t buf_len);
// Register a dynamic library from a path.
void registerFile(const std::string &filename, const std::string &fullpath);
// Unregister a dynamic library.
void unregister(const std::string &filename);

// Add a dynamic library search directory.
void addSearchDir(const std::string &path);
// Remove a dynamic library search directory.
void removeSearchDir(const std::string &path);

}
