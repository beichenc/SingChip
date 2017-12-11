  # labwork.S
  # Written 2017 by Amy Chen & Beichen Chen
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

.data
.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
.text
.global delay
.global time2string
.global enable_interrupt

  # Written 2017-09-07 by Beichen Chen & Amy Chen

  enable_interrupt:
    PUSH $ra
    ei
    nop

    POP $ra
  	jr	$ra
  	nop

  hexasc:
  	li	$t0, 0x0000000F	# Set $t0 to have 28 bits of 0s and 4 bits of 1s
  	and	$t1, $a0, $t0	# $t1 will contain the filtered value of what we want to convert to ACSII.
  	ble	$t1, 9, digit	# branch to digit if the value we want to convert is 0-9, otherwise continue to letter
  	nop

  letter:
  	addi	$v0, $t1, 0x37	# $v0 will contain the ACSII hex for $s1
  	jr 	$ra		# return
  	nop

  digit:
  	addi	$v0, $t1, 0x30	# v0 will contain the ACSII hex for $s1
  	jr	$ra		# return
  	nop

  time2string:

  	PUSH $ra
  	PUSH $s1
  	PUSH $s2
  	PUSH $s3
  	PUSH $s4
  	PUSH $a0		# Question: shouldn't $a0 be pushed in main (the caller)? But we still need it in the second part of time2string. How to do this???

  	and 	$s1, $a1, 0xF000
  	div 	$s1, $s1, 0x1000
  	#srl	$s1, $s1, 12	# Does the same thing as above
  	move	$a0, $s1
  	jal	hexasc
  	nop
  	move	$s1, $v0	# $s1 now contains the ASCII hex for the first digit

  	and	$s2, $a1, 0x0F00
  	div	$s2, $s2, 0x0100
  	move	$a0, $s2
  	jal	hexasc
  	nop
  	move	$s2, $v0	# $s2 now contains the ASCII hex for the second digit

  	and	$s3, $a1, 0x00F0
  	div	$s3, $s3, 0x0010
  	move	$a0, $s3
  	jal	hexasc
  	nop
  	move	$s3, $v0	# $s3 now contains the ASCII hex for the third digit

  	and	$s4, $a1, 0x000F
  	div	$s4, $s4, 0x0001
  	move	$a0, $s4
  	jal	hexasc
  	nop
  	move	$s4, $v0	# $s4 now contains the ASCII hex for the fourth digit
  	beq	$s4, 0x32, two	# if last digit is 2, branch to two

  	POP $a0
  	sb	$s1, 0($a0)	# storing bytes
  	sb	$s2, 1($a0)
  	li	$t5, 0x3A
  	sb	$t5, 2($a0)
  	sb	$s3, 3($a0)
  	sb	$s4, 4($a0)
  	sb	$0, 5($a0)

  	POP $s4
  	POP $s3
  	POP $s2
  	POP $s1

  	POP $ra
  	jr	$ra
  	nop

  delay:
  	blez	$a0, donewhile
  	nop
  	sub	$a0, $a0, 1
  	li	$t0, 0
  	#li	$t1, 10		# Constant to change so that 1000 as input = 1000ms delay
  	#li	$t1, 10		# On Macbook
  	li	$t1, 4500	# on Chip

  for:
  	beq	$t0, $t1, donefor
  	nop
  	addi	$t0, $t0, 1
  	j	for
  	nop

  donefor:
  	j	delay
  	nop

  donewhile:
  	jr	$ra
  	nop

  two:
  	POP $a0

  	PUSH $s1
  	PUSH $s2
  	PUSH $s3
  	PUSH $s4
  	PUSH $s5
  	PUSH $s6
  	PUSH $s7

  	li	$s4, 0x54
  	li	$s5, 0x57
  	li	$s6, 0x4F

  	sb	$s1, 0($a0)	# storing bytes
  	sb	$s2, 1($a0)
  	li	$t5, 0x3A
  	sb	$t5, 2($a0)
  	sb	$s3, 3($a0)
  	sb	$s4, 4($a0)
  	sb	$s5, 5($a0)
  	sb	$s6, 6($a0)
  	sb	$0, 7($a0)

  	POP $s7
  	POP $s6
  	POP $s5
  	POP $s4
  	POP $s3
  	POP $s2
  	POP $s1

  	POP $s4	# Pop all the ones pushed in beginning of time2string
  	POP $s3
  	POP $s2
  	POP $s1

  	POP $ra
  	jr	$ra
  	nop
