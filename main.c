#include "defs.h"

int main(int argc, char *argv[])
{
  pthread_t threads[MAX_HUNTERS + 1];
  HunterType *hunters[MAX_HUNTERS];
  // Initialize a random seed for the random number generators
  srand(time(NULL));

  BuildingType *b = NULL;
  initBuilding(&b);
  populateRooms(b);

  for (int i = 0; i < MAX_HUNTERS; i++)
  {
    char name[MAX_STR];
    HunterType *h = NULL;
    printf("\nPlease enter a Hunter name: ");
    scanf("%[^\n]%*c", name);
    initHunter(name, i, b, &h);
    hunters[i] = h;
  }

  GhostType *g = NULL;
  initGhost(b, &g);


// creating threads 
  for (int i = 0; i < MAX_HUNTERS; i++)
  {
    printHunter(hunters[i]);
    printf("STARTING THREAD\n");
    pthread_create(threads + i, NULL, updateHunter, hunters[i]);
  }


  pthread_create(threads + MAX_HUNTERS, NULL, updateGhost, g);

  for (int i = 0; i < MAX_HUNTERS + 1; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // printWinner 
  bool areScared = true;
  for (int i = 0; i < MAX_HUNTERS; i++) {
    if (!(hunters[i]->fear >= MAX_FEAR)) {
        areScared = false; 
    }
    else {
      printHunter(hunters[i]);
    }
  }
  if (areScared) {
    printf("THE GHOST HAS WON!\n");
    for (int i = 0; i < MAX_HUNTERS; i++) {
      printHunter(hunters[i]);
    }
  }
  else {
    printf("\nHUNTERS HAVE WON WITH THE MISSING EVIDENCE BEING %s\n", evidenceEnumToStr(g->type));
  }
  printGhost(g);

  cleanupBuilding(b);
  return 0;
}