
// Include Guard
#ifndef CALCULATE__H__
#define CALCULATE__H__

#define MAX_YEARS 100 

typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS+1];   // investment array   ==SecA.4.9==
} Investment;    
  int i;




void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==

#endif