#include "meteors.h"

#include <cmath>

#include "spaceship.h"
#include "game.h"
#include "app.h"

using namespace app;
using namespace game;
using namespace spaceship;

namespace app
{
	namespace meteors
	{
		struct Canon
		{
			Rectangle rec;
			Vector2 position;
			float angle;
			Shoot shoot;
			float timer;
			float shootSpeed;
			float shootDirection;
		};

		static const int maxBigMeteors = 6;
		static Meteor bigMeteor[maxBigMeteors];

		static const int maxFloorMeteors = 5;
		static Meteor floorMeteors[maxFloorMeteors];
		static Canon canons[maxFloorMeteors];
		static Texture2D spriteFM; //floor meteors
		static Texture2D spriteFMS; //floor meteors shoot
		static Vector2 vDirection; //determina la direccion del disparo de las maquinas
		static int destroyedFloorMeteorsCount;

		static int destroyedMeteorsCount;
		static float meteorSpeed;
		static const float speedBoost = 100.0f; //variable para que la velocidad se ajuste al GetFrameTime
		static int initialCorrection = 150;//para que los meteoros no tomen la posicion de la nave en el inicio
		static float posx, posy;
		static float velx, vely;
		static bool correctRange;

		//Imagenes y texturas
		static Image meteorImage;
		static Texture2D meteorTexture;

		//Numeros magicos para escalar las textura
		static float bigMeteorScale;
		static Vector2 bigMeteorScalePos;

		//Sonidos 
		static Sound explosionSound;
		bool pauseSoundExplosion=false;

		static bool init;
		static int scaleAux = 1600;
		static void initFloorMeteors();
		static void updateFloorMeteors();
		static void drawFloorMeteors();
		static const float floorMeteorsDistance = 400 * 6 / maxFloorMeteors;
		const int posMachinesY = 695;

		static void initFloorMeteors()
		{
			destroyedFloorMeteorsCount = 0;
			floorMeteors[0].position = { (float)GetScreenWidth() / 6 * 4, posMachinesY };
			for (int i = 0; i < maxFloorMeteors; i++)
			{
				floorMeteors[i].active = true;
				floorMeteors[i].radius = 50.0f;
				floorMeteors[i].color = RED;
				floorMeteors[i].speed = { 100.0f * 6 / maxFloorMeteors,0.0f };
				if (i != 0)
				{
					floorMeteors[i].position = { floorMeteors[i - 1].position.x + floorMeteorsDistance,posMachinesY };
				}
				floorMeteors[i].tag = i;
				//-----------------------------------

				canons[i].rec.x = floorMeteors[i].position.x;
				canons[i].rec.y = floorMeteors[i].position.y;
				canons[i].rec.width = floorMeteors[i].radius*3/2;
				canons[i].rec.height = 24;
				canons[i].angle = -90;

				spriteFM = LoadTexture("res/assets/machines.png");
				spriteFMS = LoadTexture("res/assets/shoot2.png");

				canons[i].timer = 0.0f;
				canons[i].shoot.active = false;
				canons[i].shoot.position.x = canons[i].rec.x;
				canons[i].shoot.position.y = canons[i].rec.y;
				canons[i].shoot.rotation = canons[i].angle;
				canons[i].shootSpeed = 400.0f;
				canons[i].shoot.radius = 3;
			}
		}

		static int lastMeteorX;

		static void checkFloorMeteorsCol()
		{
			for (int i = 0; i < shipMaxBombs; i++)
			{
				for (int j = 0; j < maxFloorMeteors; j++)
				{
					if (CheckCollisionCircles(bombs[i].position, bombs[i].radius, floorMeteors[j].position, floorMeteors[j].radius) && floorMeteors[j].active)
					{
						floorMeteors[j].active = false;
						bombs[i].active = false;
						destroyedFloorMeteorsCount++;
					}
				}
			}
		}

		static void checkLastMeteor()
		{
			lastMeteorX = 0;
			for (int k = 0; k < maxFloorMeteors; k++)
			{
				if (floorMeteors[k].position.x > lastMeteorX)
				{
					lastMeteorX = floorMeteors[k].position.x;
				}
			}
		}

		static void updateCanonShoots()
		{
			int angleFixed = 79;
			for (int i = 0; i < maxFloorMeteors; i++)
			{
				canons[i].shoot.rotation = canons[i].angle + angleFixed;

				if (!canons[i].shoot.active)
				{
					canons[i].timer += GetFrameTime();
				}
				if (canons[i].timer > 3)
				{
					canons[i].shoot.active = true;
					canons[i].timer = 0;
				}
				if (!canons[i].shoot.active)
				{
					canons[i].shootDirection = canons[i].shoot.rotation;
					canons[i].shoot.position.x = canons[i].rec.x;
					canons[i].shoot.position.y = canons[i].rec.y;
				}
				else
				{
					canons[i].shoot.position.x += sin(canons[i].shootDirection*DEG2RAD)*canons[i].shootSpeed*GetFrameTime();
					canons[i].shoot.position.y -= cos(canons[i].shootDirection*DEG2RAD)*canons[i].shootSpeed*GetFrameTime();
				}
				if (canons[i].shoot.position.x<0 || canons[i].shoot.position.x>GetScreenWidth() ||
					canons[i].shoot.position.y<0 || canons[i].shoot.position.y>GetScreenHeight())
				{
					canons[i].shoot.active = false;
				}

				if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, canons[i].shoot.position, canons[i].shoot.radius))
				{
					gameOver = true;
					victory = false;
					currentScreen = GameOver;
				}
			}
		}
		static void updateFloorMeteors()
		{
			for (int i = 0; i < maxFloorMeteors; i++)
			{
				if (floorMeteors[i].active)
				{
					floorMeteors[i].position.x -= floorMeteors[i].speed.x*GetFrameTime();
					if (floorMeteors[i].position.x < -floorMeteors[i].radius * 2)
					{
						checkLastMeteor();
						floorMeteors[i].position.x = lastMeteorX + floorMeteorsDistance;
					}
					//--------------------------------------
					canons[i].rec.x = floorMeteors[i].position.x;
					canons[i].rec.y = floorMeteors[i].position.y;

					vDirection.x = ship.position.x-canons[i].rec.x;

					vDirection.y = ship.position.y-canons[i].rec.y;

					canons[i].angle = atan2(vDirection.y, vDirection.x)*RAD2DEG;
					//----------------------------------------

					
				}
			}
			updateCanonShoots();
			checkFloorMeteorsCol();
		}

		static void drawFloorMeteors()
		{
			for (int i = 0; i < maxFloorMeteors; i++)
			{
				if (floorMeteors[i].active)
				{
					DrawTexture(spriteFMS, canons[i].shoot.position.x - spriteFMS.width / 2,
						canons[i].shoot.position.y - spriteFMS.height / 2, WHITE);
					DrawTexturePro(spriteFM, { 0.0f,0.0f,(float)spriteFM.width / 2,(float)spriteFM.height },
						{ floorMeteors[i].position.x,floorMeteors[i].position.y,(float)spriteFM.width/2,(float)spriteFM.height },
						{ (float)spriteFM.width / 4,(float)spriteFM.height / 2 }, 0.0f, WHITE);
					DrawTexturePro(spriteFM, { (float)spriteFM.width / 2,0.0f,(float)spriteFM.width / 2,(float)spriteFM.height },
						{floorMeteors[i].position.x,floorMeteors[i].position.y,(float)spriteFM.width/4,(float)spriteFM.height /2},
						{ 0.0f, (float)spriteFM.height / 4 }, canons[i].angle, WHITE);
				}
			}
		}

		void InitMeteors()
		{
			explosionSound = LoadSound("res/assets/explosion.wav");
			meteorImage = LoadImage("res/assets/asteroide.png");
	
			meteorTexture = LoadTextureFromImage(meteorImage);

			correctRange = false;
			init = false;

			destroyedMeteorsCount = 0;
			meteorSpeed = 2;

			for (int i = 0; i < maxBigMeteors; i++)
			{
				posx = GetRandomValue(GetScreenWidth()-bigMeteor[i].radius,GetScreenWidth()-500);

				posy = GetRandomValue(0+ bigMeteor[i].radius*2, GetScreenHeight()- bigMeteor[i].radius*2);

				bigMeteor[i].position = { posx, posy };

				velx = GetRandomValue(-meteorSpeed, -meteorSpeed-1);
				vely = GetRandomValue(-meteorSpeed, meteorSpeed);

				while (!correctRange)
				{
					if (velx == 0 && vely == 0)
					{
						velx = -meteorSpeed;
						vely = GetRandomValue(-meteorSpeed, meteorSpeed);
					}
					else correctRange = true;
				}

				bigMeteor[i].speed = { velx, vely };
				bigMeteor[i].radius = (50 * GetScreenWidth()) / scaleAux;
				bigMeteor[i].active = true;
				bigMeteor[i].color = BLUE;
			}

			bigMeteorScale = (GetScreenWidth()* 0.25f) / scaleAux;
			bigMeteorScalePos = { (bigMeteorScale*meteorImage.width) / 2 ,(bigMeteorScale*meteorImage.height) / 2 };
			initFloorMeteors();
		}

		void UpdateMeteors()
		{
			if (!gameOver)
			{
				for (int a = 0; a < maxBigMeteors; a++)
				{
					if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active)
					{
						gameOver = true;
						victory = false;
						currentScreen = GameOver;
					}
				}

				// Meteors logic: big meteors
				for (int i = 0; i < maxBigMeteors; i++)
				{
					if (bigMeteor[i].active)
					{
						// Movement
						bigMeteor[i].position.x += bigMeteor[i].speed.x*GetFrameTime() * speedBoost;
						bigMeteor[i].position.y += bigMeteor[i].speed.y*GetFrameTime() * speedBoost;

						// Collision logic: meteor vs wall
						if (bigMeteor[i].position.x > GetScreenWidth() + bigMeteor[i].radius)
						{
							bigMeteor[i].position.x = -(bigMeteor[i].radius);
						}
						else if (bigMeteor[i].position.x < 0 - bigMeteor[i].radius)
						{
							bigMeteor[i].position.x = GetScreenWidth() + bigMeteor[i].radius;
							bigMeteor[i].position.y= GetRandomValue(bigMeteor[i].radius, GetScreenHeight());
						}
						if (bigMeteor[i].position.y > GetScreenHeight() - bigMeteor[i].radius)
						{
							bigMeteor[i].speed.y *= -1;
						}
						else if (bigMeteor[i].position.y < 0 + bigMeteor[i].radius)
						{
							bigMeteor[i].speed.y *= -1;
						}
					}
				}

				// Collision logic: player-shoots vs meteors
				for (int i = 0; i < shipMaxShoots; i++)
				{
					if ((shoot[i].active))
					{
						for (int a = 0; a < maxBigMeteors; a++)
						{
							if (bigMeteor[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, bigMeteor[a].position, bigMeteor[a].radius))
							{
								if (!pauseSoundExplosion)
								{
									PlaySound(explosionSound);
								}
								else
								{
									PauseSound(explosionSound);
								}
								shoot[i].active = false;
								bigMeteor[a].active = false;
								destroyedMeteorsCount++;
								a = maxBigMeteors;
							}
						}

						
					}
				}
			}

			updateFloorMeteors();

			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount==maxFloorMeteors)
			{
				victory = true;
				gameOver = true;
				currentScreen = GameOver;
			}

		}

		void DrawMeteors()
		{
			for (int i = 0; i < maxBigMeteors; i++)
			{
				if (bigMeteor[i].active)
				{
					DrawCircleV(bigMeteor[i].position, bigMeteor[i].radius, DARKGRAY);
					DrawTextureEx(meteorTexture, { bigMeteor[i].position.x - bigMeteorScalePos.x,bigMeteor[i].position.y - bigMeteorScalePos.y }, 0, bigMeteorScale, WHITE);
				}
				else DrawCircleV(bigMeteor[i].position, bigMeteor[i].radius, BLANK);
			}
			drawFloorMeteors();
		}

		void UnloadMeteors()
		{
			UnloadSound(explosionSound);
			UnloadTexture(meteorTexture);
			UnloadImage(meteorImage);
			UnloadTexture(spriteFM);
			UnloadTexture(spriteFMS);
		}

		void ResetMeteors()
		{
			correctRange = false;
			init = false;

			destroyedMeteorsCount = 0;
			meteorSpeed = 2;

			for (int i = 0; i < maxBigMeteors; i++)
			{
				posx = GetRandomValue(GetScreenWidth() - bigMeteor[i].radius, GetScreenWidth() - 500);

				posy = GetRandomValue(0 + bigMeteor[i].radius * 2, GetScreenHeight() - bigMeteor[i].radius * 2);

				bigMeteor[i].position = { posx, posy };

				velx = GetRandomValue(-meteorSpeed, -meteorSpeed - 1);
				vely = GetRandomValue(-meteorSpeed, meteorSpeed);

				while (!correctRange)
				{
					if (velx == 0 && vely == 0)
					{
						velx = -meteorSpeed;
						vely = GetRandomValue(-meteorSpeed, meteorSpeed);
					}
					else correctRange = true;
				}

				bigMeteor[i].speed = { velx, vely };
				bigMeteor[i].radius = (50 * GetScreenWidth()) / scaleAux;
				bigMeteor[i].active = true;
				bigMeteor[i].color = BLUE;
			}

			bigMeteorScale = (GetScreenWidth()* 0.25f) / scaleAux;
			bigMeteorScalePos = { (bigMeteorScale*meteorImage.width) / 2 ,(bigMeteorScale*meteorImage.height) / 2 };
			initFloorMeteors();
		}
	}
}