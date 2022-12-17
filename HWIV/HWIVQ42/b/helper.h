/******************************************************************************
 * HELPER FUNCTION PROTOTYPES  ==SecA.4.2==
 ******************************************************************************/


// Include Guard
#ifndef HELPER__H__
#define HELPER__H__

int getUserInput(Investment *invp);     // invp is a pointer to type ...
void calculateGrowth(Investment *invp); // ... Investment ==SecA.4.6, A.4.8==
void sendOutput(double *arr, int years);

#endif // HELPER__H__