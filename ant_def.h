/*****************************************************************/
/* Define the ant's map */
/*define MAP_ROWS        32*/
/*define MAP_COLS        32*/

#define TRUE 1
#define FALSE 0

#define DEBUG 0

#define MSDOS 1
#define UNIX  0

#define CHILD -1
#define PARENT -2

#include <stdio.h>

/* N O T E : The port to UNIX is incomplete as of this writing. */

#if MSDOS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <time.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#endif

#if UNIX
#include <sys/time.h>
#include <malloc.h>

#define huge
#endif

#define MAXPARAMLINELENGTH    255
#define MAXCHECKLINELENGTH    255
#define MALLOC  malloc
#define FREE    free
#define REALLOC realloc
/**************************************************************************/
/* Ant's gene is a state machine. */
/* Each state has associated with it :
   (1) an output code corresponding to each possible input code; and
   (2) the next state to assume. */

/* How many possible input codes are there? */
#define N_INPUT_CODES   2

/* How many possible output codes are there? */
#define N_OUTPUT_CODES  4
#define OUTPUT_MASK 192
#define OUTPUT_SHIFT 6

/* How many states are available? */
#define N_STATES        32
#define STATE_MASK      31

/* How many nodes are in virus line? */
#define VIR_NUM   6
/* Integer percent of ants from population */
/* to choose they and check they to virus. */
/**************************************************************************/
/* Bit patterns for output codes */
#define MOVE_MASK       0x3
#define NO_MOVE         0
#define MOVE_FWD        1
#define MOVE_RIGHT      2
#define MOVE_LEFT       3
/**************************************************************************/
/**************************************************************************/
#define SELECT_STRATEGY_TRUNCATE   0  /* Truncation selection strategy */
#define SELECT_STRATEGY_ROULETTE   1  /* Roulette-wheel selection strategy */

/**************************************************************************/
#define EAST    0
#define NORTH   1
#define WEST    2
#define SOUTH   3
#define PARAM_COPY_NONE   0
#define PARAM_COPY_NAME   1
#define PARAM_COPY_VALUE  2
#define PARAMETER_MINSIZE       31
#define PARAMETER_CHUNKSIZE     16

#define OUT_SYS    0
