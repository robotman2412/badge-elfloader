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

#include "kernel_include.S"



	# Interrupt vector table.
	.global interruptVectorTable



	# Interrupt handler forwarding function.
	.global customISR
	.type customISR, %function
	.text
	.align 2
customISR:
	# Preserve tempregs.
	csrrw t0, mscratch, t0   # ctx in t0,  prev. t0 in mscratch
	sw t1, ctx_scratch_1(t0)
	sw t2, ctx_scratch_2(t0)
	sw t3, ctx_scratch_3(t0)
	csrrw t1, mscratch, t0   # prev. t0 in t1,  ctx in mscratch
	sw t1, ctx_scratch_0(t0)
	
	# Jump to handler.
	.option push
	.option norvc
	auipc t1, 0
	lw t1, 12(t1)
	jr t1
	.word customISR0
	.option pop

	# Size of customISR function.
	.global customISRSize
	.align 2
customISRSize:
	.word .-customISR



	# Interrupt handler function.
	.type customISR0, %function
	.text
	.align 2
customISR0:
	# Tempregs already saved.
	
	# Preserve CSRs.
	csrr t1, mstatus
	sw t1, ctx_scratch_4(t0)
	csrr t1, mepc
	sw t1, ctx_scratch_5(t0)
	
	# Disable interrupts.
	li t1, 0x00000080
	csrc mstatus, t1
	
	# Set return address.
	la t1, .restore
	csrw mepc, t1
	
	# Upgrade privilege to M-mode.
	li t1, 0x00001800
	csrs mstatus, t1
	
	# Get forwarded address.
	csrr t1, mcause
	andi t1, t1, 0x1f
	slli t1, t1, 2
	la t2, interruptVectorTable
	add t1, t2, t1
	lw t1, 0(t1)
	
	# Forward interrupt.
	jr t1
	
.restore:
	# Restore CSRs.
	lw t1, ctx_scratch_4(t0)
	li t2, 0x00001880
	and t1, t2, t1
	csrs mstatus, t1
	lw t1, ctx_scratch_5(t0)
	csrw mepc, t1
	
	# Restore tempregs.
	lw t1, ctx_scratch_0(t0) # prev. t0 in t1,  ctx in t0
	csrw mscratch, t1        # prev. t0 in mscratch
	lw t3, ctx_scratch_3(t0)
	lw t2, ctx_scratch_2(t0)
	lw t1, ctx_scratch_1(t0)
	csrrw t0, mscratch, t0   # prev. t0 in t0,  ctx in mscratch
	
	# Return from interrupt.
	mret
