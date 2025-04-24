; Keyboard input tests
; Example loop that tests the KBSR status register until
; it has a 1, indicating a key press, then the key
; should be in KBDR, which is copied into R2 
; The halt trap uses and clobbers R0 and R1, so to test
; key press, put it into R2 in this routine
					.ORIG 	0x3000
START			LDI			R1, KBSR	; Test for
					BRzp		START			; character input
					LDI			R2, KBDR	; copy key pressed to R2
					TRAP		0x25			; halt
KBSR			.FILL		0xFE00		; Address of KBSR
KBDR			.FILL		0xFE02		; Address of KBDR
					.END
