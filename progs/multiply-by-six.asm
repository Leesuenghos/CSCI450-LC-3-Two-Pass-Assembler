; LC-3 assembly
; Program to multiply an integer by the constant 6
; Before execution, an integer must be stored in NUMBER.
;
        .ORIG   0x3050
        LD      R1, SIX
        LD      R2, NUMBER
        AND     R3, R3, #0      ; Clear R3, it will contain
                                ; the resulting product.

; The inner loop
;
AGAIN   ADD     R3, R3, R2
        ADD     R1, R1, #-1     ; R1 keeps track of
        BRp     AGAIN           ; the iterations
;
        TRAP    0x25
;
NUMBER  .BLKW   5
SIX     .FILL   0x0006
MSG     .STRINGZ "Error Message"
;
        .END