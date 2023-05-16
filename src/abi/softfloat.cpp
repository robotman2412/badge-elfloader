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

#include "softfloat.hpp"

extern "C" void __adddf3();
extern "C" void __addsf3();
extern "C" void __addtf3();
extern "C" void __divdf3();
extern "C" void __divsf3();
extern "C" void __divtf3();
extern "C" void __eqdf2();
extern "C" void __eqsf2();
extern "C" void __eqtf2();
extern "C" void __extenddftf2();
extern "C" void __extendsfdf2();
extern "C" void __extendsftf2();
extern "C" void __fixdfdi();
extern "C" void __fixdfsi();
extern "C" void __fixsfdi();
extern "C" void __fixsfsi();
extern "C" void __fixtfdi();
extern "C" void __fixtfsi();
extern "C" void __fixunsdfdi();
extern "C" void __fixunsdfsi();
extern "C" void __fixunssfdi();
extern "C" void __fixunssfsi();
extern "C" void __fixunstfdi();
extern "C" void __fixunstfsi();
extern "C" void __floatdidf();
extern "C" void __floatdisf();
extern "C" void __floatditf();
extern "C" void __floatsidf();
extern "C" void __floatsisf();
extern "C" void __floatsitf();
extern "C" void __floatundidf();
extern "C" void __floatundisf();
extern "C" void __floatunditf();
extern "C" void __floatunsidf();
extern "C" void __floatunsisf();
extern "C" void __floatunsitf();
extern "C" void __gedf2();
extern "C" void __gesf2();
extern "C" void __getf2();
extern "C" void __ledf2();
extern "C" void __lesf2();
extern "C" void __letf2();
extern "C" void __muldf3();
extern "C" void __mulsf3();
extern "C" void __multf3();
extern "C" void __negdf2();
extern "C" void __negsf2();
extern "C" void __negtf2();
extern "C" void __subdf3();
extern "C" void __subsf3();
extern "C" void __subtf3();
extern "C" void __truncdfsf2();
extern "C" void __trunctfdf2();
extern "C" void __trunctfsf2();
extern "C" void __unorddf2();
extern "C" void __unordsf2();
extern "C" void __unordtf2();

// Exports ABI symbols into `map` (no wrapper).
void abi::softfloat::exportSymbolsUnwrapped(elf::SymMap &map) {
	map["__adddf3"] = (size_t) &__adddf3;
	map["__addsf3"] = (size_t) &__addsf3;
	map["__addtf3"] = (size_t) &__addtf3;
	map["__divdf3"] = (size_t) &__divdf3;
	map["__divsf3"] = (size_t) &__divsf3;
	map["__divtf3"] = (size_t) &__divtf3;
	map["__eqdf2"] = (size_t) &__eqdf2;
	map["__eqsf2"] = (size_t) &__eqsf2;
	map["__eqtf2"] = (size_t) &__eqtf2;
	map["__extenddftf2"] = (size_t) &__extenddftf2;
	map["__extendsfdf2"] = (size_t) &__extendsfdf2;
	map["__extendsftf2"] = (size_t) &__extendsftf2;
	map["__fixdfdi"] = (size_t) &__fixdfdi;
	map["__fixdfsi"] = (size_t) &__fixdfsi;
	map["__fixsfdi"] = (size_t) &__fixsfdi;
	map["__fixsfsi"] = (size_t) &__fixsfsi;
	map["__fixtfdi"] = (size_t) &__fixtfdi;
	map["__fixtfsi"] = (size_t) &__fixtfsi;
	map["__fixunsdfdi"] = (size_t) &__fixunsdfdi;
	map["__fixunsdfsi"] = (size_t) &__fixunsdfsi;
	map["__fixunssfdi"] = (size_t) &__fixunssfdi;
	map["__fixunssfsi"] = (size_t) &__fixunssfsi;
	map["__fixunstfdi"] = (size_t) &__fixunstfdi;
	map["__fixunstfsi"] = (size_t) &__fixunstfsi;
	map["__floatdidf"] = (size_t) &__floatdidf;
	map["__floatdisf"] = (size_t) &__floatdisf;
	map["__floatditf"] = (size_t) &__floatditf;
	map["__floatsidf"] = (size_t) &__floatsidf;
	map["__floatsisf"] = (size_t) &__floatsisf;
	map["__floatsitf"] = (size_t) &__floatsitf;
	map["__floatundidf"] = (size_t) &__floatundidf;
	map["__floatundisf"] = (size_t) &__floatundisf;
	map["__floatunditf"] = (size_t) &__floatunditf;
	map["__floatunsidf"] = (size_t) &__floatunsidf;
	map["__floatunsisf"] = (size_t) &__floatunsisf;
	map["__floatunsitf"] = (size_t) &__floatunsitf;
	map["__gedf2"] = (size_t) &__gedf2;
	map["__gesf2"] = (size_t) &__gesf2;
	map["__getf2"] = (size_t) &__getf2;
	map["__ledf2"] = (size_t) &__ledf2;
	map["__lesf2"] = (size_t) &__lesf2;
	map["__letf2"] = (size_t) &__letf2;
	map["__muldf3"] = (size_t) &__muldf3;
	map["__mulsf3"] = (size_t) &__mulsf3;
	map["__multf3"] = (size_t) &__multf3;
	map["__negdf2"] = (size_t) &__negdf2;
	map["__negsf2"] = (size_t) &__negsf2;
	map["__negtf2"] = (size_t) &__negtf2;
	map["__subdf3"] = (size_t) &__subdf3;
	map["__subsf3"] = (size_t) &__subsf3;
	map["__subtf3"] = (size_t) &__subtf3;
	map["__truncdfsf2"] = (size_t) &__truncdfsf2;
	map["__trunctfdf2"] = (size_t) &__trunctfdf2;
	map["__trunctfsf2"] = (size_t) &__trunctfsf2;
	map["__unorddf2"] = (size_t) &__unorddf2;
	map["__unordsf2"] = (size_t) &__unordsf2;
	map["__unordtf2"] = (size_t) &__unordtf2;
	
	map["__ltsf2"] = (size_t) &__lesf2;
	map["__ltdf2"] = (size_t) &__ledf2;
	map["__lttf2"] = (size_t) &__letf2;
	map["__gtsf2"] = (size_t) &__gesf2;
	map["__netf2"] = (size_t) &__eqtf2;
	map["__nedf2"] = (size_t) &__eqdf2;
	map["__nesf2"] = (size_t) &__eqsf2;
	map["__gttf2"] = (size_t) &__getf2;
	map["__gtdf2"] = (size_t) &__gedf2;
}
