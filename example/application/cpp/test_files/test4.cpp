
// Tests the correctness of parameter replacement, if the parameter is
// adjacent to an operator '##'.

#define CONCAT(a, b) PRIMITIVE_CONCAT(a, b)
#define PRIMITIVE_CONCAT(a, b) a ## b

//CONCAT(1, PRIMITIVE_CONCAT(2, 3))           // 123
CONCAT(1, CONCAT(2, 3))                     // 123
PRIMITIVE_CONCAT(1, CONCAT(2, 3))           // 1CONCAT(2, 3)
PRIMITIVE_CONCAT(1, PRIMITIVE_CONCAT(2, 3)) // 1PRIMITIVE_CONCAT(2, 3)
