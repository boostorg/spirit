#define CONCAT(a, b) a ## b
#define CONCAT_INDIRECT() CONCAT

CONCAT(CON, CAT)(1, 2) // should expand to CONCAT(1, 2)
CONCAT(CON, CAT(1, 2)) // should expand to CONCAT(1, 2)
CONCAT(1, CONCAT(2, 3)) // should expand to 1CONCAT(2, 3)
CONCAT(1, CONCAT_INDIRECT()(2, 3)) // should expand to 1CONCAT_INDIRECT()(2, 3)
CONCAT(CONCAT_, INDIRECT)()(1, 2) // should expand to 12
