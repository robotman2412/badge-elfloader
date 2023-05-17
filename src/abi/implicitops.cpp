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

#include "implicitops.hpp"

extern "C" {

void __adddf3();
void __addsf3();
void __addtf3();
void __divdf3();
void __divsf3();
void __divtf3();
bool __eqdf2(float,float);
bool __eqsf2(float,float);
bool __eqtf2(float,float);
void __extenddftf2();
void __extendsfdf2();
void __extendsftf2();
void __fixdfdi();
void __fixdfsi();
void __fixsfdi();
void __fixsfsi();
void __fixtfdi();
void __fixtfsi();
void __fixunsdfdi();
void __fixunsdfsi();
void __fixunssfdi();
void __fixunssfsi();
void __fixunstfdi();
void __fixunstfsi();
void __floatdidf();
void __floatdisf();
void __floatditf();
void __floatsidf();
void __floatsisf();
void __floatsitf();
void __floatundidf();
void __floatundisf();
void __floatunditf();
void __floatunsidf();
void __floatunsisf();
void __floatunsitf();
bool __gedf2(float,float);
bool __gesf2(float,float);
bool __getf2(float,float);
bool __ledf2(float,float);
bool __lesf2(float,float);
bool __letf2(float,float);
void __muldf3();
void __mulsf3();
void __multf3();
void __negdf2();
void __negsf2();
void __negtf2();
void __subdf3();
void __subsf3();
void __subtf3();
void __truncdfsf2();
void __trunctfdf2();
void __trunctfsf2();
void __unorddf2();
void __unordsf2();
void __unordtf2();

bool __ltsf2(float,float);
bool __ltdf2(float,float);
bool __lttf2(float,float);
bool __gtsf2(float,float);
bool __netf2(float,float);
bool __nedf2(float,float);
bool __nesf2(float,float);
bool __gttf2(float,float);
bool __gtdf2(float,float);

int __mulsi3(int, int);
int __divsi3(int, int);
unsigned int __udivsi3(unsigned int, unsigned int);
int __modsi3(int, int);
unsigned int __umodsi3(unsigned int, unsigned int);
long __muldi3(long, long);
long __divdi3(long, long);
unsigned long __udivdi3(unsigned long, unsigned long);
long __moddi3(long, long);
unsigned long __umoddi3(unsigned long, unsigned long);

} // extern "C"

// Exports ABI symbols into `map` (no wrapper).
void abi::implicitops::exportSymbolsUnwrapped(elf::SymMap &map) {
	// Floating-point.
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
	
	map["__ltsf2"] = (size_t) &__ltsf2;
	map["__ltdf2"] = (size_t) &__ltdf2;
	map["__lttf2"] = (size_t) &__lttf2;
	map["__gtsf2"] = (size_t) &__gtsf2;
	map["__netf2"] = (size_t) &__netf2;
	map["__nedf2"] = (size_t) &__nedf2;
	map["__nesf2"] = (size_t) &__nesf2;
	map["__gttf2"] = (size_t) &__gttf2;
	map["__gtdf2"] = (size_t) &__gtdf2;
	
	// Integer.
	map["__mulsi3"]  = (size_t) &__mulsi3;
	map["__divsi3"]  = (size_t) &__divsi3;
	map["__udivsi3"] = (size_t) &__udivsi3;
	map["__modsi3"]  = (size_t) &__modsi3;
	map["__umodsi3"] = (size_t) &__umodsi3;
	map["__muldi3"]  = (size_t) &__muldi3;
	map["__divdi3"]  = (size_t) &__divdi3;
	map["__udivdi3"] = (size_t) &__udivdi3;
	map["__moddi3"]  = (size_t) &__moddi3;
	map["__umoddi3"] = (size_t) &__umoddi3;
}
