setPixel(int,int,int):
	li $v0, 17
	syscall
	j $31

rgb(int, int, int):
	sll     $5,$5,8
	or      $4,$5,$4
	sll     $2,$6,16
	or      $2,$4,$2
	j       $31
