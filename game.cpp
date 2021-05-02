#include "game.h"
#include "surface.h"
#include <cstdio> 

namespace Tmpl8
{
	int screenSelected = 1;
	int mousecoordinateX, mousecoordinateY;

	void Game::Init()
	{

	}
	
	void Game::Shutdown()
	{

	}

	void Game::Tick(float deltaTime)
	{
		if(screenSelected == 1)
		{
			screen->Print("Welcome to my game", 2, 2, 0xffffff);
			
		}
		else if (screenSelected == 0) 
		{
			screen->Print("Start Screen", 2, 2, 0xffffff);
		}
		printf("mouse coordinates %i %i \n", mousex, mousey);
		printf("mouse is pressed %i \n", mousePressed);
		
		mousecoordinateX += mousex;
		mousecoordinateY += mousey;


	}
};