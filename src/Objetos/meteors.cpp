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
			bool alive;
		};

		struct Enemy
		{
			Rectangle rec;
			Vector2 position;
			float angle;
			Shoot shoot;
			float timer;
			float shootSpeed;
			float shootDirection;
			Vector2 speed;
			bool active;
			int tag;
		};

		struct Boss
		{
			Rectangle rec;
			Vector2 position;
			float angle;
			Shoot shoot;
			Shoot super;
			float timer;
			float shootSpeed;
			float superSpeed;
			float shootDirection;
			Vector2 speed;
			bool active;
			int vida;
		};

		struct Coin
		{
			Rectangle rec;
			Vector2 position;
			Vector2 speed;
			int radius;
			bool active;
		};

		static const int maxCoins = 15;
		static Coin coins[maxCoins];
		static Texture2D coinTexture;
		static Rectangle frameRec;
		static int currentFrame;

		static float framesCounter;
		static float framesSpeed;

		static const int maxBoss = 1;
		static Boss boss;
		static Texture2D bossTexture;
		static Texture2D spriteBS;
		static Texture2D spriteSS;

		static Vector2 vDir2;
		static int destroyedBoss;

		static const int maxBigMeteors = 6;
		static Meteor bigMeteor[maxBigMeteors];

		static const int maxFloorMeteors = 5;
		static Meteor floorMeteors[maxFloorMeteors];
		static Canon canons[maxFloorMeteors];
		static Texture2D spriteFM; //floor meteors
		static Texture2D spriteFMS; //floor meteors shoot
		static Vector2 vDirection; //determina la direccion del disparo de las maquinas
		static int destroyedFloorMeteorsCount;

		static const int maxEnemies = 10;
		static Enemy enemies[maxEnemies];
		static Texture2D enemyTexture;
		static Texture2D spriteES;
		static Vector2 vDir;
		static int destroyedEnemies;

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
		static Sound coinSound;
		bool pauseSoundExplosion=false;

		static bool init;
		static int scaleAux = 1600;
		static void initFloorMeteors();
		static void updateFloorMeteors();
		static void drawFloorMeteors();
		static const float floorMeteorsDistance = 400 * 6 / maxFloorMeteors;
		const int posMachinesY = 695;

		static void initEnemies();
		static void updateEnemies();
		static void drawEnemies();
		static const float enemiesDistance = 450;

		static void initBoss();
		static void updateBoss();
		static void drawBoss();

		static void initCoin();
		static void updateCoin();
		static void drawCoin();

		static void initCoin()
		{
			coinSound = LoadSound("res/assets/coin.wav");
			coins[0].position = { (float)GetRandomValue(GetScreenWidth(),GetScreenWidth()),(float)GetRandomValue(0 + 100, GetScreenHeight() - 100) };
			for (int i = 0; i < maxCoins; i++)
			{
				coins[i].active = true;
				coins[i].radius = 16;
				coins[i].speed = { 300.0f * 6 / maxEnemies,0.0f };
				if (i != 0)
				{
					coins[i].position = { coins[i - 1].position.x + 2400,(float)GetRandomValue(0 + 100, GetScreenHeight() - 100) };
				}
				//-----------------------------------

				coinTexture = LoadTexture("res/assets/collectablesprite.png");

				frameRec = { 0.0f, 0.0f, (float)coinTexture.width, (float)224 / 7 };
				currentFrame = 0;

				framesCounter = 0;
				framesSpeed = 0.8f;

				coins[i].rec.x = coins[i].position.x;
				coins[i].rec.y = coins[i].position.y;
				coins[i].rec.width = coinTexture.width;
				coins[i].rec.height = coinTexture.height/7;

			}
		}

		static void updateCoin()
		{
			for (int i = 0; i < maxCoins; i++)
			{
				framesCounter += GetFrameTime();

				if (framesCounter >= framesSpeed)
				{
					framesCounter = 0;
					currentFrame++;

					if (currentFrame > 7) currentFrame = 0;

					frameRec.y = (float)currentFrame*(float)coinTexture.height / 7;
				}
				if (coins[i].active)
				{
					coins[i].position.x -= coins[i].speed.x*GetFrameTime();
					if (coins[i].position.x < -coins[i].rec.width * 2)
					{
						coins[i].active = false;
					}
					//--------------------------------------
					coins[i].rec.x = coins[i].position.x;
					coins[i].rec.y = coins[i].position.y;

					if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, coins[i].position, coins[i].radius))
					{
						if (!pauseSoundExplosion)
						{
							PlaySound(coinSound);
						}
						else
						{
							PauseSound(coinSound);
						}
						coins[i].active = false;
					}
				}
			}
		}

		static void drawCoin()
		{
			for (int i = 0; i < maxCoins; i++)
			{
				if (coins[i].active)
				{
					DrawTextureRec(coinTexture, frameRec, coins[i].position, WHITE);
				}
			}
		}

		static void initBoss()
		{
			destroyedBoss = 0;
			
			boss.vida = 25;
			boss.active = true;
			boss.speed = { 200.0f * 6 / maxEnemies,250 };
			

			bossTexture = LoadTexture("res/assets/boss.png");
			spriteBS = LoadTexture("res/assets/shoot3.png");
			spriteSS = LoadTexture("res/assets/super.png");

			boss.position = { (float)GetScreenWidth(),(float)GetScreenHeight() / 2 - bossTexture.height/2 };

			boss.rec.x = boss.position.x;
			boss.rec.y = boss.position.y;
			boss.rec.width = bossTexture.width;
			boss.rec.height = bossTexture.height;

			boss.timer = 0.0f;
			boss.shoot.active = false;
			boss.shoot.position.x = boss.rec.x;
			boss.shoot.position.y = boss.rec.y;
			boss.shoot.rotation = boss.angle;
			boss.shootSpeed = 900.0f;
			boss.shoot.radius = 3;

			boss.super.active = false;
			boss.super.position.x = boss.rec.x;
			boss.super.position.y = boss.rec.y;
			boss.super.rotation = boss.angle;
			boss.superSpeed = 700.0f;
			boss.super.radius = 8;

		}

		static void updateBossShoots()
		{
			int angleFixed = 90;

			if (boss.active)
			{
				boss.shoot.rotation = boss.angle + angleFixed;

				if (!boss.shoot.active)
				{
					boss.timer += GetFrameTime();
				}
				if (boss.timer > 0.5f)
				{
					boss.shoot.active = true;
					boss.timer = 0;
				}
				if (!boss.shoot.active)
				{
					boss.shootDirection = boss.shoot.rotation;
					boss.shoot.position.x = boss.rec.x + bossTexture.width / 2;
					boss.shoot.position.y = boss.rec.y + bossTexture.height / 2;
				}
				else
				{
					boss.shoot.position.x += sin(boss.shootDirection*DEG2RAD)*boss.shootSpeed*GetFrameTime();
					boss.shoot.position.y -= cos(boss.shootDirection*DEG2RAD)*boss.shootSpeed*GetFrameTime();
				}
				if (boss.shoot.position.x<0 || boss.shoot.position.x>GetScreenWidth() ||
					boss.shoot.position.y<0 || boss.shoot.position.y>GetScreenHeight())
				{
					boss.shoot.active = false;
				}

				if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, boss.shoot.position, boss.shoot.radius))
				{
					gameOver = true;
					victory = false;
					currentScreen = GameOver;
				}

				boss.super.rotation =angleFixed;

				if (!boss.super.active)
				{
					boss.timer += GetFrameTime();
				}
				if (boss.timer > 0.5f)
				{
					boss.super.active = true;
					boss.timer = 0;
				}
				if (!boss.super.active)
				{
					boss.super.position.x = boss.rec.x + bossTexture.width / 2;
					boss.super.position.y = boss.rec.y + bossTexture.height / 2;
				}
				else
				{
					boss.super.position.x -= boss.superSpeed*GetFrameTime();
				}
				if (boss.super.position.x<0 || boss.super.position.x>GetScreenWidth() )
				{
					boss.super.active = false;
				}

				if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, boss.super.position, boss.super.radius))
				{
					gameOver = true;
					victory = false;
					currentScreen = GameOver;
				}
			}
		}

		static void updateBoss()
		{

			if (boss.active)
			{
				boss.position.x -= boss.speed.x*GetFrameTime();
				boss.position.y -= boss.speed.y*GetFrameTime();

				if (boss.position.y > GetScreenHeight() - 150)
				{
					boss.speed.y *= -1;
				}
				else if (boss.position.y < 0 + 150)
				{
					boss.speed.y *= -1;
				}
				if (boss.position.x < GetScreenWidth()-400)
				{
					boss.position.x = GetScreenWidth() - 400;
				}
				//--------------------------------------
				boss.rec.x = boss.position.x;
				boss.rec.y = boss.position.y;

				vDir2.x = ship.position.x - boss.rec.x;

				vDir2.y = ship.position.y - boss.rec.y;

				boss.angle = atan2(vDir2.y, vDir2.x)*RAD2DEG;
				//----------------------------------------


			}

			updateBossShoots();

			for (int i = 0; i < shipMaxShoots; i++)
			{
				if ((shoot[i].active))
				{

					if (boss.active && CheckCollisionCircleRec(shoot[i].position, shoot[i].radius, boss.rec))
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
						boss.vida--;
						if (boss.vida <= 0)
						{
							boss.active = false;
							destroyedBoss++;
						}
					}
					

				}
			}
		}

		static void drawBoss()
		{
			if (boss.active)
			{
				DrawTexture(spriteBS, boss.shoot.position.x - boss.shoot.radius * 3,
					boss.shoot.position.y - boss.shoot.radius * 3, WHITE);
				DrawTexture(spriteSS, boss.super.position.x - boss.super.radius * 2,
					boss.super.position.y - boss.super.radius * 2, WHITE);
			//	DrawCircle(boss.super.position.x, boss.super.position.y, boss.super.radius, WHITE);
				//DrawCircle(boss.shoot.position.x, boss.shoot.position.y, boss.shoot.radius, WHITE);
				DrawTexture(bossTexture, boss.position.x, boss.position.y, WHITE);

			}

		}

		static void initEnemies()
		{
			destroyedEnemies = 0;
			enemies[0].position = { 1280,(float) GetRandomValue(0+150, GetScreenHeight()-150) };
			for (int i = 0; i < maxEnemies; i++)
			{
				enemies[i].active = true;
				enemies[i].speed = { 400.0f * 6 / maxEnemies,0.0f };
				if (i != 0)
				{
					enemies[i].position = { enemies[i - 1].position.x + enemiesDistance,(float)GetRandomValue(0 +150, GetScreenHeight()-150) };
				}
				enemies[i].tag = i;
				//-----------------------------------

				enemyTexture = LoadTexture("res/assets/enemies.png");
				spriteES = LoadTexture("res/assets/shoot3.png");

				enemies[i].rec.x = enemies[i].position.x;
				enemies[i].rec.y = enemies[i].position.y;
				enemies[i].rec.width = enemyTexture.width;
				enemies[i].rec.height = enemyTexture.height;

				enemies[i].timer = 0.0f;
				enemies[i].shoot.active = false;
				enemies[i].shoot.position.x = enemies[i].rec.x;
				enemies[i].shoot.position.y = enemies[i].rec.y;
				enemies[i].shoot.rotation = enemies[i].angle;
				enemies[i].shootSpeed = 900.0f;
				enemies[i].shoot.radius = 3;
			}
		}
		static float lastEnemyX;

		static void checkLastEnemy()
		{
			lastEnemyX = 0;
			for (int k = 0; k < maxEnemies; k++)
			{
				if (enemies[k].position.x > lastEnemyX)
				{
					lastEnemyX = enemies[k].position.x;
				}
			}
		}

		static void updateEnemyShoots()
		{
			int angleFixed = 90;
			for (int i = 0; i < maxEnemies; i++)
			{
				if (enemies[i].active)
				{
					enemies[i].shoot.rotation = enemies[i].angle + angleFixed;

					if (!enemies[i].shoot.active)
					{
						enemies[i].timer += GetFrameTime();
					}
					if (enemies[i].timer > 1.5f)
					{
						enemies[i].shoot.active = true;
						enemies[i].timer = 0;
					}
					if (!enemies[i].shoot.active)
					{
						enemies[i].shootDirection = enemies[i].shoot.rotation;
						enemies[i].shoot.position.x = enemies[i].rec.x + enemyTexture.width / 2;
						enemies[i].shoot.position.y = enemies[i].rec.y + enemyTexture.height / 2;
					}
					else
					{
						enemies[i].shoot.position.x += sin(enemies[i].shootDirection*DEG2RAD)*enemies[i].shootSpeed*GetFrameTime();
						enemies[i].shoot.position.y -= cos(enemies[i].shootDirection*DEG2RAD)*enemies[i].shootSpeed*GetFrameTime();
					}
					if (enemies[i].shoot.position.x<0 || enemies[i].shoot.position.x>GetScreenWidth() ||
						enemies[i].shoot.position.y<0 || enemies[i].shoot.position.y>GetScreenHeight())
					{
						enemies[i].shoot.active = false;
					}

					if (CheckCollisionCircles({ ship.collider.x, ship.collider.y }, ship.collider.z, enemies[i].shoot.position, enemies[i].shoot.radius))
					{
						gameOver = true;
						victory = false;
						currentScreen = GameOver;
					}
				}
				
			}
		}

		static void updateEnemies()
		{
			for (int i = 0; i < maxEnemies; i++)
			{

				if (enemies[i].active)
				{
					enemies[i].position.x -= enemies[i].speed.x*GetFrameTime();
					if (enemies[i].position.x < -enemies[i].rec.width * 2)
					{
						enemies[i].position.x = 1280;
					}
					//--------------------------------------
					enemies[i].rec.x = enemies[i].position.x;
					enemies[i].rec.y = enemies[i].position.y;

					vDir.x = ship.position.x - enemies[i].rec.x;

					vDir.y = ship.position.y - enemies[i].rec.y;

					enemies[i].angle = atan2(vDir.y, vDir.x)*RAD2DEG;
					//----------------------------------------


				}
			}

			updateEnemyShoots();

			for (int i = 0; i < shipMaxShoots; i++)
			{
				if ((shoot[i].active))                 
				{
					for (int a = 0; a < maxEnemies; a++)
					{
						if (enemies[a].active && CheckCollisionCircleRec(shoot[i].position, shoot[i].radius,enemies[a].rec))
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
							enemies[a].active = false;
							destroyedEnemies++;
						}
					}
				}
			}
		}

		static void drawEnemies()
		{
			for (int i = 0; i < maxEnemies; i++)
			{
				if (enemies[i].active)
				{
					DrawTexture(spriteES, enemies[i].shoot.position.x- enemies[i].shoot.radius*3,
						enemies[i].shoot.position.y- enemies[i].shoot.radius*3, WHITE);
					//DrawCircle(enemies[i].shoot.position.x, enemies[i].shoot.position.y, enemies[i].shoot.radius, WHITE);
					DrawTexture(enemyTexture, enemies[i].position.x, enemies[i].position.y, WHITE);
					/*DrawTexturePro(enemyTexture, { 0.0f,0.0f,(float)enemyTexture.width / 2,(float)enemyTexture.height },
						{ enemies[i].position.x,enemies[i].position.y,(float)enemyTexture.width / 2,(float)enemyTexture.height },
						{ (float)enemyTexture.width / 4,(float)enemyTexture.height / 2 }, 0.0f, WHITE);*/
					/*DrawTexturePro(spriteFM, { (float)spriteFM.width / 2,0.0f,(float)spriteFM.width / 2,(float)spriteFM.height },
						{ floorMeteors[i].position.x,floorMeteors[i].position.y,(float)spriteFM.width / 4,(float)spriteFM.height / 2 },
						{ 0.0f, (float)spriteFM.height / 4 }, canons[i].angle, WHITE);*/
				}
			}
		}


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
				canons[i].alive = true;

				spriteFM = LoadTexture("res/assets/machines2.png");
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
						canons[i].alive = false;
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
				if (floorMeteors[i].active)
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
				else
				{
					canons[i].shoot.position = { 0,0 };
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
					DrawCircle(canons[i].shoot.position.x, canons[i].shoot.position.y, canons[i].shoot.radius, WHITE);
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
			initEnemies();
			initBoss();
			initCoin();
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
			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount == maxFloorMeteors)
			{
				updateEnemies();
			}

			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount == maxFloorMeteors && destroyedEnemies == maxEnemies)
			{
				updateBoss();
			}
			

			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount==maxFloorMeteors && destroyedEnemies==maxEnemies && destroyedBoss==maxBoss)
			{
				victory = true;
				gameOver = true;
				currentScreen = GameOver;
			}
			updateCoin();
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
			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount == maxFloorMeteors)
			{
				drawEnemies();
			}

			if (destroyedMeteorsCount == maxBigMeteors && destroyedFloorMeteorsCount == maxFloorMeteors && destroyedEnemies == maxEnemies)
			{
				drawBoss();
			}
			drawCoin();
		}

		void UnloadMeteors()
		{
			UnloadSound(explosionSound);
			UnloadTexture(meteorTexture);
			UnloadImage(meteorImage);
			UnloadTexture(spriteFM);
			UnloadTexture(spriteFMS);
			UnloadTexture(enemyTexture);
			UnloadTexture(bossTexture);
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
			initEnemies();
			initBoss();
			initCoin();
		}
	}
}