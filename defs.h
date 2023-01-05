#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_STR 64
#define FEAR_RATE 1
#define MAX_FEAR 100
#define MAX_HUNTERS 4
#define USLEEP_TIME 50000
#define BOREDOM_MAX 99

// You may rename these types if you wish
typedef enum
{
  EMF,
  TEMPERATURE,
  FINGERPRINTS,
  SOUND
} EvidenceClassType;

const char *evidenceEnumToStr(EvidenceClassType);

typedef enum
{
  POLTERGEIST,
  BANSHEE,
  BULLIES,
  PHANTOM
} GhostClassType;

/* evidence.c */

typedef struct EvidenceType
{
  EvidenceClassType type;
  float value;
  int refrences;
} EvidenceType;

void initEvidence(EvidenceClassType, float, EvidenceType **);
bool isGhostly(EvidenceType *);
void cleanupEvidence(EvidenceType *);

typedef struct EvidenceNode
{
  EvidenceType *evidence;
  struct EvidenceNode *next;
  struct EvidenceNode *prev;
} EvidenceNode;

void initEvidenceNode(EvidenceType *, EvidenceNode **);
void copyEvidence(EvidenceType *, EvidenceNode **);
void cleanupEvidenceNode(EvidenceNode *);

typedef struct EvidenceList
{
  struct EvidenceNode *head;
  struct EvidenceNode *tail;
} EvidenceList;

void initEvidenceList(EvidenceList **);
void addEvidence(EvidenceNode *, EvidenceList *);
int delEvidence(EvidenceNode *, EvidenceList *);
void cleanupEvidenceNodes(EvidenceList *);
void cleanupEvidenceList(EvidenceList *);

void printEvidence(EvidenceNode *);
void printEvidenceList(EvidenceList *);

/* ghost.c */

typedef struct GhostType
{
  GhostClassType type;
  struct RoomType *room;
  int boredom;
  struct BuildingType *building;
} GhostType;

void cleanupGhost(GhostType *);
/* hunter.c*/

typedef struct HunterType
{
  struct RoomType *room;
  EvidenceClassType equipment;
  struct EvidenceList *evidence;
  char name[MAX_STR];
  int fear;
  int boredom;
  struct BuildingType *building;
  int hasDifferentGhostly;
  // types collected
  int typesCollected[MAX_HUNTERS]; 
} HunterType;

typedef struct HunterNotebook
{
  HunterType *hunters[MAX_HUNTERS];
  int count;
} HunterNotebook;

void initNotebook(HunterNotebook **);
void cleanupNotebook(HunterNotebook *);
void addHunter(HunterType *, HunterNotebook *);
void removeHunter(HunterType *, HunterNotebook *);

/* room.c */

typedef struct RoomType
{
  char name[MAX_STR];
  struct RoomList *rooms;
  EvidenceList *evidence;
  HunterNotebook *hunters;
  struct GhostType *ghost;
  sem_t mutex;
} RoomType;

void initRoom(char *, RoomType **);
bool collectEvidence(HunterType *);
void cleanupRoom(RoomType *);

typedef struct RoomNode
{
  RoomType *room;
  struct RoomNode *next;
  struct RoomNode *prev;
} RoomNode;

void initRoomNode(RoomType *, RoomNode **);
void copyRoom(RoomType *, RoomNode **);
bool hasHunters(RoomType *);
bool hasGhost(RoomType *);
bool hasHunter(RoomType *);
void connectRooms(RoomNode *, RoomNode *);
void cleanupRoomNode(RoomNode *);
void printRoom(RoomNode *);

typedef struct RoomList
{
  struct RoomNode *head;
  struct RoomNode *tail;
} RoomList;

void initRoomList(RoomList **);
void appendRoom(RoomNode *, RoomList *);
RoomType *findRandRoom(RoomList *);
void copyRoom(RoomType *e, RoomNode **node);
void cleanupRoomNodes(RoomList *);
void cleanupRoomList(RoomList *);
void printRooms(RoomNode *);

/* building.c */

typedef struct BuildingType
{
  RoomList *rooms;
  HunterNotebook *noteBook;
  GhostType *ghost;
  EvidenceList *evidence;
} BuildingType;

// building protos
void createBuilding();
void initBuilding(BuildingType **);
void populateRooms(BuildingType *);
void cleanupBuilding(BuildingType *);

// function protos for ghost
void initGhost(BuildingType *, GhostType **);
void *updateGhost(void *);
void createEvidence(GhostType *);
void updateGhostRoom(GhostType *, RoomType *);

// randint and float
int randInt(int, int);
float randFloat(float, float);

// didnt know where to put these i just added, others i already added to the top UwU
void *updateHunter(void *);
void updateHunterRoom(HunterType *, RoomType *);
HunterType *pickRandomHunter(HunterNotebook *);

void shareGhostlyEvidence(HunterType *, HunterType *);
int ghostlyEvidenceCount(EvidenceList *);

void generateStandardEvidence(HunterType *);

void initHunter(char *, EvidenceClassType, BuildingType *, HunterType **);

void printHunter(HunterType *);
void printHunters(HunterNotebook *);

void printGhost(GhostType *);
void cleanupHunter(HunterType *);
void cleanupHunters(HunterNotebook *);


// new
 int findSizeOfAdjacentRooms(RoomList*);
void ghostlyIsDifferent (HunterType *, EvidenceClassType);