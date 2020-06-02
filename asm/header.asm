
	jal updateDisplay
	jal pause
	jal main
	jal updateDisplay
	li  $v0,10
	syscall
	
updateDisplay:
	li $v0, 18
	syscall
	j $31
	
pause:
	li $v0, 19
	syscall
	j $31