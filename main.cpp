#include <chrono>
#include <graphics.h>

#include "engine.h"

void initialize();
void run();

int main()
{
    Engine* engine = new Engine();
    
    
}

void initialize()
{
     int gd = DETECT, gm;
     initwindow(WINDOW_WIDTH, WINDOW_HEIGHT);
     
     setactivepage(1);
     setvisualpage(0);
}

void run()
{
    using namespace std::chrono;
    bool running = true;
    while (running) {
        steady_clock::time_point start = steady_clock::now();
        engine->update(TIME_BETWEEN_FRAMES);
        engine->draw();
        
        // Calculate how long to wait for the next frame
        // This ensures the frame rate stays consistent
        steady_clock::time_point end = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(end - start);
        double delayTime = TIME_BETWEEN_FRAMES - time_span.count();
        delay((int) (delayTime * 1000));
    }
}
