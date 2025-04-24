verbose: 1
input  file <progs/cout.asm>
output file <output/cout.lc3>
Pass 1 Symbol Table Results
Symbol             ADDRESS (indx)
---------------------------------
SaveR1..............0x0428 (0271)
Return..............0x0424 (1886)
DDR.................0x0427 (2395)
DSR.................0x0426 (2860)
TryWrite............0x0421 (3712)
WriteIt.............0x0423 (3978)


Pass 2 Assembly Results
LABEL               OPCODE    OPERANDS                                 ADDR: INST           BINARY
--------------------------------------------------------------------------------------------------
                    .ORIG     0x0420                                   0420: 0000 0000000000000000
                    ST        R1, SaveR1                               0420: 3207 0011001000000111
TryWrite            LDI       R1, DSR                                  0421: A204 1010001000000100
                    BRzp      TryWrite                                 0422: 07FE 0000011111111110
WriteIt             STI       R0, DDR                                  0423: B003 1011000000000011
Return              LD        R1, SaveR1                               0424: 2203 0010001000000011
                    RTI                                                0425: 8000 1000000000000000
DSR                 .FILL     xFE04                                    0426: FE04 1111111000000100
DDR                 .FILL     xFE06                                    0427: FE06 1111111000000110
SaveR1              .BLKW     1                                        0428: 0000 0000000000000000
                    .END                                               0429: 0000 0000000000000000
Assembly complete
    bin file: <output/cout.lc3>
    words written: <0X000B>
    section: <0X0001> address: <0x0420> size: <0X0009>
