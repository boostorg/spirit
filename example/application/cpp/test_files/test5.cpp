
// The follwoing is a parametized partially expanding concatenation macro.
// It is a extremly good test of expansion order and the order of operations 
// during macro expansion in general. 

#define CAT(a, b) a ## b

#define PARTIAL_CAT(i) CAT(PARTIAL_CAT_, i)

#define PARTIAL_CAT_0(a, b) CAT(a, b)
#define PARTIAL_CAT_1(a, b) CAT(PARTIAL ## a, b)
#define PARTIAL_CAT_2(a, b) CAT(a, b ## PARTIAL)
#define PARTIAL_CAT_3(a, b) CAT(PARTIAL ## a, b ## PARTIAL)

#define PARTIAL
#define PARTIALPARTIAL

#define X Token1
#define Y Token2

PARTIAL_CAT(0)( PARTIAL X, Y PARTIAL ) // Token1Token2
PARTIAL_CAT(1)( PARTIAL X, Y PARTIAL ) // XToken2
PARTIAL_CAT(2)( PARTIAL X, Y PARTIAL ) // Token1Y
PARTIAL_CAT(3)( PARTIAL X, Y PARTIAL ) // XY
