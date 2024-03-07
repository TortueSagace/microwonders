# Lab 3: Answers
For this report, the operator $<$ was implemented. First, a set of test vectors is given to cover several possible corner cases. The operator is then implemented, first using IS[0] architecture, and then using Special IS[1] 17-bit architecture.
## Question 1 - creating test vectors
Different test vectors are considered. The program of question 2.1 is seen to succeed in almost all cases except in those where the computation of (r1-r2) in two's complement arithmetic causes register overflow: "r1-r2<-32768 or r1-r2>32767". This is because the program relies on the computation of (r1-r2) as an intermediary result.
The program of question 2.2 secceeds for all test vectors.

```java
// Zero Comparisons
(0x0000, 0x0001) // 0 < 1
(0x0000, 0xFFFF) // 0 < -1
(0x0000, 0x0000) // 0 < 0

// Positive Number Comparisons
(0x0001, 0x0002) // 1 < 2
(0x0002, 0x0001) // 2 < 1
(0x0001, 0x0001) // 1 < 1

// Negative Number Comparisons
(0xFFFE, 0xFFFF) // -2 < -1
(0xFFFF, 0xFFFE) // -1 < -2
(0xFFFF, 0xFFFF) // -1 < -1

// Cross-Zero Comparisons
(0xFFFF, 0x0000) // -1 < 0
(0x0001, 0x0000) // 1 < 0
(0xFFFF, 0x0001) // -1 < 1

// Boundary Conditions
(0x8000, 0x7FFF) // -32768 < 32767
(0x7FFF, 0x8000) // 32767 < -32768
(0x8000, 0x0000) // -32768 < 0
(0x0000, 0x7FFF) // 0 < 32767

// Random Mid-Range Comparisons
(0xCFC7, 0x3039) // -12345 < 12345
(0x0929, 0xE577) // 2345 < -6789

```

## Question 2 - implementation of $<$

We chose to implement the $<$ sign.
### 2.1 Using the original 8 instructions

Given two signed number in two's complement arithmetic r1 and r2, checking whether r1<r2 can be done by checking whether (r1-r2)<0. This is done by first transforming r2 in its negative, two's complement representation -r2 then adding it to r1 and finally checking whether the result is negative - i.e. whether its MSB is equal to 1 as are all negative numbers in two's complement arithmetic. The result of the computation is written in register 7: "r7<-1 if r1<r2 else r7<-0".

```java
	movi 1,30 // r1 to arbitrary integer
	movi 2,10 // r2 to arbitrary integer
	sw 2,0,2
	movi 3,32768 // r3 to 1000000000000000 to check msb
	addi 4,0,16 // r4 as a counter
loop:	// loop to compute -r2 in two's complement notation: -r2 = (not r2)+1, stores it in r6
	add 6,6,6 // shift r6 left
	nand 5,3,2 // r5 <- nand(1000000000000000, r2)
	addi 3,3,-1 // r3 <- 0111111111111111
	beq 3,5,msb // if r3 = r5, r2's MSB is 1 
	addi 6,6,1 // if r2's MSB is 0, write 1 in r6
msb:	add 2,2,2 // shift r2 on the left
	addi 4,4,-1 // decrement the counter
	addi 3,3,1 // r3 <- 10000000000000000
	beq 4,0,end // if the counter is exhausted, end the program
	beq 0,0,loop // else loop
end:	addi 3,6,1 // store r6 containing -r2 in r3
	add 3,1,3 // r3 <- (r1 - r2)
	lw 2,0,2 // reset r2's original value
	movi 4,32768 // r4 to 1000000000000000 to check msb of difference btw r1 & r2
	nand 5,4,3  // r5 <- nand(r3, 10000000000000000) checks whether r3's msb is 1
	addi 4,4,-1 // r4 <- 01111111111111111
	beq 5,4,neg // if r3's msb is 1, (r1 -r2) is negative & r1 < r2 => r7 <- 1
	beq 0,0,pos // else not r1 < r2 and r7 remains 0
neg:	addi 7,0,1
pos:	add 4,0,0
	add 5,0,0
	add 6,0,0
	halt
```

### 2.2 Using the special IS[1] 17-bits

```java
 movi 1, 0x0001
    movi 2, 0x0002
    movi 3, 0       // Initialize reg3 to 0 by default
    bl   1, 2, isLess   // If reg1 < reg2, branch to isLess
    beq  0, 0, end     // If not less, jump to end
isLess:	addi 3, 0, 1    // Set reg3 to 1 to indicate reg1 < reg2
end:	halt
```

## Question 3 - multiplication
### 3.1 - using IS[1]
Thanks to IS1's extended set of instructions and most noticeably its ability to shift registers of a constant amount as well as automatically branch when an overflow is detected, the multiplication can now be performed in linear time w.r.t. the operand's size as opposed to exponential time. Instead of decrementing one operand until it reaches zero while adding the other to the current value of the result register, the following program performs conventional binary multiplication.

```java
	MOVI 1,20000
	MOVI 2,8
 	MOVI 3,0
	MOVI 4,0
	MOVI 5,0
	MOVI 6,0
	MOVI 7,0
    // Initialize loop counter (assuming 16-bit multiplication, thus 16 iterations)
	BEQ 1,0,end
	BEQ 2,0,end
	SW 2,0,2
multiply_loop: ADDI 5,0,1  // set r5 to 00000000000000001
	NAND 6, 2, 5 
 	NOR 6, 6, 0 		
	BEQ 6,5,lsb2	 // check whether r2's LSB is 1	
	BEQ 0,0,nlsb2
lsb2:	ADD 5,1,0  // if r2's LSB is 1, store r1 in r5 and shift it left by the rank of r2's current lsb (0 -> 15)
	SHA 5,5,7,shift_overflow // manages shift overflow
	BEQ 0,0,nso // else go to nso; no shift overflow
shift_overflow: ADD 5,1,0 // if there is shift overflow, store r1 back in r5
	ADDI 6,0,16 //  r6 <- 16 - r7
	SUB 6,6,7
	SUB 6,0,6
	SHA 6,5,6 // r6 <- r5 >> 16-r7  (store the overflowing bits in r6) 
	ADD 4,4,6 // add the overflowing bits to r4
	SHA 5,5,7 // reshift r5 after having managed the overflowed bits
	ADD 3,3,5,ad_overflow // add r5 (shifted value of r1 of the current r2's LSB rank) to r3  
	BEQ 0,0,nlsb2
nso:	ADD 3,3,5,ad_overflow 
	BEQ 0,0,nlsb2
ad_overflow: ADDI 4,4,1 // if adding r5 to r3 causes overflow, add carry to r4
nlsb2:	SHIFTI 2,2,-1 // shift r2 on the right
	ADDI 7,7,1 // increase rank of r2's current lsb
   	BEQ 2,0,end // if r2 has been shifted to 0, end of the program
	BEQ 0,0,multiply_loop // else, continue to add shifted values of r1 to r3
end: 	LW 2,0,2 // clean results
	MOVI 5,0
	MOVI 6,0
	MOVI 7,0
	HALT
```

![bugOlineTool](https://github.com/TortueSagace/microwonders/assets/99293212/d680fc29-747e-4018-a931-a534543146aa )
The jar simulator on the left returns the right value but not the online verification tool on the right.



### 3.2 - using IS[2]
```java
movi 1, 0xffff
movi 2, 0x0003
// Perform multiplication
MUL 4, 1, 2 //Multiply reg1 and reg2 and store result in reg3 and reg4
// Because the mul operator stores the LSB in reg4 and MSB in reg3, we need to switch the content of the two registers
SW 4,0,1 // Store data of reg4
ADD 4,3,0 // Set data of reg3 in reg4
LW 3,0,1 // Load data of reg4 that was stored
halt
```
### 3.3 - Conclusion

Firstly, the absence of a multiplication instruction in Special IS[1] leads to a noticeably more complex and lengthy coding process. Programmers need to manually orchestrate a series of steps—like looping, conditional adding, and shifting—to mimic multiplication. This not only makes the code harder to grasp but also increases the likelihood of errors. On the flip side, Special IS[2] streamlines this with its `MUL` instruction, cutting down the code to a straightforward few lines. This simplicity is a boon for clarity and reduces error margins.

When it comes to execution efficiency, the manual method required by Special IS[1] is inherently slower and more resource-intensive, given its reliance on multiple instructions for what can be accomplished in a single step by Special IS[2]. The `MUL` instruction is optimized at the hardware level to perform the operation efficiently, offering a clear advantage in speed and performance.

Resource utilization is another critical aspect. Special IS[1]'s manual approach consumes additional registers for the loop counter, temporary values, and intermediate results, whereas Special IS[2]’s `MUL` instruction minimizes the need for auxiliary registers, freeing up resources for other tasks.

Lastly, from a scalability and maintainability perspective, the benefits of Special IS[2] become even more pronounced as programs increase in complexity. Reduced code complexity directly translates to easier maintenance, debugging, and enhancements. The performance efficiency gained from specialized instructions like `MUL` also means that more complex algorithms and larger data sets can be handled more effectively.
