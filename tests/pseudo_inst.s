# TEST_RETURN_CODE = PASS

LW $18, test1
SW $22, test2
MOVE $12, $3
NEG $18, $3
LI $14, 0x128
BLT $6, $3, test3
