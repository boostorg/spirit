
// Test, if wrongly placed '##' operators are detected

#define TEST1() A ## B
#define TEST2() ## A
#define TEST3() A ##

TEST1()	    // AB
TEST2()	    // error
TEST3()	    // error
