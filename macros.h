#define FPS (144)

#define Snake_Head '█'
#define Snake_Body {'▃', '▅', '▆'}  // Unicode U+2583, U+2585, U+2586
#define Snake_Tail '▂'

#define Apple '🍎'

#define print(fmt, ...) \
do { \
        printf(fmt, ##__VA_ARGS__); \
        fflush(stdout); \
} while(0)
