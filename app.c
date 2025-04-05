#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

#include "macros.h"
#include "structures.h"

int init_terminal_width, init_terminal_height;

typedef struct {
        char value;
} RendererData;

typedef struct {
        int y, x;
} Player;

void __CLS__();
void Intro();
void Outro(int score, char* end_message);

void Initialize_Terminal_Data(RendererData **Data) {
        // Fill entire area with dots
        for (int i = 0; i < init_terminal_height; i++) {
                for (int j = 0; j < init_terminal_width; j++) {
                        Data[i][j].value = ' ';
                }
        }

        // Top and bottom borders
        for (int j = 0; j < init_terminal_width; j++) {
                Data[0][j].value = '-';  // Top border
                Data[init_terminal_height-1][j].value = '-';  // Bottom border
        }

        // Left and right borders
        for (int i = 0; i < init_terminal_height; i++) {
                Data[i][0].value = ':';  // Left border
                Data[i][init_terminal_width-1].value = ':';  // Right border
        }
}

void Player_Init(RendererData **Data) {
        int center_x = init_terminal_width / 2;
        int center_y = init_terminal_height / 2;

        Data[center_y][center_x].value = Snake_Head;
        Data[center_y - 1][center_x].value = Snake_Body;
}

void Renderer(RendererData **Data) {
        __CLS__();
        for (int i = 0; i < init_terminal_height; i++) {
                for (int j = 0; j < init_terminal_width; j++) {
                        printf("%c", Data[i][j].value);
                }
                printf("\n");
        }
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

        // Make stdin non-blocking
        int old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);

        // Hide the cursor
        printf("\033[?25l");
        fflush(stdout);

        Intro();

        struct winsize terminalBounds;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &terminalBounds);

        init_terminal_height = terminalBounds.ws_row - 1;
        init_terminal_width = terminalBounds.ws_col;

        // Allocate 2D array dynamically
        RendererData **terminal_data = (RendererData **)malloc(init_terminal_height * sizeof(RendererData *));
        for (int i = 0; i < init_terminal_height; i++) {
                terminal_data[i] = (RendererData *)malloc(init_terminal_width * sizeof(RendererData));
        }

        Initialize_Terminal_Data(terminal_data);
        Player_Init(terminal_data);

        bool game_is_running = true;
        int SCORE = 0;
        char *end_message = NULL;
        char ch;

        while (game_is_running) {
                // Get current terminal size in case of resizing
                ioctl(STDIN_FILENO, TIOCGWINSZ, &terminalBounds);
                if (terminalBounds.ws_col != init_terminal_width || (terminalBounds.ws_row - 1) != init_terminal_height) {
                        end_message = "Terminal was resized, exiting....";
                        break;
                }

                // Handle input
                while (read(STDIN_FILENO, &ch, 1) > 0) {
                        if (ch == 4) {  // CTRL-D = 4 in ASCII
                                end_message = "Game was exited successfully";
                                game_is_running = false;
                                break;
                        }

                        switch (ch) {
                                case 'w':
                                case 'W':
                                        print("W Pressed:\n");
                                        break;
                                case 's':
                                case 'S':
                                        print("S Pressed:\n");
                                        break;
                                case 'a':
                                case 'A':
                                        print("A pressed:\n");
                                        break;
                                case 'd':
                                case 'D':
                                        print("D Pressed:\n");
                                        break;
                                default:
                                        print("%c\n", ch);
                                        break;
                        }
                }

                Renderer(terminal_data);

                // Sleep to control FPS
                usleep(1000000 / FPS);
        }

        printf("\033[?25h");  // Show the cursor again

        Outro(SCORE, end_message);

        // Free allocated memory
        for (int i = 0; i < init_terminal_height; i++) {
                free(terminal_data[i]);
        }
        free(terminal_data);

        // Restore original flags and terminal settings
        fcntl(STDIN_FILENO, F_SETFL, old_flags);
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
        return 0;
}

// Function to clear terminal
void __CLS__() {
        printf("\033[H\033[J");
        fflush(stdout);
}

void Intro() {
        __CLS__();
        printf(".....................\n");
        printf("Press CTRL-D to exit.\n");
        printf(".....................\n");
}

void Outro(int score, char* end_message) {
        __CLS__();
        if (end_message) {
                printf("%s\n", end_message);
        }
        printf("..................\n");
        printf("Total Score Was %d\n", score);
        printf("..................\n");
}
