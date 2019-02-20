#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POS_X  0
#define POS_Y 1
#define MASS 2

//declare node structure (X)
//maybe good idea is to compact it! (ie no padding)
typedef struct tree_node{
  int depth;
  int body_id;
  double x_lim; //x division (middle point in x)
  double y_lim; // y division (middle point in y)
  double width; //width of the box
  double cm_x; // = 0, this points to 0 and
  double cm_y; //center of mas s of the quadrant
  double tot_mass; // = 0, total mass of the quadrant
  struct tree_node *left_down; //Q3 child
  struct tree_node *left_up; //Q2 child
  struct tree_node *right_down; //Q4 child
  struct tree_node *right_up; //Q1 child
}node_t;
void print_qtree(node_t* node){
  if(node == NULL){printf("Tree is empty \n"); return;}

  if(node != NULL){
    printf("Depth: %d, total mass: %lf, cm: (%lf, %lf) limits: (%lf,%lf)\t",
    node->depth,
    node->tot_mass,
    node->cm_x, node->cm_y,
    node->x_lim, node->y_lim);
  }

  if (node->right_up != NULL) printf("RU : total mass %lf,", node->right_up->tot_mass);
  if (node->left_up != NULL) printf("LU: total mass: %lf,", node->left_up->tot_mass);
  if (node->left_down != NULL) printf("LD: total mass: %lf,", node->left_down->tot_mass);
  if (node->right_down != NULL) printf("RD: total mass; %lf,", node->right_down->tot_mass);
  printf("\n");

  if (node->right_up != NULL) print_qtree(node->right_up);
  if (node->left_up != NULL) print_qtree(node->left_up);
  if (node->left_down != NULL) print_qtree(node->left_down);
  if (node->right_down != NULL) print_qtree(node->right_down);
  return;
}
void create_children(node_t* node, double * pow_2){

  if(node->right_up != NULL){printf("L178, create_children. Something went wrong RU children already exist"); return;}
  //create right up
  printf("Creating RU\n");
  node->right_up=(node_t*)malloc(sizeof(node_t));
  node->right_up->depth = node->depth +1;
  node->right_up->x_lim = node->x_lim + pow_2[node->right_up->depth];
  node->right_up->y_lim =  node->y_lim + pow_2[node->right_up->depth];
  node->right_up-> width = pow_2[node->depth];
  node->right_up-> cm_x = 0;
  node->right_up-> cm_y = 0;
  node->right_up-> tot_mass = -1.0;
  node->right_up->left_down = NULL;
  node->right_up->left_up = NULL;
  node->right_up->right_down = NULL;
  node->right_up->right_up = NULL;

  if(node->right_down != NULL){printf("L178, create_children. Something went wrong RD children already exist"); return;}
  //create right down
  printf("Creating RD\n");
  node->right_down=(node_t*)malloc(sizeof(node_t));
  node->right_down->depth = node->depth +1;
  node->right_down->x_lim = node->x_lim + pow_2[node->right_down->depth];
  node->right_down->y_lim =  node->y_lim - pow_2[node->right_down->depth];
  node->right_down-> width = pow_2[node->depth];
  node->right_down-> cm_x = 0;
  node->right_down-> cm_y = 0;
  node->right_down-> tot_mass = -1.0;
  node->right_down->left_down = NULL;
  node->right_down->left_up = NULL;
  node->right_down->right_down = NULL;
  node->right_down->right_up = NULL;


  if(node->left_up != NULL){printf("L178, create_children. Something went wrong RU children already exist"); return;}
  //create left_up
  printf("Creating LU\n");
  node->left_up=(node_t*)malloc(sizeof(node_t));
  node->left_up->depth = node->depth +1;
  node->left_up->x_lim = node->x_lim - pow_2[node->left_up->depth];
  node->left_up->y_lim =  node->y_lim + pow_2[node->left_up->depth];
  node->left_up-> width = pow_2[node->depth];
  node->left_up-> cm_x = 0;
  node->left_up-> cm_y = 0;
  node->left_up-> tot_mass = -1.0;
  node->left_up->left_down = NULL;
  node->left_up->left_up = NULL;
  node->left_up->right_down = NULL;
  node->left_up->right_up = NULL;


  if(node->left_down != NULL){printf("L178, create_children. Something went wrong RU children already exist"); return;}
  //create left down
  printf("Creating LD\n");
  node->left_down=(node_t*)malloc(sizeof(node_t));
  node->left_down->depth = node->depth +1;
  node->left_down->x_lim = node->x_lim - pow_2[node->left_down->depth];
  node->left_down->y_lim =  node->y_lim - pow_2[node->left_down->depth];
  node->left_down-> width = pow_2[node->depth];
  node->left_down-> cm_x = 0;
  node->left_down-> cm_y = 0;
  node->left_down-> tot_mass = -1.0;
  node->left_down->left_down = NULL;
  node->left_down->left_up = NULL;
  node->left_down->right_down = NULL;
  node->left_down->right_up = NULL;

}
//modify what node we are using (ie pointing to)
void insert(node_t **node, double x_pos, double y_pos, double mass, double* pow_2){
  // inserts given bodyin tree, creating new nodes if necessary such that
  //every external node (leaf) contains only one body.
  //each node represents a quadrant and contains coordinates of
  //its center of mass and total mass within the quadrant.

  if((*node) == NULL){ printf("ERROR: Given node is NULL"); return;}

  //if node X does not contain a body, we put the new body in it.
  if((*node)->tot_mass == -1.0){
    //the node contains no bodies so we can insert the new one in it.
    (*node)-> cm_x = x_pos;
    (*node)-> cm_y = y_pos;
    (*node)-> tot_mass = mass;

  }else{
    /*The node contains a body.
    If node is an internal node i.e has children we:
     update the center of mass and total mass of the node
     and go deeper to appropiate child (RU, RD, LU or LD) */
     if((*node)->right_up != NULL){
      //Node is internal (not a leaf)
      //UPDATE cm and MASS
      double cm_mass = (*node)->tot_mass;
      //update center of mass new_cm = (old_mass*old_cm + pos_new_particle*mass_new_particle)/(new total mass)
      (*node)->tot_mass+=mass;
      //we can save operations in here by declaring new varaibles
      (*node)->cm_x = cm_mass/(*node)->tot_mass * (*node)->cm_x + x_pos*(mass/(*node)->tot_mass); // Note: This maybe we can do more efficently
      (*node)->cm_y = cm_mass/(*node)->tot_mass* (*node)->cm_y + y_pos*(mass/(*node)->tot_mass);

      //FIND APPROPIATE CHILD and call insert with child as new node
      int expr = 2*(x_pos>(*node)->x_lim) + (y_pos >(*node)->y_lim);
      switch (expr) {
        case 0:
        //False, False we go left down
          node= &(*node)->left_down;
          break;
        case 1:
        //False, True we go left_up
          node= &(*node)->left_up;
          break;
        case 2:
        //True False, we go right down
          node = &(*node)->right_down;
          break;
        case 3:
        //True True we go right up
          node = &(*node)->right_up;
          break;
      }
      //insert with correct node
      insert(node, x_pos, y_pos, mass, pow_2);
    }else{
    //The node has mass but no children hence its a leaf
    //If it is an external node (leafs) we have
    //to create new children (furhter subdivide the space)
      create_children((*node), pow_2);
      printf("We created children\n");
      print_qtree((*node));
      //insert particle that was occupying the leaf, it will go to appropiate
      //quadrant
      double mass_in_node = (*node)->tot_mass;
      //set the mass to 0 since we remove a particle from the node
      (*node)->tot_mass = 0;
      //save position in the tree
      node_t ** parent_node = node;
      //insert particle that was already in node
      insert(parent_node, (*node)->cm_x, (*node)->cm_y, mass_in_node, pow_2);
      printf("We inserted particle \n");
      print_qtree((*node));
      //try inserting particle i again
      insert(node, x_pos, y_pos, mass, pow_2);
      printf("We inserted particle i \n");
      print_qtree((*node));
      }
    }
    return;
  }

int main(int argc, char const *args[]){

  if (argc!=7){
    printf("Invalid number of arguments \n");
    printf("galsim expects: N filename nsteps delta_t theta_max graphics\n");
    return -1;
  }
  clock_t begin = clock();

  char *file_name = args[2];
  const int N = atoi(args[1]);
  const int n_steps = atoi(args[3]);
  /*not sure if this is the correct way of converting from
  character to double, maybe a single cast would suffice */
  const double delta_t = atof(args[4]);
  const double theta_max = atof(args[5]);

  //Read the file with initial conditions

  FILE *file;
  file = fopen(file_name , "rb");
  /*maybe in this case we could allocate memory for this
  matrix statically*/
  double **arr = (double **)malloc(N*sizeof(double*));
  for (int i = 0 ; i<N ; i++){
    arr[i] = (double*)malloc(6 * sizeof(double));
  }

  for (int i = 0 ; i<(N) ; i++){
    double x , y , vx , vy , mass , bright;
    fread(&x , sizeof(double) , 1 ,file);
    fread(&y , sizeof(double) , 1 ,file);
    fread(&mass , sizeof(double) , 1 ,file);
    fread(&vx , sizeof(double) , 1 ,file);
    fread(&vy , sizeof(double) , 1 ,file);
    fread(&bright, sizeof(double) , 1 ,file);
    arr[i][0] = x;
    arr[i][1] = y;
    arr[i][2] = mass;
    arr[i][3] = vx;
    arr[i][4] = vy;
    arr[i][5] = bright;
  }
  fclose(file);
  const double G = 100/(double)N;
  const double epsilon_0 = 0.001;
  //find powers of two so we only have to do it once
  const int K=200;
  double pow_2[K];
  pow_2[0]=1.0;
  for(int i=1; i< K; i++)
    pow_2[i]= pow_2[i-1]/2;

  //initialize root
  node_t * root= (node_t*)malloc(sizeof(node_t));
  root->depth = 1;
  root->x_lim = 0.5;
  root->y_lim = 0.5;
  root->width = 0.5;
  root->tot_mass = -1.0;
  root->left_down = NULL;
  root->left_up = NULL;
  root->right_up=NULL;
  root->right_down=NULL;
  // insert particles in quad tree one by one
  //we pass a pointer to the node since we want to
  //modify what node we are using (ie pointing to)
  printf("inserting nodes in the tree...\n");
  for(int i=0; i<N; i++){
    insert(&root ,arr[i][POS_X], arr[i][POS_Y], arr[i][MASS], pow_2);
  }
  clock_t end = clock();
  printf("Time: %lu \n", end - begin);
  printf("Particles: \n");
  for(int i=0; i<N; i++){
    printf("particle %d: m = %lf, (%lf,%lf) \n", i, arr[i][MASS],
    arr[i][POS_X], arr[i][POS_Y]);
  }
  printf("Printing tree .. \n");
  print_qtree(root);
  return 0;
}

  /*
  void create_children(node_t * node){
    if(node->right_up != NULL){"L178, create_children. Something went wrong RU children already exist"}
    //create right up
    node->right_up=(node_t*)malloc(sizeof(node_t));
    node->right_up->depth = node->depth +1;
    node->right_up->x_lim = node->x_lim + pow_2[node->right_up->depth];
    node->right_up->y_lim =  node->y_lim + pow_2[node->right_up->depth];
    node->right_up-> width = pow_2[node->depth];
    node->right_up-> cm_x = 0;
    node->right_up-> cm_y = 0;
    node->right_up-> tot_mass = 0;
    node->right_up->left_down = NULL;
    node->right_up->left_up = NULL;
    node->right_up->right_down = NULL;
    node->right_up->right_up = NULL;

    if(node->right_down != NULL){"L178, create_children. Something went wrong RD children already exist"}
    //create right down
    node->right_down=(node_t*)malloc(sizeof(node_t));
    node->right_down->depth = node->depth +1;
    node->right_down->x_lim = node->x_lim + pow_2[node->right_down->depth];
    node->right_down->y_lim =  node->y_lim + pow_2[node->right_down->depth];
    node->right_down-> width = pow_2[node->depth];
    node->right_down-> cm_x = 0;
    node->right_down-> cm_y = 0;
    node->right_down-> tot_mass = 0;
    node->right_down->left_down = NULL;
    node->right_down->left_up = NULL;
    node->right_down->right_down = NULL;
    node->right_down->right_up = NULL;

    if(node->left_up != NULL){"L178, create_children. Something went wrong LU children already exist"}
    //create left up
    node->left_up=(node_t*)malloc(sizeof(node_t));
    node->left_up->depth = node->depth +1;
    node->left_up->x_lim = node->x_lim + pow_2[node->left_up->depth];
    node->left_up->y_lim =  node->y_lim + pow_2[node->left_up->depth];
    node->left_up-> width = pow_2[node->depth];
    node->left_up-> cm_x = 0;
    node->left_up-> cm_y = 0;
    node->left_up-> tot_mass = 0;
    node->left_up->left_down = NULL;
    node->left_up->left_up = NULL;
    node->left_up->right_down = NULL;
    node->left_up->right_up = NULL;

    if(node->left_down != NULL){"L178, create_children. Something went wrong LD children already exist"}
    //create left down
    node->left_down=(node_t*)malloc(sizeof(node_t));
    node->left_down->depth = node->depth +1;
    node->left_down->x_lim = node->x_lim + pow_2[node->left_down->depth];
    node->left_down->y_lim =  node->y_lim + pow_2[node->left_down->depth];
    node->left_down-> width = pow_2[node->depth];
    node->left_down-> cm_x = 0;
    node->left_down-> cm_y = 0;
    node->left_down-> tot_mass = 0;
    node->left_down->left_down = NULL;
    node->left_down->left_up = NULL;
    node->left_down->right_down = NULL;
    node->left_down->right_up = NULL;
  }

void print_qtree(node_t * node){
  if(node == NULL){printf("Tree is empty \n"); return;}

  if(node != NULL){
    printf("total mass: %lf, cm: (%lf, %lf) limits: (%lf,%lf). body: (%lf, %lf) m- %lf) \t",
    node->tot_mass,
    node->cm_x, node->cm_y,
    node->x_lim, node->x_lim,
    node->body_node->x_pos, node->body_node->y_pos, node->body_node->mass);
  }
  if (node->right_up != NULL) printf("Q1: total mass %lf,", node->right_up->tot_mass);
  if (node->left_up != NULL) printf("Q2: total mass: %lf,", node->left_up->tot_mass);
  if (node->left_down != NULL) printf("Q3: total mass: %lf,", node->left_down->tot_mass);
  if (node->right_down != NULL) printf("Q4: total mass; %lf,", node->right_down->tot_mass);
  printf("\n");

  if (node->right_up != NULL) print_qtree(node->right_up);
  if (node->left_up != NULL) print_qtree(node->left_up);
  if (node->left_down != NULL) print_qtree(node->left_down);
  if (node->right_down != NULL) print_qtree(node->right_down);
  return;
}
*/
