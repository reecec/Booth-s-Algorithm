/// Function prototypes
int BoothsAlg(char, char);                     // Assembly language implementation of Booth's alg
long TestBooths();                             // Test harness for Booth's algorithm
extern void trace(word, word) asm ("TRACE");   // make the trace function visible to the assembler
void printBinary(word);                        // 16 bit 'Decimal' to binary conversion to serial monitor

// Global variable to enable/disable printing to serial monitor
boolean traceflag = false;

void setup()
{
  Serial.begin(9600);
  Serial.println();
 
   //calling the booth's algorithm assembly code 
    BoothsAlg(2,2);

    //calling Testbooths method and printing number of errors
    int DisplayError = TestBooths();
    Serial.println(DisplayError);
}


void loop()
{ 
  //  Used below code to check that printbinary was working 
  //  word nn = 32768;
  //  if(nn>0)
  //  {
  //  printBinary(nn);
  //  nn >>= 1;
  //  Serial.println();
  //  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Wrapper for the C trace function, which saves and restores
/// registers and flags
//////////////////////////////////////////////////////////////////////////////////////////////////////////
asm volatile(

"dumpRegs:   push r22           ; save the multiplicand, Yreg         \n"
"            push r23           ; save the loop count, Nreg           \n"
"            push r24           ; save the product lo byte Pl         \n"
"            push r25           ; save the product hi byte Ph         \n"  
"            in r16, __SREG__   ; save the status register SREG       \n"
"            call TRACE         ; call the C function trace()         \n"
"            out __SREG__, r16  ; restore the status register, SREG   \n"
"            pop r25            ; restore working registers....       \n"
"            pop r24            ; note that these must be popped off  \n"
"            pop r23            ; the stack in reverse order to that  \n"
"            pop r22            ; in which they were pushed           \n"
"            ret                ; return back to Booth's Alg          \n"
);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The actual trace function body - write in C
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void trace(word PhPl, word NregYreg)
//used to print from dumpregs in assembly code
{
  //using traceflag to hide result of booths/testing printed to the serial monitor
  if(traceflag == true)
  {
     //printing registers 
     printBinary(PhPl);
     Serial.print("  ");

     printBinary(NregYreg);
     Serial.println("");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Signed Multiplication using Booth's Algorithm
///
/// returns 16 bit signed product of multiplicand * multiplier
///
/// parameters are passed as:
/// r24 = multiplier and Pl
/// r25 = 0              Ph
/// r22 = multiplicand   YReg
/// r23 = 0              Nreg
///
/// returns the product in r25,r24
///
/// ref C function parameter passing, https://gcc.gnu.org/wiki/avr-gcc#Register_Layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int BoothsAlg(char multiplier, char multiplicand)
{
  asm volatile(

  /// insert your assembly language implementation here
  /// Do NOT remove or edit anything beyond this point
  
                  //Booths algorithm implemented assembly code
                  
      
        "               CLR r25                            ; clearing register 25 \n"
        "               LDI r23,8                          ; loading an 8 bit register and 8 loop counter \n"
        "               call dumpRegs                      ; displaying state of registers \n"
        "               CLC                                ; clearing the carry flag \n"     
        
        "loop%=:                                           ; loop for booths algorithm \n"
        "               BST r24,0                          ; stores bit 0 r24 in T Flag i.e least signifcant bit of product register\n"
        "               BRCS carry1                        ; branch to carry1 method if carry was set, meaning carries value is 1\n"
        "               BRTS do_subtract                   ; branch to do_subtract if carry is 0 \n"
        "               JMP do_nothing                     ; skip carry1, do_subtract and do_addition and go straight to do_nothing if the carry and last bit are both 00 or 11. \n"
        
        "carry1:                                           ; if carry is 1 method\n"
        "               BRTS do_nothing                    ; if the branch was set and carry and least signifcant bit of product register are equal 00 or 11, go straight to do_nothing i.e \n"
        "               JMP do_addition                    ; skip do_subtraction and go to do_addition if carry is and least signicantleast signifcant bit of product register is 0 \n"
 
        "do_subtract:                                      ; method for subtracting registers\n"
        "               SUB r25,r22                        ; subtract Ph/r25 from multiplicand/r22 \n"
        "               JMP shift_right                    ; skip do_addition and do_ nothing and go straight to shift right \n"
 
        "do_addition:                                      ; method for adding registers \n"
        "               ADD r25,r22                        ; add Ph/r25 to multilicand/r22 \n"
        "               JMP shift_right                    ; skip do_nothing and go straight to shift right \n"
     
        "do_nothing:                                       ; if carry and least signifcant bit of product register = 00 or 11 dont't add or subtract  \n"  

        "shift_right:                                      ; method for shifting right\n"
        "               ASR r25                            ; shift the Ph register to the right\n"
        " ;              ROR r24                            ; shifts the muliplier/Pl to the right and bit 0 to carry flag \n"
        "               DEC r23                            ; decrement from r23, the loop counter  \n"
        "               call dumpRegs                      ; display state of registers at all 8 steps of algorithm\n"
        "               BRNE loop%=                        ; return to loop if counter isn't 0\n"

 
 
 
       "          push r24                                 ; \n"      
       "          push r25                                 ; \n"
:
:
: 
    "r16", "r22", "r23", "r24", "r25" // clobber list
  );

  delay(0);                     // ensures that the optimiser doesn't mess with the code
  asm("pop r25");               // recover the product high and lo bytes to return to caller
  asm("pop r24");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prints a 16 bit word as binary, with leading zeros ('decimal' to binary conversion) - write in C
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void printBinary(word n)
{
    // Goes from  most signifcant bit ot least significant bit to print binary value
    for (int a =15; a>=0; a--)
    {
      
      //using varable to store each bit value along for loop
      word binaryCon;
      
      //checking each bit value along 16 bits
      binaryCon = n >> a;
      
      //if binarycon at an step along is 1 then print 1 otherwise print 0
      if(binaryCon & 1)
        Serial.print("1");
      else 
        Serial.print("0");
    }
}


long TestBooths() 
{  
      //variable used for adding up amount of errors
      long counter  = 0;

     // multiplier between -128 and +127
     for(int mp= -128; mp <= 127; mp++) {  
       
     // multiplicand between -127 and +127       
     for(int mc = -127; mc <= 127; mc++) {              
      
     //expected result 
     int expected = mp  * mc;

     //call BoothsAlg returning result
     int outcome = BoothsAlg(mp, mc); 

      // checks to see if result of booths is what it should be
      if(outcome != expected)  
      {  
        traceflag =true;
        BoothsAlg(mp, mc);
        traceflag = false;
        Serial.println("");
      // if theres an error adds 1 to error count
        counter++;
      }
    }
  }
    //printin to serial monitor with the error amount
    Serial.println("Testing Algorithm");
    Serial.print("Errors = ");
    //returns counter amount 
    return counter;
}

  
  





