#define CAT(a, b) a ## b
#define ARGS (1, 2)

CAT ARGS // expands to CAT (1, 2) not 12

#define INVOKE(macro) macro ARGS

INVOKE(CAT) // CAT (1, 2) not 12

#define EXPAND(x) x

EXPAND(CAT ARGS) // expands to 12 because of rescanning
