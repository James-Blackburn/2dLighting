#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>
#define PI 3.14159265359

struct Tile{
    int x;
    int y;
    int isOn = 1;
    int brightness = 255;
};

struct Zombie{
    int x;
    int y;
    int isOn;
    int brightness;
    float angle;
};

struct Player{
    int x,y;
    int move_x = 0;
    int move_y = 0;
    int speed = 2;
    float angle = 0;
};

struct Chunk{
    std::vector<Tile*> tiles;
    int x,y;
    void calculatePosition(){
        Tile* middleTile = tiles[static_cast<int>(tiles.size()/2)];
        this->x = middleTile->x;
        this->y = middleTile->y;
    }
};


class Game{
    private:
        sf::RenderWindow window;
        static const int WINDOW_X = 1920;
        static const int WINDOW_Y = 1080;
        static const int BLOCK_SIZE = 8;
        static const int FONT_SIZE = 20;
        static const int TILE_NUM = (WINDOW_X/BLOCK_SIZE)*(WINDOW_Y/BLOCK_SIZE);
        static const int FPS = 60;

        std::vector<Tile*> tiles = std::vector<Tile*>(TILE_NUM);
        std::vector<Chunk*> chunks;
        std::vector<Zombie*> zombies;
        Player* player = new Player;
        sf::Color woodColour;
        sf::Color zombieColour;

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
                Tile* newTile = new Tile;
                newTile->x = x;
                newTile->y = y;
                tiles[i] = newTile;

                if (!(rand()%512)){
                    Zombie* newZomb = new Zombie;
                    newZomb->x = x;
                    newZomb->y = y;
                    zombies.push_back(newZomb);
                }

                x += BLOCK_SIZE;
            }
            player->x = WINDOW_X/2;
            player->y = WINDOW_Y/2;
        }

        void getFps(){
            double currentTime = clock.restart().asSeconds();
            int fps = (1.f / currentTime);
            std::string str = std::to_string(fps);
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
                    for (Zombie* zomb : zombies) delete zomb;
                    for (Tile* tile : tiles) delete tile;
                    window.close();
                } else if (event.type == sf::Event::KeyPressed){
                    // Key Pressed
                    if (event.key.code == sf::Keyboard::Escape){
                        window.close();
                    } else if (event.key.code == sf::Keyboard::W){
                        player->angle = 0;
                        player->move_y = -player->speed;
                    } else if (event.key.code == sf::Keyboard::S){
                        player->angle = 180;
                        player->move_y = player->speed;
                    } else if (event.key.code == sf::Keyboard::A){
                        player->angle = -90;
                        player->move_x = -player->speed;
                    } else if(event.key.code == sf::Keyboard::D){
                        player->angle = 90;
                        player->move_x = player->speed;
                    }
                } else if (event.type == sf::Event::KeyReleased){
                    // Key Released
                    player->move_x = 0;
                    player->move_y = 0;
                }
            }
        }

        template <class T>
        void applyLighting(T &vect){
            for (int i=0; i<vect.size(); i++){
                auto* object = vect[i];
                object->isOn = 1;
                double dist_y = object->y-player->y;
                double dist_x = object->x-player->x;
                double distance = (int)sqrt((dist_x*dist_x)+(dist_y*dist_y))/8;
                if (distance > 16){
                    object->isOn = 0;
                } else{
                    if (distance == 16){
                        object->brightness = 255-(distance*BLOCK_SIZE*2)+1;
                    } else{
                        object->brightness = 255-(distance*BLOCK_SIZE*2);
                    }
                }
            }
        }

        void updateZombies(){
            // This method updates zombies
            // It rotates and moves them towards the player
            for (Zombie* zombie : zombies){
                //Rotate zombie
                float adjacent = zombie->x - player->x;
                float opposite = zombie->y - player->y;
                float angle = atan2(opposite,adjacent)*180/PI;
                zombie->angle = angle-90;
                //Move the Zombie
                if (zombie->x < player->x){
                    zombie->x += rand()%2;
                } else if (zombie->x > player->x){
                    zombie->x -= rand()%2;
                }

                if (zombie->y < player->y){
                    zombie->y += rand()%2;
                } else if (zombie->y > player->y){
                    zombie->y -= rand()%2;
                }

                if (rand()%6 == 2){
                    zombie->x += (-2+(rand()%5));
                    zombie->y += (-2+(rand()%5));
                }

            }
        }

        void update(){
            // This method updates all of the sprites on screen
            updateZombies();
            applyLighting(tiles);
            applyLighting(zombies);
            player->x += player->move_x;
            player->y += player->move_y;
            if (player->x > WINDOW_X) player->x = WINDOW_X;
            else if (player->x < 0) player->x = 0;
            if (player->y > WINDOW_Y) player->y = WINDOW_Y;
            else if (player->y < 0) player->y = 0;
        }

        void display(){
            window.clear();
            for (Tile* tile : tiles){
                if (tile->isOn){
                    woodSprite.setPosition(tile->x,tile->y);
                    woodSprite.setColor(sf::Color(tile->brightness
                                                  ,tile->brightness
                                                  ,tile->brightness));
                    window.draw(woodSprite);
                }
            }

            for (Zombie* zombie : zombies){
                if (zombie->isOn){
                    zombieSprite.setPosition(zombie->x,zombie->y);
                    zombieShadowSprite.setPosition(zombie->x-5,zombie->y-5);
                    zombieSprite.setRotation(zombie->angle);
                    zombieShadowSprite.setRotation(zombie->angle);
                    zombieSprite.setColor(sf::Color(zombie->brightness
                                                    ,zombie->brightness
                                                    ,zombie->brightness));
                    window.draw(zombieShadowSprite);
                    window.draw(zombieSprite);
                }
            }
            playerSprite.setRotation(player->angle);
            playerSprite.setPosition(player->x,player->y);
            window.draw(playerSprite);
            window.draw(text);
            window.display();
        }
    public:
        Game() : window(sf::VideoMode(WINDOW_X,WINDOW_Y),"Lighting Test"){
            window.setFramerateLimit(FPS);
            window.setVerticalSyncEnabled(true);
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
