#include "defs.h"

/// @brief initalizes the EvidenceType with its class type and a value
/// @param type is the enumerated type defined by EvidenceClassType of the evidence
/// @param value is the recorded value of the evidence
/// @param e is the EvidenceType being initalized
void initEvidence(EvidenceClassType type, float value, EvidenceType **e)
{
  *e = calloc(1, sizeof(EvidenceType));
  (*e)->type = type;
  (*e)->value = value;
  (*e)->refrences = 0;
}

/// @brief frees the memory associated with an EvidenceType
/// @param e is the EvidenceType being free'd
void cleanupEvidence(EvidenceType *e)
{
  free(e);
}

/// @brief initializes a node containing some evidence and a refrence to other nodes. next and prev are defaulted to null
/// @param e is the EvidenceType data held by the node
/// @param node is the node being initalized
void initEvidenceNode(EvidenceType *e, EvidenceNode **node)
{
  *node = calloc(1, sizeof(EvidenceNode));
  (*node)->evidence = e;
  (*node)->next = NULL;
  (*node)->prev = NULL;
  (*node)->evidence->refrences++;
}

/// @brief copies the evidence data (assumed from another node) into a new node
/// @param e is the evidence data being coppied
/// @param node is the node being coppied to
void copyEvidence(EvidenceType *e, EvidenceNode **node)
{
  initEvidenceNode(e, node);
}

/// @brief frees the memory for a node (not the evidence)
/// @param node is the node being free'd
void cleanupEvidenceData(EvidenceNode *node)
{
  node->evidence->refrences--;
  cleanupEvidence(node->evidence);
  free(node);
}

/// @brief frees the memory for a node (not the evidence)
/// @param node is the node being free'd
void cleanupEvidenceNode(EvidenceNode *node)
{
  node->evidence->refrences--;
  free(node);
}
/// @brief initalizes an evidence DLL list. the head and tail are defaulted to null, size is set to 0
/// @param l is the list being initalized
void initEvidenceList(EvidenceList **l)
{
  *l = calloc(1, sizeof(EvidenceList));
  (*l)->head = NULL;
  (*l)->tail = NULL;
}

/// @brief frees the memory for a list and all of its nodes
/// @param l is the list being free'd
void cleanupEvidenceList(EvidenceList *l)
{
  EvidenceNode *temp;
  while (l->head != NULL)
  {
    temp = l->head;
    l->head = l->head->next;
    cleanupEvidenceData(temp);
  }
  free(l);
}

/// @brief frees the memory for a list and all of its nodes
/// @param l is the list being free'd
void cleanupEvidenceNodes(EvidenceList *l)
{
  EvidenceNode *temp;
  while (l->head != NULL)
  {
    temp = l->head;
    l->head = l->head->next;
    cleanupEvidenceNode(temp);
  }
  free(l);
}

/// @brief adds an EvidenceNode to the back of the EvidenceList
/// @param n is the node being added
/// @param l is the list being added to
void addEvidence(EvidenceNode *node, EvidenceList *l)
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

/// @brief deletes first node with matching type and value from the list
/// @param type is the type of data recorded
/// @param value is the magnitutde of the measurment
/// @param l is the list being deleted fro
/// @return 0 upon succesful delete, otherwise -1 if the value is not found
int delEvidence(EvidenceNode *node, EvidenceList *l)
{
  EvidenceNode *temp = l->head;
  while (temp != NULL)
  {
    if (temp == node)
    {
      break;
    }
    temp = temp->next;
  }
  if (temp == NULL)
  {
    return -1;
  }

  if (temp->next != NULL && temp->prev != NULL)
  {
    temp->next->prev = temp->prev;
    temp->prev->next = temp->next;
  }
  else if (temp->prev == NULL && temp->next != NULL)
  {
    temp->next->prev = NULL;
    l->head = temp->next;
  }
  else if (temp->next == NULL && temp->prev != NULL)
  {
    temp->prev->next = NULL;
    l->tail = temp->prev;
  }
  cleanupEvidenceNode(temp);
  return 0;
}

/// @brief checks whether the evidencetype is ghostly or not
/// @param evidence that is being checked
/// @return returns a bool, true if the evidence is ghostly, false otherwise

bool isGhostly(EvidenceType *evidence)
{
  int a = evidence->type;
  float val = evidence->value;
  switch (a)
  {
  case 0:
    return val >= 4.7 && val <= 5.0;
  case 1:
    return val >= -10.0 && val <= 1.0;
  case 2:
    return val == 1.0;
  case 3:
    return val >= 65.0 && val <= 75.0;
  default:
    return false;
  }
}

/// @brief shares ghostly evidence between two hunters
/// @param c hunter that is sharing evidence
/// @param r hunter that is getting evidence shared to them

void shareGhostlyEvidence(HunterType *c, HunterType *r)
{
  EvidenceNode *temp = c->evidence->head;
  EvidenceNode *node = NULL;
  while (temp != NULL)
  {
    if (isGhostly(temp->evidence))
    {
      copyEvidence(temp->evidence, &node);
      addEvidence(node, r->evidence);
      printf("HUNTER: %s HAS SHARED %s GHOSTLY EVIDENCE WITH %s\n", c->name, evidenceEnumToStr(node->evidence->type), r->name);
      ghostlyIsDifferent(r, node->evidence->type);
      break;
    }
    temp = temp->next;
  }
}
void ghostlyIsDifferent(HunterType *hunter, EvidenceClassType evi)
{
  bool doesnthave = true; 
  for (int i = 0; i < MAX_HUNTERS; i++) {
   if (hunter->typesCollected[i] == evi) {
    doesnthave = false; 
    break; 
   }
  }
  if (doesnthave) {
    hunter->typesCollected[hunter->hasDifferentGhostly -1] = (int)evi;
    hunter->hasDifferentGhostly++;
  }
}


/// @brief prints the data (type and value) of the evidence node
/// @param e is the evidence node being printed
void printEvidence(EvidenceNode *e)
{
  if (e == NULL)
  {
    printf("NULL EVIDENCE\n");
    return;
  }
  printf("type: %-12s value: %-5.2f refrences: %d\n",
         evidenceEnumToStr(e->evidence->type),
         e->evidence->value,
         e->evidence->refrences);
}

/// @brief prints out all the data in an evidence list
/// @param l is the list being printed
void printEvidenceList(EvidenceList *l)
{
  printf("Printing all evidence: \n");
  EvidenceNode *temp = l->head;
  while (temp != NULL)
  {
    printEvidence(temp);
    temp = temp->next;
  }
  printf("\n");
}

/// @brief matches the evidence class type enum to the string it represenets
/// @param t is the enumerated evidence class
/// @return the appropriate string it maps to
const char *evidenceEnumToStr(EvidenceClassType t)
{
  switch (t)
  {
  case 0:
    return "EMF";

  case 1:
    return "TEMPERATURE";

  case 2:
    return "FINGERPRINTS";

  case 3:
    return "SOUND";

  default:
    return "THIS BAD";
  }
}