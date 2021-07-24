#include "engine.h" // class's header file

#include <Windows.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>

// class constructor
Engine::Engine()
{
	ship = new Ship();
	noProjectiles = 0;
	noAsteroids = 6;
	for (int i = 0; i < noAsteroids; i++)
	{
        asteroids[i] = new Asteroid();
    }
    
    lives = 3;
    leftPressed = false;
    rightPressed = false;
    accelerationPressed = false;
    firePressed = false;
    projectileCooldown = 0;
    gameOver = false;
    
    srand(time(0));
}

// class destructor
Engine::~Engine()
{
	// insert your code here
}

// This function is called every frame. The time from the last frame to
// the current frame is stored in `delta`.
void Engine::update(double delta)
{
    // First get keyboard input
    getInput();
    
    // Update every object in the game
    shipUpdateLogic(delta);
    projectilesUpdateLogic(delta);
    asteroidsUpdateLogic(delta);
    
    // Detect and handle collisions
    projectileAsteroidCollision();
    shipAsteroidCollision();
}
 
void Engine::getInput()
{                  
    // Get input.
    // Check if the keys are currently pressed; if yes, then
    // the corresponding value is set to true.
    leftPressed         = (GetKeyState(VK_LEFT)  & 0x8000) != 0;
    rightPressed        = (GetKeyState(VK_RIGHT) & 0x8000) != 0;
    accelerationPressed = (GetKeyState(VK_UP)    & 0x8000) != 0;
    firePressed         = (GetKeyState(VK_SPACE) & 0x8000) != 0;
}

void Engine::shipUpdateLogic(double delta)
{
    // Ship controls
    if (!ship->isExploded())
    {
        if (leftPressed) ship->applyLeftRotation(delta);
        if (rightPressed) ship->applyRightRotation(delta);
        if (accelerationPressed) ship->applyAcceleration(delta);
        if (firePressed) 
        {
            // To avoid having too many projectiles, the projectile
            // will only fire if there is less than 20
            if (noProjectiles < 20)
            {
                projectileCooldown++;
                projectileCooldown = projectileCooldown % 10;
                
                if (projectileCooldown == 0) { 
                    Projectile* projectile = new Projectile(ship->getPosition(), 
                            ship->getRotation());
                    projectiles[noProjectiles] = projectile;
                    noProjectiles++;
                }
            }
        }
    }

    // Ship logic
    ship->advance(delta);
    if (ship->isExploded() && ship->getExplosionTime() > 0.5)
    {
        ship->reset();
        lives--;
        if (lives < 0)
        {
            // Game over
            gameOver = true;
            gameWon = false;
        }
    }
}
 
void Engine::projectilesUpdateLogic(double delta)
{   
    // Projectile logic
    for (int i = 0; i < noProjectiles; i++)
    {
        // Move projetiles
        projectiles[i]->advance(delta);
        
        // Remove projectiles that are outside the screen
        // Also perform reindexing
        if (projectiles[i]->isOut())
        {
            Projectile* projectile = projectiles[i];
            for (int j = i; j < noProjectiles - 1; j++)
                projectiles[j] = projectiles[j + 1];
            projectiles[noProjectiles - 1] = NULL;
            delete projectile;
            noProjectiles--;
            i--;
        }
    }
}

void Engine::asteroidsUpdateLogic(double delta)
{    
    // Asteroid logic
    for (int i = 0; i < noAsteroids; i++)
    {
        asteroids[i]->advance(delta);
        if (asteroids[i]->getSize() == 0 && asteroids[i]->getExplosionTime() > 0.5)
        {
            // If the asteroid already exploded for 0.5 secs, remove it
            Asteroid* asteroid = asteroids[i];
            for (int k = i; k < noAsteroids - 1; k++)
                asteroids[k] = asteroids[k + 1];
            asteroids[noAsteroids - 1] = NULL;
            delete asteroid;
            noAsteroids--;
            
            // Win condition
            if (noAsteroids == 0)
            {
                gameOver = true;
                gameWon = true;
            }
        }
    }
}

void Engine::projectileAsteroidCollision()
{   
    // Collision detection: Projectile and Asteroid
    for (int i = 0; i < noAsteroids; i++)
    {
        for (int j = 0; j < noProjectiles; j++)
        {
            Projectile* projectile = projectiles[j];
            Asteroid* asteroid = asteroids[i];
            
            double distance = (asteroid->getPosition() - projectile->getPosition()).length();
            double size = asteroid->getSize() * ASTEROID_SIZE_MULTIPLIER;
            
            // If they do not collide immediately go to next loop
            if (distance >= size) continue;
                
            // Remove projectile first
            for (int k = j; k < noProjectiles - 1; k++)
                projectiles[k] = projectiles[k + 1];
            projectiles[noProjectiles - 1] = NULL;
            delete projectile;
            noProjectiles--;
                
            // Explode asteroid and create 2 new
            if (asteroid->getSize() > 1)
            {
                Point2D currentSpeed = asteroid->getSpeed();
                
                // New asteroid 1
                Point2D newSpeed1 = currentSpeed * 1.5;
                Asteroid* newAsteroid1 = new Asteroid(asteroid->getPosition(), 
                        asteroid->getSize()/2, newSpeed1);
                asteroids[noAsteroids] = newAsteroid1;
                noAsteroids++;
                
                // New asteroid 2
                Point2D newSpeed2 = currentSpeed * -1.5;
                Asteroid* newAsteroid2 = new Asteroid(asteroid->getPosition(),
                        asteroid->getSize()/2, newSpeed2);
                asteroids[noAsteroids] = newAsteroid2;
                noAsteroids++;
                
                // Remove old asteroid
                for (int k = i; k < noAsteroids - 1; k++)
                    asteroids[k] = asteroids[k + 1];
                asteroids[noAsteroids - 1] = NULL;
                delete asteroid;
                noAsteroids--;
            }
            else
            {
                // If asteroid is too small it explodes
                asteroid->explode();
            }
            
            return; // We only need to check collision once, so
                    // we return early if we already found one.
        }
    }
}

void Engine::shipAsteroidCollision()
{
    if (ship->isExploded()) return;
    
    for (int i = 0; i < noAsteroids; i++)
    {
        Asteroid* asteroid = asteroids[i];
        
        double distance = (asteroid->getPosition() - ship->getPosition()).length();
        // Asteroid size + ship size
        double size = asteroid->getSize() * ASTEROID_SIZE_MULTIPLIER + 5;
        
        if (distance < size)
        {
            ship->explode();
            return;
        }
    }
}

void Engine::draw()
{
    cleardevice();
    
    for (int i = 0; i < lives; i++)
    {
        // Draw lives
        drawLive(5 + i * 22, 5);
    }
    
    for (int i = 0; i < noProjectiles; i++)
        projectiles[i]->draw();
    
    if (!gameOver || gameWon)
    {
        ship->draw();
    }
    
    for (int i = 0; i < noAsteroids; i++)
    {
        asteroids[i]->draw();
    }
    
    // Game over state
    if (gameOver)
    {
        solidRectangle(WINDOW_WIDTH / 4, WINDOW_HEIGHT/3, 
                3 * WINDOW_WIDTH / 4, 2 * WINDOW_HEIGHT / 3);
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
        if (gameWon)
        {
            outtextxy(WINDOW_WIDTH/2 - (7 * 16), WINDOW_HEIGHT/2 - 32, "YOU WIN");
        }
        else
        {
            outtextxy(WINDOW_WIDTH/2 - (9 * 16), WINDOW_HEIGHT/2 - 32, "GAME OVER");
        }
    }
    
    int oldv = getvisualpage( );
    int olda = getactivepage( );
    setvisualpage(olda);
    setactivepage(oldv);
}

void Engine::drawLive(int x, int y)
{
    line(x + 10, y     , x + 20, y + 20);
    line(x + 20, y + 20, x + 10, y + 15);
    line(x + 10, y + 15, x     , y + 20);
    line(x     , y + 20, x + 10, y     );
    setfillstyle(SOLID_FILL, WHITE);
    floodfill(x + 10, y + 10, WHITE);
}

void Engine::solidRectangle(int x1, int y1, int x2, int y2)
{
     for (int x = x1; x <= x2; x++)
         for (int y = y1; y <= y2; y++)
             putpixel(x, y, BLACK);
}
