#include "game.h"
#include "surface.h"
#include "template.h"

#include <cstdio> 
#include <windows.h>

#define WIN32_LEAN_AND_MEAN
#define ANTSIZE 20
// if you're going to change this keep in mind that you need to move to edit 
// the sprite by changing it to "assets/ant2scaleddown(same num as ANTSIZE).jpg"

namespace Tmpl8
{
	//Scene Manager
	int lifePoints = 20;
	int screenSelected = 1;
	int cursorSize = 7;
	int timeDeltaTime; //simple timer


	//Input Variables
	bool confirmation;
	bool destinationPlaced;
	bool boxDrawing;

	//Box Variables
	int boxStartX, boxStartY, boxEndX, boxEndY;
	int numSelected;
	int boxStyle;
	bool doThisOnce = true;
	int debugInt;
	
	//Ant Variables
	int numOfActiveAnts;
	int numOfStartingAnts = 15;
	int antSpeed = 1;
	int distanceToObjToStop = 10;
	
	//Sprites
	//credit to CREATOR ID: 9664c4 for the ant sprite (http://pixelartmaker.com/art/f18f54d668c00f3)
	Sprite ant(new Surface("assets/ant2scaleddown20.jpg"), 8);
	Sprite enemyAnt(new Surface("assets/enemyantsprite20.jpg"), 8);

	class Building
	{
	public:

		Building()
		{
			x = 310;
			y = 310;
			x_width = 50;
			y_height = 50;
		}

		void BuildingCode(Surface* gameScreen)
		{
			DrawBuilding(gameScreen);

		}

		//drawing the building
		void DrawBuilding(Surface* gameScreen) {
			for (int i = 1; i <= 50; i += 10) {
				gameScreen->Box(x + i, y + i, x + x_width - i, y + y_height - i, 0xB00B69);
			}

			if (buildingSelected)
				gameScreen->Box(x - 1, y - 1, x + x_width + 1, y + y_height + 1, 0x29B1CA);
		}

		bool CheckIfMouseOnBuilding(int mousex, int mousey) {
			if (x < mousex && mousex < (x + x_width) && y < mousey && mousey < (y + y_height)) {
				return true;
			}
			else return false;
		}

		int x, y; // x and y are the top right coordinates of each ants
		int x_width, y_height;
		bool buildingSelected;
		bool antSpawnMarkerPlaced;
		int xAntSpawnMarker, yAntSpawnMarker;
	};

	class Ant
	{
	public:

		Ant() //init values
		{
			x = IRand(710);
			y = IRand(710);
			rotation = 0;
			antDestinationX = 0;
			antDestinationY = 0;
			antSelected = false;
			countedThisAntAlready = false;
			antHeadingToDestination = false;
			bestDecision = 0;
			distanceToMarker = 0;
		}

		void AntCode(Surface* gameScreen)
		{
			if (isSpawnedAnt){
				//drawing the ant
				ant.SetFrame(rotation);
				ant.Draw(gameScreen, x, y);

				//formula for calculating distance between two points
				distanceToMarker = sqrt(pow(x - antDestinationX, 2) + pow(y - antDestinationY, 2));

				//draws blue outline for selected ants

				if (antSelected)
					gameScreen->Box(x, y, x + ANTSIZE, y + ANTSIZE, 0x29B1CA);


				if (distanceToMarker <= distanceToObjToStop)
					antHeadingToDestination = false;
				else if (antSelected && destinationPlaced && confirmation)
					antHeadingToDestination = true;

				BoxConfiguration();

				if (antHeadingToDestination == true) {
					//difference between the two pathfinding methods is almost none but the basic one is faster so I'll stick to that

					BasicPathfinding(antDestinationX, antDestinationY);

					//BestPossiblePathCheck();
					//AdvancedPathfinding();

					//the best pathfinding method would be to use the same algorithm that calculates the lines 
					//and then have the ant take the same path as the line
					//I am not sure how to do that though
				}

				KeepAntsInMap();
			}
		}

		//collider check between two ants (returns true whether they collide)
		bool checkCollideBetweenAnts(int xTwo, int yTwo, int TwoWidth, int TwoHeight)
		{
			int oWidth = ANTSIZE;
			int oHeight = ANTSIZE;
			int oTwoWidth = TwoWidth;
			int oTwoHeight = TwoHeight;

			// AABB 1
			int x1Min = x;
			int x1Max = x + oWidth;
			int y1Max = y + oHeight;
			int y1Min = y;

			// AABB 2
			int x2Min = xTwo;
			int x2Max = xTwo + oTwoWidth;
			int y2Max = yTwo + oTwoHeight;
			int y2Min = yTwo;

			// Collision tests
			if (x1Max < x2Min || x1Min > x2Max) return false;
			if (y1Max < y2Min || y1Min > y2Max) return false;

			return true;
		}

		/*	void BestPossiblePathCheck() {
			float testCase[8];
			float minValue = 1000;
			int shotestToDest;

			 testCase[0] = sqrt(pow((x + (distanceToObjToStop - 2)) - antDestinationX, 2) + pow((y + (distanceToObjToStop - 2)) - antDestinationY, 2));  //bottom right
			 testCase[1] = sqrt(pow((x - (distanceToObjToStop - 2)) - antDestinationX, 2) + pow((y - (distanceToObjToStop - 2)) - antDestinationY, 2));	 //up left
			 testCase[2] = sqrt(pow((x - (distanceToObjToStop - 2)) - antDestinationX, 2) + pow((y + (distanceToObjToStop - 2)) - antDestinationY, 2));  //bottom left
			 testCase[3] = sqrt(pow((x + (distanceToObjToStop - 2)) - antDestinationX, 2) + pow((y - (distanceToObjToStop - 2)) - antDestinationY, 2));	 //up right
			 testCase[4] = sqrt(pow(x - antDestinationX, 2) + pow((y - (distanceToObjToStop - 2)) - antDestinationY, 2));  //up
			 testCase[5] = sqrt(pow(x - antDestinationX, 2) + pow((y + (distanceToObjToStop - 2)) - antDestinationY, 2));  //down
			 testCase[6] = sqrt(pow((x - (distanceToObjToStop - 2)) - antDestinationX, 2) + pow(y - antDestinationY, 2));  //left
			 testCase[7] = sqrt(pow((x + (distanceToObjToStop - 2)) - antDestinationX, 2) + pow(y - antDestinationY, 2));  //right

			 for (int i = 0; i < 7; i++) {

				 if (testCase[i] < minValue) {
					 minValue = testCase[i];
					 shotestToDest = i;
				 }
			 }

			 bestDecision = shotestToDest;
		}*/

		void BasicPathfinding(int destX, int destY) {

			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) > y) {
				//destination in bottom right
				rotation = 7;
				debugInt = 4;
				x += antSpeed;
				y += antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) > y) {
				//destinaiton in bottom left
				rotation = 6;
				debugInt = 3;
				x -= antSpeed;
				y += antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) < y) {
				//destination in top left
				rotation = 5;
				debugInt = 2;
				x -= antSpeed;
				y -= antSpeed;

			}
			
			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) < y) {
				//destination in top right
				rotation = 4;
				debugInt = 1;
				x += antSpeed;
				y -= antSpeed;

			}
			
			if (destX - (ANTSIZE / 2) == x && destY - (ANTSIZE / 2) > y) {
				//destinaiton below
				rotation = 1;
				debugInt = 5;
				y += antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) == x && destY - (ANTSIZE / 2) < y) {
				//destination above
				rotation = 0;
				debugInt = 6;
				y -= antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) == y) {
				//destination to the right
				rotation = 3;
				debugInt = 7;
				x += antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) == y) {
				//destination to the left
				rotation = 2;
				debugInt = 8;
				x -= antSpeed;
			}
		}

		void AdvancedPathfinding() {

			if (bestDecision == 0) {
				//destination in bottom right
				rotation = 7;
				debugInt = 4;
				x += antSpeed;
				y += antSpeed;
			}
			else if (bestDecision == 2) {
				//destinaiton in bottom left
				rotation = 6;
				debugInt = 3;
				x -= antSpeed;
				y += antSpeed;
			}
			else if (bestDecision == 1) {
				//destination in top left
				rotation = 5;
				debugInt = 2;
				x -= antSpeed;
				y -= antSpeed;

			}
			else if (bestDecision == 3) {
				//destination in top right
				rotation = 4;
				debugInt = 1;
				x += antSpeed;
				y -= antSpeed;

			}
			else if (bestDecision == 5) {
				//destinaiton below
				rotation = 1;
				debugInt = 5;
				y += antSpeed;
			}
			else if (bestDecision == 4) {
				//destination above
				rotation = 0;
				debugInt = 6;
				y -= antSpeed;
			}
			else if (bestDecision == 7) {
				//destination to the right
				rotation = 3;
				debugInt = 7;
				x += antSpeed;
			}
			else if (bestDecision == 6) {
				//destination to the left
				rotation = 2;
				debugInt = 8;
				x -= antSpeed;
			}
		}

		void AntAvoidance(int destX, int destY) {

			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) > y) {
				//destination in bottom right
				rotation = 7;
				debugInt = 4;
				x -= antSpeed;
				y -= antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) > y) {
				//destinaiton in bottom left
				rotation = 6;
				debugInt = 3;
				x += antSpeed;
				y -= antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) < y) {
				//destination in top left
				rotation = 5;
				debugInt = 2;
				x += antSpeed;
				y += antSpeed;

			}
			
			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) < y) {
				//destination in top right
				rotation = 4;
				debugInt = 1;
				x -= antSpeed;
				y += antSpeed;

			}
			
			if (destX - (ANTSIZE / 2) == x && destY - (ANTSIZE / 2) > y) {
				//destinaiton below
				rotation = 1;
				debugInt = 5;
				y -= antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) == x && destY - (ANTSIZE / 2) < y) {
				//destination above
				rotation = 0;
				debugInt = 6;
				y += antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) > x && destY - (ANTSIZE / 2) == y) {
				//destination to the right
				rotation = 3;
				debugInt = 7;
				x -= antSpeed;
			}
			
			if (destX - (ANTSIZE / 2) < x && destY - (ANTSIZE / 2) == y) {
				//destination to the left
				rotation = 2;
				debugInt = 8;
				x += antSpeed;
			}
		}

		void BoxConfiguration() {
			//Simple Visualisation saved in Notepad in Assets
			//box dragged to top left
			if (boxDrawing) {
				if (boxStartX < boxEndX && boxStartY > boxEndY) {
					boxStyle = 2;
					if (x > boxStartX && y < boxStartY && x < boxEndX && y > boxEndY) {
						antSelected = true;
					}
				}

				//box dragged to bottom right
				if (boxStartX > boxEndX && boxStartY < boxEndY) {
					boxStyle = 4;
					if (x < boxStartX && y > boxStartY && x > boxEndX && y < boxEndY)
						antSelected = true;
				}

				//box dragged to bottom left
				if (boxStartX > boxEndX && boxStartY > boxEndY) {
					boxStyle = 3;
					if (x < boxStartX && y < boxStartY && x > boxEndX && y > boxEndY)
						antSelected = true;
				}

				//box dragged to top left
				if (boxStartX < boxEndX && boxStartY < boxEndY) {
					boxStyle = 1;
					if (x > boxStartX && y > boxStartY && x < boxEndX && y < boxEndY)
						antSelected = true;
				}
			}
		}

		void KeepAntsInMap() {
			if (x <= 0) {
				x = 0;
			}

			if (x >= 720 - ANTSIZE) {
				x = 720 - ANTSIZE;
			}

			if (y <= 0) {
				y = 0;
			}

			if (y >= 720 - ANTSIZE) {
				y = 720 - ANTSIZE;
			}
		}

		void InitAnt() {
			x = 310;
			y = 310;
			isSpawnedAnt = true;
		}

		//found out I could use vec2's here but updating it would be too time consuming 
		int x, y, rotation; // x and y are the top right coordinates of each ants
		//int xCollisionUpRight, yCollisionUpRight;
		int antDestinationX, antDestinationY;
		bool antSelected;
		bool countedThisAntAlready;
		bool antHeadingToDestination;
		bool isSpawnedAnt = false;
		float distanceToMarker;
		int bestDecision;
	};

	class EnemyAnt : public Ant
	{
	public:
		EnemyAnt() {
			// those need to be the same as the building (around it's midpoint)
			antDestinationX = 320;
			antDestinationY = 320;

			whereToSpawn = rand() % 4 + 1;

			if (whereToSpawn == 1) {
				//up
				x = rand() % 710;
				y = -20;
			}
			else if (whereToSpawn == 2) {
				x = rand() % 710;
				y = 740;
			}
			else if (whereToSpawn == 3) {
				x = -20;
				y = rand() % 710;
			}
			else if (whereToSpawn == 4) {
				x = 710;
				y = rand() % 710;
			}
		}

		void EnemyAntCode(Surface* gameScreen)
		{
			//drawing the ant
			enemyAnt.SetFrame(rotation);
			enemyAnt.Draw(gameScreen, x, y);

			BasicPathfinding(antDestinationX, antDestinationY);
		}

		void RandomizeNewSpawn() {
			whereToSpawn = IRand(4) + 1;
			if (whereToSpawn == 1) {
				//up
				x = rand() % 710;
				y = -20;
			}
			
			if (whereToSpawn == 2) {
				//down
				x = rand() % 710;
				y = 740;
			}
			
			if (whereToSpawn == 3) {
				//left
				x = -20;
				y = rand() % 710;
			}
			
			if (whereToSpawn == 4) {
				//right
				x = 700;
				y = rand() % 710;
			}
		}
		int whereToSpawn;
	};

	//declaring values
	Ant myant[100];
	EnemyAnt myenemyant[10];
	Building mybuilding;

	void Game::Init()
	{
		for (int i = 0; i <= numOfStartingAnts; i++) {
			myant[i].isSpawnedAnt = true;
		}
	}
	
	void Game::Shutdown()
	{
		//do nothing
	}

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);

		//Cursor
		screen->Line(mousex - cursorSize, mousey - cursorSize, mousex + cursorSize, mousey + cursorSize, 0xff0000);
		screen->Line(mousex - cursorSize, mousey + cursorSize, mousex + cursorSize, mousey - cursorSize, 0xff0000);

		if (screenSelected == 1) {

			timeDeltaTime = timeDeltaTime + (int)deltaTime;

			//a reference map
			for (int y = 0; y < 72; y++)		// the y of the map is drawn
				for (int x = 0; x < 72; x++)	// the x of the map is drawn
					//box is 10x10
					screen->Box(x * 10, y * 10, x * 10 + 10, y * 10 + 10, 0x3C2B0A);

			screen->Print("Create more ants by pressing CTRL", 20, 20, 0xffffff);

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				boxDrawing = true;
				if (doThisOnce == true) {
					//start coords of the box
					boxStartX = mousex;
					boxStartY = mousey;

					for (int n = 0; n < 100; n++) {
						myant[n].antSelected = false; //ant deselector
						myant[n].countedThisAntAlready = false;
					}

					numSelected = 0;
					mybuilding.buildingSelected = false; //building deselector

					if (mybuilding.CheckIfMouseOnBuilding(mousex, mousey)) {
						mybuilding.buildingSelected = true;
					}

					//this needs to be last
					doThisOnce = false;
				}
				//end coords of the box
				boxEndX = mousex;
				boxEndY = mousey;

				//drawing the box
				// 0x29B1CA -> nice cyan color for selected 
				screen->Box(boxStartX, boxStartY, boxEndX, boxEndY, 0x29B1CA);


			}
			else { doThisOnce = true; boxDrawing = false; }

			if (GetAsyncKeyState(VK_RBUTTON)) {

				if (numSelected > 0) {
					for (int n = 0; n < 100; n++) {
						if (myant[n].antSelected) {
							myant[n].antDestinationX = mousex;
							myant[n].antDestinationY = mousey;
						}
					}
				}

				destinationPlaced = true;
				confirmation = true;

				if (mybuilding.buildingSelected) {
					mybuilding.xAntSpawnMarker = mousex;
					mybuilding.yAntSpawnMarker = mousey;
					mybuilding.antSpawnMarkerPlaced = true;
				}

			}
			else confirmation = false;

			mybuilding.BuildingCode(screen);

			//fix this so it only appeared when there is more that 1 ant selected at the moment and make the ants travel towards it
			if (destinationPlaced && numSelected > 0) {
				for (int n = 0; n < 100; n++) {
					if (myant[n].antSelected) {

						//	screen->Line(myant[n].x + (ANTSIZE / 2), myant[n].y + (ANTSIZE / 2), myant[n].antDestinationX, myant[n].antDestinationY, 0xB00B69);
						screen->Line(myant[n].antDestinationX - cursorSize, myant[n].antDestinationY - cursorSize, myant[n].antDestinationX + cursorSize, myant[n].antDestinationY + cursorSize, 0x29B1CA);
						screen->Line(myant[n].antDestinationX - cursorSize, myant[n].antDestinationY + cursorSize, myant[n].antDestinationX + cursorSize, myant[n].antDestinationY - cursorSize, 0x29B1CA);
					}
				}
			}

			//marker for building destination (I don't have the energy to think up a solution to that at the moment)
		/*	if (mybuilding.antSpawnMarkerPlaced && mybuilding.buildingSelected) {
				screen->Line(mybuilding.xAntSpawnMarker - cursorSize, mybuilding.yAntSpawnMarker - cursorSize, mybuilding.xAntSpawnMarker + cursorSize, mybuilding.yAntSpawnMarker + cursorSize, 0xB00B69);
				screen->Line(mybuilding.xAntSpawnMarker - cursorSize, mybuilding.yAntSpawnMarker + cursorSize, mybuilding.xAntSpawnMarker + cursorSize, mybuilding.yAntSpawnMarker - cursorSize, 0xB00B69);
			}*/

			//goes through the class array and executes something for each ant
			for (int i = 0; i < 100; i++) {

				myant[i].AntCode(screen);

				//compares each ant of the array to the others
				for (int z = 100 - 1; z > i; z--) {

					if (myant[i].checkCollideBetweenAnts(myant[z].x, myant[z].y, ANTSIZE, ANTSIZE))
						if (myant[z].isSpawnedAnt) {
							myant[i].AntAvoidance(myant[z].x, myant[z].y);
						}
					//example:
					//myant[0] -> should check for all ants from -> 1 to 9 (numOfActiveAnts - 1 ) ;
					//myant[3] -> should check for all ants from -> 4 to 9 (numOfActiveAnts - 1 ) ;

				}

				//counts the number of ants selected
				if (myant[i].antSelected == true) {
					if (!myant[i].countedThisAntAlready) {
						numSelected += 1;
						myant[i].countedThisAntAlready = true;
					}
				}

				if (myant[i].checkCollideBetweenAnts(mybuilding.x, mybuilding.y, mybuilding.x_width, mybuilding.y_height))
					myant[i].AntAvoidance(mybuilding.x, mybuilding.y);


				//AMOUNT OF WORKING ANTS COUNTER
				if (myant[i].isSpawnedAnt) {
					numOfActiveAnts++;
				}

				if (i == 99) {
					printf("%i \n", numOfActiveAnts);
					numOfActiveAnts = 0;
				}
			}


			for (int i = 0; i < 10; i++) {
				myenemyant[i].EnemyAntCode(screen);
			}

			for (int i = 0; i < 10; i++) {

				for (int z = 100 - 1; z > i; z--) {

					if (myenemyant[i].checkCollideBetweenAnts(myenemyant[z].x, myenemyant[z].y, ANTSIZE, ANTSIZE))
						myenemyant[i].AntAvoidance(myenemyant[z].x, myenemyant[z].y);
				}

				for (int n = 0; n < 100; n++) {
					if (myenemyant[i].checkCollideBetweenAnts(myant[n].x, myant[n].y, ANTSIZE, ANTSIZE))
					{
						if (myant[n].isSpawnedAnt) {
							myenemyant[i].RandomizeNewSpawn();
							myant[n].isSpawnedAnt = false;
						}
					}
				}

				if (myenemyant[i].checkCollideBetweenAnts(mybuilding.x, mybuilding.y, mybuilding.x_width, mybuilding.y_height))
				{
					myenemyant[i].RandomizeNewSpawn();
					lifePoints-= 1;
				}

				printf("Enemy ant %i: x %i  y %i \n", i, myenemyant[i].x, myenemyant[i].y);
			}

			if (GetAsyncKeyState(VK_LCONTROL)) {
				if (numOfActiveAnts < 1000)
					for (int i = 0; i < 100; i++)
					{
						if (!myant[i].isSpawnedAnt) {
							myant[i].InitAnt();
							i = 100;
						}
					}
			}

			if (lifePoints <= 0) {
				screenSelected = 2;
			}

			Sleep(3);
		}

		if (screenSelected != 1) {
			//screen->Clear(0x941509);
		}

		// DEBUG COMMANDS:
		
		//printf(" %i", mybuilding.CheckIfMouseOnBuilding(mousex, mousey));
		//printf("%i" , myant[1].antSelected);
		//printf("direction: %i \n", debugInt);
		//printf("antDir: %i %i \n", antDestinationX, antDestinationY);
		//printf("antCoords: %i %i \n", myant[1].x, myant[1].y);
		//printf("ant 1 distance: %f \n", myant[1].distanceToMarker);
		//printf("mouse coordinates %i %i \n", mousex, mousey);
		//printf("num of ants selected: %i \n", numSelected);
		//printf("Box Style %i \n", boxStyle);
		//printf("start X: %i  start Y: %i \n", boxStartX, boxStartY);
		//printf("end X: %i  end Y: %i \n", boxEndX, boxEndY);
	}

};