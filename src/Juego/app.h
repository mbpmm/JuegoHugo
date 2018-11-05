#ifndef APP_H
#define APP_H

namespace app
{
	enum Screens
	{
		Menu = 0, Gameplay, GameOver, Credits, Pause
	};

	extern bool exit;
	extern Screens currentScreen;

	void ExecuteGame();
}
#endif // !APP_H