#include "./app/app.h"

int main(int argc, char* argv[]) {
    Application app; // Create an instance of our Application struct

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Initialize the application
    if (application_init(&app, WINDOW_WIDTH, WINDOW_HEIGHT) != 0) {
        fprintf(stderr, "Failed to initialize application.\n");
        return 1; // Exit with error code
    }

    // Run the main application loop
    application_loop(&app);

    // Clean up resources before exiting
    application_clean_up(&app);
    
    // Quit SDL subsystems (global call)
    application_exit(&app); // Ensures SDL_Quit is called

    return 0; // Indicate successful execution
}
