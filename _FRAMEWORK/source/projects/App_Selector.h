#pragma once
// Here we determine which application is currently active
// Create the define here using the "ActiveApp_..." convention and set up the correct include and typedef in the #ifdef below.

#define ActiveApp_AStar


//---------- Registered Applications -----------
#ifdef ActiveApp_AStar
#include "projects/Movement/Pathfinding/PathfindingAStar/App_PathfindingAStar.h"
typedef App_PathfindingAStar CurrentApp;
#endif

class App_Selector {
public: 
	static IApp* CreateApp() {
		IApp* myApp = new CurrentApp();
		return myApp;
	}
};