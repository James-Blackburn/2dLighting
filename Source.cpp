#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>

/*
Copyright James Blackburn
10/09/2018
distributed under the GNU liscense
*/

struct Tile{
    int x;
    int y;
	// isOn defines if the sprite should be displayed to
	// the screen or not
    int isOn;
    int brightness;
};

struct Zombie{
    int x;
    int y;
    int isOn;
    int brightness;
	// angle reffers to the sprite angle 
	// in relation to the player
    float angle;
};

struct Player{
    int x;
    int y;
    int move_x = 0;
    int move_y = 0;
    int speed = 2;
};


class Game{
    private:
		// declaring class members
        sf::RenderWindow window;
        const int WINDOW_X = 1280;
        const int WINDOW_Y = 720;
        const int BLOCK_SIZE = 8;
        const int FONT_SIZE = 20;
        const int TILE_NUM = (WINDOW_X/BLOCK_SIZE)*(WINDOW_Y/BLOCK_SIZE);
        std::vector<Tile> tiles = std::vector<Tile>(TILE_NUM);
		// Not defining size of the vector zombies, as the size will be dynamic
        std::vector<Zombie> zombies;
        sf::Color woodColour;
        sf::Color zombieColour;

		// Textures are class members so that they do not 
		// Go out of range, and be destroyed
        sf::Texture woodTexture;
        sf::Texture playerTexture;
        sf::Texture zombieTexture;
        sf::Texture zombieShadowTexture;

        sf::Sprite woodSprite;
        sf::Sprite playerSprite;
        sf::Sprite zombieSprite;
        sf::Sprite zombieShadowSprite;

        sf::Clock clock;
        sf::Text text;
        sf::Font font;
        Player player;

        void loadFiles(){
            // This method loads and applies the textures to their corresponding sprites
            woodTexture.loadFromFile("wood.png");
            woodTexture.setSmooth(true);
            woodSprite.setTexture(woodTexture);
            woodColour = woodSprite.getColor();

            playerTexture.loadFromFile("player.png");
            playerTexture.setSmooth(true);
            playerSprite.setTexture(playerTexture);
            playerSprite.setOrigin(8, 8);

            zombieTexture.loadFromFile("zombie.png");
            zombieTexture.setSmooth(true);
            zombieSprite.setTexture(zombieTexture);
            zombieSprite.setOrigin(16, 16);
            zombieColour = zombieSprite.getColor();

            zombieShadowTexture.loadFromFile("zombieShadow.png");
            zombieShadowTexture.setSmooth(true);
            zombieShadowSprite.setTexture(zombieShadowTexture);
            zombieShadowSprite.setOrigin(16, 16);

            font.loadFromFile("agencyfb.ttf");
            text.setFont(font);
            text.setCharacterSize(FONT_SIZE);
        }

        void initLevel(){
            // This method creates the level
            int x = 0;
            int y = 0;
            for (int i=0; i<TILE_NUM; i++){
                if (x > WINDOW_X){
                    x = 0;
                    y += BLOCK_SIZE;
                }
                Tile newTile;
                newTile.x = x;
                newTile.y = y;
                tiles[i] = newTile;
				
                if (!(rand()%256)){
                    Zombie newZomb;
                    newZomb.x = x;
                    newZomb.y = y;
                    zombies.push_back(newZomb);
                }

                x += BLOCK_SIZE;
            }
            player.x = WINDOW_X/2;
            player.y = WINDOW_Y/2;
        }

        void getFps(){
			// This method updates the text for the fps counter
            double currentTime = clock.restart().asSeconds();
            int fps = (1.f / currentTime);

            char c[10];
            std::sprintf(c, "%d", fps);
            std::string string(c);
            sf::String str(string);
            text.setString("FPS: "+str);
        }

        void frame(){
            // This method is on control of what happens each frame
            while(window.isOpen()){
                handleEvents();
                update();
                getFps();
                display();
            }
        }

        void handleEvents(){
            // This method handles events each frame
            sf::Event event;
            while (window.pollEvent(event)){
                if (event.type == sf::Event::Closed){
                    // Window Closed
                    window.close();
                } else if (event.type == sf::Event::KeyPressed){
                    // Key Pressed
                    if (event.key.code == sf::Keyboard::Escape){
                        window.close();
                    } else if (event.key.code == sf::Keyboard::W){
                        player.move_y = -player.speed;
                    } else if (event.key.code == sf::Keyboard::S){
                        player.move_y = player.speed;
                    } else if (event.key.code == sf::Keyboard::A){
                        player.move_x = -player.speed;
                    } else if(event.key.code == sf::Keyboard::D){
                        player.move_x = player.speed;
                    }
                } else if (event.type == sf::Event::KeyReleased){
                    // Key Released
                    player.move_x = 0;
                    player.move_y = 0;
                }
            }
        }

		// Using templates as the value passed through the method will
		// not always be the same.
        template <class T>
        void applyLighting(T &vect){
			// This method changes how dark or light a sprite is
			// depending on the distance from the player
			// thus giving the illusion of a dynamic light source
            for (int i=0; i<vect.size(); i++){
                vect[i].isOn = 1;
                double dist_y = vect[i].y-player.y;
                double dist_x = vect[i].x-player.x;
                double distance = (int)sqrt((dist_x*dist_x)+(dist_y*dist_y))/8;
                if (distance > 16){
                    vect[i].isOn = 0;
                } else{
                    if (distance == 16){
                        vect[i].brightness = 255-(distance*BLOCK_SIZE*2)+1;
                    } else{
                        vect[i].brightness = 255-(distance*BLOCK_SIZE*2);
                    }
                }
            }
        }

        void updateZombies(){
            // This method updates zombies
            // It rotates and moves them towards the player
            for (int i=0; i<zombies.size(); i++){
                //Rotate zombie
                float adjacent = zombies[i].x - player.x;
                float opposite = zombies[i].y - player.y;
                float angle = atan2(opposite,adjacent)*180/3.14159265359;
                zombies[i].angle = angle-90;
                //Move the Zombie to the current position of the player
                if (zombies[i].x < player.x){
                    zombies[i].x += rand()%2;
                } else if (zombies[i].x > player.x){
                    zombies[i].x -= rand()%2;
                }

                if (zombies[i].y < player.y){
                    zombies[i].y += rand()%2;
                } else if (zombies[i].y > player.y){
                    zombies[i].y -= rand()%2;
                }

                if (rand()%6 == 2){
                    zombies[i].x += (-2+(rand()%5));
                    zombies[i].y += (-2+(rand()%5));
                }

            }
        }

        void update(){
            // This method updates all of the sprites on screen
            updateZombies();
            applyLighting(tiles);
            applyLighting(zombies);
            player.x += player.move_x;
            player.y += player.move_y;
            if (player.x > WINDOW_X) player.x = WINDOW_X;
            else if (player.x < 0) player.x = 0;
            if (player.y > WINDOW_Y) player.y = WINDOW_Y;
            else if (player.y < 0) player.y = 0;
        }

        void display(){
			// This method displays all sprites to the screen
			// dirty rects may be utilised in the future to 
			// only update the sprites that have changed - increasing performance
            window.clear();
            for (int i=0; i<tiles.size(); i++){
                if (tiles[i].isOn){
                    woodSprite.setPosition(tiles[i].x,tiles[i].y);
                    woodSprite.setColor(sf::Color(tiles[i].brightness
                                                  ,tiles[i].brightness
                                                  ,tiles[i].brightness));
                    window.draw(woodSprite);
                }
            }

            for (int i=0; i<zombies.size(); i++){
                if (zombies[i].isOn){
                    zombieSprite.setPosition(zombies[i].x,zombies[i].y);
                    zombieShadowSprite.setPosition(zombies[i].x-5,zombies[i].y-5);
                    zombieSprite.setRotation(zombies[i].angle);
                    zombieShadowSprite.setRotation(zombies[i].angle);
                    zombieSprite.setColor(sf::Color(zombies[i].brightness
                                                    ,zombies[i].brightness
                                                    ,zombies[i].brightness));
                    window.draw(zombieShadowSprite);
                    window.draw(zombieSprite);
                }
            }
            playerSprite.setPosition(player.x,player.y);
            window.draw(playerSprite);
            window.draw(text);
            window.display();
        }
    public:
        Game() : window(sf::VideoMode(1280,720),"Lighting Test"){
			//Constructor for the game class
            window.setFramerateLimit(60);
            loadFiles();
            initLevel();
            frame();
        }

};

int main()
{
    Game game;
    return 0;
}
