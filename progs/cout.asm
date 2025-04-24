; Service Routine for console output
;
; Patt & Patel Figure 9.12
; x21 OUT
; x0420 Trap Service Vector
;
; Write a character in R0[7:0] to console display
	.ORIG	0x0420
	ST	R1, SaveR1		; R1 will be used to poll the DSR
					; hardware
; Write the character
TryWrite LDI	R1, DSR			; Get status 
	BRzp	TryWrite		; Bit 15 on says display is ready
WriteIt	STI	R0, DDR			; Write character

; return from trap 
Return	LD	R1, SaveR1		; Restore registers 
		RTI 			; Return from trap 
;
DSR	.FILL	xFE04			; Address of display status register
DDR	.FILL	xFE06			; Address of display data register
SaveR1	.BLKW	1
	.END