# TEST_RETURN_CODE = PASS
# allons au ru
.set noreorder
.text
    Lw $t0 , lunchtime          #0
    LW $6, -200($7)             #8
    ADDI $t1,$zero,8            #12
boucle:                         #16
    BEQ $t0 , $t1 , byebye      #16
    NOP                         #20
    addi $t1 , $t1 , 1          #24
    J boucle                    #26
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime:
    .word 12
    .word menu
lunchtime_bis:
    .asciiz "ils disent : \"au ru!\""

.bss
menu:
    .space 24
menu_bis:
    .space 12
