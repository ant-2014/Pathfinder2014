#include "ant.h"
extern int ant_strategy;
extern int **map;
extern int MAP_COLS,MAP_ROWS;
/**************************************************************************/
/* Select the best genes from the population. */
/* Return average, minimum, and maximum scores. */
double select(Gene_track huge *gene_track_array, int n_pop, int n_select,
	      int selection_strategy, int *r_max, int *r_min)
{
  int i, max_score, min_score;
  int target_score;
  double avg_score;
  int children_to_allocate = n_pop - n_select; /* Keep the pop. constant */
  int children_per_selected = children_to_allocate / n_select ;
  extern FILE *sys_file;

  /* Initialize */
  max_score = 0;
  min_score = 100;
  for (i=0, avg_score = 0.0 ; i<n_pop ; i++)
  {

    avg_score += gene_track_array[i].score;
    gene_track_array[i].n_children = 0;
    if (gene_track_array[i].score > max_score)
      max_score = gene_track_array[i].score;
    if (gene_track_array[i].score < min_score)
      min_score = gene_track_array[i].score;
  }

  /* number of populations without dead ants */
 avg_score /= n_pop;
  *r_max = max_score;
  *r_min = min_score;

  /* Now allocate children */
  while (children_to_allocate > 0)
  {
   /* if (kbhit())
    {
      getch();
      keypressed = TRUE;
      printf(" <%d> " ,children_to_allocate);
    }*/

    for ( target_score = max_score ;
	  (target_score > 0)&&(children_to_allocate > 0) ;
	  target_score-- )

    {
      for ( i = 0 ; i<n_pop ; i++)
      {
	if ( gene_track_array[i].score == target_score )
	{
	  if ((selection_strategy == SELECT_STRATEGY_ROULETTE) &&
	      (rand() % max_score > gene_track_array[i].score))
	      continue;

	  if (children_to_allocate > children_per_selected)
	  {
	    gene_track_array[i].n_children = children_per_selected;
	    children_to_allocate -= children_per_selected;
	  }
	  else
	  {
	    gene_track_array[i].n_children = children_to_allocate;
	    children_to_allocate = 0;
	  }
	}
      }
    }
  }
return(avg_score);
}

/**************************************************************************/
void init_ant(Ant *p_ant)
{
  p_ant->score = 0;
  p_ant->cur_state = 0;
  p_ant->x = 0;
  p_ant->y = 0;
  p_ant->heading = EAST;
}

/**************************************************************************/
void geom_incr(int *px, int *py, int head)
{
  int x = *px;
  int y = *py;

  switch (head)
  {
    case (EAST):
	x++;
	if (x==MAP_COLS)
	  x = 0;
	break;
    case (NORTH):
	y--;
	if (y<0)
	  y = MAP_ROWS - 1;
	break;
    case (WEST):
	x--;
	if (x<0)
	  x = MAP_COLS - 1;
	break;
    case (SOUTH):
	y++;
	if (y==MAP_ROWS)
	  y = 0;
	break;
    default:
       /* error_crash("Confused about heading!"); */
	break;
  };
  *px = x;
  *py = y;

}

/**************************************************************************/
int ant_input(Ant *pant) /* Returns what ant sees */
{
  int x = pant->x;
  int y = pant->y;

  /* What is the square that the ant sees? */
  geom_incr(&x, &y, pant->heading);

 switch(ant_strategy)
 {
  case 0: return(map[y][x] > 0);
  case 1:
	   if(map[y][x] ==  pant->score+1 )
		  return 1;
	   else
		  return 0;

  default: printf("Fatal error! (parameter \"ant_strategy\")");
	   exit(0);
 }

}

/**************************************************************************/
void move_ant(Ant *pant, int output_code)
{
  int head = pant->heading;

  switch (output_code & MOVE_MASK)
  {
    case NO_MOVE:
	break;
    case MOVE_FWD:
	geom_incr(&(pant->x), &(pant->y), head);
	break;
    case MOVE_RIGHT:
	head++;
	if (head > SOUTH)
	  head = EAST;
	pant->heading = head;
	break;
    case MOVE_LEFT:
	head--;
	if (head < EAST)
	  head = SOUTH;
	pant->heading = head;
	break;
  }
}

/**************************************************************************/
/* Score the ant */
void score_ant(Ant *p_ant)
{
  int x = p_ant->x;
  int y = p_ant->y;

  if (p_ant->score + 1 == map[y][x])
    (p_ant->score)++;

  if(ant_strategy==0)
  {
    if (map[y][x] > 0)
	map[y][x] *= -1;
  }
}

/**************************************************************************/

/*********************************************************************/
/* Update the ant, given its gene and its current state */
void update_ant( Ant *p_ant, State_machine huge *p_gene, int draw_it )
{
  int input_code = ant_input(p_ant);
  /* input_code=1,if the square>0 , else =0 */
  int output_code;
  int next_state;

  run_machine(p_gene, p_ant->cur_state, input_code,
	      &output_code, &next_state);

  if (draw_it)
    erase_ant(p_ant);

  /* Move the ant according to the output code */
  move_ant(p_ant, output_code);

  if (draw_it)
    draw_ant(p_ant);

  /* Score the ant */
  score_ant(p_ant);

  /* And reset its state */
  p_ant->cur_state = next_state;
}

/**************************************************************************/
/*
   Given a gene:
   Create an ant, run it through a life cycle,
   and return the score.
*/
int ant_life(State_machine huge *p_gene, int n_cycles, int draw_it)
{
  Ant this_ant;
  int cycle;
  int x,y;
  int single_step = FALSE;

  if (draw_it)
  {
    printf("Do you want to single-step the ant through its life?");
    single_step = getyn();
  }

  if(ant_strategy==0)
  {
    /* Fix map */
    for ( x=0 ; x<MAP_COLS ; x++ )
      for ( y=0 ; y<MAP_ROWS ; y++ )
	if (map[y][x] < 0)
	  map[y][x] *= -1;
   }

  init_ant(&this_ant);

  for (cycle = 0 ; cycle < n_cycles ; cycle++)
  {
    update_ant(&this_ant, p_gene, draw_it);
    if (draw_it)
      delay(100);
    if (single_step)
    {
	while (!kbhit());
	getch();
    }
  }

  if (draw_it)
    gotoxy(1,35);

  return(this_ant.score);
}

/**************************************************************************/
void expose(Gene_track huge *gene_track_array, int n_pop, int n_steps)
{
  int i, score;

  for (i=0 ; i<n_pop ; i++)
  {
   /* if (kbhit())
    {
      keypressed = TRUE;
      getch();
      printf(" <%d> ", i);
    } */


    score = ant_life((State_machine huge *)gene_track_array[i].p_gene,
		      n_steps, 0);
    gene_track_array[i].score = score;
  }
}

/**************************************************************************/
void random_search(unsigned long n_passes, int n_life_steps)
{
  unsigned long score_bins[ 90 ];
  unsigned long i;
  int score;
  State_machine rgene;

  /* Initialize the score bins */
  for ( i=0 ; i<90 ; i++)
    score_bins[i] = 0;

  /* Perform the loop */
  for ( i=0 ; i<n_passes ; i++ )
  {
    randomize_gene((char *)&rgene);
    score = ant_life(&rgene, n_life_steps, 0);
    score_bins[ score ] ++;
    if ( (i % 10) == 0 )
      printf("%lu ", i);
  }

}
