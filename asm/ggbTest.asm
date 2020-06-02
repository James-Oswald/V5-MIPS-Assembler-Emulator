
main:
        addiu   $sp,$sp,-32
        sw      $31,28($sp)
        sw      $fp,24($sp)
        move    $fp,$sp
        move    $6,$0
        move    $5,$0
        li      $4,255              
        jal     rgb(int, int, int)
        nop

        move    $4,$2
        jal     print(int)
        nop

        move    $2,$0
        move    $sp,$fp
        lw      $31,28($sp)
        lw      $fp,24($sp)
        addiu   $sp,$sp,32
        j       $31
        nop
		