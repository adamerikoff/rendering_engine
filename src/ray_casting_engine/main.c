#include "./app/app.h"
#include "./canvas/canvas.h"

int main(int argc, char* argv[]) {
    Canvas canvas = canvas_new(800, 600);
    // Declare an Application struct instance
    Application my_app;

    // Initialize the application
    if (application_init(&my_app, &canvas) != 0) {
        fprintf(stderr, "Failed to initialize application.\n");
        return 1; // Exit with error code if initialization fails
    }

    // Run the main application loop
    application_loop(&my_app);

    // Clean up SDL resources (renderer, window)
    application_clean_up(&my_app);

    // Quit SDL subsystems
    application_exit(&my_app);
    
    return 0; // Indicate successful execution
}
