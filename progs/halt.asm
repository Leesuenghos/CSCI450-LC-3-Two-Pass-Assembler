; Service Routine to halt machine
;
; Patt & Patel Figure 9.12
; x25 HALT
; x0520 Trap Service Vector
;
; Halt execution and print a message on the console
	.ORIG	0x0520
	ST	R1, SaveR1		; R1 a temp for MC register
	ST	R0, SaveR0		; R0 is used as working space

; print message that mchine is halting
	LD	R0, ASCIINewline
	TRAP 	x21
	LEA	R0, Message 
	TRAP	x22
	LD	R0, ASCIINewline
	TRAP 	x21

; clear bit 15 at xFFFE to stop the machine
	LDI	R1, MCR			; Load MC register into R1
	LD	R0, MASK		; R0 = x7FFF
	AND 	R0, R1, R0		; Mask to clear the top bit 
	STI	R0, MCR			; Store R0 into MC register

; return from HALT routine.
; (how can this routine return if machine is halted above?)
	LD	R1, SaveR1		; Restore registers
	LD	R0, SaveR0
	RTI 

;
; Some constants
ASCIINewline .FILL x000A
SaveR0	.BLKW	1
SaveR1	.BLKW	1
Message	.STRINGZ "Halting the machine."
MCR	.FILL	xFFFE			; Address of MCR
MASK	.FILL	x7FFF			; Mask to clear the top bit
	.END
