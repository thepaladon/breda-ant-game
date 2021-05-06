#include "game.h"
#include "surface.h"
#include <cstdio> 

#include <windows.h>
#include "template.h"

#define WIN32_LEAN_AND_MEAN
#define ANTSIZE 20

namespace Tmpl8
{
	//Scene Manager
	int screenSelected = 1;
	int cursorSize = 5;

	//Box Variables
	int boxStartX, boxStartY, boxEndX, boxEndY;
	int antDestinationX, antDestinationY;
	bool doThisOnce = true;
	bool destinationPlaced = false;
	int boxStyle;
	int numSelected;
	int debugInt;
	int numOfActiveAnts = 10;

	int antSpeed = 1;
	
	Sprite ant(new Surface("assets/ant2scaleddown20.jpg"), 8);

	class Ant
	{
	public:
		
		Ant()
		{
			x = IRand(1200);
			y = IRand(700);
			rotation = 0;
		}

		void Move(Surface* gameScreen)
		{
			ant.SetFrame(rotation);
			ant.Draw(gameScreen, x, y);

			destinationToMarker = sqrt(pow(x - antDestinationX, 2) + pow(y - antDestinationY, 2));

			if (antSelected == true) 				
				gameScreen->Box(x, y, x + ANTSIZE, y + ANTSIZE, 0x29B1CA);

			BoxConfiguration();
			BasicPathfinding();
		}

		//works perfectly
		bool checkCollide(int xTwo, int yTwo)
		{
			int oWidth = ANTSIZE;
			int oHeight = ANTSIZE;
			int oTwoWidth = ANTSIZE;
			int oTwoHeight = ANTSIZE;

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

		void BasicPathfinding() {

			if (destinationToMarker <= 10)
				antHeadingToDestination = false;
			else if (antSelected && destinationPlaced)
				antHeadingToDestination = true;

			if (antHeadingToDestination == true) {
				if (antDestinationX - (ANTSIZE / 2) > x && antDestinationY - (ANTSIZE / 2) > y) {
					//destination in bottom right
					rotation = 7;
					debugInt = 4;
					x += antSpeed;
					y += antSpeed;
				}
				else if (antDestinationX - (ANTSIZE / 2) < x && antDestinationY - (ANTSIZE / 2) > y) {
					//destinaiton in bottom left
					rotation = 6;
					debugInt = 3;
					x -= antSpeed;
					y += antSpeed;
				}
				else if (antDestinationX - (ANTSIZE / 2) < x && antDestinationY - (ANTSIZE / 2) < y) {
					//destination in top left
					rotation = 5;
					debugInt = 2;
					x -= antSpeed;
					y -= antSpeed;

				}
				else if (antDestinationX - (ANTSIZE / 2) > x && antDestinationY - (ANTSIZE / 2) < y) {
					//destination in top right
					rotation = 4;
					debugInt = 1;
					x += antSpeed;
					y -= antSpeed;

				}else if (antDestinationX - (ANTSIZE / 2) == x && antDestinationY - (ANTSIZE / 2) > y) {
					//destinaiton below
					rotation = 1;
					debugInt = 5;
					y += antSpeed;	
				}
				else if (antDestinationX - (ANTSIZE / 2) == x && antDestinationY - (ANTSIZE / 2) < y) {
					//destination above
					rotation = 0;
					debugInt = 6;
					y -= antSpeed;
				}
				else if (antDestinationX - (ANTSIZE / 2) > x && antDestinationY - (ANTSIZE / 2) == y) {
					//destination to the right
					rotation = 3;
					debugInt = 7;
					x += antSpeed;
				}
				else if (antDestinationX - (ANTSIZE / 2) < x && antDestinationY - (ANTSIZE / 2) == y) {
					//destination to the left
					rotation = 2;
					debugInt = 8;
					x -= antSpeed;
				}
			}
		}

		void BoxConfiguration() {
			//Simple Visualisation saved in Notepad in Assets
			//box dragged to top left
			if (boxStartX < boxEndX && boxStartY > boxEndY) {
				boxStyle = 2;
				if (x > boxStartX && y < boxStartY && x < boxEndX && y > boxEndY) {
					antSelected = true;
				}
				else antSelected = false;
			}

			//box dragged to bottom right
			 if (boxStartX > boxEndX && boxStartY < boxEndY) {
				boxStyle = 4;
				if (x < boxStartX && y > boxStartY && x > boxEndX && y < boxEndY)
					antSelected = true;

				else antSelected = false;
			}

			//box dragged to bottom left
			if (boxStartX > boxEndX && boxStartY > boxEndY) {
				boxStyle = 3;
				if (x < boxStartX && y < boxStartY && x > boxEndX && y > boxEndY)
					antSelected = true;

				else antSelected = false;
			}

			//box dragged to top left
			if (boxStartX < boxEndX && boxStartY < boxEndY) {
				boxStyle = 1;
				if (x > boxStartX && y > boxStartY && x < boxEndX && y < boxEndY)
					antSelected = true;

				else antSelected = false;
			}

			//void AntMove()
			//{
			//	//do nothing
			//}
			
		}
		
		//found out I could use vec2's here but updating it would be too time consuming 
		int x, y, rotation; // x and y are the top right coordinates of each ants
		int xCollisionUpRight, yCollisionUpRight;
		bool antSelected = false;
		bool countedThisAntAlready = false;
		bool antHeadingToDestination = false;
		float destinationToMarker;
	};

	Ant myant[50];

	void Game::Init()
	{
		//for (int i = 0; i < 49; i++) {
		//	myant->SetAntGraphic();
		//}
	}
	
	void Game::Shutdown()
	{
		//do nothing
	}

	void Game::Tick(float deltaTime)
	{


		screen->Clear(0);

		//a reference map
		for (int y = 0; y < 72; y++) // the y of the map is drawn
			for (int x = 0; x < 128; x++) //the x of the map is drawn
				screen->Box(x * 10, y * 10, x * 10 + 10, y * 10 + 10, 0x3C2B0A);

		screen->Print("Create more ants by pressing X", 20, 20, 0xffffff);
		screen->Line(mousex - cursorSize, mousey - cursorSize, mousex + cursorSize, mousey + cursorSize, 0xff0000);
		screen->Line(mousex - cursorSize, mousey + cursorSize, mousex + cursorSize, mousey - cursorSize, 0xff0000);

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			if (doThisOnce == true) {
				//start coords of the box
				boxStartX = mousex;
				boxStartY = mousey;
				
				for (int n = 0; n < numOfActiveAnts; n++) {
					myant[n].antSelected = false;
					myant[n].countedThisAntAlready= false;
				}
				
				numSelected = 0;
				doThisOnce = false;
			}
			//end coords of the box
			boxEndX = mousex;
			boxEndY = mousey;

			//drawing the box
			// 0x29B1CA -> nice cyan color for selected 
			screen->Box(boxStartX, boxStartY, boxEndX, boxEndY, 0x29B1CA);

		}
		else doThisOnce = true;

		if (GetAsyncKeyState(VK_RBUTTON) && numSelected > 0) {
			destinationPlaced = true;
			antDestinationX = mousex;
			antDestinationY = mousey;
		}

		//fix this so it only appeared when there is more that 1 ant selected at the moment and make the ants travel towards it
		if (destinationPlaced && numSelected > 0) {
			screen->Line(antDestinationX - cursorSize, antDestinationY - cursorSize, antDestinationX + cursorSize, antDestinationY + cursorSize, 0x29B1CA);
			screen->Line(antDestinationX - cursorSize, antDestinationY + cursorSize, antDestinationX + cursorSize, antDestinationY - cursorSize, 0x29B1CA);
		}
		
		//goes through the class array and executes something for each ant
		for (int i = 0; i < numOfActiveAnts; i++) {
			
			myant[i].Move(screen);

			//compares each ant of the array to the others
			for(int z = numOfActiveAnts-1; z > i; z--) {

				if (myant[i].checkCollide(myant[z].x, myant[z].y))
					printf("Collision between ant %i and %i \n", i, z);

				//example:
				//myant[0] -> should check for all ants from -> 1 to 9 (numOfActiveAnts - 1 ) ;
				//myant[3] -> should check for all ants from -> 4 to 9 (numOfActiveAnts - 1 ) ;
			}

			//displays the number of ants selected
			if (myant[i].antSelected == true) {
				if (!myant[i].countedThisAntAlready) {
					numSelected += 1;
					myant[i].countedThisAntAlready = true;
				}
			}
		}

		//doesn't work for some reason
		if (GetAsyncKeyState(VK_LCONTROL)) {
			Shutdown();
			if(numOfActiveAnts < 49)
				numOfActiveAnts++;
		}
		
		//Basic Scene Code
		/*if(mousePressed == 1)
		{
			screen->Print("Welcome to my game", 2, 2, 0xffffff);
		}
		else if (mousePressed == 2)
		{
			screen->Print("Start Screen", 2, 2, 0xffffff);
		}*/

		// DEBUG COMMANDS:
		
		//printf("direction: %i \n", debugInt);
		//printf("antDir: %i %i \n", antDestinationX, antDestinationY);
		//printf("antCoords: %i %i \n", myant[1].x, myant[1].y);
		//printf("ant 1 distance: %f \n", myant[1].destinationToMarker);
		//printf("mouse coordinates %i %i \n", mousex, mousey);
		//printf("num of ants selected: %i \n", numSelected);
		//printf("Box Style %i \n", boxStyle);
		//printf("start X: %i  start Y: %i \n", boxStartX, boxStartY);
		//printf("end X: %i  end Y: %i \n", boxEndX, boxEndY);

	}
};