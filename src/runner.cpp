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

#include "runner.hpp"
#include "abi.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
static const char *TAG = "badgert";

#include <sys/types.h>
#include <dirent.h>

#include <memory>
#include <set>
#include <vector>



namespace runtime {

struct FILEPTR_DELETE {
	void operator()(FILE *fd) const {
		fclose(fd);
	}
};
using FILEPTR = std::unique_ptr<FILE, FILEPTR_DELETE>;

// Represents a program's dynamic linking information.
struct Dynamic {
	// Pointer to dynamic table of a file.
	const elf::DynEntry *dynamic;
	// Amount of dynamic table entries excluding null terminator.
	size_t numDynamic;
	// Dependency list.
	std::vector<std::string> needed;
	// Pointer to init function of a file, if any.
	const void *initFunc;
	// Pointer to exit function of a file, if any.
	const void *finiFunc;
};

// List of dynamic linking information.
// Will be sorted such that forward iteration is the correct FINI order.
using DynList = std::vector<Dynamic>;

// Simple struct used to pass parameters to THE APP.
struct Params {
	// Loaded program to run.
	loader::Linkage  prog;
	// ABI context to run it in.
	abi::Context    &actx;
	// Dynamic linking information.
	// DynList          dyn;
	// On exit callback.
	Callback         cb;
};

// Function pointer for typical `main`.
typedef int (*main_t)(int,char**,char**);

// The little assembly that enables the app to exit without having to return.
extern "C" int _badgert_jump_to_app(int argc, char **argv, char **envp, main_t entry, size_t *pc_save, size_t *sp_save);


// Simple struct to hold registered dynamic libraries.
struct Registered {
	bool isBuffer;
	std::string path;
	const void *buf;
	size_t buf_len;
};

// Explicitly registered dynamic libraries.
static std::map<std::string, Registered> registered;

// Dynamic library search path.
static std::vector<std::string> searchPath;



#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
// Set up user context and run user code.
int runUserCode(bool &success, kernel::ctx_t &kctx, loader::Linkage &prog, abi::Context &actx, int argc, const char **argv, const char **envp) {
	memset(&kctx.u_regs, 0, sizeof(kctx.u_regs));
	
	// Allocate user stack.
	kctx.u_regs.sp = actx.map(CONFIG_BADGERT_STACK_DEPTH * sizeof(long));
	if (!kctx.u_regs.sp) {
		success = false;
		return -1;
	}
	kctx.u_regs.sp += CONFIG_BADGERT_STACK_DEPTH * sizeof(long);
	
	// Measure envp.
	// int envp_len;
	// for (envp_len = 0; envp[envp_len]; envp_len++);
	
	// Map memory for argc and envp.
	// size_t mem = actx.map(envp_len * sizeof(char*) + argc * sizeof(char*));
	// if (!mem) {
	// 	success = false;
	// 	return -1;
	// }
	
	// Copy argv and envp to user memory.
	// char **new_argv = (char **) mem;
	// char **new_envp = (char **) (mem + argc * sizeof(char*));
	// memcpy(new_argv, argv, argc * sizeof(char*));
	// memcpy(new_envp, envp, envp_len * sizeof(char*));
	
	// Set up usermode registers.
	kctx.u_abi_table = abi::getAbiTable();
	kctx.u_abi_size  = abi::getAbiTableSize();
	kctx.u_pc        = (long) prog.getEntryFunc();
	// kctx.u_regs.a0 = argc;
	// kctx.u_regs.a1 = (long) new_argv;
	// kctx.u_regs.a2 = (long) new_envp;
	asm volatile ("mv %0, gp" : "=r" (kctx.u_regs.gp));
	asm volatile ("mv %0, tp" : "=r" (kctx.u_regs.tp));
	
	// Run user program.
	kernel::setCtx(&kctx);
	asm volatile (
		"  fence\n"		// Fence for user data
		"  fence.i\n"	// Fence for user code
		"  li a0, %0\n"	// Set system call number
		"  ecall\n"		// Perform system call
		:: "i" (kernel::SYS_USERJUMP)
		: "a0", "memory"
	);
	
	// Return from this whole ordeal.
	success = true;
	return -1;
}
#endif

// FreeRTOS task-code.
static void taskCode(void *context) {
	// Get program context.
	auto  params = (Params *) context;
	auto &prog = params->prog;
	auto &actx = params->actx;
	prog.garbageCollect();
	
	#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
	// Allocate a kernel context.
	kernel::ctx_t kctx;
	kctx.pid = actx.getPID();
	#endif
	
	// Collect user parameters.
	int         argc   = 1;
	const char *argv[] = { "a.out" };
	const char *envp[] = { NULL };
	
	#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
	// Run user code.
	bool success;
	int ec = runUserCode(success, kctx, prog, actx, argc, argv, envp);
	if (success) {
		ESP_LOGI(TAG, "Process %d exited with code %d\n", kctx.pid, ec);
	} else {
		ESP_LOGE(TAG, "Unable to run process %d\n", kctx.pid);
	}
	
	kernel::setDefaultCtx();
	#else
	// Set context.
	abi::setContext(actx.getPID());
	
	// Run user code.
	main_t entryFunc = (main_t) prog.getEntryFunc();
	ESP_LOGI(TAG, "Starting process %d at entrypoint %p", actx.getPID(), entryFunc);
	int ec = _badgert_jump_to_app(argc, (char**) argv, (char**) envp, entryFunc, &actx.exitPC, &actx.exitSP);
	
	// Report status.
	ESP_LOGI(TAG, "Process %d exited with code %d\n", actx.getPID(), ec);
	#endif
	
	// Free resources and exit.
	abi::deleteContext(actx);
	delete params;
	vTaskDelete(NULL);
}

// Take a pre-loaded linkage and start it under a new thread.
static bool startPreloaded(loader::Linkage &&linkage, abi::Context &ctx, Callback cb) {
	// This pointer will be managed by the task from now on.
	auto ptr = new Params { std::move(linkage), ctx, std::move(cb) };
	TaskHandle_t handle;
	
	// Assert context is ready to run.
	if (!ptr->prog.isProgReady()) {
		ESP_LOGE(TAG, "Cannot start process %d: Program not ready for execution", ctx.getPID());
		abi::deleteContext(ctx);
		delete ptr;
		return false;
	}
	
	// Start task.
	#ifdef CONFIG_BADGEABI_ENABLE_KERNEL
	auto res = xTaskCreate(taskCode, "", 4096, ptr, 0, &handle);
	#else
	auto res = xTaskCreate(taskCode, "", CONFIG_BADGERT_STACK_DEPTH, ptr, 0, &handle);
	#endif
	
	if (res == pdPASS) {
		return true;
	} else {
		ESP_LOGE(TAG, "Cannot start process %d: Task creation failed", ctx.getPID());
		abi::deleteContext(ctx);
		delete ptr;
		return false;
	}
}



// Try to load a dynamic library.
static FILE *loadLibraryAbs(const std::string &filename, loader::Linkage &prog, const Registered &lib) {
	FILE *fd;
	
	// Open file handle.
	if (lib.isBuffer) {
		fd = fmemopen((void*) lib.buf, lib.buf_len, "r");
	} else {
		fd = fopen(lib.path.c_str(), "rb");
	}
	
	// Add to the linkage.
	if (!fd) {
		ESP_LOGE(TAG, "Failed to load %s: %s", filename.c_str(), strerror(errno));
		return nullptr;
	}
	bool res = prog.loadLibrary(filename, fd);
	if (!res) {
		ESP_LOGE(TAG, "Failed to load %s", filename.c_str());
		fclose(fd);
		return nullptr;
	}
	
	return fd;
}

// // Try to find and load a dynamic library.
// static FILE *loadLibrarySP(loader::Linkage &prog, const std::string &searchDir) {
// 	// Try to open directory.
// 	DIR *dirp = opendir(searchDir.c_str());
// 	if (!dirp) return nullptr;
	
// 	// Iterate directory.
// 	while (1) {
// 		auto ent = readdir(dirp);
// 		if (!ent) return nullptr;
// 	}
// }

// Try to find and load a dynamic library.
static FILE *loadLibrary(loader::Linkage &prog, const std::string &name) {
	// TODO: Better mechanism for built-in libs.
	if (name == "libc.so" || name == "libbadge.so") return (FILE *) -1;
	
	// Check registered libraries.
	for (const auto &pair: registered) {
		if (pair.first == name) {
			// Found! Loading time.
			return loadLibraryAbs(pair.first, prog, pair.second);
		}
	}
	
	// TODO: Check search path.
	// for (const auto &dir: searchPath) {
	// 	if (loadLibrarySP(prog, dir)) return true;
	// }
	
	ESP_LOGE(TAG, "Dynamic library not found: %s", name.c_str());
	return nullptr;
}

// Parse dynamic information from a loaded segment.
static bool parseDynamic(Dynamic &out, const elf::Program &loaded, const elf::ELFFile &file) {
	// TODO.
	return false;
}

// Go from file descriptor straight to running a program.
// File descriptor is closed when finished.
bool startFD(const std::string &filename, FILE *fd, Callback cb) {
	std::vector<FILEPTR> fds;
	fds.emplace_back(fd);
	int res;
	
	// Load program into memory.
	auto &actx = abi::newContext();
	loader::Linkage prog {actx};
	res = prog.loadExecutable(filename, fd);
	if (!res) {
		ESP_LOGE(TAG, "Failed to load %s", filename.c_str());
		abi::deleteContext(actx);
		return false;
	}
	
	// Catalog of already loaded libraries.
	std::set<std::string> libs;
	for (const auto &filename: prog.getFilenames()) {
		libs.emplace(filename);
	}
	libs.emplace("libc.so");
	libs.emplace("libbadge.so");
	libs.emplace("libm.so");
	libs.emplace("libimplicitops.so");
	libs.emplace("libdisplay.so");
	
	// Load libraries.
	load_my_lib:
	for (const auto &file: prog.getFiles()) {
		for (const auto &lib: file.getDynLibs()) {
			// Library requirement already met.
			if (libs.find(lib) != libs.end()) continue;
			
			// New library to try to load!
			libs.emplace(lib);
			auto fd = loadLibrary(prog, lib);
			if (!fd) {
				ESP_LOGE(TAG, "Failed to load %s", lib.c_str());
				abi::deleteContext(actx);
				return false;
			} else {
				if (fd != (FILE *) -1) fds.emplace_back(fd);
				ESP_LOGD(TAG, "Loaded dynamic library %s", lib.c_str());
			}
			goto load_my_lib;
		}
	}
	
	// Link the program.
	abi::exportSymbols(prog.getSymbols());
	res = prog.link();
	if (!res) {
		ESP_LOGE(TAG, "Failed to load %s: Dynamic linking error", filename.c_str());
		abi::deleteContext(actx);
		return false;
	}
	
	// Start the process.
	return startPreloaded(std::move(prog), actx, std::move(cb));
}


// Register a dynamic library from a buffer.
// The buffer must exist until a matching `badgert_unregister` call is made.
void registerBuf(const std::string &filename, const void *buf, size_t buf_len) {
	registered[filename] = { 1, {}, buf, buf_len };
}

// Register a dynamic library from a path.
void registerFile(const std::string &filename, const std::string &fullpath) {
	registered[filename] = { 0, fullpath, nullptr, 0 };
}

// Unregister a dynamic library.
void unregister(const std::string &filename) {
	auto iter = registered.find(filename);
	if (iter != registered.end()) registered.erase(iter);
}


// Add a dynamic library search directory.
void addSearchDir(const std::string &path) {
	auto iter = std::find(searchPath.begin(), searchPath.end(), path);
	if (iter == searchPath.end()) searchPath.push_back(path);
}

// Remove a dynamic library search directory.
void removeSearchDir(const std::string &path) {
	auto iter = std::find(searchPath.begin(), searchPath.end(), path);
	if (iter != searchPath.end()) searchPath.erase(iter);
}



// Load and run a program in a new task.
extern "C" bool badgert_start(const char *path) {
	// Find filename from path.
	const char *ptr = std::max(strrchr(path, '/'), strrchr(path, '\\'));
	const char *filename = ptr ? ptr+1 : path;
	
	// Forward the rest.
	return startFD(filename, fopen(path, "rb"));
}

// Load and run a program in a new task.
// File descriptor is closed when finished.
extern "C" bool badgert_start_fd(const char *filename, FILE *fd) {
	return startFD(filename, fd);
}


// Register a dynamic library from a buffer.
// The buffer must exist until a matching `badgert_unregister` call is made.
extern "C" void badgert_register_buf(const char *filename, const void *buf, size_t buf_len) {
	registerBuf(filename, buf, buf_len);
}

// Register a dynamic library from a path.
extern "C" void badgert_register_file(const char *filename, const char *fullpath) {
	registerFile(filename, fullpath);
}

// Unregister a dynamic library.
extern "C" void badgert_unregister(const char *filename) {
	unregister(filename);
}


// Add a dynamic library search directory.
extern "C" void badgert_add_search_dir(const char *path) {
	addSearchDir(path);
}

// Remove a dynamic library search directory.
extern "C" void badgert_remove_search_dir(const char *path) {
	removeSearchDir(path);
}

}
