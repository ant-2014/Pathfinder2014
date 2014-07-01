#include "ant.h"
extern int test_run;

static char huge *gene_store;
/*static int gene_number;*/
/*//static int gene_number_store;*/

static char Gene[] = { 0x67,0x4F,0xA2,0x06,0x43,0x64,0x74,0x40,0xD7,
0xB6,0xF0,0x3A,0x88,0x0F,0x7D,0xEE,0xFE,0x70,0xF0,0x77,
0x64,0x03,0x1D,0x05,0x7A,0x0B,0x27,0xA2,0x78,0xA4,0x73,
0x59,0x0F,0x17,0x41,0xB6,0xC2,0xFC,0x09,0x00,0xEE,0x19,
0xF1,0x8A,0xEB,0x70,0x5B,0x59,0xAA,0x8F,0xAF,0x23,0x63,0x7B,
0x80,0x92,0x19,0x76,0xC2,0x50,0xD3,0x94,0x40,0x21 };

/**************************************************************************/
/* Create a test gene */
void randomize_test_gene(char huge *p_gene,int ind_number)
{
  int i,j;
  char tmp;

/*  //set first individuum*/
  if(ind_number==0)
 {
   for (i = 0 ; i < GENE_SIZE ; i++)
   {
     p_gene[i] = Gene[i];
     gene_store[i] = p_gene[i];
   }
/*  // gene_number=0;
  // gene_number_store=0;*/
 }

  if(ind_number>0)
   {
     /*//generate another random byte */
/*Again:  tmp = rand() % 0xff;

     if(tmp == gene_store[gene_number_store]) goto Again;

     for (i = 0 ; i < GENE_SIZE ; i++)
      {
	p_gene[i] = gene_store[i];
	if(i==gene_number_store) p_gene[gene_number_store]=tmp;
      }
     gene_number++;
    if(gene_number==16)
     {
       gene_number_store++;
       gene_number=0;
     } */

/*// tmp = (char)(Gene[test_run-1]) ;*/

 for (i = 0 ; i < GENE_SIZE ; i++)
 {
   /*// tmp = [0..255]
  // tmp = (char) ((tmp+1) % 256);
  // printf("\n%02X ",tmp);
  // getch();*/
   p_gene[i] = gene_store[i];
   /*//if(i==test_run-1) p_gene[test_run-1]=tmp;*/

 }
 if(ind_number==p_gene[test_run-1])
 p_gene[test_run-1] = 0;
 else p_gene[test_run-1] = ind_number;
/*//  printf("\nant#%d  gene#%d  %02X ",ind_number,test_run-1,p_gene[test_run-1]);
//  getch();*/

}
 return(0);
}

/**************************************************************************/

/*************************************************************************/
void run_machine(State_machine huge *s, int this_state, int in,
		 int *r_out, int *r_next)
{
  int code = s->state[this_state].code_next_state[in];

  *r_out = (code & OUTPUT_MASK) >> OUTPUT_SHIFT ;
 /*// printf("\n%x",code & STATE_MASK);getch();*/
  *r_next = code & STATE_MASK ;
}

/**************************************************************************/
/* Create a random gene */
void randomize_gene(char huge *p_gene)
{
  int i;
  for (i = 0 ; i < GENE_SIZE ; i++)
  p_gene[i] = rand() % 0xff;
}
/**************************************************************************/

/* Determine whether a random real number lies within a given threshold. */
int froll(double threshold)
{
  int inum;
  double dnum;

  inum = rand() & 0x3ff /* % 16384 */ ;
  dnum = ((double)inum)/16384.0;

  return(dnum < threshold);
}

/* Create a new random population. */
int initialize_gene_pool(Gene_track huge **gene_track_array_ptr, int n_pop)
{
  Gene_track huge *gtp;
  int i,j;

 /*// if (gene_track_array_ptr == NULL)
  //  error_crash("Hey!  Bad pointer.\n");*/

  gene_store = ((char huge *)malloc(GENE_SIZE));

  gtp = *gene_track_array_ptr = calloc(n_pop, GENE_TRACK_SIZE);

  if (gtp == NULL)
  {
    fprintf(stderr, "\nCan't allocate gene tracking array!\n");
    return(1);
  }

  for (i=0 ; i<n_pop ; i++, gtp++)
  {
    gtp->score = 0;
    gtp->n_children = 0;

    gtp->p_gene = (char huge *)malloc(GENE_SIZE);
    if (gtp->p_gene == NULL)
    {
      fprintf(stderr, "\nCan't allocate gene!\n");
      return(1);
    }
    if(test_run!=0)
      randomize_test_gene(gtp->p_gene,i);
    else
      randomize_gene(gtp->p_gene);
  }
  free(gene_store);
  return(0);
}
/*///////////////////////////////////////////////////*/
void free_all( Gene_track huge **gene_track_array_ptr, int free_map )
{
  extern int n_pop;
  extern int **map;
  extern int MAP_ROWS;
  int i;
  Gene_track huge *gtp;

  gtp = *gene_track_array_ptr;
  for(i=0; i<n_pop; i++, gtp++)  free(gtp->p_gene);
  free(gtp);
  free(*gene_track_array_ptr);

if(free_map==1)
{
  free(map);
  for ( i = 0; i < MAP_ROWS; ++i )
  free(map[i]);
}

 }
