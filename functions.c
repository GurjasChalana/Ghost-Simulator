#include "defs.h"

/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number,
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1)
*/
// WONT LET ME USE THESE FUNCTIONS?
int randInt(int min, int max)
{
  return rand() % (max - min) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number,
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1)
*/
float randFloat(float a, float b)
{
  // Get a percentage between rand() and the maximum
  float random = ((float)rand()) / (float)RAND_MAX;
  // Scale it to the range we want, and shift it
  return random * (b - a) + a;
}