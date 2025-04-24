; LC-3 assembly
; Program to multiply an integer by the constant 6
; Before execution, an integer must be stored in NUMBER.
;
        .ORIG   0x3050
START   LD      R1, SIX
        LD      R2, NUMBER
        AND     R3, R3, #0      ; Clear R3, it will contain
                                ; the resulting product.

; The inner loop
;
AGAIN   ADD     R3, R3, R2
        ADD     R1, R1, #-1     ; R1 keeps track of
        BRp     AGAIN           ; the iterations
        BRzp    START
ALL     JMP     R4
        RET
SUBS    JSR     START           ; jump to start as a subroutine
        JSRR    R3
;
LOADS   LDI     R5, NUMBER
        LDR     R6, R4, 0x22
        LEA     R2, AGAIN
ARITH   NOT     R4, R5
;
        TRAP    0x25
;
NUMBER  .BLKW   5
SIX     .FILL   0x0006
MSG     .STRINGZ "Error Message"
;
        .END