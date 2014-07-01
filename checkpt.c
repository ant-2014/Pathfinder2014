#include "ant.h"
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
extern FILE *log_file;
extern char sys_file_name[80];
extern FILE *sys_file;
extern char his_file_name[80];
extern FILE *his_file;
extern char map_file_name[80];
extern FILE *map_file;
extern int selection_strategy;
extern start_pattern_strategy;
extern stop_pattern_strategy;
extern int pattern_start;
extern int pattern_stop;
extern int pattern_print;
extern int pattern_on_screen;
extern int params_in_file;
extern int MAP_ROWS,MAP_COLS;
/*///////////////////////////////////////////////////////////////////*/
int save_state(char *save_file_name, int n_pop, int n_generations,
	       int gen_number, int n_life_steps, int n_sel,
	       int selection_strategy, double pcross, double pmutate,
	       Gene_track huge *gene_track_array)
{
  FILE *savefile;
  time_t t_now;
  char *time_string;
  int i,j;

  t_now = time(NULL);
  time_string = ctime(&t_now);

  savefile = fopen(save_file_name, "w");

  if (savefile == NULL)
  {
    fprintf(stderr, "Sorry, can't open save file\n");
    return(1);
  }

  fprintf(savefile,"ANT-CHECKPOINT-FILE.\n");
  fprintf(savefile,"%d\n", ant_strategy);
  fprintf(savefile,"%d\n", n_pop);
  fprintf(savefile,"%d\n", n_generations);
  fprintf(savefile,"%d\n", gen_number);
  fprintf(savefile,"%d\n", n_life_steps);
  fprintf(savefile,"%d\n", n_sel);
  fprintf(savefile,"%d\n", selection_strategy);
  fprintf(savefile,"%lf\n", pcross);
  fprintf(savefile,"%lf\n", pmutate);
  fprintf(savefile,"%s\n", log_file_name);
  fprintf(savefile,"%d\n",start_pattern_strategy);
  fprintf(savefile,"%d\n",stop_pattern_strategy);
  fprintf(savefile,"%d\n",pattern_percent);
  fprintf(savefile,"%d\n",pattern_pair);
  fprintf(savefile,"%d\n",pattern_pair_percent);
  fprintf(savefile,"%s\n",sys_file_name);
  fprintf(savefile,"%s\n",his_file_name);
  /*fprintf(savefile, "seed_number=%u\n",seed_number);*/
  fprintf(savefile,"%d\n",pattern_start);
  fprintf(savefile,"%d\n",pattern_stop);
  fprintf(savefile,"%d\n",pattern_print);
  fprintf(savefile,"%d\n",pattern_on_screen);
  fprintf(savefile,"%d\n",MAP_ROWS);
  fprintf(savefile,"%d\n",MAP_COLS);
  fprintf(savefile,"%s\n",map_file_name);


  for ( i=0 ; i<n_pop ; i++ )
  {
    fprintf(savefile, "%d\n",gene_track_array[i].score);
    fprintf(savefile, "%d\n",gene_track_array[i].n_children);

    for ( j=0 ; j<GENE_SIZE ; j++)
      fprintf(savefile, "%02X", (char)(gene_track_array[i].p_gene[j]) & 0xff);
    fprintf(savefile, "\n");
  }


  fprintf(savefile, "# Checkpoint of GA Experiment performed on %s\n",
	  time_string);
  fclose(savefile);

  return(0);
}
/**************************************************************************/
void checkpointed_gene(FILE *savefile,char huge *p_gene)
{
  int i;
  for (i = 0 ; i < GENE_SIZE ; i++)
   {
     fscanf(savefile, "%02X", &p_gene[i] );
     p_gene[i] =(char)(p_gene[i] & 0xff );
   }
    fscanf(savefile, "\n");

}
/*//////////////////*/
int read_checkpoint(char *savefile_name,Gene_track huge **gene_track_array)
{
 extern int gen_n;
 extern int n_sel;
 extern int Gene;
 extern int startfromcheckpoint;

 int zanovo;

  Gene_track huge *gtp;
  int i,j;
  FILE *savefile;
  char inp_file_name[100];
  char code[20];
  FILE *inp_file;

  startfromcheckpoint = 1;

  if ((savefile = fopen(savefile_name, "r"))
    == NULL)
{
   fprintf(stderr, "Cannot open checkpoint file.\n");
   exit(1);
}
else
{
   fscanf(savefile,"%s\n",code);
   if(strcmp(code,"ANT-CHECKPOINT-FILE."))
   {
     fprintf(stderr, "This is not a checkpoint file!\n");
     exit(1);
   }
}

 /* if (gene_track_array_ptr == NULL)*/
  /*//  error_crash("Hey!  Bad pointer.\n");*/

  gtp = *gene_track_array = calloc(n_pop, GENE_TRACK_SIZE);

  if (gtp == NULL)
  {
    fprintf(stderr, "\nCan't allocate gene tracking array!\n");
    return(1);
  }

  fprintf(stderr,"Do you want to solve the problem from start (Y)\n");
  fprintf(stderr,"or from check point(with given genes)? (N) ");
  zanovo = getyn();

  if( zanovo == 1 )
  {
   fprintf(stderr,"\nI'll read all parameters except genes from input file\n");
   printf("Enter the name of the input file : ");
   scanf("%s", inp_file_name);

 n_sel = (int)(((long)n_pop * psel) / 100);
 gen_n=1;

  read_parameter_file ( inp_file_name );

  for ( i=0 ; i<23; i++)
  fscanf(savefile,"\n");

 }

 if(zanovo == 0 )
{

  fscanf(savefile,"%d\n", &ant_strategy);

  fscanf(savefile,"%d\n", &n_pop);
 /* printf("%d\n", n_pop);*/

  fscanf(savefile,"%d\n", &n_gen);
 /*// printf("%d\n", n_gen);*/

  fscanf(savefile,"%d\n", &gen_n);
 /*// printf("%d\n", gen_n);*/

  fscanf(savefile,"%d\n", &n_steps);
 /*// printf("%d\n", n_steps);/*

  fscanf(savefile,"%d\n", &n_sel);
/* // printf("%d\n", n_sel);*/

  fscanf(savefile,"%d\n", &selection_strategy);
 /*// printf("%d\n", selection_strategy);*/

  fscanf(savefile,"%lf\n",&pcross);
/* // printf("%lf\n", pcross);*/

  fscanf(savefile,"%lf\n",&pmutate);
 /*// printf("%lf\n", pmutate);*/

  fscanf(savefile,"%s\n",&log_file_name);
/* // printf("%s\n", log_file_name);*/

  fscanf(savefile,"%d\n",&start_pattern_strategy);
 /*// printf("%d\n", start_pattern_strategy);*/

  fscanf(savefile,"%d\n",&stop_pattern_strategy);
/* // printf("%d\n", stop_pattern_strategy);*/

  fscanf(savefile,"%d\n",&pattern_percent);
 /*// printf("%d\n", pattern_percent);*/

  fscanf(savefile,"%d\n",&pattern_pair);
/* // printf("%d\n", pattern_pair);*/

  fscanf(savefile,"%d\n",&pattern_pair_percent);
 /*// printf("%d\n", pattern_pair_percent);*/

  fscanf(savefile,"%s\n",&sys_file_name);
/* // printf("%s\n", sys_file_name);*/

  fscanf(savefile,"%s\n",&his_file_name);
 /*// printf("%s\n", his_file_name);*/

/*  //fscanf(savefile, "seed_number=%u\n",&seed_number);*/

  fscanf(savefile,"%d\n",&pattern_start);
 /*// printf("%d\n", pattern_start);*/

  fscanf(savefile,"%d\n",&pattern_stop);
/* // printf("%d\n", pattern_stop);*/

  fscanf(savefile,"%d\n",&pattern_print);
 /*// printf("%d\n", pattern_print);*/

  fscanf(savefile,"%d\n",&pattern_on_screen);
/* // printf("%d\n", pattern_on_screen);*/

  fscanf(savefile,"%d\n",&MAP_ROWS);
 /*// printf("%d\n", MAP_ROWS);*/

  fscanf(savefile,"%d\n",&MAP_COLS);
/* // printf("%d\n", MAP_COLS);*/

  fscanf(savefile,"%s\n",&map_file_name);
 /*// printf("%s\n", map_file_name);*/

  if( pattern_start <= gen_n && pattern_start!=0 ) pattern_start=gen_n;
}

  for ( i=0 ; i<n_pop ; i++,gtp++ )
  {
/*   // fscanf(savefile, "Gene=%d\n", i);*/

    gtp->p_gene = (char huge *)malloc(GENE_SIZE);
    if (gtp->p_gene == NULL)
    {
      fprintf(stderr, "\nCan't allocate gene!\n");
      free_all(&gene_track_array,1);
      fclose(savefile);
      exit(0);
    }

    fscanf(savefile, "%d\n",&gtp->score);
    fscanf(savefile, "%d\n",&gtp->n_children);

     gtp->p_gene = (char huge *)malloc(GENE_SIZE);
    if (gtp->p_gene == NULL)
    {
      fprintf(stderr, "\nCan't allocate gene!\n");
      return(1);
    }
    checkpointed_gene(savefile,gtp->p_gene);
  }
  fclose(savefile);

  clrscr();

  return(0);
}
/*/////////////////////////////////////////////////////////////////*/
