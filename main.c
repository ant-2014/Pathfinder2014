#include "ant.h"
#include <graphics.h>
#include <assert.h>


/***************************************************/
/*//   Define all global parameters*/
int test_run;
int ant_strategy;
/*// The size of population */
int n_pop;
/*// The number of generations*/
int  n_gen;
/*// The max time steps of individual*/
int n_steps;
/*//The integer percent of population to reproduce*/
int psel;
/*// The percent of ants to check they to pattern; */
int pattern_percent;
int pattern_pair_percent;

/*// The crossover rate (P/bit)/generation*/
double pcross;
/*// The  mutation rate (P/bit)/generation */
double pmutate;
/*// The name of the log file*/
 char log_file_name[80];/*// = "ant.log";*/
 FILE *log_file;
/*// The name of the sys file*/
 char sys_file_name[80];/*// = "ant.sys";*/
 FILE *sys_file;
/*// The name of the his file*/
 char his_file_name[80];/*// = "ant.his";*/
 FILE *his_file;
/*// The name of the his file*/
 char map_file_name[80];/*// = "brennan.trl";*/
 FILE *map_file;
 int **map;
 int MAP_COLS;
 int MAP_ROWS;
/*// Default ROULETTE WHEEL selection strategy*/
 int selection_strategy;
/*// Default seed_number  - initialization in module main.c */
 unsigned seed_number; /*// = (unsigned)time(NULL);*/

 int pattern_start;
 int pattern_stop;
 int pattern_pair;

/*//Default pattern_print*/
 int pattern_print;
/*//Default pattern_on_screen*/
 int pattern_on_screen;

 int start_pattern_strategy;
 int stop_pattern_strategy;

 int params_in_file;

 int gen_n;
 int n_sel;
 int Gene;

/****************************************************************************/
int startfromcheckpoint = 0;
Gene_track huge *gene_track_array;

void main ( int argc, char **argv )
{

  /* request auto detection */
  int gdriver = DETECT, gmode, errorcode;
  char str[3];
  int x,y,x1,y1;
/*  //int Score[16231];*/

  int i,j,tmp;
  int error;
  unsigned long mem,old_mem;
  double avg_score;
  int max_score;
  int min_score;


 strcpy(log_file_name,"ant.log");
 strcpy(sys_file_name,"ant.sys");
 strcpy(his_file_name,"ant.his");
 strcpy(map_file_name,"brennan.trl");

 ant_strategy = 0;

 test_run = 0;
 n_pop = 100;
 n_gen = 100;
 n_steps = 100;
 psel = 20;
 pattern_percent =  20;
 pattern_pair_percent = 0;
 start_pattern_strategy = 5;
 stop_pattern_strategy = 8;
 pcross = 0.01;
 pmutate = 0.01;
 selection_strategy = 1;
 pattern_start = 0;
 pattern_stop = n_gen;
 pattern_pair = 0;
 pattern_print = 0;
 pattern_on_screen = 0;
 params_in_file = 0;
 MAP_ROWS = 0;
 MAP_COLS = 0;


if(!startfromcheckpoint)
  old_mem = farcoreleft();


  seed_number = (unsigned)time(NULL);
  textmode(C80);
  clrscr();

  process_commandline ( argc, argv );
  read_map(map_file_name);

/*//if(startfromcheckpoint)
//  old_mem = farcoreleft();*/

  /* Reset random number generator */
  srand(seed_number);
/**********************************************************************/
/*//doing test run ,only show the rugged fitness landscape*/
if(test_run == 1)
{
  if (strlen(sys_file_name) > 0)
  {
    open_sys(sys_file_name, n_pop, n_gen, n_steps, psel, pcross, pmutate,
	     selection_strategy, seed_number);
  }

  if (strlen(his_file_name) > 0)
  {
    open_his(his_file_name, n_pop, n_gen, n_steps, psel, pcross, pmutate,
	     selection_strategy, seed_number);
  }
  else
   oprintf(1,sys_file," No history file.\n");


  oprintf(1,sys_file," Parameter \"test_run\" is equal to 1 => there is test run,");
  oprintf(1,sys_file,"  I put all information in \"%s\" file...And Later Exit!\n",
		       sys_file_name);

  oprintf(1,sys_file, "\n - CREATE THE TEST 64 POPULATIONS CONSISTS FROM 256 ANTS -");

 oprintf(1,sys_file,"\n------------------------------------------------------\n");
 oprintf(0,sys_file,"   Ant #            It's Score\n");

 n_pop=256;
 ant_strategy=1;
 n_steps=300;

/* // 16 times for 1024 = 16384*/
 for( tmp=0; tmp<64; tmp++)
 {

    test_run=tmp+1;
    error = initialize_gene_pool(&gene_track_array, n_pop);
  if (error)
  {
    oprintf(1,sys_file," \nCouldn't build test population, sorry\n");
    exit(0);
  }
/*//  oprintf(1,sys_file, " -expose  set#%d of population\n",tmp);*/
  expose(gene_track_array, n_pop, n_steps);

/*//  oprintf(1,sys_file, " -scoring set#%d of population\n",tmp);*/

/*//  for(i=0;i<n_pop;i++)
 // {
  //    if( i+(test_run-1)*256 % 256 !=0  && i>0)
  //    Score[i+(test_run-1)*256] = gene_track_array[i].score;
   // oprintf(1,sys_file,"------------------------------------------------------\n");
   // oprintf(0,his_file,"-Ant #[%d]\n",i);
   // print_gene_track(&(gene_track_array[i]) , 0 );
   // print_gene(0,1,(State_machine huge *)gene_track_array[i].p_gene);
   // oprintf(1,sys_file,"------------------------------------------------------\n");
   //  oprintf(0,sys_file,"        [%d]            [%d]\n",i+(test_run-1)*256,gene_track_array[i].score);
 // }

// free_all(&gene_track_array,0); */
}

 free_all(&gene_track_array,1);
 oprintf(0,sys_file,"------------------------------------------------------\n");
/************ Draw the rugged landscape ***************/
  /* initialize graphics mode */
/*  initgraph(&gdriver, &gmode, "");
  /* read result of initialization */
/*  errorcode = graphresult();
  if (errorcode != grOk)  /* an error occurred */
/*	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		assert(0);             // return with error code/
	} */

   setcolor(WHITE);
   line(20,getmaxy()-20,630, getmaxy()-20);
   line(20,getmaxy()/2-10,630, getmaxy()/2-10);
   line(20,15,20,470);

   settextstyle(0, HORIZ_DIR, 1);
   outtextxy( 1, 3, " Score" );
   outtextxy( 600, getmaxy()-30 , "Ant#" );
   outtextxy( 600, getmaxy()/2-20 , "Ant#" );

   for(j=513,i=0; j<=1024,i<=510; i=i+30,j=j+30)
   {
    itoa(i,str,10);
    outtextxy(i+20,getmaxy()/2-10+3,str);
    line(i+20,getmaxy()/2-10,i+20,getmaxy()/2-15);

    itoa(j,str,10);
    outtextxy(i+20,getmaxy()-20+6,str);
    line(i+20,getmaxy()-20,i+20,getmaxy()-25);
   }

   for(i=10; i<=70; i=i+5)
   {
    itoa(i,str,10);
    outtextxy(0,getmaxy()/2-10-i*3,str);
	
    /*
	//line(10,getmaxy()/2-i*3,15,getmaxy()/2-i*3);
	*/

    outtextxy(0,getmaxy()-20-i*3,str);
    /*
	//line(10,getmaxy()-20-i*3,15,getmaxy()-20-i*3);
	*/
   }
/*
   for(i=0; i<512; i++)
   {
     x = i+20;
     y = getmaxy()/2-10 - Score[i*16] * 3;
     putpixel(x,y,WHITE);

     if(i>0) line(x1,y1,x,y);
     x1=x;
     y1=y;
   }


  for(i=512; i<1024; i++)
   {
     x = i-512+20;
     y = getmaxy() - 15 -  Score[i*16] * 3;
     putpixel(x,y,WHITE);

     if(i>512) line(x1,y1,x,y);
     x1=x;
     y1=y;
   }

   getch();
   getch();
   closegraph();
   clrscr();
*/



  oprintf(1,sys_file, "-exit");
  close_sys(0);
  close_his(0);
  exit(0);
}
/**********************************************************************/


  /* Open sys file */
  if (strlen(sys_file_name) > 0)
  {
    open_sys(sys_file_name, n_pop, n_gen, n_steps, psel, pcross, pmutate,
	     selection_strategy, seed_number);
  }

if( start_pattern_strategy > stop_pattern_strategy)
  {
      oprintf(1,sys_file,"Start_pattern_strategy(now=%d) must be <= Stop_pattern_strategy(now=%d)...Exit!\n",
			  start_pattern_strategy,stop_pattern_strategy);
      exit(0);
  }

if(!startfromcheckpoint)
  oprintf(1,sys_file,"(I have read %d parameters from input file.Others are default)\n",params_in_file);

/*//if(startfromcheckpoint)
//  oprintf(1,sys_file,"(I have read all parameters from checkpoint file.\n"); */

  oprintf(0,sys_file," The name of log file   = \"%s\"\n",log_file_name);
  oprintf(0,sys_file," The name of sys file   = \"%s\"\n",sys_file_name);
  oprintf(0,sys_file," The name of his file   = \"%s\"\n",his_file_name);
  oprintf(0,sys_file," The trail is from file   \"%s\"\n",map_file_name);

  if( !startfromcheckpoint )
{
  mem = farcoreleft();
  oprintf(1,sys_file," Memory available = %lu\n",mem);

  oprintf(1,sys_file," Building a new population...");
  error = initialize_gene_pool(&gene_track_array, n_pop);
  if (error)
  {
    oprintf(1,sys_file," \nCouldn't build your population, sorry\n");
    exit(0);
  }
  mem = farcoreleft();
  oprintf(1,sys_file," Memory available = %lu\n",mem);


  n_sel = (int)(((long)n_pop * psel) / 100);
}

  oprintf(1,sys_file," %d individuals will survive each generation.\n",
	 n_sel);

if(pattern_start!=0)
{
  oprintf(1,sys_file," From %d generation I will begin pattern checking.\n",
	  pattern_start);

  oprintf(1,sys_file," The percent of ants to check they to virus = %d.\n",
	  pattern_percent);

  tmp = (int)(((long)n_pop * pattern_percent) / 100);
  oprintf(1,sys_file," %d individuals will check to pattern each generation.\n",
	 tmp);

  tmp = (int)(((long)n_pop * pattern_pair_percent) / 100);
  oprintf(1,sys_file," %d pairs of individuals will check for infection each other each generation.\n",
	 tmp);
}
else  oprintf(1,sys_file," Simple Run without pattern checking.\n");

  if (strlen(log_file_name) > 0)
  {
    open_log(log_file_name, n_pop, n_gen, n_steps, psel, pcross, pmutate,
	     selection_strategy, seed_number);
  }
  else
   oprintf(1,sys_file," No log file.\n");

  if (strlen(his_file_name) > 0)
  {
    open_his(his_file_name, n_pop, n_gen, n_steps, psel, pcross, pmutate,
	     selection_strategy, seed_number);
  }
  else
   oprintf(1,sys_file," No history file.\n");


  evolve(gene_track_array, n_pop, n_gen, n_steps, n_sel, pcross,
	 pmutate, selection_strategy);

  free_all(&gene_track_array,1);
  close_log(0);
  close_sys(0);
  close_his(0);

if(!startfromcheckpoint)
 {
  mem = farcoreleft();
  oprintf(1,sys_file,"\nThere was -%lu- bytes of memory before starting\n",old_mem);
  oprintf(1,sys_file,"Now I free all memory\n");
  oprintf(1,sys_file ,"Not freed memory= -%lu- bytes\n",old_mem - mem);
 }
}

/**************************************************************************/
void evolve(Gene_track huge *gene_track_array, int n_pop, int n_generations,
	    int n_life_steps, int n_select, double pcross, double pmutate,
	    int selection_strategy)
{
  int tmp,j,i,gen,k;
  int startgen = 1;
  double avg_score;
  int max_score;
  int min_score;
  unsigned long mem;
  int percent,old_i,ant_number;

  State_machine huge *check_it;
  fprintf(stderr,
    " Press any key during evolution cycle.  Between select and reproduce,\n");
  fprintf(stderr,
    " The program will pause and allow you to examine the population in detail.\n");
  fprintf(stderr,"--------------------------  hit any key to continue... -------------------\n");
  getch();
/*///////////////////////////////////////////////////*/
  if( startfromcheckpoint )
  {
    startgen = gen_n;
    oprintf(1,sys_file, " -Start from %d generation-\n",startgen);
  }

  for (gen = startgen ; gen <= n_generations ; gen++)
  {
    oprintf(1,sys_file, "--------------------------------------------------------------------------\n");
    oprintf(0,sys_file, "--------------------------------------------------------------------------\n");
    oprintf(1,sys_file, " Gen %d/%d: ", gen, n_generations);

    oprintf(1,sys_file, "-expose");
    expose(gene_track_array, n_pop, n_life_steps);

    oprintf(1,sys_file, "-select");
    avg_score = select(gene_track_array, n_pop, n_select,
		       selection_strategy, &max_score, &min_score);

/*  //mem = farcoreleft();
  //oprintf(1,sys_file," Memory available = %lu\n",mem);*/

tmp = min_score;

    if (keypressed || kbhit())
    {
      getch();
      keypressed = FALSE;
      examine(gene_track_array, n_pop, n_life_steps, n_generations,
	      gen, n_select, selection_strategy, pcross, pmutate);
    }

/************* Pattern checking code ******************************************/
if(pattern_start!=0)
{
 if( pattern_start <= gen && pattern_stop >= gen )
 {
  tmp = min_score;
  if( pattern_start == gen )
  oprintf(1,sys_file,"-Starting of pattern checking-\n");
  if( pattern_stop == gen )
  oprintf(1,sys_file,"-Finishing of pattern checking-\n");

oprintf(pattern_on_screen,his_file,"------------------------------------------\n");
oprintf(pattern_on_screen,his_file, " -GENERATION #%d/%d:\n ", gen, n_gen);
  Get_pattern( gene_track_array );
if(pattern_pair==1) Get_pattern_2( gene_track_array );
 }
}
/*******************************************************************/



  /*//mem = farcoreleft();
  //oprintf(1,sys_file," Memory available = %lu\n",mem);*/

    if (gen != n_generations)
    {
      oprintf(1,sys_file, "-reproduce");
      reproduce(gene_track_array, n_pop, pcross, pmutate);
      oprintf(1,sys_file, "; ");
    }

    min_score = tmp;
    log_gen(gen, min_score, max_score, avg_score);
    oprintf(1,sys_file, "-Min/Avg/Max Score = %d / %.2lf / %d\n", min_score, avg_score, max_score);

  /*//mem = farcoreleft();
  //oprintf(1,sys_file," Memory available = %lu\n",mem);*/


  }
 }
/**************************************************************************/
int getyn(void)
{
  char in;

  for ( in = 0; (in != 'y')&&(in != 'Y')&&(in != 'n')&&(in != 'N') ; )
  {
    while (!kbhit());
    in = getche();
    if (in == 0)  /* Function or cursor key */
      in = getche();
  }

  return( (in == 'y')||(in == 'Y') );
}



/**************************************************************************/
void flush(FILE *stream)
{
     int duphandle;

     /* flush the stream's internal buffer */
     fflush(stream);

     /* make a duplicate file handle */
     duphandle = dup(fileno(stream));

     /* close the duplicate handle to flush the DOS buffer */
     close(duphandle);
}
