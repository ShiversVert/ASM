# TEST_RETURN_CODE = PASS
.set noreorder
.text
    ADD $t1, $t2, $t3
    Lw $t1 , lunchtime
    LW $zero, -200($k0)
    ADDI $t1,$t6, 0x2658
    ADDI $a3, $a0, 8
boucle: BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $31 , 1
    J boucle
    BNE $5, $10, toto
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime: .word 12
    .word menu
    .space 13
lunchtime_bis:
    .word test
    .asciiz "ils disent : \"au ru!\""
    .byte 6, 8
    .word 2

.bss
menu:
    .space 24
menu_bis:
    .space 12
    