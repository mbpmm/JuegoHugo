#include "game_over.h"

#include <raylib.h>

#include "game.h"
#include "menu.h"
#include "app.h"

using namespace app;
using namespace game;
using namespace menu;

namespace app
{
	namespace gameover
	{
		static char text1[] = "Perdiste  :(";
		static char text2[] = "Ganaste!!!";
		static char text3[] = "JUGAR DE NUEVO";
		static char text4[] = "MENU";
		static int text1PositionX = 0;
		static int text1PositionY = 0;
		static int text2PositionX = 0;
		static int text3PositionX = 0;
		static int text3PositionY = 0;
		static int text4PositionX = 0;
		static int text4PositionY = 0;
		static int winnerNumber = 0;
		static int sizeText1 = 0;
		static int sizeText2 = 0;

		static Vector2 mousePoint;
		static Rectangle rect1;
		static Rectangle rect2;
		static Color colorRect1;
		static Color colorRect2;

		static const int scaleAux1 = 800;
		static const int scaleAux2 = 1600;

		void InitGameover()
		{
			sizeText1 = (GetScreenWidth() * 50) / scaleAux1;
			sizeText2 = (GetScreenWidth() * 20) / scaleAux1;
			text1PositionX = GetScreenWidth() / 2 - MeasureText(text1, sizeText1) / 2;
			text1PositionY = GetScreenHeight() / 2 - GetScreenHeight() * 0.13333333;
			text2PositionX = GetScreenWidth() / 2 - MeasureText(text2, sizeText1) / 2;
			text3PositionX = GetScreenWidth()/2 - MeasureText(text3, sizeText2) / 2;
			text3PositionY = GetScreenHeight() / 2 + GetScreenHeight() * 0.13333333;
			text4PositionX = GetScreenWidth() / 2 - MeasureText(text4, sizeText2) / 2;
			text4PositionY = GetScreenHeight() / 2 + GetScreenHeight() * 0.23333333;

			colorRect1 = PURPLE;
			colorRect2 = PURPLE;

			rect1.height = (GetScreenWidth() * 80) / scaleAux2;
			rect1.width = (GetScreenWidth() * 385) / scaleAux2;
			rect1.x = GetScreenWidth() / 2 - rect1.width / 2;
			rect1.y = GetScreenHeight() / 2 + GetScreenHeight() * 0.11;

			rect2.height = (GetScreenWidth() * 80) / scaleAux2;
			rect2.width = (GetScreenWidth() * 385) / scaleAux2;
			rect2.x = GetScreenWidth() / 2 - rect1.width / 2;
			rect2.y = GetScreenHeight() / 2 + GetScreenHeight() * 0.21;
		}

		void UpdateGameOver()
		{
			mousePoint = GetMousePosition();
			if (CheckCollisionPointRec(mousePoint, rect1))
			{
				colorRect1.a = 120;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentScreen = Gameplay;
					ResetValues();
				}
			}
			else colorRect1.a = 255;

			if (CheckCollisionPointRec(mousePoint, rect2))
			{
				colorRect2.a = 120;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					currentScreen = Menu;
					InitMenu();
					ResetValues();
					UnloadGameplay();
				}
			}
			else colorRect2.a = 255;
		}

		void DrawGameOver()
		{
			ClearBackground(BLACK);
			DrawRectangleRec(rect1, colorRect1);
			DrawRectangleRec(rect2, colorRect2);
			if (!victory)
			{
				DrawText(text1, text1PositionX, text1PositionY, sizeText1, GRAY);
			}
			else
			{
				DrawText(text2,text2PositionX, text1PositionY, sizeText1, GRAY);
			}
			DrawText(text3, text3PositionX, text3PositionY, sizeText2, BLACK);
			DrawText(text4, text4PositionX, text4PositionY, sizeText2, BLACK);
		}
	}
}