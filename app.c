#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "macros.h"
#include "structures.h"

#define Red_Color "\033[31m"
#define Reset_Color "\033[0m"

// Function to clear the screen and move the cursor to the top-left
void clear_screen() {
    printf("\033[H\033[J");
    fflush(stdout);
}

int main() {
    struct termios old_termios, new_termios;

    // Save the current terminal settings
    tcgetattr(STDIN_FILENO, &old_termios);

    new_termios = old_termios;

    // Modify the terminal settings to enable non-blocking input
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    struct winsize terminalBounds;

    // Get initial terminal size
    ioctl(STDIN_FILENO, TIOCGWINSZ, &terminalBounds);
    int terminalHeight = terminalBounds.ws_row - 1;
    int terminalWidth = terminalBounds.ws_col;

    // Make stdin non-blocking
    int old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);

    // Hide the cursor
    printf("\033[?25l");
    fflush(stdout);

    printf("Press CTRL-D to exit.\n");

    char ch;
    bool game_is_running = true;
    bool change = true;

    while (game_is_running) {
        // Exit loop on CTRL-D
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 4) {  // CTRL-D = 4 in ASCII
                printf("Game Exited\nCleaning Up...\n");
                game_is_running = false;
            }

            switch (ch) {
                        case 'w':
                                printf("W Pressed:");
                                break;
                        default:
                                break;
            }
        }

        // Get current terminal size in case of resizing
        ioctl(STDIN_FILENO, TIOCGWINSZ, &terminalBounds);
        terminalHeight = terminalBounds.ws_row - 1;
        terminalWidth = terminalBounds.ws_col;

        // Sleep to control FPS
        usleep(1000000 / FPS);
    }

    printf("\033[?25h");  // Show the cursor again

    // Restore original flags and terminal settings
    fcntl(STDIN_FILENO, F_SETFL, old_flags);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return 0;
}
