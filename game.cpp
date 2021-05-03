#include "game.h"
#include "surface.h"
#include <cstdio> 

#define WIN32_LEAN_AND_MEAN
#define ANTSIZE 20
#include <windows.h>
#include "template.h"

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
	int numSelected = 0;
	int debugInt;
	int numOfActiveAnts = 10;

	Sprite ant(new Surface("assets/ant2.png"), 1);
	int antSpeed = 1;

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
			ant.DrawScaled(x, y , ANTSIZE, ANTSIZE, gameScreen);

			destinationToMarker = sqrt(pow(x - antDestinationX, 2) + pow(y - antDestinationY, 2));

			if (antSelected == true) 				
				gameScreen->Box(x, y, x + ANTSIZE, y + ANTSIZE, 0x29B1CA);
			BoxConfiguration();



			if (destinationToMarker <= 10)
				antHeadingToDestination = false;
			else if (antSelected && destinationPlaced)
				antHeadingToDestination = true;
			

			//basic pathfinding 
			if (antHeadingToDestination == true) {
				if (antDestinationX >= x && antDestinationY >= y) {
					//destination in bottom right
					debugInt = 4;
					x += antSpeed;
					y += antSpeed;
				}else if(antDestinationX <= x && antDestinationY >= y){
					//destinaiton in bottom left
					debugInt = 3;
					x -= antSpeed;
					y += antSpeed;
				}else if (antDestinationX <= x && antDestinationY <= y) {
					//destination in top left
					debugInt = 2;
					x -= antSpeed;
					y -= antSpeed;

				}else if(antDestinationX >= x && antDestinationY <= y) {
					//destination in top rights
					debugInt = 1;
					x += antSpeed;
					y -= antSpeed;
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

		int x, y, rotation;
		bool antSelected = false;
		bool countedThisAntAlready = false;
		bool antHeadingToDestination = false;
		float destinationToMarker;
	};

	Ant myant[10];

	void Game::Init()
	{
		//do nothing
	}
	
	void Game::Shutdown()
	{
		//do nothing
	}

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);
		screen->Print("MAIN SCREEN yes", 20, 20, 0xffffff);
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

		for (int i = 0; i < numOfActiveAnts; i++) {
			
			myant[i].Move(screen);

			//displays the number of ants selected
			if (myant[i].antSelected == true) {
				if (!myant[i].countedThisAntAlready) {
					numSelected += 1;
					myant[i].countedThisAntAlready = true;
				}
			}
		}

		//doesn't work for some 
		if (GetAsyncKeyState(VK_LCONTROL)) {
			Shutdown();
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
		printf("ant 1 distance: %f \n", myant[1].destinationToMarker);
		//printf("mouse coordinates %i %i \n", mousex, mousey);
		//printf("num of ants selected: %i \n", numSelected);
		//printf("Box Style %i \n", boxStyle);
		//printf("start X: %i  start Y: %i \n", boxStartX, boxStartY);
		//printf("end X: %i  end Y: %i \n", boxEndX, boxEndY);
		
	}
};