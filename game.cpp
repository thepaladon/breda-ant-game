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
	//Gameplay Variables
	int lifePoints = 25;
	int displayScore;
	int screenSelected = 0;
	int resourceAmount = 1000;
	int cursorSize = 7;

	//Time Variables
	float timeDeltaTime; //simple timer
	float setTimer;
	float waveTimer = 5000; //5 seconds until first change
	bool isReset;

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
	int enemyAntSpeed = 10;
	
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
				gameScreen->Box(x + i, y + i, x + x_width - i, y + y_height - i, 0x7C0000);
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
			x = rand() % 325 + 200;
			y = rand() % 325 + 200;
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
				distanceToMarker = float(sqrt(pow(x - antDestinationX, 2) + pow(y - antDestinationY, 2)));

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

		bool checkCollision(const Ant & other) const
		{
			return checkCollision(other.x, other.y, ANTSIZE, ANTSIZE);
		}

		//collider check between the and and another border rectangle (returns true whether they collide)
		bool checkCollision(int xTwo, int yTwo, int TwoWidth, int TwoHeight) const
		{
			int oWidth = ANTSIZE;
			int oHeight = ANTSIZE;

			// AABB 1
			int x1Min = x;
			int x1Max = x + oWidth;
			int y1Max = y + oHeight;
			int y1Min = y;

			// AABB 2
			int x2Min = xTwo;
			int x2Max = xTwo + TwoWidth;
			int y2Max = yTwo + TwoHeight;
			int y2Min = yTwo;

			// Collision tests
			if (x1Max < x2Min || x1Min > x2Max) return false;
			if (y1Max < y2Min || y1Min > y2Max) return false;

			return true;
		}

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

		void Avoid(int destX, int destY) {

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

		void Avoid(Ant& other) {
			 Avoid(other.x, other.y);
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
			x = rand() %150 + 250;
			y = rand() %150 + 250;
			isSpawnedAnt = true;
		}

		void ReInitAnt(void) {
			x = rand() % 325 + 200;
			y = rand() % 325 + 200;
			rotation = 0;
			antDestinationX = 0;
			antDestinationY = 0;
			antSelected = false;
			countedThisAntAlready = false;
			antHeadingToDestination = false;
			bestDecision = 0;
			distanceToMarker = 0;
		}

		void ReInitStarterAnt()
		{
			isSpawnedAnt = true;
		}

		void DespawnAnt() {
			isSpawnedAnt = false;
		}

		void TakeAntToBuildingSpawnPoint(Building& bld) {
			antDestinationX = bld.xAntSpawnMarker;
			antDestinationY = bld.yAntSpawnMarker;
			antHeadingToDestination = true;
		}

		bool isSpawned() const { return isSpawnedAnt; }


		//found out I could use vec2's here but updating it would be too time consuming 
		int x, y, rotation; // x and y are the top right coordinates of each ants
		//int xCollisionUpRight, yCollisionUpRight;
		int antDestinationX, antDestinationY;
		bool antSelected;
		bool countedThisAntAlready;
		
	private:	//encapsulation
		int bestDecision;
		float distanceToMarker;
		bool isSpawnedAnt = false;
		bool antHeadingToDestination;

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
			int delay = rand() % 100;

			if (delay <= enemyAntSpeed) {
				BasicPathfinding(antDestinationX, antDestinationY);
			}
			
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

	class Resource
	{
	public:
		Resource() {
			x = rand() % 710;
			y = rand() % 710;

			x_width = rand() % 5 + 5;
			y_height = rand() % 5 + 5;

			pelletValue = (x_width + y_height) * 100;
		}

		void DrawPellet(Surface* gameScreen) {
			gameScreen->Box(x, y, x + x_width, y + y_height, 0xffffff);
		}

		void ReInit() {
			x = rand() % 710;
			y = rand() % 710;

			x_width = rand() % 5 + 5;
			y_height = rand() % 5 + 5;
		}

		int x, y;
		int x_width, y_height;
		int pelletValue;
	};

	Ant myant[100];
	EnemyAnt myenemyant[15];
	Building mybuilding;
	Resource myresource[5];

	//Credit to @Themperror#1986 for providing me with this code and helping me understand it
	void PrintToScreenFormatted(Surface* screen, int x1, int y1, Pixel color, char* format, ...)
	{
		va_list list;
		va_start(list, format);
		int count = vsnprintf(nullptr, 0, format, list);
		thread_local char* buf = nullptr;
		thread_local int bufLen = 0;
		if (buf == nullptr || bufLen < count)
		{
			if (buf != nullptr) free(buf);
			bufLen = count * 3;
			buf = static_cast<char*>(malloc(bufLen));
		}
		memset(buf, 0, bufLen);
		vsnprintf(buf, bufLen, format, list);
		va_end(list);
		screen->Print(buf, x1, y1, color);
	}

	//Reinitialises the game after a "game over"
	void ReInitGame() {
		lifePoints = 10;
		displayScore = 0;
		resourceAmount = 1000;
		enemyAntSpeed = 10;

		for (int i = 0; i <= numOfStartingAnts; i++) {
			myant[i].ReInitStarterAnt();
		}

		for (int i = 0; i < 100; i++) {
			myant[i].ReInitAnt();
		}

		for (int i = 0; i < 15; i++) {
			myenemyant[i].antDestinationX = 320;
			myenemyant[i].antDestinationY = 320;

			myenemyant[i].whereToSpawn = rand() % 4 + 1;

			if (myenemyant[i].whereToSpawn == 1) {
				//up
				myenemyant[i].x = rand() % 710;
				myenemyant[i].y = -20;
			}
			else if (myenemyant[i].whereToSpawn == 2) {
				myenemyant[i].x = rand() % 710;
				myenemyant[i].y = 720;
			}
			else if (myenemyant[i].whereToSpawn == 3) {
				myenemyant[i].x = -20;
				myenemyant[i].y = rand() % 710;
			}
			else if (myenemyant[i].whereToSpawn == 4) {
				myenemyant[i].x = 720;
				myenemyant[i].y = rand() % 710;
			}
		}

	
	}

	void Game::Init()
	{
		for (int i = 0; i <= numOfStartingAnts; i++) {
			myant[i].ReInitStarterAnt();
		}
	}
	
	void Game::Shutdown()
	{
		//do nothing
	}

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);

		//Drawing the cursor
		screen->Line(float(mousex - cursorSize), float(mousey - cursorSize), float(mousex + cursorSize), float(mousey + cursorSize), 0x7C0000);
		screen->Line(float(mousex - cursorSize), float(mousey + cursorSize), float(mousex + cursorSize), float(mousey - cursorSize), 0x7C0000);

		//START GAME SCENE
		if (screenSelected == 0) {
			screen->Print("GAME START", 100, 100, 0xffffff);
			screen->Print("CLICK TO PLAY!", 100, 110, 0xffffff);
			
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				screenSelected = 1;
			}
		}

		//MAIN GAMEPLAY SCENE
		if (screenSelected == 1) {

			//basic timer
			timeDeltaTime = timeDeltaTime + deltaTime;

			//Outlines of the gameplay box (where ants roam) 
			screen->Box(0, 0, 720, 720, 0x3C2B0A);

			//INFORMATION FOR PLAYER:
			screen->Print("LIFE OF BASE", 730, 20, 0xffffff);
			for (int i = 1; i <= lifePoints; i++) { //draws life container box
				screen->Line(float(725 + i * 5), 30, float(725 + i * 5), 40, 0xff0000);
			}

			PrintToScreenFormatted(screen, 730, 50, 0xE4D437, "SCORE: %i", displayScore);
			PrintToScreenFormatted(screen, 730, 60, 0xE4D437, "RESOURCE AMOUNT: %i", resourceAmount);

			//OBJECTIVE:
			screen->Print("OBJECTIVE OF THE GAME:", 730, 80, 0x29B1CA);
			screen->Print("Protect your ant's base by using your ants to defeat the        enemy ants", 730, 90, 0xffffff);
			screen->Print("purple", 1072, 90, 0x9932CC);
			screen->Print("", 730, 100, 0xffffff);
			PrintToScreenFormatted(screen , 730, 100, 0xB93EF5, "CURRENT ENEMY ANT SPEED: %i", enemyAntSpeed);

			//TUTORIAL TEXT:
			screen->Print("Create more ants by pressing LEFT CTRL", 730, 120, 0xffffff);
			screen->Print("EACH NEW ANT COSTS 250 RESOURCE POINTS", 730, 130, 0xffffff);
			screen->Print("COLLECT WHITE PELLETS TO GATHER RESOURCES TO CREATE MORE ANTS", 730, 140, 0xffffff);

			screen->Print("By clicking on the building and right clicking you can", 730, 160, 0xffffff);
			screen->Print("select a location to which newly spawned ants will travel", 730, 170, 0xffffff);

			//do on left click
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
				screen->Box(boxStartX, boxStartY, boxEndX, boxEndY, 0x29B1CA);
			}else 
			{ doThisOnce = true; boxDrawing = false; }

			//do on right click
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

			//fix this so it only appeared when there is more that 1 ant selected at the moment and make the ants travel towards it
			if (destinationPlaced && numSelected > 0) {
				for (int n = 0; n < 100; n++) {
					if (myant[n].antSelected) {

						//	screen->Line(myant[n].x + (ANTSIZE / 2), myant[n].y + (ANTSIZE / 2), myant[n].antDestinationX, myant[n].antDestinationY, 0xB00B69);
						screen->Line(float(myant[n].antDestinationX - cursorSize), float(myant[n].antDestinationY - cursorSize), float(myant[n].antDestinationX + cursorSize), float(myant[n].antDestinationY + cursorSize), 0x29B1CA);
						screen->Line(float(myant[n].antDestinationX - cursorSize), float(myant[n].antDestinationY + cursorSize), float(myant[n].antDestinationX + cursorSize), float(myant[n].antDestinationY - cursorSize), 0x29B1CA);
					}
				}
			}

			//marker for building destination (I don't have the energy to think up a solution to that at the moment)
			if (mybuilding.antSpawnMarkerPlaced && mybuilding.buildingSelected) {
				screen->Line(float(mybuilding.xAntSpawnMarker - cursorSize), float(mybuilding.yAntSpawnMarker - cursorSize), float(mybuilding.xAntSpawnMarker + cursorSize), float(mybuilding.yAntSpawnMarker + cursorSize), 0xB00B69);
				screen->Line(float(mybuilding.xAntSpawnMarker - cursorSize), float(mybuilding.yAntSpawnMarker + cursorSize), float(mybuilding.xAntSpawnMarker + cursorSize), float(mybuilding.yAntSpawnMarker - cursorSize), 0xB00B69);
			}

			//code for drawing the BUILDING class
			mybuilding.BuildingCode(screen);

			//goes through the class array and executes something for each PLAYER ant
			for (int i = 0; i < 100; i++) {

				myant[i].AntCode(screen);

				//compares each ant of the array to the others
				for (int z = 100 - 1; z > i; z--) {

					if (myant[z].isSpawned())
						//if (myant[i].checkCollideBetweenAnts(myant[z].x, myant[z].y, ANTSIZE, ANTSIZE))
						if (myant[i].checkCollision(myant[z]))
							//myant[i].Avoid(myant[z].x, myant[z].y);
							myant[i].Avoid(myant[z]);
				}

				if (myant[i].isSpawned()) {
					for (int z = 0; z < 5; z++) {
						if (myant[i].checkCollision(myresource[z].x, myresource[z].y, myresource[z].x_width, myresource[z].y_height))
						//if (myant[i].checkCollideBetweenAnts(myresource[z].x, myresource[z].y, myresource[z].x_width, myresource[z].y_height))
						{
							myresource[z].ReInit();
							resourceAmount += myresource[z].pelletValue;
						}
					}
				}

				//counts the number of ants selected
				if (myant[i].antSelected == true) {
					if (!myant[i].countedThisAntAlready) {
						numSelected += 1;
						myant[i].countedThisAntAlready = true;
					}
				}

				if (myant[i].checkCollision(mybuilding.x, mybuilding.y, mybuilding.x_width, mybuilding.y_height))
					myant[i].Avoid(mybuilding.x, mybuilding.y);


				//AMOUNT OF WORKING ANTS COUNTER
				if (myant[i].isSpawned()) {
					numOfActiveAnts++;
				}

				if (i == 99) {
					printf("%i \n", numOfActiveAnts);
					numOfActiveAnts = 0;
				}
			}

			//goes through the class array and executes something for each ENEMY ant
			for (int i = 0; i < 15; i++) {

				for (int z = 15 - 1; z > i; z--) {

					if(myenemyant[i].checkCollision(myenemyant[z]))
						myenemyant[i].Avoid(myenemyant[z].x, myenemyant[z].y);
				}

				for (int n = 0; n < 100; n++) {
					if (myenemyant[i].checkCollision(myant[n]))
					{
						if (myant[n].isSpawned()) {
							myenemyant[i].RandomizeNewSpawn();
							myant[n].DespawnAnt();
							displayScore += 100;
						}
					}
				}

				if (myenemyant[i].checkCollision(mybuilding.x, mybuilding.y, mybuilding.x_width, mybuilding.y_height))
				{
					myenemyant[i].RandomizeNewSpawn();
					lifePoints-= 1;
				}

				myenemyant[i].EnemyAntCode(screen);

				//printf("Enemy ant %i: x %i  y %i \n", i, myenemyant[i].x, myenemyant[i].y);
			}

			//goes through the class array and executes something for each RESOURCE PALLET
			for (int i = 0; i < 5; i++) {
				myresource[i].DrawPellet(screen);
			}

			//this will probably be the code snippet that I present
			if (GetAsyncKeyState(VK_LCONTROL)) {
				if (numOfActiveAnts < 100 && isReset && resourceAmount >= 500) {
					for (int i = 0; i < 100; i++)
					{
						if (!myant[i].isSpawned()) { //finds an ant that is not spawned and initiates it 

							//checks to see if there is a set destination for the building and if there is the ant gets sent there instead a random loc near base
							if (mybuilding.antSpawnMarkerPlaced) {
								myant[i].TakeAntToBuildingSpawnPoint(mybuilding);
							}

							myant[i].InitAnt(); // class function to initiate a player ant
							resourceAmount -= 500; //this value has to be the same as the if up top
							i = 100; //if it finds an ant to spawn it immidiately ends the loop since I want to spawn a single ant in the loop
							setTimer = timeDeltaTime + 100; // 0.1 seconds delay between spawning ants so it doesn't spawn them every frame

						}
					}
				}
			}

			//timer every frame
			if (setTimer < timeDeltaTime) {
				isReset = true;
			}
			else isReset = false;

			//every 10 seconds enemy ants get faster
			if (waveTimer < timeDeltaTime && enemyAntSpeed <= 100) {
				waveTimer = timeDeltaTime + 5000; //5 second timer
				enemyAntSpeed++;
			}

			//game over condition
			if (lifePoints <= 0) {
				screenSelected = 2;
			}

			Sleep(10); //
		}

		//GAME OVER SCENE
		if (screenSelected == 2) {
			screen->Print("GAME OVER", 100, 100, 0xff0000);
			screen->Print("PRESS LEFT CTRL TO PLAY AGAIN", 100, 110, 0xffffff);

			if (GetAsyncKeyState(VK_LCONTROL)) {
				ReInitGame();
				screenSelected = 1;
			}
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