#include "ant_def.h"
/************************************************************/
/*static int Debug = 1;*/
static int keypressed = FALSE;
/************************************************************/
typedef struct td_state
{
  /* for each input code, there is an output code and a next state */
  char code_next_state [ N_INPUT_CODES ];
} State;
/************************************************************/
typedef struct td_state_machine
{
  State state [ N_STATES ];
} State_machine;

#define GENE_SIZE sizeof(State_machine)

/* A structure to associate a gene with its score. */
typedef struct td_gene_track
{
  int score;
  int n_children;
  char huge *p_gene;
} Gene_track;

#define GENE_TRACK_SIZE sizeof(Gene_track)

/**************************************************************************/
typedef struct td_ant
{
  int score;            /* starts at zero */
  int cur_state;        /* starts at zero */
  int x,y;              /* position; starts at (0,0) */
  int heading;          /* E=0, N=1, W=2, S=3; starts at E */
} Ant;

/***************** functions prototype ****************************/
/*ant.c*/
 void init_ant(Ant *p_ant);
 void score_ant(Ant *p_ant);
 void move_ant(Ant *pant, int output_code);
 int ant_input(Ant *pant);

 double select(Gene_track huge *gene_track_array, int n_pop, int n_select,
	      int selection_strategy, int *r_max, int *r_min);
 void geom_incr(int *px, int *py, int head);
 void update_ant( Ant *p_ant, State_machine huge *p_gene, int draw_it );
 int ant_life(State_machine huge *p_gene, int n_cycles, int draw_it);
 void expose(Gene_track huge *gene_track_array, int n_pop, int n_steps);
 void random_search(unsigned long n_passes, int n_life_steps);

/* main.c*/
 void evolve(Gene_track huge *gene_track_array, int n_pop, int n_generations,
	    int n_life_steps, int n_select, double pcross, double pmutate,
	    int selection_strategy);
 int getyn(void);
 void flush(FILE *stream);

/*reproduc.c*/
 void crossover(char huge *p_gene1, char huge *p_gene2, double pcross);
 void mutate(char huge *p_gene);
 int reproduce(Gene_track huge *gene_track_array, int n_pop,
	      double pcross, double pmutate);

/*print.c*/
 void oprintf( int screen_print, FILE *stream, char *format, ... );
 void print_movename(int on_screen,int output_code);
 void print_gene(int on_screen,int format,State_machine huge *p_gene);
 void print_gene_track( Gene_track huge *g , int on_screen);
 void print_pop_stats( Gene_track huge *gene_track_array, int n_pop);
 int find_ant_w_score( Gene_track huge *gene_track_array, int n_pop,
		      int score, int start_with );
 void draw_trail(void);
 void draw_ant(Ant *p_ant);
 void erase_ant(Ant *p_ant);
 int save_state(char *save_file_name, int n_pop, int n_generations,
	       int gen_number, int n_life_steps, int n_sel,
	       int selection_strategy, double pcross, double pmutate,
	       Gene_track huge *gene_track_array);
 void examine(Gene_track huge *gene_track_array, int n_pop, int n_life_steps,
	      int n_gen, int gen_number, int n_select, int selection_strategy,
	     double pcross, double pmutate);
 int open_log(char *filename, int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number);
 void close_log(int stop);
 void log_gen(int gen, int min, int max, double avg);
 int open_sys(char *filename, int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number);
 void close_sys(int stop);
 int open_his(char *filename, int n_pop, int n_generations,
	     int n_life_steps, int n_select, double pcross, double pmutate,
	     int selection_strategy, unsigned seed_number);
 void close_his(int stop);
 void print_trail ();


/*genes.c*/
 void run_machine(State_machine huge *s, int this_state, int in,
		 int *r_out, int *r_next);
 void randomize_gene(char huge *p_gene);
 int froll(double threshold);
 int initialize_gene_pool(Gene_track huge **gene_track_array_ptr, int n_pop);
 void free_all(Gene_track huge **gene_track_array_ptr,int free_map );

/*input.c*/
 void process_commandline ( int argc, char **argv );
 void initialize_parameters ( void );
 void free_parameters ( void );
 void add_parameter ( char *name, char *value, int copyflags );
 int delete_parameter ( char *name );
 void read_parameter_database ( FILE *f );
 void read_parameter_file ( char * );
 int delete_comment ( char * );
 int check_continuation ( char * );
 int parse_one_parameter ( char *buffer );
 int trim_string ( char *string );

/*pattern.c*/
void Get_pattern(Gene_track huge *gene_track_array);
void Get_pattern_2(Gene_track huge *gene_track_array);
