#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <vector>

using namespace std;

class SnowFlake
{
	float angle, radius, speed, direction;
	sf::ConvexShape shape;
public:
	SnowFlake()
	{
		angle = 0.0f;
		radius = 120.0f;
		speed = 2.5f;
		direction = 1.0f;

		const int pointsCount = 12;
		shape.setPointCount(pointsCount);

		float maxRadius = 8.f;
		float minRadius = 4.f;

		const float PI = 3.14159265f;
		float angleStep = (2 * PI) / pointsCount;

		for (int i = 0; i < pointsCount; ++i)
		{
			float currentAngle = i * angleStep;

			float currentRadius = (i % 2 == 0) ? maxRadius : minRadius;

			float vertexX = currentRadius * std::cos(currentAngle);
			float vertexY = currentRadius * std::sin(currentAngle);

			shape.setPoint(i, sf::Vector2f(vertexX, vertexY));
		}

		shape.setFillColor(sf::Color(0, 255, 255, 200));
		shape.setOutlineThickness(2.f);
		shape.setOutlineColor(sf::Color::White);
	}
	void update(float dt, sf::Vector2f center)
	{
		angle += direction * speed * dt;
		float x = center.x + radius * std::cos(angle);
		float y = center.y + radius * std::sin(angle);
		shape.setPosition(x, y);
	}
	void changeRadius(float amount)
	{
		radius += amount;

		if (radius < 20.f) radius = 20.f;
		if (radius > 350.f) radius = 350.f;

	}
	void invertDirection()
	{
			direction *= -1;
	}
	void draw(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	sf::FloatRect getBounds() 
	{ 
		return shape.getGlobalBounds(); 
	}
	void modifySpeed(float factor)
	{
		speed += factor;
		if (speed < 1.0f) speed = 1.0f;
		if (speed > 6.0f) speed = 6.0f;
	}
};

class MainHero
{
	sf::Sprite sprite;

	float currentFrame;
	float animationSpeed;
	int totalFrames;
public:
	MainHero(const sf::Texture& texture)
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));

		sprite.setOrigin(32.f, 32.f);
		sprite.setPosition(640.f, 360.f);

		currentFrame = 0.f;
		animationSpeed = 3.f;
		totalFrames = 3;
	}
	void update(float dt, sf::Vector2f center)
	{
		sprite.setPosition(center);
		currentFrame += animationSpeed * dt;

		if (currentFrame >= totalFrames)
		{
			currentFrame = 0.f;
		}
		int frameX = static_cast<int>(currentFrame) * 64;
		sprite.setTextureRect(sf::IntRect(frameX, 0, 64, 64));
	}
	void draw(sf::RenderWindow& window)
	{
		window.draw(sprite);
	}
	sf::Vector2f getPosition() 
	{ 
		return sprite.getPosition();
	}
	sf::FloatRect getBounds()
	{
		return sprite.getGlobalBounds();
	}
};

class Snowman
{
	float speed;
	sf::Sprite sprite;

	float currentFrame;
	float animationSpeed; 
	int totalFrames;
public:
	Snowman(sf::Vector2f startPos, const sf::Texture& texture)
	{
		speed = 80.f;

		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));

		sprite.setOrigin(32.f, 32.f);
		sprite.setPosition(startPos);

		currentFrame = 0.f;
		animationSpeed = 6.f;
		totalFrames = 3;

	}
	void update(float dt, sf::Vector2f HeroPos)
	{
		float Dx = HeroPos.x - sprite.getPosition().x;
		float Dy = HeroPos.y - sprite.getPosition().y;

		float length = sqrt(Dx * Dx + Dy * Dy);
		if (length != 0)
		{
			float Ax = (Dx / length) * speed * dt;
			float Ay = (Dy / length) * speed * dt;
			sprite.move(Ax, Ay);
		}
		currentFrame += animationSpeed * dt;

		if (currentFrame >= totalFrames)
		{
			currentFrame = 0.f;
		}
		int frameX = static_cast<int>(currentFrame) * 64;
		sprite.setTextureRect(sf::IntRect(frameX, 0, 64, 64));

		if (Dx < 0) 
		{
			sprite.setScale(-1.f, 1.f);
		}
		else 
		{
			sprite.setScale(1.f, 1.f);
		}
	}
	void draw(sf::RenderWindow& window)
	{
		window.draw(sprite);
	}
	sf::FloatRect getBounds()
	{
		return sprite.getGlobalBounds();
	}
	sf::Vector2f getPosition()
	{
		return sprite.getPosition();
	}
};

class Booster
{
	sf::CircleShape shape;
	float type;

public:
	Booster(sf::Vector2f pos, int boosterType)
	{
		type = boosterType;
		shape.setRadius(10.f);
		shape.setOrigin(10.f, 10.f);
		shape.setPosition(pos);

		if (type == 1) {
			shape.setFillColor(sf::Color::Green); 
		}
		else {
			shape.setFillColor(sf::Color::Red);
		}
	}

	void draw(sf::RenderWindow& window)
	{
		window.draw(shape);
	}

	sf::FloatRect getBounds()
	{
		return shape.getGlobalBounds();
	}

	int getType() const { return type; }
};

enum class GameState {
	Intro,
	Action,
	GameOver
};

int main()
{
	system("cls"); //НЕТ ПОДДЕРЖКИ ЛИНУКС!


	srand(static_cast<unsigned int>(std::time(nullptr)));
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Snowman adventure");
	window.setFramerateLimit(120);

	SnowFlake Flake;
	float spawnTimer = 0.f;
	float spawnInterval = 2.f;
	vector<Snowman> army;

	vector<Booster> boosters;

	int score = 0;

	sf::Clock clock;
	sf::Texture HeroTexture;
	if (!HeroTexture.loadFromFile("hero.png")) {
		return -1;
	}
	MainHero Hero(HeroTexture);
	sf::Texture snowmanTexture;
	if (!snowmanTexture.loadFromFile("snowman.png")) {
		return -1;
	}

	army.push_back(Snowman(sf::Vector2f(100.f, 100.f), snowmanTexture));
	army.push_back(Snowman(sf::Vector2f(1100.f, 100.f), snowmanTexture));
	army.push_back(Snowman(sf::Vector2f(640.f, 50.f), snowmanTexture));

	snowmanTexture.setSmooth(false);
	HeroTexture.setSmooth(false);
	sf::Font font;
	if (!font.loadFromFile("EpilepsySansBold.ttf")) {
		return -1;
	}

	sf::RenderTexture pixelLightTexture;
	if (!pixelLightTexture.create(320, 180)) return -1;
	pixelLightTexture.setSmooth(false); 

	sf::Sprite pixelLightSprite;
	pixelLightSprite.setTexture(pixelLightTexture.getTexture());
	pixelLightSprite.setScale(4.f, 4.f);

	sf::CircleShape pixelLightShape;
	float pixelLightRadius = 45.f;
	pixelLightShape.setRadius(pixelLightRadius);
	pixelLightShape.setOrigin(pixelLightRadius, pixelLightRadius);

	pixelLightShape.setPosition(160.f, 90.f);

	pixelLightShape.setFillColor(sf::Color(255, 240, 150, 35));

	pixelLightShape.setOutlineThickness(300.f);
	pixelLightShape.setOutlineColor(sf::Color(15, 15, 20, 250));

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20.f, 20.f);

	sf::Text introText;
	introText.setFont(font);
	introText.setCharacterSize(40);
	introText.setFillColor(sf::Color::White);
	introText.setPosition(50.f, 100.f);
	introText.setString(
		L"Автор: Бурдин Данил, студент Н253Б.\n\n"
		L"Цель: истребление снеговиков.\n"
		L"Защищайтесь, используя снежинку.\n\n"
		L"Управление:\n"
		L"LMB - увеличить радиус снежинки.\n"
		L"RMB - уменьшить радиус снежинки.\n"
		L"SPACE - инвертировать направление снежинки.\n\n"
		L"Нажмите ENTER для начала..."
	);
	GameState state = GameState::Intro;

	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(50);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(440.f, 260.f);

	sf::SoundBuffer hitBuffer;
	if (!hitBuffer.loadFromFile("hit1.wav")) return -1;

	sf::SoundBuffer pickupBuffer;
	if (!pickupBuffer.loadFromFile("wobbleup2.wav")) return -1;

	sf::SoundBuffer GameOverBuffer;
	if (!GameOverBuffer.loadFromFile("fail.wav")) return -1;

	sf::Sound hitSound;
	hitSound.setBuffer(hitBuffer);
	hitSound.setVolume(50.f);

	sf::Sound pickupSound;
	pickupSound.setBuffer(pickupBuffer);
	pickupSound.setVolume(60.f);

	sf::Sound GameOverSound;
	GameOverSound.setBuffer(GameOverBuffer);
	GameOverSound.setVolume(60.f);

	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();

		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (state == GameState::Intro && event.key.code == sf::Keyboard::Enter)
				{
					state = GameState::Action;
				}
				if (state == GameState::Action && event.key.code == sf::Keyboard::Space)
				{
					Flake.invertDirection();
				}
				if (state == GameState::GameOver && event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
			}
		}

		if (state == GameState::Action)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				Flake.changeRadius(150 * dt);
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				Flake.changeRadius(-150 * dt);
			}

			Flake.update(dt, sf::Vector2f(640.f, 360.f));
			Hero.update(dt, sf::Vector2f(640.f, 360.f));

			for (auto it = boosters.begin(); it != boosters.end(); )
			{
				if (Flake.getBounds().intersects(it->getBounds()))
				{
					pickupSound.play();
					if (it->getType() == 1) {
						Flake.modifySpeed(0.8f);
					}
					else {
						Flake.modifySpeed(-0.8f);
					}
					it = boosters.erase(it);
				}
				else
				{
					++it;
				}
			}

			spawnTimer += dt;
			if (spawnTimer >= spawnInterval)
			{
				spawnTimer = 0.f;
				float randomAngle = (rand() % 360) * 3.14159265f / 180.f;
				float spawnDistance = 800.f;
				float spawnX = 640.f + spawnDistance * std::cos(randomAngle);
				float spawnY = 360.f + spawnDistance * std::sin(randomAngle);
				army.push_back(Snowman(sf::Vector2f(spawnX, spawnY), snowmanTexture));
			}

			for (auto it = army.begin(); it != army.end(); )
			{
				it->update(dt, sf::Vector2f(640.f, 360.f));

				if (Hero.getBounds().intersects(it->getBounds()))
				{
					GameOverSound.play();
					state = GameState::GameOver;
					army.clear();
					boosters.clear();
					break;
				}

				if (Flake.getBounds().intersects(it->getBounds()))
				{

					sf::Vector2f deadSnowmanPos = it->getPosition();
					hitSound.play();
					it = army.erase(it); 
					score += 10;

					if ((rand() % 100) < 30)
					{
						int randomType = rand() % 2;
						boosters.push_back(Booster(deadSnowmanPos, randomType));
					}
				}
				else
				{
					++it;
				}
			}
			scoreText.setString("Score: " + std::to_string(score));
		}
		scoreText.setString("Score: " + std::to_string(score));

		sf::RenderStates lightStates;
		lightStates.blendMode = sf::BlendMin; 

		pixelLightTexture.clear(sf::Color::Transparent);
		pixelLightTexture.draw(pixelLightShape);
		pixelLightTexture.display();

		window.clear(sf::Color::White);

		if (state == GameState::Intro)
		{
			window.clear(sf::Color(10, 10, 25));
			window.draw(introText);
		}
		else if (state == GameState::Action)
		{
			for (Snowman& snowman : army)
				snowman.draw(window);

			for (Booster& booster : boosters)
				booster.draw(window);

			Flake.draw(window);
			Hero.draw(window);

			window.draw(pixelLightSprite);

			scoreText.setFillColor(sf::Color(255, 255, 200));
			window.draw(scoreText);
		}
		else if (state == GameState::GameOver)
		{
			window.clear(sf::Color(10, 10, 25));
			gameOverText.setString("GAME OVER\nFinal Score: " + std::to_string(score) + "\nPress ESC to exit");
			window.draw(gameOverText);
		}

		window.display();
	}
	return 0;
}