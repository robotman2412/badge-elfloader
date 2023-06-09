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

#include "math.hpp"

extern "C" {
double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double cos(double x);
double sin(double x);
double tan(double x);
double cosh(double x);
double sinh(double x);
double tanh(double x);
double exp(double x);
double frexp(double x, int *exponent);
double log(double x);
double log10(double x);
double modf(double x, double *integer);
double pow(double x, double y);
double sqrt(double x);
double ceil(double x);
double floor(double x);
double fmod(double x, double y);
float acosf(float x);
float asinf(float x);
float atanf(float x);
float atan2f(float y, float x);
float cosf(float x);
float sinf(float x);
float tanf(float x);
float coshf(float x);
float sinhf(float x);
float tanhf(float x);
float expf(float x);
float frexpf(float x, int *exponent);
float logf(float x);
float log10f(float x);
float modff(float x, float *integer);
float powf(float x, float y);
float sqrtf(float x);
float ceilf(float x);
float floorf(float x);
float fmodf(float x, float y);
}

// Exports ABI symbols into `map` (no wrapper).
void abi::math::exportSymbolsUnwrapped(elf::SymMap &map) {
	// From math.h:
	map["acos"] = (size_t) &acos;
	map["asin"] = (size_t) &asin;
	map["atan"] = (size_t) &atan;
	map["atan2"] = (size_t) &atan2;
	map["cos"] = (size_t) &cos;
	map["sin"] = (size_t) &sin;
	map["tan"] = (size_t) &tan;
	map["cosh"] = (size_t) &cosh;
	map["sinh"] = (size_t) &sinh;
	map["tanh"] = (size_t) &tanh;
	map["exp"] = (size_t) &exp;
	map["frexp"] = (size_t) &frexp;
	map["log"] = (size_t) &log;
	map["log10"] = (size_t) &log10;
	map["modf"] = (size_t) &modf;
	map["pow"] = (size_t) &pow;
	map["sqrt"] = (size_t) &sqrt;
	map["ceil"] = (size_t) &ceil;
	map["floor"] = (size_t) &floor;
	map["fmod"] = (size_t) &fmod;
	map["acosf"] = (size_t) &acosf;
	map["asinf"] = (size_t) &asinf;
	map["atanf"] = (size_t) &atanf;
	map["atan2f"] = (size_t) &atan2f;
	map["cosf"] = (size_t) &cosf;
	map["sinf"] = (size_t) &sinf;
	map["tanf"] = (size_t) &tanf;
	map["coshf"] = (size_t) &coshf;
	map["sinhf"] = (size_t) &sinhf;
	map["tanhf"] = (size_t) &tanhf;
	map["expf"] = (size_t) &expf;
	map["frexpf"] = (size_t) &frexpf;
	map["logf"] = (size_t) &logf;
	map["log10f"] = (size_t) &log10f;
	map["modff"] = (size_t) &modff;
	map["powf"] = (size_t) &powf;
	map["sqrtf"] = (size_t) &sqrtf;
	map["ceilf"] = (size_t) &ceilf;
	map["floorf"] = (size_t) &floorf;
	map["fmodf"] = (size_t) &fmodf;
}
