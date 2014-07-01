#include "ant.h"
/**************************************************************************/
/** Reproduce the population.  */
int reproduce(Gene_track huge *gene_track_array, int n_pop,
	      double pcross, double pmutate)
{
  extern FILE *sys_file;
  extern int pattern_on_screen;
  int i,j,k,n;
  int count1=0;
  int count2=0;
  int count3=0;

  int nmutations = (int)(n_pop * pmutate * GENE_SIZE);

  /* Build all children. */
  for ( i=0, j=0 ; (i<n_pop)&&(j<n_pop) ; i++)
  {

    if (gene_track_array[i].n_children <= 0)
       continue;


    for (n = gene_track_array[i].n_children ; n > 0 ; n--)
    {
      /* Search for a place to build a child */
      for ( ; (gene_track_array[j].n_children != 0)&&(j<n_pop) ;
	    j++ );

      if ( j == n_pop ) continue;


      /* Copy it: */
      memcpy( (void *)(gene_track_array[j].p_gene),
	      (void *)(gene_track_array[i].p_gene),
	      (size_t)GENE_SIZE );

      /* Overwrite the child as reproduced: */
      gene_track_array[j].n_children = CHILD;

   }
    count1++;
    /* Overwrite the parent as reproduced: */
     gene_track_array[i].n_children = PARENT;

  }

  /* Perform crossover */
  for ( i=0 ; i<(n_pop/2) ; i++ )
  {

    /* Select the parent genes */
    /*Eleminate PARENTs and dead ants */
    while ((j = rand() % n_pop),(gene_track_array[j].n_children == PARENT));
    while ((k = rand() % n_pop),(gene_track_array[k].n_children == PARENT));

    /* Perform crossover on CHILDrens ants */
    count2++;
    crossover(gene_track_array[j].p_gene, gene_track_array[k].p_gene,
	      pcross);

  }

  /* Perform mutation */
  for ( i=0 ; i<nmutations ; i++ )
  {
    /*Eleminate PARENTs and dead ants */
    while ((j = rand() % n_pop),(gene_track_array[j].n_children == PARENT));

    count3++;
    mutate(gene_track_array[ j ].p_gene );
  }

 /* oprintf(0,sys_file,"\n- There are %d overwriten, %d crossovering and %d mutationing ants -", */
  /*		     count1,count2,count3); */

  return(0);
}


/**************************************************************************/
/** Perform crossover for two given pairs of genes */
void crossover(char huge *p_gene1, char huge *p_gene2, double pcross)
{
  int index, bit_number;
  unsigned char char1, char2, mask1, mask2;
  int state = 0;
  int will_cross, where_cross;

  /* To improve speed, we are adopting the simplifying restriction that
     only one crossover occurs per byte in the gene.  Math majors and stats
     wonks will hate the next line : compute the prob. of crossover in one
     byte -- I know it's a bad computation, but it's a good approximation. */
  pcross *= 8.0 ;


  for (index = 0 ; index < GENE_SIZE ; index ++ )
  {
    char1 = p_gene1 [ index ];
    char2 = p_gene2 [ index ];
    mask1 = mask2 = 0;

    if ( will_cross = froll(pcross))
      where_cross = rand() % 8;
    else
      where_cross = -1;

    for ( bit_number = 0 ; bit_number < 8 ; bit_number ++ )
    {
      if (DEBUG)
	printf("gene # %4d bit # %d\n", index, bit_number);

      if ( bit_number == where_cross )
	state = !state ;  /* Switch state */
      mask1 += state << bit_number;
    }
    mask2 = mask1 ^ 0xff;
    p_gene1 [ index ] = (char1 & mask1) | (char2 & mask2);
    p_gene2 [ index ] = (char1 & mask2) | (char2 & mask1);
  }
}

/**************************************************************************/
/** Perform mutation on some given gene */
void mutate(char huge *p_gene)
{
  int stateno, bit;

  stateno = rand() % GENE_SIZE;
  bit = rand() % 8;

  p_gene [ stateno ] ^= ( 1 << bit);
}

/**************************************************************************/
