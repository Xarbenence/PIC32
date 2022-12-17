

// If you control F and search scanf and printf, you will see those lines commented out, and next to them are my NU32_READ and WRITE UART commands



#include <stdio.h>      // input/output library
#define MAX_YEARS 100   // constant indicating max number of years to track



typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS+1];   // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment



int getUserInput(Investment *invp);     // invp is a pointer to type ... 
void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==
void sendOutput(double *arr, int years);


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


 
void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years; i= i + 1) {   // relational operators ==SecA.4.10==
                                              // struct access ==SecA.4.4==
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
} // ***** END calculateGrowth *****


int getUserInput(Investment *invp) {

  int valid;       // int used as a boolean ==SecA.4.10==

  // I/O functions in stdio.h ==SecA.4.14==
  char msg[100] = {};
  sprintf(msg,"Enter investment, growth rate, number of yrs (up to %d):\r\n", MAX_YEARS);
  NU32_WriteUART3(msg);
  //printf("Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);

  
  char message[100] = {};
  NU32_ReadUART3(message, 100);
  sscanf(message, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years)); // why the & before (invp->inv0)
  
  //scanf("%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  // logical operators ==SecA.4.11==
  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);
  // printf("Valid input?  %d\n",valid);
  char msg1[100] = {};
  sprintf(msg1,"Valid input? %d\r\n", valid);
  NU32_WriteUART3(msg1);

  // if-else ==SecA.4.12==
  if (!valid) { // ! is logical NOT ==SecA.4.11== 
    // printf("Invalid input; exiting.\n");
    char msg2[100] = {};
    sprintf(msg2,"Invalid input; exiting.\r\n");
    NU32_WriteUART3(msg2);
  }
  return(valid);
} // ***** END getUserInput *****



void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];      // defining a string ==SecA.4.9==

  // printf("\nRESULTS:\n\n");
  char msg3[100] = {};
  sprintf(msg3,"\nRESULTS:\n\r");
  NU32_WriteUART3(msg3);
  for (i=0; i<=yrs; i++) {  // ++, +=, math in ==SecA.4.7==
    sprintf(outstring,"Year %3d:  %10.2f\n",i,arr[i]); 
    // printf("%s",outstring);
    char msg4[100] = {};
    sprintf(msg4,"%s\r\n", outstring);
    NU32_WriteUART3(msg4);
  }
  char msg5[100] = {};
  sprintf(msg5,"\r\n");
  NU32_WriteUART3(msg5);
  //printf("\n");
} // ***** END sendOutput *****
