
// Tests macro expansion order

#define A(x, y) x, y
#define B(x, y) [x][y]
#define C(x) B(x)

C( A(2, 3) )	// should expand to [2][3]
