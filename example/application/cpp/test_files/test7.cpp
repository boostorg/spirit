
// Test if macros at not used parameter positions are expanded correctly

#define MACRO() /**/
#define ID(x)           // parameter not used at all
#define CAT(x) X ## x   // expanded parameter not used

ID( MACRO(*) )  // error: MACRO takes no arguments (too many arguments)
CAT( MACRO(*) ) // error: MACRO takes no arguments (too many arguments)
