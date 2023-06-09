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

#include "libc.hpp"

#include <stdlib.h>
#include <string.h>

#include <abi.hpp>



// Called when an app aborts.
static void appAborted() {
	printf("App aborted!\n");
	abort();
}

// Called when an app exits.
static void appExited(int ec) {
	// Find context.
	auto ctx = abi::getContext();
	if (!ctx) {
		printf("Fatal error: Missing context.\n");
		abort();
	}
	
	// Perform jump.
	asm volatile (
		"  mv a0, %0\n"
		"  mv sp, %1\n"
		"  jr %2\n"
		:: "r" (ec), "r" (ctx->exitSP), "r" (ctx->exitPC)
		: "memory"
	);
}


// Exports ABI symbols into `map` (no wrapper).
void abi::libc::exportSymbolsUnwrapped(elf::SymMap &map) {
	// No header file:
	map["_exit"] = (size_t) &appExited;
	
	// From malloc.h:
	// map["malloc"]  = (size_t) &malloc;
	// map["free"]    = (size_t) &free;
	// map["calloc"]  = (size_t) &calloc;
	// map["realloc"] = (size_t) &realloc;
	
	// From stdio.h:
	map["__get_stdin"]  = (size_t) +[]{ return stdin; };
	map["__get_stdout"] = (size_t) +[]{ return stdout; };
	map["__get_stderr"] = (size_t) +[]{ return stderr; };
	map["__printf_chk"] = (size_t) &printf;
	
	map["remove"]    = (size_t) &remove;
	map["rename"]    = (size_t) &rename;
	map["tmpfile"]   = (size_t) &tmpfile;
	map["tmpnam"]    = (size_t) &tmpnam;
	map["fclose"]    = (size_t) &fclose;
	map["fflush"]    = (size_t) &fflush;
	map["fopen"]     = (size_t) &fopen;
	map["freopen"]   = (size_t) &freopen;
	map["vfprintf"]  = (size_t) &vfprintf;
	map["fprintf"]   = (size_t) &fprintf;
	map["vprintf"]   = (size_t) &vprintf;
	map["printf"]    = (size_t) &printf;
	map["vasprintf"] = (size_t) &vasprintf;
	map["asprintf"]  = (size_t) &asprintf;
	map["vsnprintf"] = (size_t) &vsnprintf;
	map["vsprintf"]  = (size_t) &vsprintf;
	map["snprintf"]  = (size_t) &snprintf;
	map["sprintf"]   = (size_t) &sprintf;
	map["vfscanf"]   = (size_t) &vfscanf;
	map["fscanf"]    = (size_t) &fscanf;
	map["vscanf"]    = (size_t) &vscanf;
	map["scanf"]     = (size_t) &scanf;
	map["vsscanf"]   = (size_t) &vsscanf;
	map["sscanf"]    = (size_t) &sscanf;
	map["fgets"]     = (size_t) &fgets;
	map["fgetc"]     = (size_t) &fgetc;
	map["getchar"]   = (size_t) &getchar;
	map["fread"]     = (size_t) &fread;
	map["ungetc"]    = (size_t) &ungetc;
	map["fputs"]     = (size_t) &fputs;
	map["puts"]      = (size_t) &puts;
	map["fputc"]     = (size_t) &fputc;
	map["putchar"]   = (size_t) &putchar;
	map["fwrite"]    = (size_t) &fwrite;
	map["fgetpos"]   = (size_t) &fgetpos;
	map["fsetpos"]   = (size_t) &fsetpos;
	map["fseek"]     = (size_t) &fseek;
	map["ftell"]     = (size_t) &ftell;
	map["rewind"]    = (size_t) &rewind;
	map["setbuf"]    = (size_t) &setbuf;
	map["setvbuf"]   = (size_t) &setvbuf;
	map["clearerr"]  = (size_t) &clearerr;
	map["feof"]      = (size_t) &feof;
	map["ferror"]    = (size_t) &ferror;
	map["perror"]    = (size_t) &perror;
	
	// From stdlib.h:
	map["atof"]    = (size_t) &atof;
	map["atoi"]    = (size_t) &atoi;
	map["atol"]    = (size_t) &atol;
	map["strtod"]  = (size_t) &strtod;
	map["strtol"]  = (size_t) &strtol;
	map["strtoul"] = (size_t) &strtoul;
	map["rand"]    = (size_t) &rand;
	map["srand"]   = (size_t) &srand;
	map["abort"]   = (size_t) &appAborted;
	map["atexit"]  = (size_t) &atexit;
	map["exit"]    = (size_t) &exit;
	// `getenv` is up to the program itself to implement.
	
	// From string.h:
	map["memchr"]     = (size_t) &memchr;
	map["memrchr"]    = (size_t) &memrchr;
	map["memcmp"]     = (size_t) &memcmp;
	map["memccpy"]    = (size_t) &memccpy;
	map["memcpy"]     = (size_t) &memcpy;
	map["memmove"]    = (size_t) &memmove;
	map["memset"]     = (size_t) &memset;
	map["strchr"]     = (size_t) &strchr;
	map["strrchr"]    = (size_t) &strrchr;
	map["strcmp"]     = (size_t) &strcmp;
	map["strncmp"]    = (size_t) &strncmp;
	map["strcat"]     = (size_t) &strcat;
	map["strncat"]    = (size_t) &strncat;
	map["strspn"]     = (size_t) &strspn;
	map["strcspn"]    = (size_t) &strcspn;
	map["strstr"]     = (size_t) &strstr;
	map["strcasestr"] = (size_t) &strcasestr;
	map["strlen"]     = (size_t) &strlen;
	map["strnlen"]    = (size_t) &strnlen;
	map["strerror"]   = (size_t) &strerror;
}
