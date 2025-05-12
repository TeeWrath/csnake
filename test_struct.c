// Test struct and static typing
struct Point {
    int x;
    float y;
};

int main() {
    struct Point p;
    p.x = 10;
    p.y = 20.5;
    printf("Point: (%d, %f)\n", p.x, p.y);
    return 0;
}