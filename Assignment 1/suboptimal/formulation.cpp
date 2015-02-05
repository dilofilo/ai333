#ifndef FORMULATION_CPP
	#define FORMULATION_CPP

#include "formulation.h"
#include <iterator>

//Misc Functions
void SeqProblem::print() { //Trivial Code to print the problem out.
	cout << "Clock time is : \t " << clockTime << "\n";
	cout << "CC is 			 \t " << CC << "\n";
	cout << "######## MC ########\n";
	for(int r = 0 ; r<aSize+1 ; ++r) {
		for(int c = 0; c<aSize+1 ; ++c) {
			cout << MC[r][c] << " ";
		}
		cout << "\n";
	}
	cout << "The alphabets are : "; for(int i=0; i<aSize ; ++i) { cout << alphabet[i] << " "; } cout << "\n";
	cout << "string and their lengths are : \n";
	for(int i = 0; i<k ; ++i) {
		cout << stringLengths[i] << "\t" << sequences[i] << "\n";
	}
	cout << "minimum final length is : " << minimumFinalLength << "\n";
	return;
} //QC passed -H

void SeqProblem::printState(const SeqState& state) { //Mostly trivial Code to print out the strings associated with state
	for(int stringIDX = 0; stringIDX < k; ++stringIDX) {
		//prints out the strings.
		int ctr = 0; //Counts how many positions have been parsed.
		//Pointers to the dash/sequence thingies			
		int dctr = 0; //number of dashes read.
		int cctr = 0; //Number of characters read.
		copy(state.dashPos[stringIDX].begin(),state.dashPos[stringIDX].end(), ostream_iterator<int>(cout, " "));
		cout<<"\n"; 	
		while( ctr < state.length ) {
			if ( (dctr<state.dashPos[stringIDX].size()) && (state.dashPos[stringIDX][dctr] + dctr == ctr)) {
				cout << '-';
				++dctr;
			} else {
				cout << sequences[stringIDX][cctr];
				++cctr;
			}
			++ctr;
		}
		cout << "\n";
		//cout << "\n The cost is : " << state.cost << "\n"; //Cost printed. use it to test evalCost.
	}
	return;
} //QC passed -H

void SeqProblem::initialize(INIT_TYPE initMode) {
	std::random_device rd;
	std::mt19937 engine(rd());
	uniform_int_distribution<int> initializer(0, (int)(minimumFinalLength / 5));
	if(initMode == RANDOM)
	{
		initialState.dashPos.resize(k);
		initialState.length = minimumFinalLength + initializer(engine);
		for(int i = 0; i < k; i++) {
			randomInit(initialState.dashPos[i], initialState.length - sequences[i].size(), 0, sequences[i].size());
		}
		initialState.cost = evalCost(initialState);
	}
} //QC depends on randomInit

void SeqProblem::initialize(INIT_TYPE initMode, int numStates) {
	initialStates.resize(numStates);
	std::random_device rd;
	std::mt19937 engine(rd());
	uniform_int_distribution<int> initializer(0, (int)(minimumFinalLength / 5));
	if(initMode == RANDOM)
	{
		for(int i = 0; i < numStates; i++)
		{
			initialStates[i].length = minimumFinalLength + initializer(engine);
			initialStates[i].dashPos.resize(k);
			for(int j = 0; j < k; i++)
				randomInit(initialStates[i].dashPos[j], initialStates[i].length - (int)sequences[j].size(), 0, (int)sequences[j].size());
			initialState.cost = evalCost(initialState);
		}
	}
}

void SeqProblem::initializeInto(INIT_TYPE initMode , SeqState& state) {
	std::random_device rd;
	std::mt19937 engine(rd());
	uniform_int_distribution<int> initializer(0, (int)(minimumFinalLength / 5));
	if(initMode == RANDOM)
	{
		state.length = minimumFinalLength + initializer(engine);
		state.dashPos.resize(k);
		for(int i = 0; i < k; i++)
			randomInit(state.dashPos[i], state.length - (int)sequences[i].size(), 0, (int)sequences[i].size());
		state.cost = evalCost(state);
	}
}

void SeqProblem::initializeInto(INIT_TYPE initMode, vector<SeqState>& states, int numStates) {
	states.resize(numStates);
	std::random_device rd;
	std::mt19937 engine(rd());
	uniform_int_distribution<int> initializer(0, (int)(minimumFinalLength / 5));
	if(initMode == RANDOM)
	{
		for(int i = 0; i < numStates; i++)
		{
			states[i].length = minimumFinalLength + initializer(engine);
			states[i].dashPos.resize(k);
			for(int j = 0; j < k; i++)
				randomInit(states[i].dashPos[j], states[i].length - (int)sequences[j].size(), 0, (int)sequences[j].size());
			initialState.cost = evalCost(initialState);
		}
	}
}

void SeqProblem::randomInit(vector<int>& vec, int x, int start, int end)
{
	std::random_device rd;
	std::mt19937 engine(rd());
	uniform_int_distribution<int> initializer(start, end);
	vec.resize(x);
	for(int i = 0; i < x; i++)
		vec[i] = initializer(engine);
	sort(vec.begin(), vec.end());
} //QC Not passed - is it not random? Or is it just sad?

void SeqProblem::getNBD(const SeqState& state , vector<SeqState>& nbd) {
	getNBD_singleDashMove(state , nbd); //Change According to will
}

void SeqProblem::getNBD_singleDashMove(const SeqState& state , vector<SeqState>& nbd) {
	//One neighbour per dash per string. , big-O(k*n*n) neighbours are generated here.
	for(int stringIDX = 0; stringIDX < k ; ++stringIDX) {
		//For each string in the state, generate children by moving each dash randomly. and one more dash by one length more and one length less.
		for(int di = 0; di< state.dashPos[stringIDX].size(); ++di) {
			//If possible, swap the dash with the previous and the next character.
			if (state.dashPos[stringIDX][di] > 0) {
				SeqState child = state;
				--child.dashPos[stringIDX][di];
				//Check if the di'th position needs to be sorted.
				/*
				Feasible alternative to sorting is to assert child.dsahPos[stringIDX][di] > child.dsahPos[stringIDX][di+1] 
															and also to assert child.dashPos[stringIDX][di] < childdashPos[stringIDX][di-1]
				*/
				if ( di+1 < child.dashPos[stringIDX].size()) {
					//There is another element after di
					//ASSERT : child.dashPos[stringIDX][di] < child.dashPos[stringIDX][di+1]
					if (child.dashPos[stringIDX][di] < child.dashPos[stringIDX][di+1]) {
						//PASS
					} else {
						//Swap to restore invariant.
						int temp = child.dashPos[stringIDX][di];
						child.dashPos[stringIDX][di] = child.dashPos[stringIDX][di+1];
						child.dashPos[stringIDX][di+1] = temp;  
					}
				}
				if ( di > 0 ) {
					//There is one element before di.
					//ASSERT : child.dashPos[stringIDX][di] > child.dashPos[stringIDX][di-1]
					if (child.dashPos[stringIDX][di] > child.dashPos[stringIDX][di-1]) {
						//PASS
					} else {
						//Swap to restore invariant.
						int temp = child.dashPos[stringIDX][di];
						child.dashPos[stringIDX][di] = child.dashPos[stringIDX][di-1];
						child.dashPos[stringIDX][di-1] = temp;  
					}
				}
				//std::sort(child.dashPos[stringIDX]); //Possibly slowing us down.
				setChildCost_singleDash(state, child , stringIDX);
				nbd.push_back(child);
			}
			if (state.dashPos[stringIDX][di] < stringLengths[stringIDX]-1 ) {
				SeqState child = state;
				++child.dashPos[stringIDX][di];
				//Check if the di'th position needs to be sorted.
				/*
				Feasible alternative to sorting is to assert child.dsahPos[stringIDX][di] > child.dsahPos[stringIDX][di+1] 
															and also to assert child.dashPos[stringIDX][di] < childdashPos[stringIDX][di-1]
				*/
				if ( di+1 < child.dashPos[stringIDX].size()) {
					//There is another element after di
					//ASSERT : child.dashPos[stringIDX][di] < child.dashPos[stringIDX][di+1]
					if (child.dashPos[stringIDX][di] < child.dashPos[stringIDX][di+1]) {
						//PASS
					} else {
						//Swap to restore invariant.
						int temp = child.dashPos[stringIDX][di];
						child.dashPos[stringIDX][di] = child.dashPos[stringIDX][di+1];
						child.dashPos[stringIDX][di+1] = temp;  
					}
				}
				if ( di > 0 ) {
					//There is one element before di.
					//ASSERT : child.dashPos[stringIDX][di] > child.dashPos[stringIDX][di-1]
					if (child.dashPos[stringIDX][di] > child.dashPos[stringIDX][di-1]) {
						//PASS
					} else {
						//Swap to restore invariant.
						int temp = child.dashPos[stringIDX][di];
						child.dashPos[stringIDX][di] = child.dashPos[stringIDX][di-1];
						child.dashPos[stringIDX][di-1] = temp;  
					}
				}
				//std::sort(child.dashPos[stringIDX]); //Possibly slowing us down.
				setChildCost_singleDash(state, child , stringIDX);
				nbd.push_back(child);
			}
		}
	}

	
	//Code that handles neighbours of varying length.
	//Random distribution devices from c++11 's  random class.
	std::random_device rd;
	std::mt19937 engine(rd()); //Some cool mersenne twister algorithm
	
	for(int i = 0; i<LONGER_LENGTH_CHILDREN; ++i) { //Vary these parameters to decide how many longer length children are wanted
		SeqState child = state;
		for( int stringIDX = 0; stringIDX < k ; ++stringIDX) {
			//insert a random dash.
			//uniform_int_distrib's are lightweight.
			int pos = std::uniform_int_distribution<int>(0 , stringLengths[stringIDX])(engine); //engine is the mt19937 random number generator engine.
			child.dashPos[stringIDX].push_back(pos);
			std::sort(child.dashPos[stringIDX].begin() , child.dashPos[stringIDX].end()); //Necessary to keep it sorted.		
		} 
		++child.length;
		child.cost = evalCost(child);
		nbd.push_back(child); //Generates one random child of larger length.
	}
	//Remove a dash from the child only if possible.
	if( state.length > minimumFinalLength ) {
		for(int i = 0; i< SHORTER_LENGTH_CHILDREN ; ++i) { //Vary these parameters to decide how many shorter length children are wanted.
			SeqState child = state;
			for (int stringIDX = 0; stringIDX < k ; ++stringIDX) {
				//Remove a dash.	
				//ASSERT : for all stringIDX , dashPos[stringIDX].size()>=1				
				child.dashPos[stringIDX].pop_back();
			}
			--child.length;
			child.cost = evalCost(child);
			nbd.push_back(child);
		}
	}
}


double SeqProblem::evalCost(const SeqState& state) {
	double cost = 0.0;
	/*
	Following algorithm :
	forAll 0 <= i < j < k :
	cost = MisMatchCost( String_i [...ctr] , String_j [...ctr]) where String_i and String_j are the i'th and j'tj resultant strings.
		It can be calculated easily without copying and inserting and stuff.
		maintain 5 pointers, two for the dashes , two for the sequences and one for the length processed. Repeat this for every pair.
	*/
	for( int stringIDX_1 = 0; stringIDX_1 < k; ++stringIDX_1) {
		for( int stringIDX_2 = stringIDX_1 + 1 ; stringIDX_2 < k; ++stringIDX_2) {
			int ctr = 0; //Counts how many positions have been parsed.
			//Pointers to the dash/sequence thingies			
			int dashIDX_1 	= 0;	
			int seqIDX_1 	= 0;
			int dashIDX_2 	= 0;
			int seqIDX_2	= 0;
			while ( ctr < state.length ) {
				//StringIDX_1 's character is calculated here.
				char s1 , s2; //s1 and s2 are the 
				if ( (dashIDX_1 < state.dashPos[stringIDX_1].size()) && (state.dashPos[stringIDX_1][dashIDX_1] + dashIDX_1== ctr )) {
					//String_IDX1 has a '-' at this position.
					s1 = '-';
					cost += CC;
					++dashIDX_1;
				} else {
					s1 = sequences[stringIDX_1][seqIDX_1];
					++seqIDX_1;
				}

				//StringIDX_2 's character is calculated here.
				if ( (dashIDX_2< state.dashPos[stringIDX_2].size()) && (state.dashPos[stringIDX_2][dashIDX_2] + dashIDX_2== ctr)) {
					//String_IDX2 has a '-' at this position
					s2 = '-';
					cost += CC;
					++dashIDX_2;
				} else {
					s1 = sequences[stringIDX_2][seqIDX_2];
					++seqIDX_2;
				}
				cost += MC[ charToInt[s1] ][ charToInt[s2] ];
				++ctr;
			}	
		}
	}
	return cost;
} // Akshay QC : 

void SeqProblem::setChildCost_singleDash(const SeqState& parent ,  SeqState& child , int stringIDX_1) { //Children are genereated by moving a dash around for one string.
	
	double deltaCost = 0.0;
	for(int stringIDX_2 = 0; stringIDX_2 < k ; ++stringIDX_2) {
		if ( stringIDX_2 == stringIDX_1 )
			continue;
		else {
			int ctr = 0; //Counts how many positions have been parsed.
			//Pointers to the dash/sequence thingies			
			int dashIDX_1 	= 0;	
			int seqIDX_1 	= 0;
			int dashIDX_2 	= 0;
			int seqIDX_2	= 0;
			while ( ctr < parent.length ) {
				//StringIDX_1 's character is calculated here.
				//Parent Cost
					char s1 , s2; //s1 and s2 are the 
					if ( (dashIDX_1<parent.dashPos[stringIDX_1].size() ) && (parent.dashPos[stringIDX_1][dashIDX_1] + dashIDX_1 == ctr)) {
						//String_IDX1 has a '-' at this position.
						s1 = '-';
						deltaCost -= CC; //CC doesnt actually need to be evaluated , but alright.
						++dashIDX_1;
					} else {
						s1 = sequences[stringIDX_1][seqIDX_1];
						++seqIDX_1;
					}

					//StringIDX_2 's character is calculated here.
					if ( (dashIDX_2 < parent.dashPos[stringIDX_2].size() ) && (parent.dashPos[stringIDX_2][dashIDX_2] + dashIDX_2 == ctr)) {
						//String_IDX2 has a '-' at this position
						s2 = '-';
						deltaCost -= CC;
						++dashIDX_2;
					} else {
						s1 = sequences[stringIDX_2][seqIDX_2];
						++seqIDX_2;
					}
					deltaCost -= MC[ charToInt[s1] ][ charToInt[s2] ];
				
				//ChildCost.
					char c1 , c2;
					if ( (dashIDX_1< child.dashPos[stringIDX_1].size() ) && (child.dashPos[stringIDX_1][dashIDX_1] + dashIDX_1 == ctr)) {
						//String_IDX1 has a '-' at this position.
						s1 = '-';
						deltaCost += CC;
						++dashIDX_1;
					} else {
						s1 = sequences[stringIDX_1][seqIDX_1];
						++seqIDX_1;
					}

					//StringIDX_2 's character is calculated here.
					if ( (dashIDX_2<child.dashPos[stringIDX_2].size()) && (child.dashPos[stringIDX_2][dashIDX_2] + dashIDX_2 == ctr)) {
						//String_IDX2 has a '-' at this position
						s2 = '-';
						deltaCost += CC;
						++dashIDX_2;
					} else {
						s1 = sequences[stringIDX_2][seqIDX_2];
						++seqIDX_2;
					}
					deltaCost += MC[ charToInt[s1] ][ charToInt[s2] ];
				++ctr;
			}	
		}
	}
	child.cost = parent.cost + deltaCost;
}

#endif