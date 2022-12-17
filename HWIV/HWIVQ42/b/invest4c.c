

/******************************************************************************
 * PREPROCESSOR COMMANDS   ==SecA.4.3==
 ******************************************************************************/

#include <stdio.h>      // input/output library
#include "io.h"
#include "calculate.h"


/******************************************************************************
 * DATA TYPE DEFINITIONS (HERE, A STRUCT)  ==SecA.4.4==
 ******************************************************************************/

/******************************************************************************
 * MAIN FUNCTION   ==SecA.4.2==
 ******************************************************************************/


int main(void) {
  

  Investment inv;                // variable definition, ==SecA.4.5==

  while(getUserInput(&inv)) {    // while loop ==SecA.4.13== 
    inv.invarray[0] = inv.inv0;  // struct access ==SecA.4.4==
    calculateGrowth(&inv);       // & referencing (pointers) ==SecA.4.6, A.4.8==
    sendOutput(inv.invarray,     // passing a pointer to an array ==SecA.4.9==
               inv.years);       // passing a value, not a pointer ==SecA.4.6==
  }
  return 0;                      // return value of main ==SecA.4.6==
} // ***** END main *****


