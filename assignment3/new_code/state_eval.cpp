#ifndef STATE_EVAL_CPP
#define STATE_EVAL_CPP

#include <time.h>

double nmt_cal(int diff)
{
	if (diff<=2) return 0;
	
	else return (diff-2)*(diff-2);
}

double State::evaluate()
{
 
 	double b = shortest_path(*this,this->mypn);							// my shortest path to goal
 	double c = shortest_path(*this,(this->mypn==1)?(2):(1));				// opp shortest path to goal
	double a = c-b;															// my shortest path - opp shortest path
//	cout << "----------\n"; cout << "b= " << b << " " << "c= " << c << " a= " << a << "\n"; pos_present.print(true) ; cout << endl ; pos_other.print(true) ; cout << endl;
 	if (I_won()) { /*cout <<"eval: " << 100000<<endl; */return (100000 + a);}
 	if (I_lost()) {/*cout <<"eval: " << 100000<<endl;*/ return (-100000 + a );}


	double wc = ((50-plies)/50)*((50-plies)/50)*((this->mypn==this->pn)?(this->n_present_walls):(this->n_other_walls));	// only for the walls I used (wall_cost)
	
	double nmt = ((this->mypn==this->pn)?(this->n_other_walls - this->n_present_walls):(this->n_present_walls-this->n_other_walls));	
	nmt = nmt_cal(nmt);
	// not more than three walls than oponent
		
 	return a*abs(a) - 5*b + 3*wc - 3.5*nmt;
	// double ans = w.diff_shortest_path(*this);
	// return ans;
//	return (rand()%10) ;
	// if ( pn==1 ) {
	// 	return (pos_present.r - 1)*(rand()%100);
	// } else {
	// 	return (pos_present.r - N)*(-1.0)*(rand()%100);
	// }
}

#endif