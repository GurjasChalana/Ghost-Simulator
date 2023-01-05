#include "defs.h"


/// @brief initializes a ghost with given and default paraemters, uses existing functions to help  
/// @param b pointer to the building of the ghost 
/// @param g double pointer to store the new initialized ghost 
void initGhost(BuildingType *b, GhostType **ghost)
{

  (*ghost) = calloc(1, sizeof(GhostType));
  (*ghost)->boredom = BOREDOM_MAX;
  (*ghost)->type = randInt(0, 4);
  (*ghost)->building = b;
  b->ghost = (*ghost);

  RoomType *c = findRandRoom((*ghost)->building->rooms);
  if (c == b->rooms->head->room)
  {
    c = b->rooms->tail->room;
  }
  (*ghost)->room = c;
}

/// @brief cleans up the ghost by freeing it  
/// @param g pointer to ghost to free
void cleanupGhost(GhostType *ghost)
{
  free(ghost);
}

/// @brief similar to updateHunterRoom, updates past and curr room pointers with the ghost  
/// @param g pointer to ghost to update all conditions
void updateGhostRoom(GhostType *g, RoomType *r)
{
  g->room->ghost = NULL;
  g->room = r;
  if (g->room != NULL)
  {
    g->room->ghost = g;
  }
}

/// @brief generates a ghostly evidence depending on the ghost type, and adds it to the rooms collection 
/// @param g pointer to ghost that will drop the evidence 
void createEvidence(GhostType *g)
{
  GhostClassType type;
  int rand = randInt(0, 3);

  switch (g->type)
  {
  case POLTERGEIST:
    switch (rand)
    {
    case 0:
      type = EMF;
      break;

    case 1:
      type = TEMPERATURE;
      break;

    case 2:
      type = FINGERPRINTS;
      break;
    }

  case BANSHEE:
    switch (rand)
    {
    case 0:
      type = EMF;
      break;

    case 1:
      type = TEMPERATURE;
      break;

    case 2:
      type = SOUND;
      break;
    }
  case BULLIES:
    switch (rand)
    {
    case 0:
      type = EMF;
      break;

    case 1:
      type = FINGERPRINTS;
      break;

    case 2:
      type = SOUND;
      break;
    }
  case PHANTOM:
    switch (rand)
    {
    case 0:
      type = TEMPERATURE;
      break;

    case 1:
      type = FINGERPRINTS;
      break;

    case 2:
      type = SOUND;
      break;
    }
  }

  float val = 0;
  switch (g->type)
  {
  case POLTERGEIST:
    val = randFloat(0, 5.0);
    break;
  case BANSHEE:
    val = randFloat(-10.0, 27.0);
    break;
  case BULLIES:
    val = (float)randInt(0, 2);
  case PHANTOM:
    val = randFloat(40.0, 75.0);
    break;
  }

  // init a evidence
  EvidenceType *e = NULL;
  EvidenceNode *node = NULL;
  initEvidence(type, val, &e);
  initEvidenceNode(e, &node);
  addEvidence(node, g->room->evidence);
  initEvidenceNode(e, &node);
  addEvidence(node, g->building->evidence);
  printf("THE GHOST HAS LEFT: %s\n", evidenceEnumToStr(node->evidence->type));
}

/// @brief function in which the ghost thread wil be ran, constantly updating the hunter based on randomized conditions posibily moving rooms, dropping evidence, or doing nothing and updating its exit conditions     
/// @param ghostArg void pointer, will by typecasted to a ghost argument 
void *updateGhost(void *ghostArg)
{
  bool foundHunterAgain = false; 
  GhostType *ghost = (GhostType *)ghostArg;
  while (ghost->boredom > 0)
  {
    if (hasHunter(ghost->room))
    {
      
      if (foundHunterAgain) {
        ghost->boredom = BOREDOM_MAX;
        foundHunterAgain = false; 
      }
      int a = randInt(0, 2);
      if (a == 0)
      {
        RoomType *curr = ghost->room;
        if (!sem_trywait(&(curr->mutex)))
        {
          createEvidence(ghost);
          sem_post(&(curr->mutex));
        }
      }
    }
    // if the ghost-> room doesn't have hunters do one of the following
    // (move to an adjacent room, make new evidence, or nothing)
    else
    {
      int b = randInt(0, 3);

      switch (b)
      {
      case 0:
        RoomType *prev = ghost->room;
        RoomType *next = NULL;
        if (!sem_trywait(&(prev->mutex)))
        {
          while (true)
          {
            next = findRandRoom(ghost->room->rooms);
            if (!sem_trywait(&(next->mutex)))
            {
              updateGhostRoom(ghost, next);
              if (hasHunter(next)) {
                foundHunterAgain = true; 
              }
              printf("THE GHOST HAS MOVED TO: %s\n", next->name);
              break;
            }
          }
          sem_post(&(prev->mutex));
          sem_post(&(next->mutex));
        }

        break;

      case 1:
        printf("\n");
        RoomType *curr = ghost->room;
        if (!sem_trywait(&(curr->mutex)))
        {
          createEvidence(ghost);
          sem_post(&(curr->mutex));
        }

        break;

      case 2:
        // do nothing
        break;
      }
    }
    ghost->boredom--;
    sleep(randFloat(0.5, 1));
  }
  printf("THE GHOST HAS GOT BORED\n");
  return NULL;
}

/// @brief prints the ghost to the screen  
/// @param g pointer to ghost the ghost that will be printed  
void printGhost(GhostType *ghost)
{
  printf("Ghost - room: %s\n", ghost->room->name);
}