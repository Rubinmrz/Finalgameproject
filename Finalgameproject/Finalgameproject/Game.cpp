#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <sstream>

//constants it defines tile size and maze dimension
const int ts=60;
const int mw=10;
const int mh=10;
const int ww=ts*mw;
const int wh= ts*mh;

//map of the maze a 2d array
int maze[mh][mw]=
{
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,0,1,0,1,0,1},
    {1,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

//mainmenu to define diffrent game screen
enum GameMenu
{ 
    MAIN_MENU,PLAYING,GAME_OVER,WIN 
};

//walls 
bool Wall(int x,int y)
{
    int gridX=x/ts;
    int gridY=y/ts;
    if(gridX<0||gridX>=mw||gridY<0||gridY>=mh)return true;
    return maze[gridY][gridX]==1;
}

//class made for firing bullets by the game bots
class Bullet
{
public:
    sf::RectangleShape shape;
    sf::Vector2f direction;
    float s;

    Bullet(float x,float y,sf::Vector2f dir) 
    {
        shape.setSize(sf::Vector2f(10.f,10.f));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(x,y);
        direction=dir;
        s=4.0f;
    }

    void update() 
    {
        shape.move(direction*s);
    }

    void draw(sf::RenderWindow&window) 
    {
        window.draw(shape);
    }
};

// Bot Class to handle position, drawing and sooting
class Bot 
{
public:
    sf::RectangleShape shape;
    sf::Vector2f position;
    float shootCooldown;
    float shootTimer;

    Bot(float x,float y) 
    {
        shape.setSize(sf::Vector2f(ts-4,ts-4));
        shape.setFillColor(sf::Color::Red);
        position=sf::Vector2f(x,y);
        shape.setPosition(position);
        shootCooldown=3.0f;
        shootTimer=0.0f;
    }

    void shoot(std::vector<Bullet>&bullets,sf::Vector2f playerPos,float deltaTime) 
    {
        shootTimer+=deltaTime;
        if(shootTimer>=shootCooldown)
        {
            sf::Vector2f direction=playerPos-position;
            float length=std::sqrt(direction.x*direction.x+direction.y*direction.y);
            if(length!=0) direction/=length;
            bullets.push_back(Bullet(position.x+ts/2,position.y+ts/2,direction));
            shootTimer=0.0f;
        }
    }

    void draw(sf::RenderWindow&window) 
    {
        window.draw(shape);
    }
};

//draws maze wall tiles using the maze 2d array
void drawMaze(sf::RenderWindow&window,sf::RectangleShape&wall) 
{
    for(int i=0;i<mh;i++) 
    {
        for (int j=0;j<mh;j++) 
        {
            if (maze[i][j]==1)
            {
                wall.setPosition(j*ts,i*ts);
                window.draw(wall);
            }
        }
    }
}

//display level death or alive
void drawHUD(sf::RenderWindow&window,sf::Font& font,int level,bool alive)
{
    sf::Text levelText("Level:"+std::to_string(level),font,20);
    levelText.setPosition(10,10);
    window.draw(levelText);

    sf::Text aliveText(alive? "Alive":"Dead",font,20);
    aliveText.setPosition(10,70);
    window.draw(aliveText);
}

//clear the bot and add new in the new level
void loadLevel(std::vector<Bot>& bots,std::vector<Bullet>& bullets,int level) 
{
    bots.clear();
    bullets.clear();
    if(level==1) 
    {
        bots.push_back(Bot(300.f,300.f));
    }
    else if(level==2) 
    {
        bots.push_back(Bot(120.f,420.f));
        bots.push_back(Bot(420.f,240.f));
    }
    else if(level==3) 
    {
        bots.push_back(Bot(240.f,180.f));
        bots.push_back(Bot(420.f,420.f));
        bots.push_back(Bot(180.f,240.f));
    }
}

//now the main code to handel windows and all game logic
int main() 
{
    //game setup
    sf::RenderWindow window(sf::VideoMode(ww,wh),"KIllbot Maze");
    window.setFramerateLimit(60);

    sf::RectangleShape player(sf::Vector2f(ts-4,ts-4));
    player.setFillColor(sf::Color::Green);

    sf::RectangleShape wall(sf::Vector2f(ts,ts));
    wall.setFillColor(sf::Color::White);

    sf::Font font;

    if(!font.loadFromFile("arial.ttf"))
        return 1;

    sf::Text mainMenuText("Maar TYO bot LAI\n\nPress Y to Start\nPress E to Exit", font, 28);
    mainMenuText.setPosition(100, 180);

    sf::Text gameOverText("You Died!\n\nPress Y to Restart\nPress E to Exit",font,28);
    gameOverText.setPosition(100,180);

    sf::Text winText("You Win!\n\nPress Y to Play Again\nPress E to Exit",font,28);
    winText.setPosition(100,180);


    //game main menu
    sf::Clock clock;
    GameMenu state=MAIN_MENU;
    int level=1;
    bool isAlive=true;

    std::vector<Bot>bots;
    std::vector<Bullet>bullets;

    while(window.isOpen()) 
    {
        sf::Event event;
        while(window.pollEvent(event)) 
        {
            if(event.type==sf::Event::Closed)window.close();
        }

        window.clear();

        if(state==MAIN_MENU) 
        {
            window.draw(mainMenuText);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) 
            {
                state=PLAYING;
                player.setPosition(60,60);
                loadLevel(bots,bullets,level);
                isAlive=true;
                clock.restart();
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) 
            {
                window.close();
            }
        }

        else if(state==GAME_OVER) 
        {
            window.draw(gameOverText);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) 
            {
                level=1;
                player.setPosition(60,60);
                loadLevel(bots,bullets,level);
                isAlive=true;
                state=PLAYING;
                clock.restart();
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) 
            {
                window.close();
            }
        }

        else if(state==WIN) 
        {
            window.draw(winText);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) 
            {
                level=1;
                loadLevel(bots,bullets,level);
                player.setPosition(60,60);
                isAlive=true;
                state=PLAYING;
                clock.restart();
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) 
            {
                window.close();
            }
        }

        else if(state==PLAYING) 
        {
            float deltaTime=clock.restart().asSeconds();
            sf::Vector2f pos=player.getPosition();

            if(isAlive) 
            {
                //for player to move using WASD
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)&&
                    !Wall(pos.x+2,pos.y-4)&&
                    !Wall(pos.x+ts-6,pos.y-4))
                    player.move(0.f,-4.f);

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)&&
                    !Wall(pos.x+2,pos.y+ts-4+4)&&
                    !Wall(pos.x+ts-6,pos.y+ts-4+4))
                    player.move(0.f,4.f);

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)&&
                    !Wall(pos.x-4,pos.y+2)&&
                    !Wall(pos.x-4,pos.y+ts-6))
                    player.move(-4.f,0.f);

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)&&
                    !Wall(pos.x+ts-4+4,pos.y+2)&&
                    !Wall(pos.x+ts-4+4,pos.y+ts-6))
                    player.move(4.f,0.f);

                for(auto&bot:bots)
                    bot.shoot(bullets,player.getPosition(),deltaTime);

                for(auto& bullet:bullets)
                    bullet.update();

                for(auto& bullet:bullets) 
                {
                    if(bullet.shape.getGlobalBounds().intersects(player.getGlobalBounds())) 
                    {
                        isAlive=false;
                        state=GAME_OVER;
                    }
                }

                for(auto it=bots.begin();it!=bots.end();) 
                {
                    if(player.getGlobalBounds().intersects(it->shape.getGlobalBounds())) 
                    {
                        it=bots.erase(it);
                    }
                    else 
                    {
                        ++it;
                    }
                }

                //check all the levels are completed or not 
                if(bots.empty()) 
                {
                    level++;
                    if(level>3) 
                    {
                        state=WIN;
                    }
                    else 
                    {
                        player.setPosition(60,60);
                        loadLevel(bots,bullets,level);
                    }
                }
            }

            drawMaze(window,wall);
            drawHUD(window,font,level,isAlive);

            if(isAlive)window.draw(player);

            for(auto&bot:bots) bot.draw(window);

            for(auto&bullet:bullets)bullet.draw(window);
        }

        window.display();
    }

    return 0;
}
