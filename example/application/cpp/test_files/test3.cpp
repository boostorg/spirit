
// Tests macro expansion order.

#define A() B
#define B(x) x

A()(123)            // should expand to 123
