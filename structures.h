enum Direction {
        up,
        right,
        down,
        left
};

typedef struct {
        int x;
        int y;
} Position;

typedef struct {
        Position *body;
        enum Direction direction;
        int length;
} Snake;

typedef struct {
        Position pos;
        bool eaten;
} Food;
