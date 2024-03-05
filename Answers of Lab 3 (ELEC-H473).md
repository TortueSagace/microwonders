# Lab 3: Answers

## Question 1 - creating test vectors

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
(0x7FFF, 0x8000) // 32767 < -32768

// Random Mid-Range Comparisons
(0xCFC7, 0x3039) // -12345 < 12345
(0x0929, 0xE577) // 2345 < -6789

```

## Question 2 - implementation of $<$
### 2.1 Using the original 8 instructions

```java
// Logic: if r1 - r2 < 0, then r2 > r1 and r7 is set to 1 else r7 remains 0

	movi 1,10 // r1 to arbitrary integer
	movi 2,30 // r2 to arbitrary integer
	sw 2,0,2
	movi 3,32768 // r3 to 1000000000000000 to check msb
	addi 4,0,16 // r4 as a counter
loop:	add 6,6,6 // loop to compute -r2 in two's complement notation: -r2 = (not r2)+1
	nand 5,3,2 
	addi 3,3,-1
	beq 3,5,msb
	addi 6,6,1
msb:	add 2,2,2
	addi 4,4,-1
	addi 3,3,1
	beq 4,0,end
	beq 0,0,loop
end:	addi 3,6,1 // -r2 stored in r3
	add 3,1,3 // r3 <- (r1 - r2)
	lw 2,0,2
	movi 4,32768 // r4 to 1000000000000000 to check msb of difference btw r1 & r2
	nand 5,4,3  // r5 <- nand(r3, 10000000000000000) checks whether r3's msb is 1
	addi 4,4,-1
	beq 5,4,neg // if r3's msb is 1, (r1 -r2) is negative & r1 < r2 => r7 <- 1
	beq 0,0,pos // else r1 not < r2 and r7 remains 0
neg:	addi 7,0,1
pos:	add 4,0,0
	add 5,0,0
	add 6,0,0
	halt
```

### 2.2 Using the special IS[1] 17-bits

```java
// Initialize registers (assuming values are already loaded into reg1 and reg2)
// reg3 is used to store the comparison result: 1 if reg1 < reg2, else 0

    movi 1, 0x0001
    movi 2, 0x0002
    movi 3, 0       // Initialize reg3 to 0 by default
    bl   1, 2, isLess   // If reg1 < reg2, branch to isLess
    beq  0, 0, end     // If not less, jump to end
isLess: addi 3, 0, 1    // Set reg3 to 1 to indicate reg1 < reg2
end: halt
```

## Question 3 - multiplication
### 3.1 - using IS[1]

```java
    movi 1, 0x0002
    movi 2, 0x0003
    // Initialize result registers to 0
    ADDI 3, 3, 0  // Initialize reg3 (result LSB) to 0
    ADDI 4, 4, 0  // Initialize reg4 (result MSB) to 0, assuming we manage overflow

    // Initialize loop counter (assuming 16-bit multiplication, thus 16 iterations)
    ADDI 5, 0, 16  // reg5 is the loop counter, initialized to 16

multiply_loop: SHL 6, 2, -1  // Shift reg2 left by 15 (to the MSB), reg6 is a temporary register
    SHA 6, 6, 15  // Arithmetic shift right by 15, moving MSB to LSB, preserving sign

    // If LSB was 1, add reg1 to the result (reg3)
    BEQ 6, 0, skip  // If reg6 is 0 (LSB of reg2 was 0), skip addition
    ADD 3, 3, reg1      // Add reg1 to reg3 (result)

    // Shift reg1 left by 1 (double it) for the next bit in reg2
    SHL 1, 1, 1

    // Prepare for the next iteration
    SHL 2, 2, -1  // Logical shift reg2 right by 1

    // Decrement loop counter and loop if not done
    ADDI 5, 5, -1
    BEQ 5, 0, end_multiply  // If reg5 is 0, multiplication is done
    BEQ 0, 0, multiply_loop // Unconditional jump to next loop iteration

skip: BEQ 0, 0, continue_loop  // Jump to continue loop if addition was skipped

continue_loop: ADDI 5, 5, -1
    BEQ 5, 0, end_multiply  // Check if loop is done
    BEQ 0, 0, multiply_loop // Unconditional jump to next loop iteration

end_multiply: halt
```
### 3.2 - using IS[2]
```java
movi 1, 0x00012
movi 2, 0x0003
// Perform multiplication
MUL 3, 1, 2
// reg3:LSB, reg4:MSB of the result
halt
```
### 3.3 - Conclusion

Firstly, the absence of a multiplication instruction in Special IS[1] leads to a noticeably more complex and lengthy coding process. Programmers need to manually orchestrate a series of steps—like looping, conditional adding, and shifting—to mimic multiplication. This not only makes the code harder to grasp but also increases the likelihood of errors. On the flip side, Special IS[2] streamlines this with its `MUL` instruction, cutting down the code to a straightforward few lines. This simplicity is a boon for clarity and reduces error margins.

When it comes to execution efficiency, the manual method required by Special IS[1] is inherently slower and more resource-intensive, given its reliance on multiple instructions for what can be accomplished in a single step by Special IS[2]. The `MUL` instruction is optimized at the hardware level to perform the operation efficiently, offering a clear advantage in speed and performance.

Resource utilization is another critical aspect. Special IS[1]'s manual approach consumes additional registers for the loop counter, temporary values, and intermediate results, whereas Special IS[2]’s `MUL` instruction minimizes the need for auxiliary registers, freeing up resources for other tasks.

Lastly, from a scalability and maintainability perspective, the benefits of Special IS[2] become even more pronounced as programs increase in complexity. Reduced code complexity directly translates to easier maintenance, debugging, and enhancements. The performance efficiency gained from specialized instructions like `MUL` also means that more complex algorithms and larger data sets can be handled more effectively.
