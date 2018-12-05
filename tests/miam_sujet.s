# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text
    Lw $t1 , lunchtime
    LW $zero, -200($k0)
    ADDI $t1,$at,8
    ADDI $a3, $a0, 8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $32 , 1
    J boucle
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime:
    .word 12
    .word menu
    .space 13   
lunchtime_bis:
    .word test
    .asciiz "ils disent : \"au ru!\""

.bss
menu:
    .space 24
menu_bis:
    .space 12
