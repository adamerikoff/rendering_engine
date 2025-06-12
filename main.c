// --- main.c: Simple SDL2 Screen Creation with Struct ---
#include "src/app/app.h"

// Define screen dimensions for clarity
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// --- main Function ---
int main(int argc, char* args[]) {
    // Declare an instance of our SDLApp struct.
    App myApp;

    // Initialize the application.
    // If initialization fails, SDLApp_Init will return non-zero.
    if (!App_Init(&myApp, "Rendering Engine", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        // If initialization is successful, run the application.
        App_Run(&myApp);
    }

    // Clean up resources after the application has finished running.
    App_Cleanup(&myApp);

    return 0; // Indicate success
}