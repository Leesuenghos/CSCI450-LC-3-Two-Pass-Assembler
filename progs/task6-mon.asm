; monitor/display input tests
; Example loop that tests the DDR display status
; register until it is ready, then puts a new character
; into the memory mapped DDR to be displayed
					.ORIG 	0x3000
START			LDI			R1, DSR 	; Test to see if
					BRzp		START			; output register is ready
          LD      R2,  OUTC ; initialize R2 to 0
					STI			R2, DDR	  ; copy R2 to display data register
					TRAP		0x25			; halt
DSR 			.FILL		0xFE04		; Address of DSR
DDR 			.FILL		0xFE06		; Address of DDR
OUTC      .STRINGZ "y"      ; character to output
					.END
