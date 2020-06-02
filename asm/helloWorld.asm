$LC0:
        .ascii  "hello world!\000"
main:
        addiu   $sp,$sp,-32
        sw      $31,28($sp)
        sw      $fp,24($sp)
        move    $fp,$sp
        lui     $2,%hi($LC0)
        addiu   $4,$2,%lo($LC0)
        jal     print(char*)
        nop

        move    $2,$0
        move    $sp,$fp
        lw      $31,28($sp)
        lw      $fp,24($sp)
        addiu   $sp,$sp,32
        j       $31
        nop