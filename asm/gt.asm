main:
        addiu   $sp,$sp,-40
        sw      $31,36($sp)
        sw      $fp,32($sp)
        move    $fp,$sp
        sw      $0,24($fp)
$L5:
        lw      $2,24($fp)
        nop
        slt     $2,$2,144
        beq     $2,$0,$L2
        nop

        sw      $0,28($fp)
$L4:
        lw      $2,28($fp)
        nop
        slt     $2,$2,255
        beq     $2,$0,$L3
        nop

        lw      $6,24($fp)
        lw      $5,28($fp)
        lw      $4,24($fp)
        jal     rgb(int, int, int)
        nop

        move    $6,$2
        lw      $5,24($fp)
        lw      $4,28($fp)
        jal     setPixel(int, int, int)
        nop

        lw      $2,28($fp)
        nop
        addiu   $2,$2,1
        sw      $2,28($fp)
        b       $L4
        nop

$L3:
        lw      $2,24($fp)
        nop
        addiu   $2,$2,1
        sw      $2,24($fp)
        b       $L5
        nop

$L2:
        move    $2,$0
        move    $sp,$fp
        lw      $31,36($sp)
        lw      $fp,32($sp)
        addiu   $sp,$sp,40
        j       $31
        nop