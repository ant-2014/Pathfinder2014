#include "ant.h"
#include <ctype.h>
extern test_run;
extern int ant_strategy;
extern int n_pop;
extern int n_gen;
extern int n_steps;
extern int psel;
extern int pattern_percent;
extern int pattern_pair;
extern int pattern_pair_percent;
extern double pcross;
extern double pmutate;
extern char log_file_name[80];
extern char sys_file_name[80];
extern char his_file_name[80];
extern char map_file_name[80];
extern FILE *map_file;
extern int selection_strategy;
extern unsigned seed_number;
extern int params_in_file;
extern int pattern_start;
extern int pattern_stop;
extern int pattern_print;
extern int pattern_on_screen;
extern int start_pattern_strategy;
extern int stop_pattern_strategy;
extern int startfromcheckpoint;
extern Gene_track huge *gene_track_array;

/**************************************************************************/
void process_commandline ( int argc, char **argv )
{
     extern FILE *sys_file;
     int i,params;
     int errorflag = 0;


     /* if there are no arguments, print out a brief statement of usage
	and exit. */
   if ( argc < 2 )
     {
	  fprintf( stderr ,"usage: %s options\nValid options are:\n", argv[0] );
	  fprintf( stderr , "  ant -f parameterfile    : read named parameter file\n" );
	  fprintf( stderr , "  ant -c checkpointfile   : read named checkpoint file\n" );
	  fprintf( stderr , "  ant -d                  : read default parameters\n" );
	  exit(1);
     }

     for ( i = 1; i < argc; ++i )
     {
	  /* all options begin with '-' and have two characters */
    if ( argv[i][0] != '-' )
	  {
	       fprintf( stderr , "\nunrecognized command line option: \"%s\".",
		      argv[i] );
	       errorflag = 1;
	       continue;
	  }

	  switch ( argv[i][1] )
	  {
	     case 'f':
	       /* load a parameter file, named in the next argument. */
	       oprintf( 1,sys_file," Read parameters from input file \"%s\"\n",argv[++i]);
	       delay(1000);
	       startfromcheckpoint = 0;
	       read_parameter_file ( argv[2] );
	       break;

	     case 'c':
	       /* load a checkpoint file, named in the next argument. */
	       oprintf( 1,sys_file,"Read parameters from checkpoint file \"%s\"\n",argv[++i]);
	       read_checkpoint ( argv[2] , gene_track_array );
	       break;

	     case 'd':
	       /* load the default parameters. */
	       startfromcheckpoint = 0;
	       oprintf(1,sys_file," Read all default parameters.\n");
	       delay(1000);
	       return;
	       break;

	     default:
	       fprintf( stderr ,  "\nunrecognized command line option: \"%s\".",
		      argv[i] );
	       errorflag = 1;
	       break;
		  }
     }

     if ( errorflag )
	 {
	   fprintf( stderr ,  "\ncommand line errors occurred.  dying." );
	   exit(1);
	 }

/*//  read_checkpoint ( "ant1.ckp",&gene_track_array);
//  read_parameter_file ( "input.ant");*/

return;
}


/* read_parameter_file()
 *
 * reads and parses a parameter file.
 */

void read_parameter_file ( char *filename )
{
     FILE *f;
     char buffer[MAXPARAMLINELENGTH+1];
     char *buf2;
     int buf2size;
     int line = 0;
     int errors = 0;
     int including = 1;
      int flag;
     int buflen, buf2len;
     int cont;

     /** open it. **/
     f = fopen ( filename, "r" );
     if ( f == NULL )
	  fprintf( stderr ,  "\ncan't open parameter file \"%s\".",
		 filename );

     /** lines are read into buffer.  they are appended to buf2 until a
       line which is not continued is hit, then buf2 is parsed and the
       parameter added. **/

     /* initial allocation of buf2. */
     buf2 = (char *)malloc ( MAXPARAMLINELENGTH * sizeof ( char ) );
     buf2size = MAXPARAMLINELENGTH;
     buf2[0] = 0;
     buf2len = 0;
     cont = 0;

     while ( fgets ( buffer, MAXPARAMLINELENGTH, f ) != NULL )
     {
	  ++line;

	  /* remove comments, skip line if it's blank after comment
	     removal. */
	  if ( delete_comment ( buffer ) )
	       continue;

	  /* is this line continued?  (is the last nonwhitespace character
	     a backslash?) */
	  cont = check_continuation ( buffer );

	  /** make buf2 bigger if necessary. **/
	  buflen = strlen ( buffer );
	  while ( buf2len + buflen > buf2size + 10 )
	  {
	       buf2size += MAXPARAMLINELENGTH;
	       buf2 = (char *)realloc ( buf2, buf2size * sizeof ( char ) );
	  }

	  /** tack the current line onto buf2. **/
	  strcat ( buf2, buffer );
	  buf2len += buflen;

	  /** if this line is not continued further, then parse buf2. and
	   add the parameter. **/
	  if ( !cont )
	  {
	       if ( parse_one_parameter ( buf2 ))
	       {
		    errors = 1;
		    fprintf( stderr ,  "\n%s line %d: syntax error.",
			   filename, line );
	       }

	       /** reset buf2 as empty. **/
	       buf2len = 0;
	       buf2[0] = 0;
	  }
     }

     /* close file. */
     fclose ( f );

     /** if the last line was continued, and nothing followed it, that's
       an error. **/
     if ( buf2len != 0 )
     {
	  errors = 1;
	   fprintf( stderr ,  "\nunexpected EOF." );
     }

     /** if any errors occurred during parsing, stop now. **/
     if ( errors )
     {
	 fprintf( stderr ,
		 "\nsome syntax errors occurred parsing \"%s\".", filename );
	 free( buf2 );
	 exit(1);
    }

     free ( buf2 );

}

/* check_continuation()
 *
 * returns true/false depending on whether the given string is continued
 * (the last nonwhitespace character is a backslash).  If so, the backslash
 * and everything after it is chopped.
 */

int check_continuation ( char *buffer )
{
     int i, l;
     l = strlen ( buffer );
     for ( i = l-1; i >= 0; --i )
	  if ( !isspace(buffer[i]) )
	  {
	       if ( buffer[i] == '\\' )
	       {
		    buffer[i] = '\n';
		    buffer[i+1] = 0;
		    return 1;
	       }
	       else
		    return 0;
	  }

     /* a blank line was passed. */
     return 0;
}

/* delete_comment()
 *
 * this searches the string for a '#' or ';' and chops everything
 * following, if one is found.  returns 1 if the resulting line is blank
 * (all whitespace), 0 otherwise.
 */

int delete_comment ( char *buffer )
{
     int i, l;

     l = strlen ( buffer );
     /* zero-length lines are considered blank. */
     if ( l == 0 )
	  return 1;
     /* if the last character is a newline, chop it. */
     if ( buffer[--l] == '\n' )
	  buffer[l] = 0;
     for ( i = 0; i < l; ++i )
	  if ( buffer[i] == '#' || buffer[i] == ';' )
	  {
	       /* chop the line at a '#' or ';'. */
	       buffer[i] = 0;
	       break;
	  }
     /* look for a nonwhitespace character. */
     l = strlen ( buffer );
     for ( i = 0; i < l; ++i )
	  if ( !isspace(buffer[i]) )
	       /* found one, return 0. */
	       return 0;

     /* blank line, return 1. */
     return 1;
}

/* parse_one_parameter()
 *
 * breaks a string at the first equals sign into name and value parts.
 * removes leading and trailing whitespace from both parts. inserts the
 * resulting pair into the parameter database.
 */

int parse_one_parameter ( char *buffer)
{
     char name[MAXPARAMLINELENGTH+1];
     char data[MAXPARAMLINELENGTH+1];
     int i, j, k, l;
     int n, d;

     k = -1;
     j = 0;
     l = strlen ( buffer );
     /** scan for a equals sign. **/
     for ( i = 0; i < l; ++i )
     {
	  /* j records whether or not we have found a nonwhitespace
	    character. */
	  j += (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n');
	  if ( buffer[i] == '=' )
	  {
	       k = i;
	       /* copy the name part. */
	       strncpy ( name, buffer, k );
	       name[k] = 0;
	       /* copy the value part. */
	       strcpy ( data, buffer+k+1 );
	       break;
	  }
     }

     /* if we found no '=', return an error unless the line was
	completely blank. */
     if ( k == -1 )
	  return !!j;

     /* trim leading and trailing whitespace. */
     n = trim_string ( name );
     d = trim_string ( data );

     /** if either section is blank, return an error, otherwise add
       the pair as a parameter. **/
     if ( n == 0 || d == 0 )
	  return 1;
     else
	  add_parameter ( name, data, PARAM_COPY_NAME|PARAM_COPY_VALUE );
     return 0;
}

/* trim_string()
 *
 * trims leading and trailing whitespace from a string, overwriting the
 * argument with the result.  returns number of characters in result.
 */

int trim_string ( char *string )
{
     int i, j, l;

     j = -1;
     l = strlen ( string );
     for ( i = 0; i < l; ++i )
     {
	  if ( j == -1 )
	  {
	       if ( string[i] != ' ' && string[i] != '\t' &&
		    string[i] != '\n' )
	       {
		    j = i;
		    --i;
	       }
	  }
	  else
	       string[i-j] = string[i];
     }
     if ( j == -1 )
     {
	  string[0] = 0;
	  return 0;
     }

     string[i-j] = 0;
     l = i-j;
     j = -1;
     for ( i = 0; i < l; ++i )
     {
	  if ( string[i] != ' ' && string[i] != '\t' && string[i] != '\n' )
	       j = i;
     }
     string[j+1] = 0;

     return j+1;
}

/* add_parameter()
 *
 * adds the given name/value pair to the database.  the flags indicate
 * which if any of the strings need to be copied.
 */

void add_parameter ( char *name, char *value, int copyflags )
{
 int    check1;
 double check2;

 char *par1 = "n_pop";
 char *par2 = "n_gen";
 char *par3 = "n_steps";
 char *par4 = "psel";
 char *par5 = "pattern_percent";
 char *par6 = "pcross";
 char *par7 = "pmutate";
 char *par8 = "log_file_name";
 char *par9 = "selection_strategy";
 char *par10 = "seed_number";
 char *par11 = "sys_file_name";
 char *par12 = "pattern_start";
 char *par13 = "pattern_print";
 char *par14 = "pattern_on_screen";
 char *par15 = "his_file_name";
 char *par16 = "pattern_stop";
 char *par17 = "map_file_name";
 char *par18 = "start_pattern_strategy";
 char *par19 = "stop_pattern_strategy";
 char *par20 = "pattern_pair_percent";
 char *par21 =  "ant_strategy";
 char *par22 =  "pattern_pair";
 char *par23 =  "test_run";



 if( !strcmp( name, par1 ) )
       {
	 check1 = atoi( value );
	 if( check1 == 0 )
	   {
	    fprintf(stderr,"\n\"%s\" is bad size of population",value);
	    exit(1);
	   }
	 else n_pop = check1;
       }

else if( !strcmp( name, par2 ) )
       {
	 check1 = atoi( value );
	 if( check1 == 0 )
	   {
	    fprintf(stderr,"\n\"%s\" is bad number of generation",value);
	    exit(1);
	   }
	 else n_gen = check1;
       }

else if( !strcmp( name, par3 ) )
       {
	 check1 = atoi( value );
	 if( check1 == 0 )
	   {
	    fprintf(stderr,"\n\"%s\" is bad number of individual life-steps",value);
	    exit(1);
	   }
	 else n_steps = check1;
       }

else if( !strcmp( name, par4 ) )
       {
	check1 = atoi( value );
	if( (check1 <= 0) || (check1 > 100) )
	  {
	   fprintf(stderr,"\n\"%s\"! Sorry, allowable range of integer percent of population to reproduce is 0 < x < 100."
		   ,value);
	   exit(1);
	  }
	else psel = check1;
       }

else if( !strcmp( name, par5 ) )
       {
	check1 = atoi( value );
	if( (check1 <= 0) || (check1 > 100) )
	  {
       fprintf(stderr,"\n\"%s\"! Sorry, allowable range of integer percent of individuals to check they to pattern is 0 < x < 100."
		   ,value);
	   exit(1);
	  }
    else pattern_percent = check1;
       }

else if( !strcmp( name, par6 ) )
       {
	check2 = atof( value );
	if( (check2 < 0.0)||(check2 > 1.0) )
	  {
	   fprintf(stderr,"\n\"%s\" is bad crossover rate (P/bit)/generation ",value);
	   exit(1);
	  }
	else pcross = check2;
       }

else if( !strcmp( name, par7 ) )
       {
	check2 = atof( value );
	if( (check2 < 0.0)||(check2 > 1.0) )
	  {
	   fprintf(stderr,"\n\"%s\" is bad mutation rate (P/bit)/generation ",value);
	   exit(1);
	  }
	else pmutate = check2;
       }

else if( !strcmp( name, par8 ) )
       {
	if( strlen(value) >12 )
	  {
	   fprintf(stderr,"\n\"%s\" is bad name of log file",value);
	   exit(1);
	  }
	else {
	       check2 = strlen( value );
	       for ( check1=0; check1<check2; check1++)
	       log_file_name[check1] = value[check1];
	     }
       }

else if( !strcmp( name, par9 ) )
       {
	      if( !strcmp( "0",value ) ) selection_strategy = 0;
	 else if( !strcmp( "1",value ) ) selection_strategy = 1;
	 else
	  {
	   fprintf(stderr,"\n\"%s\" is bad value of selection_strategy",value);
	   exit(1);
	  }
       }

else if( !strcmp( name, par10 ) )
       {
	check1 = atoi( value );
	if( check1 == 0 )
	  {
	   fprintf(stderr,"\n\"%s\" is bad seed number",value);
	   exit(1);
	  }
	else seed_number = check1;
       }

else if( !strcmp( name, par11 ) )
       {
	if( strlen(value) >12 )
	  {
	   fprintf(stderr,"\n\"%s\" is bad name of sys file",value);
	   exit(1);
	  }
	else {
	       check2 = strlen( value );
	       for ( check1=0; check1<check2; check1++)
	       sys_file_name[check1] = value[check1];
	     }
       }

else if( !strcmp( name, par12 ) )
       {
	 check1 = atoi( value );
	 if( check1 < 0 && check1 > n_pop )
	   {
	fprintf(stderr,"\n\"%s\" is bad number of pattern_start value",value);
	    exit(1);
	   }
     else pattern_start = check1;
       }

else if( !strcmp( name, par13 ) )
       {
	  if( !strcmp( "0",value ) ) pattern_print = 0;
     else if( !strcmp( "1",value ) ) pattern_print = 1;
     else if( !strcmp( "2",value ) ) pattern_print = 2;
	 else
	  {
       fprintf(stderr,"\n\"%s\" is bad value of pattern_print(must be 0,1,2)",value);
	   exit(1);
	  }
       }

else if( !strcmp( name, par14 ) )
       {
	  if( !strcmp( "0",value ) ) pattern_on_screen = 0;
     else if( !strcmp( "1",value ) ) pattern_on_screen = 1;
     else if( !strcmp( "-1",value ) ) pattern_on_screen = -1;

	 else
	  {
       fprintf(stderr,"\n\"%s\" is bad value of pattern_on_screen",value);
	   exit(1);
	  }
       }

else if( !strcmp( name, par15 ) )
       {
	if( strlen(value) >12 )
	  {
	   fprintf(stderr,"\n\"%s\" is bad name of history file",value);
	   exit(1);
	  }
	else {
	       check2 = strlen( value );
	       for ( check1=0; check1<check2; check1++)
	       his_file_name[check1] = value[check1];
	     }
       }

else if( !strcmp( name, par16 ) )
       {
	 check1 = atoi( value );
	 if( check1 <= 0 && check1 > n_pop )
	   {
	fprintf(stderr,"\n\"%s\" is bad number of pattern_stop value",value);
	    exit(1);
	   }
     else  pattern_stop = check1;
       }

else if( !strcmp( name, par17 ) )
       {
	if( strlen(value) >12 )
	  {
	   fprintf(stderr,"\n\"%s\" is bad name of map file",value);
	   exit(1);
	  }
	else {
	       check2 = strlen( value );
	       for ( check1=0; check1<check2; check1++)
	       map_file_name[check1] = value[check1];
	     }
       }

else if( !strcmp( name, par18 ) )
       {
	check1 = atoi( value );
	if( (check1 <= 0) || (check1 > N_STATES) )
	  {
	   fprintf(stderr,"\n\"%s\"! Sorry, allowable range of pattern length is 0 < x < (number of states)."
		   ,value);
	   exit(1);
	  }
	else start_pattern_strategy = check1;
       }

else if( !strcmp( name, par19 ) )
       {
	check1 = atoi( value );
	if( (check1 <= 0) || (check1 > N_STATES) )
	  {
	   fprintf(stderr,"\n\"%s\"! Sorry, allowable range of pattern length is 0 < x < (number of states)."
		   ,value);
	   exit(1);
	  }
	else stop_pattern_strategy = check1;
       }

else if( !strcmp( name, par20 ) )
       {
	check1 = atoi( value );
	if( (check1 <= 0) || (check1 > 100) )
	  {
       fprintf(stderr,"\n\"%s\"! Sorry, allowable range of integer percent of pairs of individuals to check they to pattern is 0 < x < 100."
		   ,value);
	   exit(1);
	  }
    else pattern_pair_percent = check1;
       }

    else if( !strcmp( name, par21 ) )
       {
	  if( !strcmp( "0",value ) ) ant_strategy = 0;
     else if( !strcmp( "1",value ) ) ant_strategy = 1;
	 else
	  {
       fprintf(stderr,"\n\"%s\" is bad value of ant_strategy",value);
	   exit(1);
	  }
       }

    else if( !strcmp( name, par22 ) )
	{
	      if( !strcmp( "0",value ) ) pattern_pair = 0;
	 else if( !strcmp( "1",value ) ) pattern_pair = 1;
	 else
	  {
	   fprintf(stderr,"\n\"%s\" is bad value of pattern_pair",value);
	   exit(1);
	  }
       }

else if( !strcmp( name, par23 ) )
       {
	      if( !strcmp( "0",value ) ) test_run = 0;
	 else if( !strcmp( "1",value ) ) test_run = 1;
	 else
	  {
	   fprintf(stderr,"\n\"%s\" is bad value of selection_strategy",value);
	   exit(1);
	  }
       }

else {
       fprintf(stderr,"\nUndefined parameter \"%s\" in input file",name);
       exit(1);
     }

 params_in_file++;
}
/*////////////////////////////////////////////////////////////////////*/


void read_map()
{
  extern int **map;
  extern int MAP_ROWS;
  extern int MAP_COLS;
  int i,j,kk;
  unsigned char k,k1;

/*  //strcpy(map_file_name,"");
  //strcpy(map_file_name, filename); */
  map_file = fopen(map_file_name, "r");
  if (map_file == NULL)
  {
    fprintf(stderr," Couldn't open map file!");
    map_file_name[0] = 0;
    exit(0);
  }

     fscanf ( map_file, "%d %d", &MAP_COLS, &MAP_ROWS );
     if ( MAP_COLS < 1 || MAP_ROWS < 1 )
	 {
	    fprintf( stderr, "bad trail size." );
	    exit(1);
	 }

     map = (int **)malloc ( MAP_ROWS * sizeof ( int * ) );
     for ( i = 0; i < MAP_ROWS; ++i )
     {
	  map[i] = (int *)malloc ( MAP_COLS *
					      sizeof ( int ) );
	  memset ( map[i],'0', MAP_COLS );
     }
     i = 0;
     while ( (kk=fgetc(map_file))!='\n' && kk != EOF );


     for (j=0; j < MAP_ROWS; j++ )
     {
       while ( i < MAP_COLS )
	{
	  fscanf( map_file,"%c",&k );
	  fscanf( map_file,"%c",&k1 );
      /*//  printf("%c%c",k,k1);

	 // if(k==' ')
	 // kk=k1-48;*/

	 /* else */ kk=(k-48)*10+(k1-48);

	  map[j][i] = kk;
    /*//	  printf(" map[%d][%d]=%d\n",j,i,map[j][i]); */
	  i++;
	}
	fscanf(map_file,"\n");
	i=0;
      }
    }



