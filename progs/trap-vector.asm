; Define the trap vector service table
;
; Patt & Patel Figure 9.10
; 
; 0020: x03E0 x20 GETC
; 0021: x0420 x21 OUT
; 0022: x0460 x22 PUTS
; 0023: x04A0 x23 IN 
; 0024: x04E0 x24 PUTSP
; 0025: x0520 x25 HALT
	.ORIG	0x0000			; Trap vector service table starts at top of memory 0x0000
	.BLKW	x20			; addresses 0x0000 - 0x0019 are not used
	.FILL	0x03E0			; x20 GETC
	.FILL	0x0420			; x21 OUT
	.FILL	0x0460			; x22 PUTS
	.FILL	0x04A0			; x23 IN 
	.FILL	0x04E0			; x24 PUTSP
	.FILL	0x0520			; x25 HALT
	.BLKW	xDA			; Ensure 0x26 - 0xFF are also 0 on load, in case garbage there on machine start
	.END
