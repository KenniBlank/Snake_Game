#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "macros.h"
#include "structures.h"

void init_game(Snake *snake, Food *food, int max_x, int max_y);
void Intro(int max_x, int max_y);
void draw_game(WINDOW *win, Snake *snake, Food *food, int score);
void update_snake(Snake *snake, int max_x, int max_y);
bool check_collision(Snake *snake); // check if head collide with body
void handle_input(Snake *snake, bool *game_running);
void place_food(Food *food, Snake *snake, int max_x, int max_y);
int check_food_eaten(Snake *snake, Food *food);
void game_over(int score, int max_x, int max_y);

int main() {
        // Initialize ncurses header
        initscr();
        start_color(); // Enable color functionality
        cbreak();
        noecho();
        curs_set(0);
        keypad(stdscr, true);

        printf("\033[?3l");   // AI Line of Code: Tries to disable line spacing (may work or maynot)
        init_pair(1, COLOR_RED, COLOR_BLACK); // Border Color
        init_pair(2, COLOR_GREEN, COLOR_BLACK); // Food Color
        init_pair(3, COLOR_BLUE, COLOR_BLACK); // Snake Color
        init_pair(4, COLOR_BLACK, COLOR_WHITE); // Score Color

        // Get Terminal Size
        int max_x, max_y;
        getmaxyx(stdscr, max_y, max_x);
        if (max_x < MIN_TERMINAL_WIDTH || max_y < MIN_TERMINAL_HEIGHT) {
                endwin();
                printf("Terminal too small! Minimum size is %dx%d\n", MIN_TERMINAL_WIDTH, MIN_TERMINAL_HEIGHT);
                return 1;
        }

        Intro(max_x, max_y);

        nodelay(stdscr, true);

        // Init Game Elements:
        int score = 0;
        Snake snake;
        Food food;
        bool game_is_running = true;
        __useconds_t delay = 100000;

        init_game(&snake, &food, max_x, max_y);

        while (game_is_running) {
                clear(); // Defined in n_curses

                handle_input(&snake, &game_is_running);

                update_snake(&snake, max_x, max_y);

                // Check for collisions
                if (check_collision(&snake)) game_is_running = false;

                // Check if Food Eaten
                if (check_food_eaten(&snake, &food)) {
                        score += 1;
                        delay = delay - score * 1000;

                        // Grow Snake:
                        snake.length += 1;
                        snake.body = realloc(snake.body, snake.length * sizeof(Position));
                        snake.body[snake.length - 1] = snake.body[snake.length - 2]; // Copy Last Position

                        place_food(&food, &snake, max_x, max_y);
                }

                draw_game(stdscr, &snake, &food, score);

                refresh(); // Defined in n_curses 2

                usleep(delay); // Speed Delay of Speed: Clever RIGHT!!
        }

        // Game Over:
        free(snake.body);

        game_over(score, max_x, max_y);
        return 0;
}

void init_game(Snake *snake, Food *food, int max_x, int max_y) {
    // Initialize snake
    snake -> length = 2;
    snake -> body = malloc(snake -> length * sizeof(Position));
    snake -> direction = right;

    // Place snake in the middle
    int start_x = max_x / 2;
    int start_y = max_y / 2;
    for (int i = 0; i < snake -> length; i++) {
        snake -> body[i].x = start_x - i;
        snake -> body[i].y = start_y;
    }

    // Place initial food
    place_food(food, snake, max_x, max_y);

    // Seed random number generator
    srand(time(NULL));
}

void Intro(int max_x, int max_y) {
        attron(COLOR_PAIR(2));
        mvprintw(max_y / 2, (max_x - strlen("Welcome to Snake Game")) / 2, "Welcome to Snake Game");
        mvprintw(max_y / 2 + 1, (max_x - strlen("Press Any Key To Begin")) / 2, "Press Any Key To Begin");
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(3));
        mvprintw(max_y / 2 - 5, 0, "Controls: ");
        mvprintw(max_y / 2 - 4, 0, "\tWASD to move (Arrows can be used too)");
        mvprintw(max_y / 2 - 3, 0, "\tQ to quit at \"Any\" time");
        attroff(COLOR_PAIR(3));

        getch();
}

void draw_game(WINDOW *win, Snake *snake, Food *food, int score) {
        int max_x, max_y;
        getmaxyx(win, max_y, max_x);

        // Draw border
        attron(COLOR_PAIR(1));
        for (int x = 0; x < max_x; x++) {
                mvaddch(0, x, ACS_CKBOARD);
                mvaddch(max_y-1, x, ACS_CKBOARD);
        }
        for (int y = 0; y < max_y; y++) {
                mvaddch(y, 0, ACS_CKBOARD);
                mvaddch(y, max_x-1, ACS_CKBOARD);
        }
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(4));
        mvprintw(0, 3, " Score: %d ", score);
        attroff(COLOR_PAIR(4));

        // Draw snake
        attron(COLOR_PAIR(3));
        mvaddch(snake -> body[0].y, snake -> body[0].x, SNAKE_HEAD); // Head
        for (int i = 1; i < snake -> length; i++) {
                mvaddch(snake -> body[i].y, snake -> body[i].x, SNAKE_BODY); // Body
        }
        attroff(COLOR_PAIR(3));

        // Draw food
        attron(COLOR_PAIR(2));
        if (!food -> eaten) {
                attron(COLOR_PAIR(2));
                mvaddch(food -> pos.y, food -> pos.x, APPLE);
                attroff(COLOR_PAIR(2));
        }
        attroff(COLOR_PAIR(2));
}

void update_snake(Snake *snake, int max_x, int max_y) {
        // Move Each Part into positio of previous part
        for (int i = snake -> length - 1; i > 0; i--) {
                snake -> body[i] = snake -> body[i - 1];
        }

        // Set Head
        switch (snake -> direction) {
                case up:
                        snake -> body[0].y -= 1;
                        break;
                case right:
                        snake -> body[0].x += 1;
                        break;
                case down:
                        snake -> body[0].y += 1;
                        break;
                case left:
                        snake -> body[0].x -= 1;
                        break;
        }

        // Wrap around screen, For now only to test, normally this is dead condition
        if (snake -> body[0].x <= 0) snake -> body[0].x = max_x - 2;
        if (snake -> body[0].x >= max_x - 1) snake -> body[0].x = 1;
        if (snake -> body[0].y <= 0) snake -> body[0].y = max_y - 2;
        if (snake -> body[0].y >= max_y - 1) snake -> body[0].y = 1;
}

bool check_collision(Snake *snake) {
        // Check if head collides with body
        for (int i = 1; i < snake->length; i++) {
                if (snake -> body[0].x == snake -> body[i].x && snake -> body[0].y == snake -> body[i].y) {
                        return true;
                }
        }
        return false;
}

void handle_input(Snake *snake, bool *game_running) {
        int ch = getch();

        switch (ch) {
                case KEY_UP:
                case 'w':
                case 'W':
                        if (snake -> direction != down) {
                                snake -> direction = up;
                        }
                        break;
                case KEY_RIGHT:
                case 'd':
                case 'D':
                        if (snake -> direction != left) {
                                snake -> direction = right;
                        }
                        break;
                case KEY_LEFT:
                case 'a':
                case 'A':
                        if (snake -> direction != right) {
                                snake -> direction = left;
                        }
                        break;
                case KEY_DOWN:
                case 's':
                case 'S':
                        if (snake -> direction != up) {
                                snake -> direction = down;
                        }
                        break;
                case 'q':
                case 'Q':
                        *game_running = false;
                        break;
        }
}

void place_food(Food *food, Snake *snake, int max_x, int max_y) {
        bool valid = false;

        while (!valid) {
                valid = true;
                food -> pos.x = (rand() % (max_x - 2)) + 1;
                food -> pos.y = (rand() % (max_y - 2)) + 1;

                // Check if food overlaps with snake
                for (int i = 0; i < snake -> length; i++) {
                        if (food -> pos.x == snake -> body[i].x && food -> pos.y == snake -> body[i].y) {
                                valid = 0;
                                break;
                        }
                }
        }

        food -> eaten = 0;
}

int check_food_eaten(Snake *snake, Food *food) {
        if (snake -> body[0].x == food -> pos.x &&
                snake -> body[0].y == food -> pos.y) {
                food -> eaten = 1;
                return 1;
        }
        return 0;
}

void game_over(int score, int max_x, int max_y) {
        attron(COLOR_PAIR(1));
        mvprintw(max_y / 2, (max_x-9) / 2, "Game Over");
        attroff(COLOR_PAIR(1));

        char *message = malloc(50 * sizeof(char));
        sprintf(message, "Final Score: %d", score);
        attron(COLOR_PAIR(2));
        mvprintw(max_y / 2 + 1, (max_x - strlen(message)) / 2, message);
        free(message);
        attroff(COLOR_PAIR(2));

        mvprintw(max_y / 2 + 4, (max_x - strlen("Any Key To Continue..")) / 2, "Any Key To Continue..");

        refresh();
        nodelay(stdscr, false);
        getch(); // Hold the screen
        endwin();
}
