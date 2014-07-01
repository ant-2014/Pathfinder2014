#include "ant.h"
#include <stdarg.h>
#include <conio.h>
#include <alloc.h>
extern char log_file_name[80];
extern FILE *log_file;
extern char sys_file_name[80];
extern FILE *sys_file;
extern char his_file_name[80];
extern FILE *his_file;
extern int **map;
extern int MAP_ROWS,MAP_COLS;
extern int test_run;
/*///////////////////////////////////////////////// */
void oprintf( int screen_print, FILE *stream, char *format, ... )
{
  va_list argptr;
  va_start( argptr, format );

  if(screen_print==-1)return;

  if( screen_print )
   {
    vprintf( format,argptr );
   }

  if( stream !=NULL )
  {
   vfprintf( stream,format,argptr );
  }
  va_end( argptr );
}
/*/////////////////////////////////////////////////// */
void print_movename(int on_screen,int output_code)
{
  switch (output_code & MOVE_MASK)
  {
    case NO_MOVE    : oprintf(on_screen,his_file,"NOP"); break;
    case MOVE_FWD   : oprintf(on_screen,his_file,"FWD"); break;
    case MOVE_RIGHT : oprintf(on_screen,his_file,"RGT"); break;
    case MOVE_LEFT  : oprintf(on_screen,his_file,"LFT"); break;
  }
}

/**************************************************************************/
/** Print a gene:  */
void print_gene(int on_screen,int format,State_machine huge *p_gene)
{
  int i,j;
  int code, next;

  oprintf(on_screen,his_file,"Input =      0      1\n");
  oprintf(on_screen,his_file,"            ====== ======\n");

  for (i=0 ; i < N_STATES ; i++)
  {
    if(format==0)
    oprintf(on_screen,his_file,"State %02X : ", i);
    else
    oprintf(on_screen,his_file,"State %d : ", i);

    for ( j=0 ; j < 2 ; j++ )
    {
      run_machine(p_gene, i, j, &code, &next);
      if(format==1) oprintf(on_screen,his_file,"  ");
      print_movename(on_screen,code);
      if(format==0)
      oprintf(on_screen,his_file,"/%02X ", (next));
      else
      oprintf(on_screen,his_file,"/%d ", (next));

    }
    oprintf(on_screen,his_file,"\n");
  }
}
/**************************************************************************/
void print_gene_track( Gene_track huge *g,int on_screen )
{
  int i;

  oprintf(on_screen,his_file,"Score = %d\n", g->score);
  if(on_screen!=0) oprintf(on_screen,his_file,"n_children = %d\n", g->n_children);
  oprintf(on_screen,his_file,"Gene = ");
  for (i=0 ; i<GENE_SIZE ; i++)
    oprintf(on_screen,his_file,"%02X", (char)(g->p_gene[i]) & 0xff);
  oprintf(on_screen,his_file,"\n");
  if(on_screen==0) oprintf(0,his_file,"\n");

}

/**************************************************************************/
void print_pop_stats( Gene_track huge *gene_track_array, int n_pop)
{
  int score_bin[90];
  int i;

  /* Initialize */
  for ( i=0 ; i<90 ; i++ )
    score_bin[i] = 0;

  /* Count population with given score */
  for ( i=0 ; i<n_pop ; i++ )
    score_bin [ (gene_track_array[i].score) ] ++;

  /* Print out the results : */
  oprintf(1,his_file,"\nPopulation Statistics :\n");
  oprintf(1,his_file,"%8s %8s | %8s %8s | %8s %8s\n", "SCORE", "N_POP", "SCORE", "N_POP",
				      "SCORE", "N_POP");
  for ( i=0 ; i<90 ; i += 3 )
    oprintf(1,his_file,"%8d %8d | %8d %8d | %8d %8d\n", i, score_bin[i],
					i+1, score_bin[i+1],
					i+2, score_bin[i+2]);
}

/**************************************************************************/
int find_ant_w_score( Gene_track huge *gene_track_array, int n_pop,
		      int score, int start_with )
{
  int i;

  for ( i=start_with ; i < n_pop ; i++ )
    if (gene_track_array[i].score == score)
      return(i);

  /* None found */
  return(-1);
}
/**************************************************************************/
void draw_trail(void)
{
  int i,j;

  textmode(64); /* Replaced "C4350" with "64", since it wasn't defined*/
  textbackground(LIGHTGRAY);
  clrscr();

  for (j=0; j<MAP_ROWS; j++)
  {
    for (i=0; i<MAP_COLS; i++)
      if (map[j][i])
      {
	textcolor(BLUE);
	putch(254);
      }
      else
      {
	textcolor(BLACK);
	putch(250);
      }
    gotoxy(1,j+2);
  }
}

/**************************************************************************/
void draw_ant(Ant *p_ant)
{
  int ch;

  gotoxy(p_ant->x + 1, p_ant->y + 1);
  textcolor(RED);  /* It's a RED ANT! */
  switch(p_ant->heading)
  {
    case (NORTH) : ch = 30 ; break;
    case (EAST)  : ch = 16 ; break;
    case (SOUTH) : ch = 31 ; break;
    case (WEST)  : ch = 17 ; break;
  };
  putch(ch);

  gotoxy(35,3);
  oprintf(1,his_file,"Score = (dec)%02d", p_ant->score);
  gotoxy(35,4);
  oprintf(1,his_file,"State = (hex)%02x", p_ant->cur_state);
  gotoxy(35,5);
  oprintf(1,his_file,"Posn  = (%02d,%02d)", p_ant->x, p_ant->y);
}

/**************************************************************************/
void erase_ant(Ant *p_ant)
{
  int x = p_ant->x;
  int y = p_ant->y;

  gotoxy(x + 1, y + 1);
  if (map[y][x])
  {
    textcolor(BLUE);
    putch(254);
  }
  else
  {
    textcolor(BLACK);
    putch(250);
  }
}
/*************************************************************************/
void examine(Gene_track huge *gene_track_array, int n_pop, int n_life_steps,
	     int n_gen, int gen_number, int n_select, int selection_strategy,
	     double pcross, double pmutate)
{
  int n,j;
  int s,a;
  char save_file_name[100];


  while (1)
  {
    printf("\nEXAMINE :\n");

    printf("0 : Return to evolution\n");
    printf("1 : View an ant in action\n");
    printf("2 : View an ant's gene\n");
    printf("3 : View an ant's tracking information\n");
    printf("4 : View the score distribution of the entire population\n");
    printf("5 : Find an ant with a particular score\n");
    printf("6 : Save the current state of the experiment\n");
    printf("7 : Print ants info to that moment into history file \n");
    printf("8 : Exit\n");

ENTER_LOOP :
    printf("\nEnter your choice :");
    scanf("%d", &n);

    if ((n<0)||(n>8))
    {
      printf("Invalid\n");
      /*n=NULL; */
      goto ENTER_LOOP;
    }

    switch(n)
    {
      case(0) :
	return;

      case(1) :
	printf("Enter ant's # (0-%d)", n_pop - 1);
	scanf("%d", &n);
	draw_trail();
	ant_life((State_machine *)(gene_track_array[n].p_gene),
		 n_life_steps, 1);
	textmode(C80);
	textcolor(BLACK);
	break;

      case(2) :
	printf("Enter ant's # (0-%d)", n_pop - 1);
	scanf("%d", &n);
	oprintf(0,his_file,"------------------------------------------------------\n");
	oprintf(0,his_file,"   Generation = [%d], Ant #[%d]\n",gen_number,n);
	print_gene(1,0,(State_machine *)(gene_track_array[n].p_gene));
	oprintf(0,his_file,"------------------------------------------------------\n");
	break;

      case(3) :
	printf("Enter ant's # (0-%d)", n_pop - 1);
	scanf("%d", &n);
	oprintf(0,his_file,"------------------------------------------------------\n");
	oprintf(0,his_file,"   Generation = [%d], Ant #[%d]\n",gen_number,n);
	print_gene_track(&(gene_track_array[n]), 1);
	oprintf(0,his_file,"------------------------------------------------------\n");
	break;

      case(4) :
	oprintf(0,his_file,"------------------------------------------------------\n");
	oprintf(0,his_file,"   Generation = [%d], Ant #[%d]\n",gen_number,n);
	print_pop_stats( gene_track_array, n_pop);
	oprintf(0,his_file,"------------------------------------------------------\n");
	break;

      case(5) :
	printf("Enter ant's # to begin searching from (0-%d) : ", n_pop - 1);
	scanf("%d", &n);
	oprintf(0,his_file,"------------------------------------------------------\n");
	oprintf(0,his_file,"   Generation = [%d], Ant #[%d]\n",gen_number,n);
	oprintf(1,his_file,"Enter desired score : ");
	scanf("%d", &s);
	oprintf(0,his_file, "%d\n",s);
	a = find_ant_w_score( gene_track_array, n_pop, s, n);
	if (a >= 0)
	 oprintf(1,his_file,"Ant #%d has score %d\n", a, s);
	else
	 oprintf(1,his_file,"No more ants in list with score %d\n", s);
	break;

      case (6) :
	printf("Enter the name of the save file : ");
	scanf("%s", save_file_name);
	save_state(save_file_name, n_pop, n_gen, gen_number, n_life_steps,
		   n_select, selection_strategy, pcross, pmutate,
		   gene_track_array);
	break;

      case(7):
      oprintf(0,his_file,"   Generation = [%d]\n",gen_number,n);
      oprintf(0,his_file,"________________________________________________________________\n");
      break;

      break;

      case(8):
	 fprintf(stderr,"\n---------------------- Forced end of experiment ! --------------");
	 free_all(&gene_track_array,1);
	 close_log(1);
	 close_sys(1);
	 close_his(1);
	 exit(1);
	 break;
    }
  }
}


/**************************************************************************/
int open_log(char *filename, int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number)
{
  extern FILE *log_file;
  time_t t_now;
  char *time_string;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  strcpy(log_file_name, filename);
  log_file = fopen(filename, "w");
  if (log_file == NULL)
  {
    fprintf(stderr," Couldn't open log file!");
    log_file_name[0] = 0;
    return(1);
  }

  fprintf(log_file, "Record of GA experiment performed on %s\n\n",
	  time_string);

  oprintf(0,log_file, "PARAMETERS :\n");
  oprintf(0,log_file, "Random number seed     = %u\n", seed_number);
  oprintf(0,log_file, "Population Size        = %d\n", n_pop);
  oprintf(0,log_file, "Number of Generations  = %d\n", n_generations);
  oprintf(0,log_file, "Lifespan               = %d\n", n_life_steps);
  oprintf(0,log_file, "Size of gene           = %d\n ", GENE_SIZE);
  oprintf(0,log_file, "Size of gene track     = %d\n", GENE_TRACK_SIZE);
  oprintf(0,log_file, "Selection Fraction     = %d%%\n", n_select);
  oprintf(0,log_file, "Selection Strategy     = %s\n",
	  selection_strategy ? "ROULETTE" : "TRUNCATE");
  oprintf(0,log_file, "Crossover Rate         = %.3lf\n", pcross);
  oprintf(0,log_file, "Mutation Rate          = %.3lf\n\n", pmutate);

  fprintf(log_file, "%8s %8s %8s %8s\n", "GEN#", "MIN", "AVG", "MAX");

  return(0);
}

/**************************************************************************/
void close_log(int stop)
{
  extern FILE *log_file;
  time_t t_now;
  char *time_string;

  if (log_file == NULL)
    return;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  if( stop )
  fprintf(log_file, "\nForced end of experiment at %s\n", time_string);

  if( !stop )
  fprintf(log_file, "\nEnd of experiment at %s\n", time_string);

  fclose(log_file);
}

/**************************************************************************/
void log_gen(int gen, int min, int max, double avg)
{
  extern FILE *log_file;
  if (log_file == NULL)
    return;

  fprintf(log_file, "%8d %8d     %.2f %8d\n", gen, min, avg, max);
  flush(log_file);
}
/***********************************************************************/
int open_sys(char *filename,int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number)
{
  extern FILE *sys_file;
  time_t t_now;
  char *time_string;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  strcpy(sys_file_name, filename);
  sys_file = fopen(filename, "w");
  if (sys_file == NULL)
  {
    fprintf(stderr," Couldn't open sys file!\n");
    sys_file_name[0] = 0;
    return(1);
  }

  fprintf(sys_file, "Full record of GA experiment performed on %s\n\n",
	  time_string);

if(test_run==0)
{
  oprintf(1,sys_file,"PARAMETERS :\n");
  oprintf(1,sys_file,"Random number seed     = %u\n", seed_number);
  oprintf(1,sys_file,"Population Size        = %d\n", n_pop);
  oprintf(1,sys_file,"Number of Generations  = %d\n", n_generations);
  oprintf(1,sys_file,"Lifespan               = %d\n", n_life_steps);
  oprintf(1,sys_file,"Size of gene           = %d\n", GENE_SIZE);
  oprintf(1,sys_file,"Size of gene track     = %d\n", GENE_TRACK_SIZE);
  oprintf(1,sys_file,"Selection Fraction     = %d%%\n", n_select);
  oprintf(1,sys_file,"Selection Strategy     = %s\n",
	  selection_strategy ? "ROULETTE" : "TRUNCATE");
  oprintf(1,sys_file,"Crossover Rate         = %.3lf\n", pcross);
  oprintf(1,sys_file,"Mutation Rate          = %.3lf\n\n", pmutate);
}
  return(0);
}

/**************************************************************************/
void close_sys(int stop)
{
  extern FILE *sys_file;
  time_t t_now;
  char *time_string;

  if (sys_file == NULL)
    return;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  if( stop )
  fprintf(sys_file, "\nForced end of experiment at %s\n", time_string);

  if( !stop )
  fprintf(sys_file, "\nEnd of experiment at %s\n", time_string);

  fclose(sys_file);
}
/***********************************************************************/
int open_his(char *filename,int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number)
{
  extern FILE *his_file;
  time_t t_now;
  char *time_string;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  strcpy(his_file_name, filename);
  his_file = fopen(filename, "w");
  if (his_file == NULL)
  {
    fprintf(stderr," Couldn't open history file!\n");
    his_file_name[0] = 0;
    return(1);
  }

  print_trail();
  return(0);
}

/**************************************************************************/
void close_his(int stop)
{
  extern FILE *his_file;
  time_t t_now;
  char *time_string;

  if (his_file == NULL)
    return;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  if( stop )
  fprintf(his_file, "\nForced end of experiment at %s\n", time_string);

  if( !stop )
  fprintf(his_file, "\nEnd of experiment at %s\n", time_string);

  fclose(his_file);
}

/*////////////////////////////////////////////////////////////////// */
void print_trail ()
{
     extern FILE *his_file;
     int i, j;

     oprintf(0,his_file,"------------------------ The Map --------------------------\n");
     for ( j = 0; j < MAP_ROWS; ++j )
     {
	  oprintf ( 0,his_file, "     " );
	  for ( i = 0; i < MAP_COLS; ++i )
	     {
	       if( map[j][i] < 10)
	       oprintf ( 0,his_file, " ");
	       oprintf ( 0,his_file, "%d", map[j][i] );
	     }
	  oprintf ( 0, his_file, "\n" );
     }
     oprintf(0,his_file,"-----------------------------------------------------------\n");
}







