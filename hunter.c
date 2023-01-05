#include "defs.h"



/// @brief initializes the hunter using the given parameters
/// @param name name of the hunter 
/// @param type type of evidence the hunter can collect 
/// @param b pointer to the building the ghost is in  
/// @param h double pointer to which the hunter is being stored 

void initHunter(char *name, EvidenceClassType type, BuildingType *b, HunterType **h)
{
  // create hunter
  (*h) = calloc(1, sizeof(HunterType));
  strcpy((*h)->name, name);
  (*h)->equipment = type;
  (*h)->building = b;
  (*h)->room = (*h)->building->rooms->head->room;
  (*h)->fear = 0;
  (*h)->boredom = BOREDOM_MAX;
  (*h)->hasDifferentGhostly = 1;

  EvidenceList *hunterList = NULL;
  initEvidenceList(&hunterList);
  (*h)->evidence = hunterList;
  addHunter((*h), b->noteBook);
  addHunter((*h), (*h)->room->hunters);
}

/// @brief cleans up the hunter by using an existing function to clean up its evidence (only part thats independent to it), and then freeing it 
/// @param h hunter that is being cleaned 

void cleanupHunter(HunterType *h)
{
  cleanupEvidenceNodes(h->evidence);
  free(h);
}


/// @brief cleans up the hunter notebook by using an existing function to clean up each hunter 
/// @param n the hunternotebook to clean 

void cleanupHunters(HunterNotebook *n)
{
  for (int i = 0; i < MAX_HUNTERS; i++)
  {
    cleanupHunter(n->hunters[i]);
  }
  free(n);
}

/// @brief initialize the hunter notebook  
/// @param n pointer to the hunter notebook to be intialized and stored in 

void initNotebook(HunterNotebook **n)
{
  *n = calloc(1, sizeof(HunterNotebook));
  (*n)->count = 0;
}

/// @brief cleans up the hunter notebook, by freeing the notebook itself   
/// @param n pointer to the hunter notebook to be cleaned  

void cleanupNotebook(HunterNotebook *n)
{
  free(n);
}

/// @brief adds a hunter to the given hunter notebook, increments the notebook count  
/// @param hunter pointer to the hunter to be added
/// @param notebook pointer to the hunter notebook to which the hunter is going to be added   

void addHunter(HunterType *hunter, HunterNotebook *notebook)
{
  notebook->hunters[notebook->count] = hunter;
  notebook->count++;
}


/// @brief removes a hunter from the given hunter notebook, and shifts the rest of the notebook, decrements the notebook count     
/// @param hunter pointer to the hunter to be removed
/// @param notebook pointer to the hunter notebook to which the hunter is going to removed from 
void removeHunter(HunterType *hunter, HunterNotebook *notebook)
{

  int i = 0;

  // search for id
  while (notebook->hunters[i] != hunter)
  {
    i++;
    if (i >= MAX_HUNTERS)
    {
      return;
    }
  }

  notebook->count--;
  for (int j = i; j < notebook->count; j++)
  {
    notebook->hunters[j] = notebook->hunters[j + 1];
  }
}

/// @brief function in which the hunters thread wil be ran, constantly updating the hunter based on randomized conditions posibily moving rooms, collecting/sharing evidence, dropping evidence, or doing nothing updating its exit conditions     
/// @param hunterArg void pointer, will by typecasted to a hunter argument 
void *updateHunter(void *hunterArg)
{
  HunterType *hunter = (HunterType *)hunterArg;
  
  while (hunter->boredom > 0)
  {
    if (hunter->hasDifferentGhostly >= 3)
    {
      printf("HUNTER: %s, HAS FOUND MORE THAN 3 DIFFERENT GHOSTLY EVIDENCE\n", hunter->name);
      break;
    }
    else
    {

      if (hasGhost(hunter->room))
      {
        if (hunter->fear < 100)
        {
          hunter->fear++;
          hunter->boredom = BOREDOM_MAX;
        }
        else
        {
          printf("HUNTER: %s, HAS RAN AWAY SCARED\n", hunter->name);
          break;
        }
      }
    }
    int c = randInt(0,3);
    switch (c)
    {
    case 0:
      printf("\n");
      bool evidenceEmpty = false;
      RoomType *curr = hunter->room;
      if (!sem_trywait(&(curr->mutex)))
      {
        if (curr->evidence->head != NULL)
        {
          if (collectEvidence(hunter)) {
            hunter->boredom = BOREDOM_MAX; 
          }
        }
        else
        {
          evidenceEmpty = true;
        }
        sem_post(&(curr->mutex));
      }
      if (evidenceEmpty)
      {
        generateStandardEvidence(hunter);
      }
      break;

    case 1:
      //  moving
      printf("\n");
      RoomType *prev = hunter->room;
      RoomType *next = NULL;
      if (!sem_trywait(&(prev->mutex)))
      {
        while (true)
        {
          next = findRandRoom(hunter->room->rooms);
          if (!sem_trywait(&(next->mutex)))
          {
            updateHunterRoom(hunter, next);
            printf("HUNTER: %s, HAS MOVED TO THIS ROOM %s\n", hunter->name, next->name);
            break;
          }
        }
        sem_post(&(prev->mutex));
        sem_post(&(next->mutex));
      }
      break;

    case 2:
      // communiucating
      if (hasHunters(hunter->room))
      {
        HunterType *temp;
        RoomType *curr = hunter->room;
        do
        {
          temp = pickRandomHunter(curr->hunters);
        } while (temp == hunter);
        shareGhostlyEvidence(hunter, temp);
        // only share one piece of evidence right now you can change, it gets too slow
      }
      break;
    }
     hunter->boredom--;
    sleep(randFloat(0.2, 1));
  }
  printf("HUNTER: %s HAS GOTTEN BORED\n", hunter->name);
  return NULL;
}


/// @brief picks a random hunter from the given hunter notebook      
/// @param hunters hunter notebook to randomly pick a hunter from 

HunterType *pickRandomHunter(HunterNotebook *hunters)
{

  int a = randInt(0, hunters->count);
  // return random hunter
  return hunters->hunters[a];
}


/// @brief updates the hunter and its room       
/// @param hunter to which it will be removed from it's old room, and added to a new one 
/// @param room new room the hunter is in 

void updateHunterRoom(HunterType *hunter, RoomType *room)
{
  // remove the hunter from the rooms collection
  removeHunter(hunter, hunter->room->hunters);
  // new room of hunter
  hunter->room = room;
  // add hunter to the new rooms hunters collection
  addHunter(hunter, hunter->room->hunters);
}

/// @brief initialize standard evidence based on the hunters equipment, and add this to the it's evidence collection        
/// @param hunter hunter to add standard evidence too 

void generateStandardEvidence(HunterType *hunter)
{
  EvidenceType *e = NULL;
  EvidenceNode *node = NULL;
  switch (hunter->equipment)
  {
  case EMF:
    initEvidence(hunter->equipment, randFloat(0, 4.9), &e);
    break;
  case TEMPERATURE:
    initEvidence(hunter->equipment, randFloat(0, 27), &e);
    break;
  case FINGERPRINTS:
    initEvidence(hunter->equipment, 0, &e);
    break;
  case SOUND:
    initEvidence(hunter->equipment, randFloat(40, 70), &e);
    break;
  }
  initEvidenceNode(e, &node);
  addEvidence(node, hunter->evidence);
  initEvidenceNode(e, &node);
  addEvidence(node, hunter->building->evidence);
  printf("HUNTER: %s, HAS GENERATED SOME STANDARD EVIDENCE OF THIS TYPE %s\n", hunter->name, evidenceEnumToStr(node->evidence->type));
}



/// @brief print the hunter to the screen        
/// @param hunter hunter to which it's attributes will be printed 

void printHunter(HunterType *hunter)
{
  printf("HUNTER NAME: %s, IN THIS ROOM: %s\n", hunter->name, hunter->room->name);
}

/// @brief prints the hunters in a hunternotebook by using an existing function  
/// @param n hunternotebook to traverse and print it's hunters  

void printHunters(HunterNotebook *n)
{
  for (int i = 0; i < n->count; i++)
  {
    printHunter(n->hunters[i]);
  }
}