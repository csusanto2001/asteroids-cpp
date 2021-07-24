#include <windows.h>
#include <graphics.h>

#include "engine.h"

void initialize();
void run(Engine* engine);

int main()
{
    Engine* engine = new Engine();
    
    initialize();
    run(engine);
}

void initialize()
{
     int gd = DETECT, gm;
     initwindow(WINDOW_WIDTH, WINDOW_HEIGHT);
     
     setactivepage(1);
     setvisualpage(0);
}

void run(Engine* engine)
{
    bool running = true;
    while (running) {
       // FILETIME start, end;
        //GetSystemTimeAsFiletime(&start);;
        engine->update(TIME_BETWEEN_FRAMES / 1000);
        engine->draw();
        delay((int)(TIME_BETWEEN_FRAMES));
        // Calculate how long to wait for the next frame
        // This ensures the frame rate stays consistent
        //GetSystemTimeAsFiletime(&end);
        //int delayTime = 
        //delay((int) (delayTime));
    }
}
