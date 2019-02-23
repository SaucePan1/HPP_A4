#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{
   double total_acc_x=0, total_acc_y=0;
   double pos_x=0.2, pos_y = 0.3, cm_x = 0.01, cm_y =0.01, tot_mass = 1.3;
   int i, N = 1000;
   double G=100/N, epsilon_0 = 0.001;
   for (i=0; i<100000000; i++)
   {
     //calculate force
     //double x_direction = pos_x - cm_x;
     double y_direction = pos_y - cm_y;
     double dist_to_node = sqrt(x_direction*x_direction + y_direction*y_direction);
     double denominator = pow(sqrt(x_direction*x_direction +
     y_direction*y_direction+epsilon_0),3);
     total_acc_x += -G* tot_mass * x_direction/denominator;
     total_acc_y += -G* tot_mass * y_direction/denominator;
   }
   return 0;
}
