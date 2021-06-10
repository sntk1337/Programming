#include<SFML/Graphics.hpp>
#include"Levels.h"

using namespace sf;


int windowWidth = 1000;
int windowHight = 500;



bool goR, goL, goUp, goDown;
bool inMotionX;


float camPosX = 0, camPosY = 0;


class CHARACTER {

public:

	float dx, dy;
	float CharacterCurrentFrame;

	bool life;
	bool onGround;

	Texture texture;
	FloatRect rect;
	Sprite sprite;
	String name;

	CHARACTER(Image &image, String Name) {
		dx = dy = 0;
		CharacterCurrentFrame = 0;
		onGround = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
	}
};


class HERO :public CHARACTER {

public:
	
	HERO(Image &image, String Name) :CHARACTER(image, Name) {
		rect = FloatRect(3*32, 27*32, 53, 60);
		sprite.setTextureRect(IntRect(0, 59, 53, 60));
	}
	//ОБНОВЛЕНИЕ СОСТОЯНИЯ
	void update(float time) {
		


		rect.left += dx * time;
		Collision(0);
		if (!onGround)dy = dy + 0.0005 * time;//Высота прыжка

		rect.top += dy * time;
		onGround = false;
		Collision(1);

		//АНИМАЦИЯ

		CharacterCurrentFrame += 0.005*time; //Скорость анимации при движении
		if (CharacterCurrentFrame > 3) CharacterCurrentFrame -= 3;
		
			if (onGround == true) {
				if (dx > 0) // Анимация ДВИЖЕНИЕ ВПРАВО
				{
					if (int(CharacterCurrentFrame) == 0) sprite.setTextureRect(IntRect(0, 0, 45, 58)); // Кадр анимации 0
					if (int(CharacterCurrentFrame) == 1) sprite.setTextureRect(IntRect(46, 0, 46, 58)); // Кадр анимации 1
					if (int(CharacterCurrentFrame) == 2) sprite.setTextureRect(IntRect(93, 0, 55, 58)); // Кадр анимации 2
				}
				if (dx < 0) // Анимация ДВИЖЕНИЕ ВЛЕВО
				{
					if (int(CharacterCurrentFrame) == 0) sprite.setTextureRect(IntRect(46, 0, -45, 58)); // Кадр анимации 0
					if (int(CharacterCurrentFrame) == 1) sprite.setTextureRect(IntRect(93, 0, -46, 58)); // Кадр анимации 1
					if (int(CharacterCurrentFrame) == 2) sprite.setTextureRect(IntRect(148, 0, -55, 58)); // Кадр анимации 2
				}
			}

		if (dy > 0) {
			if (dx > 0 || dx == 0) sprite.setTextureRect(IntRect(50, 118, 53, 58));
			if (dx < 0) sprite.setTextureRect(IntRect(103, 118, -53, 58));
		}

		
		sprite.setPosition(rect.left - camPosX, rect.top - camPosY);

		dx = 0;
	}
	   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Collision(int XY) {
		for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
		{
			for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
			{
				if (TileMap[i][j] == '#' || TileMap[i][j] == 'g' 
					|| TileMap[i][j] == '|' || TileMap[i][j] == '-' || TileMap[i][j] == 'q' || TileMap[i][j] == 'x' 
					|| TileMap[i][j] == 'z' || TileMap[i][j] == '9' || TileMap[i][j] == '0')
				{
					if ((dx > 0) && (XY == 0)) rect.left = j * 32 - rect.width;
					if ((dx < 0) && (XY == 0)) rect.left = j * 32 + 32;
					if ((dy > 0) && (XY == 1)) {
						rect.top = i * 32 - rect.height;
						dy = 0;
						onGround = true;
					}
					if ((dy < 0) && (XY == 1)) {
						rect.top = i * 32 + 32;
						dy = 0;
					}
				}
				
				if (TileMap[i][j] == 'h'){
					dy = -0.2;
					sprite.setColor(Color::Red);
					life = 0;
				}

				if (TileMap[i][j] == 'c') { // если монета,
					TileMap[i][j] = '.';    // то изменить на пустоту
				}
			}
		}
	}
};


class ENEMY
{

public:
	float dx, dy;
	FloatRect rect;
	Sprite sprite;
	float currentFrame;
	bool life;

	void set(Texture &image, int x, int y, int type)
	{
		sprite.setTexture(image);
		rect = FloatRect(x, y, 61, 43);

		dx = 0.01;
		currentFrame = 0;
		life = true;
	}

	void update(float time)
	{
		rect.left += dx * time;

		Collision();

		if(dx<0) sprite.setTextureRect(IntRect(160, 12, 61, 43));
		if(dx>0) sprite.setTextureRect(IntRect(221, 12, -61, 43));

		sprite.setPosition(rect.left - camPosX, rect.top - camPosY);
	}


	void Collision()
	{
		for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
			for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
				if ((TileMap[i][j] == '#' || TileMap[i][j] == 'g' ||  TileMap[i][j] == 'f' 
					|| TileMap[i][j] == '/'))
				{
					if (dx > 0)
					{
						rect.left = j * 32 - rect.width; dx *= -1;
					}
					else if (dx < 0)
					{
						rect.left = j * 32 + 61;  dx *= -1;
					}
				}
	}
};


bool startGame() {

	RenderWindow window(VideoMode(windowWidth, windowHight), "Game v0.1"); // Размер окна
	window.setFramerateLimit(60); // FPS
	// Создаем героя
	Image heroImage; //  создаем текстуру героя
	heroImage.loadFromFile("Images/HeroSetV1.2.png"); // накладываем на текстуру изображение
	Texture heroTexture;
	heroTexture.loadFromImage(heroImage);

	/*Image enemy;
	enemy.loadFromFile("Images/SKenemy.png");*/

	// Создаем карту
	Image map_image; // Объект изображения для карты
	map_image.loadFromFile("Images/TilesV1.png"); // Загружаем файл с картой
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	// Создаем фон

	Texture backgroundCity;
	backgroundCity.loadFromFile("Images/Background.png");
	Sprite s_backgroundCity;
	s_backgroundCity.setTexture(backgroundCity);

	float CharacterCurrentFrame = 0;


	Clock clock;

	HERO Max(heroImage, "Max"); // создаем персонажа с именем Макс, передаем текстуру

	ENEMY Enemy1;
	Enemy1.set(heroTexture, 47 * 32, 24 * 32, 1);
	ENEMY Enemy2;
	Enemy2.set(heroTexture, 61 * 32, 23 * 32, 1);
	ENEMY Enemy3;
	Enemy3.set(heroTexture, 75 * 32, 23 * 32, 1);
	ENEMY Enemy4;
	Enemy4.set(heroTexture, 106 * 32, 23 * 32, 1);
	ENEMY Enemy5;
	Enemy5.set(heroTexture, 129 * 32, 23 * 32, 1);
	ENEMY Enemy6;
	Enemy6.set(heroTexture, 159 * 32, 23 * 32, 2);
	ENEMY Enemy7;
	Enemy7.set(heroTexture, 189 * 32, 24 * 32, 2);

	RectangleShape rectangle;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		time = time / 700;


		if ((Keyboard::isKeyPressed(Keyboard::Left)) || (Keyboard::isKeyPressed(Keyboard::Right)) || (Keyboard::isKeyPressed(Keyboard::Up)) || (Keyboard::isKeyPressed(Keyboard::A)) || (Keyboard::isKeyPressed(Keyboard::D)) || (Keyboard::isKeyPressed(Keyboard::W)))
		{
			if (Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A))) goL = true;
			if (Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D))) goR = true;
			if (Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W))) goUp = true;
		}
		else if (Max.onGround) {
			Max.sprite.setTextureRect(IntRect(0, 59, 51, 58));
		}


		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

		}


		/////////////////////////////////////////////////////

		if (goR) Max.dx = 0.2;  goR = false; inMotionX = false;

		if (goL) Max.dx = -0.2; goL = false; inMotionX = false;

		if (goUp) {

			if (Max.onGround)
			{
				Max.dy = -0.4;
				Max.onGround = false;
				if (Max.dx > 0 || Max.dx == 0) Max.sprite.setTextureRect(IntRect(0, 118, 49, 58));
				if (Max.dx < 0) Max.sprite.setTextureRect(IntRect(49, 118, -49, 58));
			}

			goUp = false;
			inMotionX = false;
		}

		////////////////////////////////////////////////////


		Max.update(time);
		Enemy1.update(time);
		Enemy2.update(time);
		Enemy3.update(time);
		Enemy4.update(time);
		Enemy5.update(time);
		Enemy6.update(time);
		Enemy7.update(time);



		if (Max.rect.intersects(Enemy1.rect))
		{
			if (Enemy1.life)
			{
				if (Max.dy > 0)
				{
					Enemy1.dx = 0;
					Max.dy = -0.2;
					Enemy1.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = false;
				}
			}
		}
		if (Max.rect.intersects(Enemy2.rect))
		{
			if (Enemy2.life)
			{
				if (Max.dy > 0)
				{
					Enemy2.dx = 0;
					Max.dy = -0.2;
					Enemy2.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = 0;
				}
			}
		}
		if (Max.rect.intersects(Enemy3.rect))
		{
			if (Enemy3.life)
			{
				if (Max.dy > 0)
				{
					Enemy3.dx = 0;
					Max.dy = -0.2;
					Enemy3.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = 0;
				}
			}
		}
		if (Max.rect.intersects(Enemy4.rect))
		{
			if (Enemy4.life)
			{
				if (Max.dy > 0)
				{
					Enemy4.dx = 0;
					Max.dy = -0.2;
					Enemy4.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = 0;
				}
			}
		}
		if (Max.rect.intersects(Enemy5.rect))
		{
			if (Enemy5.life)
			{
				if (Max.dy > 0)
				{
					Enemy5.dx = 0;
					Max.dy = -0.2;
					Enemy5.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = 0;
				}
			}
		}
		if (Max.rect.intersects(Enemy6.rect))
		{
			if (Enemy6.life)
			{
				if (Max.dy > 0)
				{
					Enemy6.dx = 0;
					Max.dy = -0.2;
					Enemy6.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = false;
				}
			}
		}
		if (Max.rect.intersects(Enemy7.rect))
		{
			if (Enemy7.life)
			{
				if (Max.dy > 0)
				{
					Enemy7.dx = 0;
					Max.dy = -0.2;
					Enemy7.life = false;
				}
				else {
					Max.sprite.setColor(Color::Red);
					Max.life = 0;
				}
			}
		}

		if (Max.rect.left > windowWidth / 2) camPosX = Max.rect.left - windowWidth / 2;
		if (Max.rect.top > windowHight / 2) camPosY = Max.rect.top - windowHight/2;
		


		if (Keyboard::isKeyPressed(Keyboard::Tab)) { return true; } // при нажатии Tab игра перезапускается
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; } // при нажатии Escape окно закрывается

		window.clear();

		window.draw(s_backgroundCity);

		/////////////////////////////Рисуем карту/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
		{
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == '.')  continue;
				if (TileMap[i][j] == '|')  s_map.setTextureRect(IntRect(488, 0, 32, 32));
				if (TileMap[i][j] == '/')  s_map.setTextureRect(IntRect(488, 0, 32, 32));
				if (TileMap[i][j] == 'g')  s_map.setTextureRect(IntRect(128, 0, 32, 32));//земля
				if (TileMap[i][j] == 'p')  s_map.setTextureRect(IntRect(160, 0, 32, 32));//земля край справа
				if (TileMap[i][j] == 'o')  s_map.setTextureRect(IntRect(96, 0, 32, 32));//земля край слева
				if ((TileMap[i][j] == '#')) s_map.setTextureRect(IntRect(0, 0, 32, 32));//поверхность земли
				if ((TileMap[i][j] == 'q')) s_map.setTextureRect(IntRect(192, 32, 32, 32)); // темная земля 1
				if ((TileMap[i][j] == 'w')) s_map.setTextureRect(IntRect(96, 128, 32, 32)); // темная земля 2
				if ((TileMap[i][j] == 'e')) s_map.setTextureRect(IntRect(128, 128, 32, 32)); // темная земля 3
				if ((TileMap[i][j] == '1')) s_map.setTextureRect(IntRect(33, 160, 32, 32)); // облако 1
				if ((TileMap[i][j] == '2')) s_map.setTextureRect(IntRect(0, 160, 32, 32)); // облако 2
				if ((TileMap[i][j] == 'k')) s_map.setTextureRect(IntRect(128, 32, 32, 32)); // край поверхности под блоком, смотрит влево
				if ((TileMap[i][j] == 'l')) s_map.setTextureRect(IntRect(160, 32, -31, 32)); // край поверхности под блоком, смотрит вправо
				if ((TileMap[i][j] == 'x')) s_map.setTextureRect(IntRect(32, 0, 32, 32)); // верхний крайний блок поверхности (справа)
				if ((TileMap[i][j] == 'z')) s_map.setTextureRect(IntRect(64, 0, 32, 32)); // верхний крайний блок поверхности (слева)
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(255, 32, -32, 32)); // верхний крайний блок поверхности (справа) подземелье
				if ((TileMap[i][j] == '9')) s_map.setTextureRect(IntRect(224, 32, 32, 32)); // верхний крайний блок поверхности (слева) подземелье
				if ((TileMap[i][j] == 'c')) s_map.setTextureRect(IntRect(256, 0, 32, 32)); // монета
				if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(256, 33, 29, 129)); // фонарь
				if ((TileMap[i][j] == '-')) s_map.setTextureRect(IntRect(288, 0, 32, 32)); // платформа
				if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(288, 32, 32, 32)); // шипы

				s_map.setPosition(j * 32 - camPosX, i * 32 - camPosY);//задаем блоку позицию

				window.draw(s_map);//рисуем блок

			}
		}
		//////////////////////////////////////////////////////////////

		window.draw(Max.sprite);

		if (Enemy1.life) window.draw(Enemy1.sprite);
		if (Enemy2.life) window.draw(Enemy2.sprite);
		if (Enemy3.life) window.draw(Enemy3.sprite);
		if (Enemy4.life) window.draw(Enemy4.sprite);
		if (Enemy5.life) window.draw(Enemy5.sprite);
		if (Enemy6.life) window.draw(Enemy6.sprite);
		if (Enemy7.life) window.draw(Enemy7.sprite);

		window.display();

		if (Max.life == 0) return 1;
	}

	return 0;
}

void gameRunning() { // функция перезапуска игры
	if ( startGame() ) {
		camPosX = 0;
		camPosY = 0;
		gameRunning(); 
	}
}

int main()
{
	gameRunning();
}

