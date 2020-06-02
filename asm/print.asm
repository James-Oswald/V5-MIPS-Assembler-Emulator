
print(char*):
	move    $a0,$4
	li      $v0,4
	syscall
	j       $31
	
print(int):
	move    $a0,$4
	li      $v0,1
	syscall
	j       $31
	
print(char):
	move    $a0,$4
	li      $v0,11
	syscall
	j       $31
