#include "defs.h"

/*
    Note: While this function is provided, you will likely want
    to modify it to suit your solution. This has been provided
    to give you a starting point to lay out the room connectivity,
    but it is entirely possible that you will want to use different
    data structures to represent the rooms and their connectivity,
    or different function names.

    Not following this function's structure exactly will not result
    in a deduction of points.

    The map that this is recreating is visible in the assignment
    specification, and also available by the original creator here:
    https://steamcommunity.com/sharedfiles/filedetails/?id=2251267947

*/


/// @brief initialize a building and its member attributes by calling existing functions 
/// @param b double pointer to which the new initalized building will be stored 

void initBuilding(BuildingType **b)
{
    (*b) = calloc(1, sizeof(BuildingType));

    RoomList *r = NULL;
    initRoomList(&r);

    HunterNotebook *h = NULL;
    initNotebook(&h);

    EvidenceList *e = NULL;
    initEvidenceList(&e);

    (*b)->rooms = r;
    (*b)->noteBook = h;
    (*b)->ghost = NULL;
    (*b)->evidence = e;
}

/// @brief clean up building and its initialized member attributes by using existing functions 
/// @param b pointer tobuilding to be cleaned 

void cleanupBuilding(BuildingType *b){
    cleanupGhost(b->ghost);
    cleanupRoomList(b->rooms);
    cleanupHunters(b->noteBook);
    cleanupEvidenceList(b->evidence);
    free(b);

}

/// @brief populate building with rooms, and connect these rooms and print the connections 
/// @param b building to be populated  

void populateRooms(BuildingType *building)
{
    RoomType *room = NULL;
    RoomNode *node = NULL;

    initRoom("Van", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *van = node;
    printRoom(node);

    initRoom("Hallway", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *hallway = node;
    printRoom(node);

    initRoom("Master Bedroom", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *masterBedroom = node;
    printRoom(node);

    initRoom("Boy's Room", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *boysRoom = node;
    printRoom(node);

    initRoom("Bathroom", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *bathroom = node;
    printRoom(node);

    initRoom("Basement", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *basement = node;
    printRoom(node);

    initRoom("Basement Hallway", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *basementHallway = node;
    printRoom(node);

    initRoom("Right Storage Room", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *rightStorageRoom = node;
    printRoom(node);

    initRoom("Left Storage Room", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *leftStorageRoom = node;
    printRoom(node);

    initRoom("Kitchen", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *kitchen = node;
    printRoom(node);

    initRoom("Living Room", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *livingRoom = node;
    printRoom(node);

    initRoom("Garage", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *garage = node;
    printRoom(node);

    initRoom("Utility Room", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *utilityRoom = node;
    printRoom(node);

    initRoom("Front Yard", &room);
    initRoomNode(room, &node);
    appendRoom(node, building->rooms);
    RoomNode *frontYard = node;
    printRoom(node);

    // Now connect the rooms. It is possible you do not need a separate
    // function for this, but it is provided to give you a starting point.
    connectRooms(hallway, van);
    connectRooms(hallway, masterBedroom);
    connectRooms(hallway, boysRoom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basementHallway, basement);
    connectRooms(basementHallway, rightStorageRoom);
    connectRooms(basementHallway, leftStorageRoom);
    connectRooms(kitchen, livingRoom);
    connectRooms(kitchen, garage);
    connectRooms(garage, utilityRoom);
    connectRooms(frontYard, van);

    printRooms(van);
    printRooms(hallway);
    printRooms(masterBedroom);
    printRooms(boysRoom);
    printRooms(bathroom);
    printRooms(basement);
    printRooms(basementHallway);
    printRooms(rightStorageRoom);
    printRooms(leftStorageRoom);
    printRooms(kitchen);
    printRooms(livingRoom);
    printRooms(garage);
    printRooms(utilityRoom);
    printRoom(frontYard);
}