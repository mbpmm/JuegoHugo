#ifndef GAME_H
#define GAME_H

namespace app
{
	namespace game
	{
		void InitValues();
		void UpdateFrame();
		void DrawGameplay();
		void ResetValues();
		void UnloadGameplay();

		extern bool victory;
		extern bool gameOver;
	}
}
#endif // !GAME_H