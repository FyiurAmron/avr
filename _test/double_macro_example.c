#define CONCAT0(x,y) x ## y
#define CONCAT(x,y) CONCAT0(x,y)
#define DOUBLE_MACRO CONCAT(var,__LINE__)
int main() {
    int DOUBLE_MACRO = 42;
    var5 = 41;
}