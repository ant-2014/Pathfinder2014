#include "ant.h"

extern int n_pop;
extern int start_pattern_strategy;
extern int stop_pattern_strategy;
extern int pattern_percent;
extern int pattern_on_screen;
extern int pattern_print;
extern FILE *his_file;

int **pattern;
/*int actions[N_STATES]={1,1,1,1,3,2,1,1,1,1,
		       1,1,1,1,1,1,1,1,1,1,1,
		       1,1,1,1,1,1,1,1,1,1,1};

int exits[N_STATES] = {0,2,3,4,5,6,7,6,1,1,
			19,1,1,1,1,1,1,1,1,4,
			1,1,1,1,1,1,1,1,1,1,5,1};
*/
/** Queue for pattern */
#define QUEUE struct queue
QUEUE
{
  int   info1;
  int   info2;
  QUEUE *next;
};

/***********************************************************************/
/* insert item to queue*/
void  insert( QUEUE **q, int item1, int item2 )
{
   QUEUE *current = *q; /*current  = point to queue head*/
   QUEUE *previos = NULL;
   QUEUE *new_node;

   while( current ) /*search the end of queue*/
   {
     previos = current;
     current = current->next;
   }

   new_node = (QUEUE *)malloc(sizeof(QUEUE));
   new_node->info1 = item1;
   new_node->info2 = item2;

   if( previos ) /*if queue is not empty then add item to tail*/
   {
     new_node -> next = previos -> next;
     previos -> next = new_node;
   }
   else   /*if queue is empty then insert first node to it*/
   {
     *q = new_node;
     (*q)->next = NULL;
   }
}

/*delete the first element from queue*/
int get_head( QUEUE **q, int *error, int *value1 ,int *value2)
{
  QUEUE *old_head = *q;
  *value1 = 0;
  *value2 = 0;

  if( *q )
  {
    *value1 = old_head->info1;
    *value2 = old_head->info2;
    *q = (*q)->next;
    free(old_head);
    *error=0;
   }
  else  { *error=1; return; }
}

/*********************************************************/
void Get_pattern( Gene_track huge *gene_track_array )
{
   int exits[N_STATES];
   int actions[N_STATES];

   int pattern_count=0;
   int pattern_count_NOP=0;
   int pattern_count_limit=0;
   int code,next;

   QUEUE *pat;
   int *temp1;
   int *temp2;

   /*values for queue*/
   int error,val1,val2;
   int queue_count;
   int percent;
   int ant_number;

   /*values for glopal loop*/
   int i,j,k,count,point,flag,per;
   int old_i=-1;
   State_machine huge *set_it;

   int real_exits;
   int real_actions;
   int pattern_flag=0;
   int cycle_flag=0;
   int temp_flag=0;
/*     pattern_on_screen=1;*/
 percent = (int)(((long)n_pop * pattern_percent) / 100);
/*Check "percent" ants to patterns*/

 for(per=0; per<percent; per++)
{
  Again: ant_number = rand() % n_pop;
  /* random number of ant except repeated ants*/
  if(ant_number==old_i) goto Again;
  old_i = ant_number;

    if(pattern_print==1)
    {
       oprintf(pattern_on_screen,his_file,"------------------------------------------");
       oprintf(pattern_on_screen,his_file,"\n-Check Ant #%d to pattern",ant_number);
    }

       set_it = (State_machine huge *)gene_track_array[ant_number].p_gene;


  /*oprintf(1,his_file,"Input =      0      1\n");
  //oprintf(1,his_file,"            ====== ======\n");*/
 for (i=0 ; i < N_STATES ; i++)
  {
  /*oprintf(pattern_on_screen,his_file,"State %d : ", i);*/
      run_machine(set_it, i, 0, &code, &next);
      switch (code & MOVE_MASK)
      {
	case NO_MOVE    : actions[i]=0;/*oprintf(1,his_file,"0");*/ break;
	case MOVE_FWD   : actions[i]=1;/*oprintf(1,his_file,"1");*/ break;
	case MOVE_RIGHT : actions[i]=2;/*oprintf(1,his_file,"2");*/ break;
	case MOVE_LEFT  : actions[i]=3;/*oprintf(1,his_file,"3");*/ break;
	default         : printf("\nFatal error!(0)"); exit(1);
      }
      exits[i]=next;
	  /*
      //oprintf(1,his_file,"/%d ", exits[i]);
      //if(i%2==0)
      //oprintf(1,his_file,"\n");
	  */
  }


   pat = NULL; /*initialize queue*/
  /* delete all nodes without entrances:make the real_ value*/
   real_exits = 0;
   real_actions = actions[0];

   insert(&pat,real_exits,real_actions);
   queue_count=1;

   /*if state 0 has NOP*/
   if(real_actions == 0 )
   {
     get_head(&pat,&error,&val1,&val2);
     pat=NULL;
     queue_count=0;
   }

   /*if state 0 has point to 0*/
   else if( exits[real_exits] == 0)
   {
     /*if on last step we search cycle to own node then rewrite exits-array*/
       for(j=0; j<N_STATES; j++)
       {
	 temp_flag=0;
	 /*is there any entrance to j state?*/
	 for(k=0;k<N_STATES;k++)
	 if(exits[k]==j)
	 {
	   temp_flag=1;
	   break;
	 }

	 /*if we search good next node*/
	 if(temp_flag==0)
	   {
	    temp_flag=2;
	    real_exits=j;
	    real_actions=actions[j];
	    pat=NULL;
	    insert(&pat,real_exits,real_actions);
	    queue_count=1;
	    break;
	  }
       }
  }

pattern_flag=0;
cycle_flag=0;

/************* Begin of the search ************************************/
for(count=1,i=exits[0]; pattern_flag==0 && count<=N_STATES;count++,i=exits[i])
   {
     real_exits = i;
     real_actions = actions[i];

     /* Insert current node in queue*/
METKA:   insert(&pat,real_exits ,real_actions );
	 queue_count++;

/****************************** Check current node to NOP*/
if(real_actions == 0)
    {
	/*if we find NOP & its ok then we find pattern*/
	if(queue_count>=start_pattern_strategy) { pattern_flag=1;
						  cycle_flag=0;
						  continue;}
	/*if we search NOP on litlle length then reset queue*/
	else
	{
	  for(j=queue_count; j>0; j--)
	  get_head(&pat,&error,&val1,&val2);
	  queue_count=0;
	  pat=NULL;
	}
     }

/******************************** if not NOP then we'll search the cycle*/
else
 {
    /*initialize the temp arrays*/
    temp1 = (int *)malloc ( (queue_count) * sizeof ( int ) );
    temp2 = (int *)malloc ( (queue_count) * sizeof ( int ) );
    memset( temp1,'0', (queue_count) * sizeof(int) );
    memset( temp2,'0', (queue_count) * sizeof(int) );

     /*Now compare current node with all nodes in pattern*/
     /* 1). Delete queue and save it*/
     for(j=0; j<queue_count; j++)
    {
      get_head(&pat,&error,&val1,&val2);/*get node from temp queue*/
      /*save the pat-queue in temporary arrays*/
      temp1[j]=val1;
      temp2[j]=val2;
    }
     /*Reset the queue */
     pat=NULL;

     /* 2). Doing compare */
     for(j=0;j<queue_count;j++)
     if(exits[real_exits]==temp1[j]) /*compare current node with all nodes */
     break; /*If there is one cycle then break */

/************   if there is cycle(we doing break from loop) */
/************   & pattern length is good then we search pattern with cycle */
     if(j<queue_count && queue_count>=start_pattern_strategy)
     {
       /*Restore the pattern-queue */
       for(j=0; j<queue_count; j++)
       insert(&pat,temp1[j],temp2[j]);
       cycle_flag=1;
       pattern_flag=1;
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

/*************  else if search the cycle by on little length then reset queue */
    else if(j<queue_count-1 && queue_count<start_pattern_strategy)
     {
       pat=NULL;
       queue_count=0;
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

/************* if we search cycle to own pattern then rewrite exits-array */
    else if(j==queue_count-1 && queue_count<start_pattern_strategy)
    {
     /*if on last step we search cycle to own node then rewrite exits-array */
       for(j=0; j<N_STATES; j++)
       {
	 temp_flag=0;
	 /*is there any entrance to j state? */
	 for(k=0;k<N_STATES;k++)
	 if(exits[k]==j)
	 {
	   temp_flag=1;
	   break;
	 }

	 if(temp_flag==0)
	 {
	  /*is this node in queue or not? */
	  for(k=0;k<queue_count;k++)
	  if(temp1[k]==j)
	  {
	    temp_flag=1;
	    break;
	  }
	 }
	  /*if we search good next node */
	  if(temp_flag==0)
	  {
	    temp_flag=2;
	    count++;
	    i=real_exits=j;
	    real_actions=actions[j];
	    break;
	  }
       }

      pat=NULL;
      queue_count=0;
      /*Free the temp arrays */
      free(temp1);
      free(temp2);
      if(temp_flag==2)goto METKA;
      if(temp_flag==0) { pattern_flag=0;
			 break;
		       }
  }

/***************  else if didn't search pattern then restore queue */
    else
     {
       pat=NULL;
       /*Restore the pattern-queue */
       for(j=0; j<queue_count; j++)
       insert(&pat,temp1[j],temp2[j]);
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

  }
}
/**************** The end of search; */
/*****************************************************************/

if( pattern_flag==0)
 {
   for(j=0;j<queue_count;j++)
   get_head( &pat,&error,&val1,&val2);
   pat=NULL;

   if(pattern_print==1)
   oprintf(pattern_on_screen,his_file,"\nThere are no patterns in gene...\n");
 }

else
{
 pattern_count++;
 /*if find pattern with NOP */
 if(cycle_flag==0)
{
   pattern_count_NOP++;
   /*Print pattern-queue in pattern-array */
/*Allocate memory for pattern-array */
  pattern = (int **)malloc ( queue_count * sizeof ( int * ) );
     for ( i = 0; i < queue_count; ++i )
     {
	  pattern[i] = (int *)malloc ( 2 * sizeof ( int ) );
	  memset ( pattern[i],'0', 2 * sizeof(int) );
     }

   if(pattern_print==1)
   oprintf(pattern_on_screen,his_file,".Find pattern with NOP. Pattern is:\n");

for(i=0;i<queue_count;i++)
 {
     get_head( &pat,&error,&val1,&val2);
     pattern[i][0]=val1;
     pattern[i][1]=val2;

    if(pattern_print==1)
    switch(pattern[i][1])
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     default: printf("\nFatal error!(1)"); exit(1);

    }
 }

 /*Reset queue */
 pat=NULL;


    if(pattern_print==1)
    oprintf(pattern_on_screen,his_file,"Change NOP to node #%d action\n",
	     pattern[queue_count-start_pattern_strategy][0]);

    actions[pattern[queue_count-1][0] ]
	     = actions[ pattern[queue_count-start_pattern_strategy][0] ];

    if(pattern_print==1)
    switch( actions[pattern[queue_count-1][0]] )
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				 pattern[queue_count-1][0],
				 exits[ pattern[queue_count-1][0] ]);

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
				  pattern[queue_count-1][0],
				  exits[ pattern[queue_count-1][0] ]);

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				  pattern[queue_count-1][0],
				  exits[ pattern[queue_count-1][0] ]);

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				  pattern[queue_count-1][0],
				  exits[ pattern[queue_count-1][0] ]);

	      break;

      default: printf("\nFatal error!(2)"); exit(1);


    }


 if(queue_count>=stop_pattern_strategy)
   {
     pattern_count_limit++;
     exits[pattern[queue_count-1][0] ]
	     = pattern[0][0];

  if(pattern_print==1)
  {
     oprintf(pattern_on_screen,his_file,"Pattern consist of more then %d nodes =>\n",stop_pattern_strategy);
     oprintf(pattern_on_screen,his_file,"              => Lock the pattern into it first state.\n");

     switch( actions[pattern[queue_count-1][0] ] )
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				  pattern[queue_count-1][0],
				  pattern[0][0] );

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
				  pattern[queue_count-1][0],
				  pattern[0][0] );
	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				  pattern[queue_count-1][0],
				  pattern[0][0] );
	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				  pattern[queue_count-1][0],
				  pattern[0][0] );
	      break;

    default: printf("\nFatal error!(3)"); exit(1);

    }
   }
  }
}

/*////////////////////////////////////////////////////////////////// */

else  if(cycle_flag==1) /*if find pattern with cycle */
{
    /*Print pattern-queue in pattern-array */
    /*Allocate memory for pattern-array */
  pattern = (int **)malloc ( queue_count * sizeof ( int * ) );
     for ( i = 0; i < queue_count; ++i )
     {
	  pattern[i] = (int *)malloc ( 2 * sizeof ( int ) );
	  memset ( pattern[i],'0', 2 * sizeof(int) );
     }

    if(pattern_print==1)
    oprintf(pattern_on_screen,his_file,".Find pattern with cycle. Pattern is:\n");

for(i=0;i<queue_count;i++)
{
       get_head( &pat,&error,&val1,&val2);
       pattern[i][0]=val1;
       pattern[i][1]=val2;

    if(pattern_print==1)
    switch(pattern[i][1])
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
					  pattern[i][0],
					  exits[ pattern[i][0] ]);

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     default: printf("\nFatal error!(4)"); exit(1);

    }
}
  pat=NULL;

  if(pattern_print==1)
  oprintf(pattern_on_screen,his_file,"Change last action to node #%d action: ",
	     pattern[queue_count-start_pattern_strategy][0]);

   actions[pattern[queue_count-1][0] ]
	     = actions[ pattern[queue_count-start_pattern_strategy][0] ];

   if(pattern_print==1)
   switch( actions[pattern[queue_count-1][0]] )
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				pattern[queue_count-1][0],
				exits[ pattern[queue_count-1][0] ]);

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
			       pattern[queue_count-1][0],
			       exits[ pattern[queue_count-1][0] ]);

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				 pattern[queue_count-1][0],
				 exits[ pattern[queue_count-1][0] ]);

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				 pattern[queue_count-1][0],
				 exits[ pattern[queue_count-1][0] ]);

	      break;
     default: printf("\nFatal error!(5)"); exit(1);

    }


 if(queue_count<stop_pattern_strategy)
 {

   if(pattern_print==1)
   oprintf(pattern_on_screen,his_file,"Change point-cycle to random point:  ");

   flag=0;
   while( !flag)
   {
    flag=1;
    point = rand() % N_STATES;
    for(j=0; j<queue_count; j++)
    if( point == pattern[j][0]) { flag=0; break; }
   }
   exits[pattern[queue_count-1][0]]=point;

   if(pattern_print==1)
   switch( actions[pattern[queue_count-1][0] ] )
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				pattern[queue_count-1][0],
				exits[ pattern[queue_count-1][0] ]);

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file, "state #%d...FWD/#%d\n",
				pattern[queue_count-1][0],
				exits[ pattern[queue_count-1][0] ]);

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				pattern[queue_count-1][0],
				exits[ pattern[queue_count-1][0] ]);

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				pattern[queue_count-1][0],
				exits[ pattern[queue_count-1][0] ]);

	      break;
    default: printf("\nFatal error!(6)"); exit(1);

    }
}

else if(queue_count>=stop_pattern_strategy)
 {
   pattern_count_limit++;

   if(pattern_print==1)
   {
     oprintf(pattern_on_screen,his_file,"Pattern consist of more then %d nodes =>\n",stop_pattern_strategy);
     oprintf(pattern_on_screen,his_file,"              => Lock the pattern into it first state.\n");
   }

     exits[pattern[queue_count-1][0] ]
	     = pattern[0][0];

    if(pattern_print==1)
    switch( actions[pattern[queue_count-1][0] ] )
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				   pattern[queue_count-1][0],
				   pattern[0][0] );

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
				   pattern[queue_count-1][0],
				   pattern[0][0] );

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				   pattern[queue_count-1][0],
				   pattern[0][0] );

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				   pattern[queue_count-1][0],
				   pattern[0][0] );

	      break;
    default: printf("\nFatal error!(7)"); exit(1);

    }
  }
 }


    /* free  the pattern-array */
    for ( i = 0; i < queue_count; ++i )
    free(pattern[i]);
    free(pattern);

/*    oprintf(pattern_on_screen,his_file,"\n\n"); */

/****** Set the gene in new state ********/

     for(i=0;i<N_STATES;i++)
{
      /*Set the gene in new state */
      /*Get the ants number i gene */
      code=set_it->state[i].code_next_state[0];
      /*Set the i aciton - bits 6,7 are actually */
      /* 1). set bits number 6,7 to 0 (00111111 = 63) */
      code=code & 63;
      /* 2). set the action (xx000000) */
      switch ( actions[i] )
      {
	case 0: code=code | 0; break;
	case 1: code=code | 64; break; /* 01000000=64 */
	case 2: code=code | 128; break; /* 10000000=128 */
	case 3: code=code | 192; break; /* 11000000=192 */
	default: oprintf(1,his_file,"Fatal ERROR!(8)"); exit(0);
      }

      /*Set the i next_state - bits 0,1,2,3,4 are actually */
      /* 1). set bits 0,1,2,3,4 to 0 (11100000 = 224 ) */
      code=code & 224;
      /* 2). set next_state */
      code=code | exits[i];

      /*Write code to gene */
       set_it->state[i].code_next_state[0] = code;
  }
  /*print_gene(1,1,(State_machine huge *)gene_track_array[ant_number].p_gene); */

 }
}
if(pattern_print==2)
oprintf(pattern_on_screen,his_file,"Find %d patterns, %d with NOP, %d with cycle; %d have length more than %d\n",
	pattern_count,pattern_count_NOP,pattern_count-pattern_count_NOP,
	pattern_count_limit,stop_pattern_strategy);
return; }
/*********************************************************/
/***********************************************************/
/*********************************************************/
void Get_pattern_2( Gene_track huge *gene_track_array )
{
   extern  int pattern_pair_percent;

   int exits[N_STATES];
   int actions[N_STATES];

   int pattern_count=0;
   int pattern_count_NOP=0;
   int code,next;

   QUEUE *pat;
   int *temp1;
   int *temp2;

   /*values for queue */
   int error,val1,val2;
   int queue_count;
   int percent;
   int ant_number1;
   int ant_number2;

   /*values for glopal loop */
   int i,j,k,count,point,flag,per;
   int old_i=-1;
   State_machine huge *set_it1;
   State_machine huge *set_it2;

   int real_exits;
   int real_actions;
   int pattern_flag=0;
   int cycle_flag=0;
   int temp_flag=0;
/*     pattern_on_screen=1; */
  percent = (int)(((long)n_pop * pattern_percent) / 100);
/*Check "percent" pairs of ants to patterns */

 for(per=0; per<percent; per++)
{
  Again1: ant_number1 = rand() % n_pop;
  /* random number of ant except repeated ants */
  if(ant_number1==old_i) goto Again1;
  old_i = ant_number1;

  ant_number2 = rand() % n_pop;

 if(pattern_print==1)
 {
  oprintf(pattern_on_screen,his_file,"------------------------------------------");
  oprintf(pattern_on_screen,his_file,"\n-Check pair of ant #%d and #%d to pattern",
	  ant_number1,ant_number2);
 }

       set_it1 = (State_machine huge *)gene_track_array[ant_number1].p_gene;
       set_it2 = (State_machine huge *)gene_track_array[ant_number2].p_gene;


  /*oprintf(1,his_file,"Input =      0      1\n"); */
  /*oprintf(1,his_file,"            ====== ======\n"); */
 for (i=0 ; i < N_STATES ; i++)
  {
  /*oprintf(pattern_on_screen,his_file,"State %d : ", i); */
      run_machine(set_it1, i, 0, &code, &next);
      switch (code & MOVE_MASK)
      {
	case NO_MOVE    : actions[i]=0;/*oprintf(1,his_file,"0");*/ break;
	case MOVE_FWD   : actions[i]=1;/*oprintf(1,his_file,"1");*/ break;
	case MOVE_RIGHT : actions[i]=2;/*oprintf(1,his_file,"2");*/ break;
	case MOVE_LEFT  : actions[i]=3;/*oprintf(1,his_file,"3");*/ break;
	default         : printf("\nFatal error!(0)"); exit(1);
      }
      exits[i]=next;
      /*oprintf(1,his_file,"/%d ", exits[i]); */
      /*if(i%2==0) */
      /*oprintf(1,his_file,"\n"); */
  }


   pat = NULL; /*initialize queue */
  /* delete all nodes without entrances:make the real_ value */
   real_exits = 0;
   real_actions = actions[0];

   insert(&pat,real_exits,real_actions);
   queue_count=1;

   /*if state 0 has NOP */
   if(real_actions == 0 )
   {
     get_head(&pat,&error,&val1,&val2);
     pat=NULL;
     queue_count=0;
   }

   /*if state 0 has point to 0 */
   else if( exits[real_exits] == 0)
   {
     /*if on last step we search cycle to own node then rewrite exits-array */
       for(j=0; j<N_STATES; j++)
       {
	 temp_flag=0;
	 /*is there any entrance to j state? */
	 for(k=0;k<N_STATES;k++)
	 if(exits[k]==j)
	 {
	   temp_flag=1;
	   break;
	 }

	 /*if we search good next node */
	 if(temp_flag==0)
	   {
	    temp_flag=2;
	    real_exits=j;
	    real_actions=actions[j];
	    pat=NULL;
	    insert(&pat,real_exits,real_actions);
	    queue_count=1;
	    break;
	  }
       }
  }

pattern_flag=0;
cycle_flag=0;

/************* Begin of the search ************************************/
for(count=1,i=exits[0]; pattern_flag==0 && count<=N_STATES;count++,i=exits[i])
   {
     real_exits = i;
     real_actions = actions[i];

     /* Insert current node in queue */
METKA:   insert(&pat,real_exits ,real_actions );
	 queue_count++;

/****************************** Check current node to NOP */
if(real_actions == 0)
    {
	/*if we find NOP & its ok then we find pattern */
	if(queue_count>=start_pattern_strategy) { pattern_flag=1;
						  cycle_flag=0;
						  continue;}
	/*if we search NOP on litlle length then reset queue */
	else
	{
	  for(j=queue_count; j>0; j--)
	  get_head(&pat,&error,&val1,&val2);
	  queue_count=0;
	  pat=NULL;
	}
     }

/******************************** if not NOP then we'll search the cycle */
else
 {
    /*initialize the temp arrays */
    temp1 = (int *)malloc ( (queue_count) * sizeof ( int ) );
    temp2 = (int *)malloc ( (queue_count) * sizeof ( int ) );
    memset( temp1,'0', (queue_count) * sizeof(int) );
    memset( temp2,'0', (queue_count) * sizeof(int) );

     /*Now compare current node with all nodes in pattern */
     /* 1). Delete queue and save it */
     for(j=0; j<queue_count; j++)
    {
      get_head(&pat,&error,&val1,&val2);/*get node from temp queue */
      /*save the pat-queue in temporary arrays */
      temp1[j]=val1;
      temp2[j]=val2;
    }
     /*Reset the queue */
     pat=NULL;

     /* 2). Doing compare */
     for(j=0;j<queue_count;j++)
     if(exits[real_exits]==temp1[j]) /*compare current node with all nodes */
     break; /*If there is one cycle then break */

/************   if there is cycle(we doing break from loop) */
/************   & pattern length is good then we search pattern with cycle */
     if(j<queue_count && queue_count>=start_pattern_strategy)
     {
       /*Restore the pattern-queue */
       for(j=0; j<queue_count; j++)
       insert(&pat,temp1[j],temp2[j]);
       cycle_flag=1;
       pattern_flag=1;
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

/*************  else if search the cycle by on little length then reset queue */
    else if(j<queue_count-1 && queue_count<start_pattern_strategy)
     {
       pat=NULL;
       queue_count=0;
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

/************* if we search cycle to own pattern then rewrite exits-array */
    else if(j==queue_count-1 && queue_count<start_pattern_strategy)
    {
     /*if on last step we search cycle to own node then rewrite exits-array */
       for(j=0; j<N_STATES; j++)
       {
	 temp_flag=0;
	 /*is there any entrance to j state? */
	 for(k=0;k<N_STATES;k++)
	 if(exits[k]==j)
	 {
	   temp_flag=1;
	   break;
	 }

	 if(temp_flag==0)
	 {
	  /*is this node in queue or not? */
	  for(k=0;k<queue_count;k++)
	  if(temp1[k]==j)
	  {
	    temp_flag=1;
	    break;
	  }
	 }
	  /*if we search good next node */
	  if(temp_flag==0)
	  {
	    temp_flag=2;
	    count++;
	    i=real_exits=j;
	    real_actions=actions[j];
	    break;
	  }
       }

      pat=NULL;
      queue_count=0;
      /*Free the temp arrays */
      free(temp1);
      free(temp2);
      if(temp_flag==2)goto METKA;
      if(temp_flag==0) { pattern_flag=0;
			 break;
		       }
  }

/***************  else if didn't search pattern then restore queue */
    else
     {
       pat=NULL;
       /*Restore the pattern-queue */
       for(j=0; j<queue_count; j++)
       insert(&pat,temp1[j],temp2[j]);
       /*Free the temp arrays */
       free(temp1);
       free(temp2);
       continue;
     }

  }
}
/**************** The end of search; */
/*****************************************************************/

if( pattern_flag==0)
 {
   for(j=0;j<queue_count;j++)
   get_head( &pat,&error,&val1,&val2);
   pat=NULL;

   if(pattern_print==1)
   oprintf(pattern_on_screen,his_file,"\nThere are no patterns in first ants gene...\n");
 }

else
{
  pattern_count++;
   /*if find pattern with NOP */
  if(cycle_flag==0)
 {
    pattern_count_NOP++;
   /*Print pattern-queue in pattern-array */
  /*Allocate memory for pattern-array */
  pattern = (int **)malloc ( queue_count * sizeof ( int * ) );
     for ( i = 0; i < queue_count; ++i )
     {
	  pattern[i] = (int *)malloc ( 2 * sizeof ( int ) );
	  memset ( pattern[i],'0', 2 * sizeof(int) );
     }

   if(pattern_print==1)
   oprintf(pattern_on_screen,his_file,".Find pattern with NOP. Pattern is:\n");

for(i=0;i<queue_count;i++)
 {
     get_head( &pat,&error,&val1,&val2);
     pattern[i][0]=val1;
     pattern[i][1]=val2;

    if(pattern_print==1)
    switch(pattern[i][1])
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
				  pattern[i][0],
				  exits[ pattern[i][0] ]);
	      break;
     default: printf("\nFatal error!(1)"); exit(1);

    }
 }
 /*Reset queue */
 pat=NULL;

}
/*////////////////////////////////////////////////////////////////// */

else  if(cycle_flag==1) /*if find pattern with cycle */
{
    /*Print pattern-queue in pattern-array */
    /*Allocate memory for pattern-array */
  pattern = (int **)malloc ( queue_count * sizeof ( int * ) );
     for ( i = 0; i < queue_count; ++i )
     {
	  pattern[i] = (int *)malloc ( 2 * sizeof ( int ) );
	  memset ( pattern[i],'0', 2 * sizeof(int) );
     }

    if(pattern_print==1)
    oprintf(pattern_on_screen,his_file,".Find pattern with cycle. Pattern is:\n");

for(i=0;i<queue_count;i++)
{
       get_head( &pat,&error,&val1,&val2);
       pattern[i][0]=val1;
       pattern[i][1]=val2;

    if(pattern_print==1)
    switch(pattern[i][1])
     {
     case(0):
	      oprintf(pattern_on_screen,his_file,"state #%d...NOP/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     case(1):
	      oprintf(pattern_on_screen,his_file,"state #%d...FWD/#%d\n",
					  pattern[i][0],
					  exits[ pattern[i][0] ]);

	      break;
     case(2):
	      oprintf(pattern_on_screen,his_file,"state #%d...RGT/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     case(3):
	      oprintf(pattern_on_screen,his_file,"state #%d...LFT/#%d\n",
					   pattern[i][0],
					   exits[ pattern[i][0] ]);

	      break;
     default: printf("\nFatal error!(4)"); exit(1);

    }
 }
  pat=NULL;

}

/*    oprintf(pattern_on_screen,his_file,"\n\n"); */

/****** Set the gene in new state ********/

     for(i=0;i<queue_count;i++)
{
      /*Set the gene in new state */
      /*Get the ants number pattern[i] gene */
      code=set_it2->state[ pattern[i][0] ].code_next_state[0];

      /*Set the i action - bits 6,7 are actually */
      /* 1). set bits number 6,7 to 0 (00111111 = 63) */
      code=code & 63;
      /* 2). set the action (xx000000) */

      switch ( pattern[i][1] )
      {
	case 0: code=code | 0; break;
	case 1: code=code | 64; break; /* 01000000=64 */
	case 2: code=code | 128; break; /* 10000000=128 */
	case 3: code=code | 192; break; /* 11000000=192 */
	default: oprintf(1,his_file,"Fatal ERROR!(8)"); exit(0);
      }

      /*Set the i next_state - bits 0,1,2,3,4 are actually */
      /* 1). set bits 0,1,2,3,4 to 0 (11100000 = 224 ) */
      code=code & 224;
      /* 2). set next_state */
      code=code | exits[ pattern[i][0] ];

      /*Write code to gene */
       set_it2->state[ pattern[i][0] ].code_next_state[0] = code;
  }

 if(pattern_print==1)
 oprintf(pattern_on_screen,his_file,"Change ants #%d gene by pattern.\n",
				     ant_number2);
/*print_gene(0,1,(State_machine huge *)gene_track_array[ant_number2].p_gene); */

  /* free  the pattern-array */
    for ( i = 0; i < queue_count; ++i )
    free(pattern[i]);
    free(pattern);
 }
}
if(pattern_print==2)
oprintf(pattern_on_screen,his_file,"In pairs checking: Find %d patterns, %d with NOP, %d with cycle.\n",
	pattern_count,pattern_count_NOP,pattern_count-pattern_count_NOP);
return; }


