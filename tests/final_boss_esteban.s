# TEST_RETURN_CODE=PASS


# allons au ru
# j'adore znmuiabzd & é  àç_  "è()

.set noreorder
.text
pouet:
    add $4,$2,$3
    addi $5,$6, 20
    sub $9,$19,$4
    BNE $5, $6, pouet
    MULT $18,$25

    LW $15, jesuispas           #inexistant (boff)
pouette:
    #DIV $29, $30
    AND $4,$2,$3
    #addi $16,$24, praaa           #inexistant (I)
    OR $26,$19,$4

    addi $5,$9, pouette           #meme section negatif (I)
    addi $5,$9, do
.data
do:
    .byte 12
    .asciiz " bonsoir \\ jazd&é \" azd \"  \n oui "
.bss
monter:
    .space 62

.text
    XOR $23,$15,$19

    Lw $t0 , monter               #changer section negatif (boff)
    SLL $24,$2, 12
    BEQ $3,$4,pouette
    BNE $5,$6, rlw
    BNE $5,$6, pouette
    beq $3,$4,rlw
    nop
    LUI $18, pouette              #meme section negatif (I)
    SRL $12,$6,13
    BGTZ $3,rlw
    BGTZ $3,pouette
    SLT $9,$3,$7
rlw:
    add $2,$3,$4
    Lw $t0 , pouette              #meme section negatif (boff)
    LW $6, -200($7)
    SW $30, -300($8)
    LUI $15, 50

    #BEQ $t0 , $t1 , rlw           #meme section neg (rel)

    MFHI $6
    J 12
.bss
menu:
    .space 24
    .space 0x10

.text

    MFLO $4
    #BLT $9,$16, tex


    BEQ $t0 , $t1 , rlw           #meme section negatif (rel) BUUUUUUUUUUg
    #BEQ $3,$28, 24                #BUUUUUUUUUUUUG


    #BNE $1,$5, 52

    #BGTZ $18, nonexistolala     #inexistant (rel)
    #BGTZ $19, 80


    lw $5, menu         ## waning chelou             #changer section neg(boff)

    JAL rlw                     #meme section negatif(target)

.data
lunchtime:
    .word 12
    .word menu
prot:
    .asciiz "ils disent : \"au ru!\" "
    .byte 52

.text
tex:

    J 68
wouah:
    JAL 12



boucle:
    addi $t1 , $t1 , 1
    Lw $t0 , lunchtime          #changer section negatif (boff)
    J boucle                    #meme section negatif (target)
    NOP
byebye:
    JR $18
    JAL viteviteauru              #inexistant (target)

    #MoVe $8,$4         #en realite un addu (opcode 0x21)
    #BLT $3,$4, -16
    neg $12,$25
    #LI $14, 654 #lbu en vrai 0x24

    Jal wouah                   #meme section negatif (target)


    #SW $30, prot       ## waning chelou           #meme section negatif (boff)
.bss
    .space 24
    .space 0x10
sicom:
    .space 45
