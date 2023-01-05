#include "defs.h"

/// @brief allocates memory for a room and initalizes all its fields
/// @param name is the name of the room
/// @param room is the room being initalized
void initRoom(char *name, RoomType **room)
{
  *room = calloc(1, sizeof(RoomType));

  strcpy((*room)->name, name);

  RoomList *r = NULL;
  initRoomList(&r);
  (*room)->rooms = r;

  EvidenceList *e = NULL;
  initEvidenceList(&e);
  (*room)->evidence = e;

  HunterNotebook *h = NULL;
  initNotebook(&h);
  (*room)->hunters = h;

  (*room)->ghost = NULL;

  sem_init(&((*room)->mutex), 0, 1);
}

/// @brief cleans up all memory associated with a room
/// @param room is the room being free'd
void cleanupRoom(RoomType *room)
{
  // dont free ghost here, free ghost when building is free'd since ghost is global to building
  cleanupRoomNodes(room->rooms); // important to free the nodes and not the contents or else we enter an infinite loop
  cleanupEvidenceNodes(room->evidence);
  cleanupNotebook(room->hunters);
  free(room);
}

/// @brief initalizes a room node with some room data
/// @param room is the room data for the node
/// @param node is the node being initalized
void initRoomNode(RoomType *room, RoomNode **node)
{
  *node = calloc(1, sizeof(RoomNode));
  (*node)->next = NULL;
  (*node)->prev = NULL;
  (*node)->room = room;
}

/// @brief cleans up the node memory only for a given node
/// @param node is the node being cleaned up here
void cleanupRoomNode(RoomNode *node)
{
  // dont free the data here, we want to preserve it for the building list
  free(node);
}

/// @brief initalizes a room list and allocates memory for it
/// @param l is the list being initalized
void initRoomList(RoomList **l)
{
  (*l) = calloc(1, sizeof(RoomList));
  (*l)->head = NULL;
  (*l)->tail = NULL;
}

/// @brief cleans up all the nodes of a list, but not the data inside
/// @param l is the list being free'd
void cleanupRoomNodes(RoomList *l)
{
  RoomNode *temp;
  while (l->head != NULL)
  {
    temp = l->head;
    l->head = l->head->next;
    cleanupRoomNode(temp);
  }
  free(l);
}

/// @brief cleans up all the data associated with a room list, including the nodes and their data
/// @param l is the list being free'd
void cleanupRoomList(RoomList *l)
{
  RoomNode *temp;
  while (l->head != NULL)
  {
    temp = l->head;
    l->head = l->head->next;
    cleanupRoom(temp->room);
    cleanupRoomNode(temp);
  }
  free(l);
}

/// @brief adds a room node to the end of a list
/// @param node is the node being added
/// @param l is the list being added to
void appendRoom(RoomNode *node, RoomList *l)
{
  if (l->head == NULL && l->tail == NULL)
  {
    l->head = node;
    l->tail = node;
  }
  else
  {
    l->tail->next = node;
    node->prev = l->tail;
    l->tail = node;
  }
}

/// @brief helper for wether or not a room has a hunter
/// @param room is the room being checked
/// @return returns true (1) if the room has the ghost, otherwise false (0)

bool hasHunter(RoomType *room)
{
  return room->hunters->count > 0;
}

/// @brief helper for wether or not a room has multiple hunters
/// @param room is the room being checked
/// @return returns true (1) if the room has hunters, otherwise false (0)
bool hasHunters(RoomType *room)
{
  return room->hunters->count > 1;
}

/// @brief helper for wether or not a room has the ghost
/// @param room is the room being checked
/// @return returns true (1) if the room has the ghost, otherwise false (0)
bool hasGhost(RoomType *room)
{
  return room->ghost != NULL;
}

/// @brief copies the room data into another node
/// @param e is the room whose data is being coppied
/// @param node is the node being coppied to
void copyRoom(RoomType *e, RoomNode **node)
{
  initRoomNode(e, node);
}

/// @brief adds the rooms to each others room list
/// @param a is the first room (being added to the second)
/// @param b is the second room (being added to the first)
void connectRooms(RoomNode *a, RoomNode *b)
{
  RoomNode *tempA = NULL;
  RoomNode *tempB = NULL;
  copyRoom(a->room, &tempA);
  copyRoom(b->room, &tempB);

  appendRoom(tempA, b->room->rooms);
  appendRoom(tempB, a->room->rooms);
}

/// @brief finds random room within the given roomlist
/// @param b is the roomlist where a random room will be selected using a counter and the randInt function
/// @return returns a roomtype pointer of the randomly selected room
RoomType *findRandRoom(RoomList *b)
{
  int size = findSizeOfAdjacentRooms(b);
  RoomNode *temp = b->head;
  int a = randInt(0, size);
  int ctr = 0;
  while (temp->next != NULL)
  {
    if (ctr == a)
    {
      break;
    }
    ctr++;
    temp = temp->next;
  }
  return temp->room;
}

/// @brief collects ghostly evidence from the room of which the hunter is in
/// @param hunter the hunter that is going to be collecting ghostly evidence from it's room
/// @return returns a boolean signifying whether the new hunter has collected a ghostly evidence or not
bool collectEvidence(HunterType *hunter)
{

  EvidenceNode *p = hunter->room->evidence->head;
  bool hasGhostly = false;
  while (p != NULL)
  {
    if (p->evidence->type == hunter->equipment)
    {
      EvidenceNode *node = NULL;
      if (isGhostly(p->evidence))
      {
        hasGhostly = true;
        copyEvidence(p->evidence, &node);
        delEvidence(p, hunter->room->evidence);
        ghostlyIsDifferent(hunter, node->evidence->type);

        addEvidence(node, hunter->evidence);
        printf("HUNTER: %s HAS COLLECTED %s GHOSTLY EVIDENCE FROM THIS ROOM %s", hunter->name, evidenceEnumToStr(node->evidence->type), hunter->room->name);

        break;
      }
    }
    p = p->next;
  }

  return hasGhostly;
}

/// @brief finds the size of the given roomlist
/// @param list the size of which is going to be returned
/// @return returns an int of the size of the roomList
int findSizeOfAdjacentRooms(RoomList *list)
{
  RoomNode *temp = list->head;
  int ctr = 0;
  while (temp != NULL)
  {
    ctr++;
    temp = temp->next;
  }
  return ctr;
}

/// @brief prints the name of the given roomnode
/// @param r the roomnode to print
// needs to be updated for when hunters and ghosts are fully implemented
void printRoom(RoomNode *r)
{
  printf("Room: %s\n", r->room->name);
}

/// @brief finds the adjacent rooms to the given roomnode and prints them
/// @param r the roomnode to which the adjacent rooms will be given
void printRooms(RoomNode *r)
{
  printf("Printing rooms connected to %s\n", r->room->name);
  RoomNode *temp = r->room->rooms->head;
  while (temp != NULL)
  {
    printRoom(temp);
    temp = temp->next;
  }
  printf("\n");
}