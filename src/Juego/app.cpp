#include "app.h"

#include "game.h"
#include "menu.h"
#include "game_over.h"
#include "credits.h"
#include "pause.h"
#include "spaceship.h"
#include "meteors.h"

using namespace app;
using namespace game;
using namespace menu;
using namespace gameover;
using namespace credits;
using namespace pause;
using namespace spaceship;
using namespace meteors;

namespace app
{
	static int screenWidth = 1280;
	static int screenHeight =768;
	bool exit = false;
	Screens currentScreen = Menu;
	static Music menuSong;
	static bool pauseMusic = false;

	static void InitGame()
	{
		SetExitKey(0);
		InitWindow(screenWidth, screenHeight, "GRAD10S");
		InitAudioDevice();
		InitMenu();
		InitCredits();
		InitGameover();
		InitPause();
		menuSong = LoadMusicStream("res/assets/menusong2.ogg");
		PlayMusicStream(menuSong);

	}

	static void UpdateGame()
	{
		UpdateMusicStream(menuSong);
		if (mute)
		{
			pauseMusic = !pauseMusic;
			pauseSoundShoot = !pauseSoundShoot;
			pauseSoundExplosion = !pauseSoundExplosion;

			if (pauseMusic) StopMusicStream(menuSong);
			else PlayMusicStream(menuSong);
			mute = false;
		}
		switch (currentScreen)
		{
		case Menu:
			UpdateMenu();
			break;
		case Credits:
			UpdateCredits();
			break;
		case Gameplay:
			UpdateFrame();
			break;
		case GameOver:
			UpdateGameOver();
			break;
		case Pause:
			UpdatePause();
			break;
		default:
			break;
		}
	}

	static void DrawGame()
	{
		BeginDrawing();
		switch (currentScreen)
		{
		case Menu:
			DrawMenu();
			break;
		case Credits:
			DrawCredits();
			break;
		case Gameplay:
			DrawGameplay();
			break;
		case GameOver:
			DrawGameOver();
			break;
		case Pause:
			DrawPause();
			break;
		default:
			break;
		}
		EndDrawing();
	}

	static void CloseGame()
	{
		CloseAudioDevice();
		CloseWindow();
	}

	void ExecuteGame()
	{
		InitGame();
		// Main game loop
		while (!WindowShouldClose()&&!exit)
		{
			UpdateGame();
			DrawGame();
		}
		CloseGame();
	}
}