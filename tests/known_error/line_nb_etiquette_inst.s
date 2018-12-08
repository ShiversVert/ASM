# TEST_RETURN_CODE = FAIL
# Erreur sur les numeros de ligne lors de l'affichage
.text
boucle: BEQ $t0 , $t1 , 200
    
.data
lunchtime: .word 12

