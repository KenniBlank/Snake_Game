#define FPS (30)

#define Snake_Head '@'
#define Snake_Body '.'

#define Apple '+'

#define print(fmt, ...) \
do { \
        printf(fmt, ##__VA_ARGS__); \
        fflush(stdout); \
} while(0)
