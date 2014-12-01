//##############################################################################
//        file: est_driver.cc
//      writer: Dan Tsafrir
// description: driver for the estimate model as defined in
//              "Modeling User Runtime Estimates", JSSPP'05
//##############################################################################


//##############################################################################
// include files
//##############################################################################
#include <vector>
#include <iostream>
# include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "est_model.hh"
#include "est_parse.hh"
# include <fstream>
#include <math.h>
using namespace std;
// NODE ATTRIBUTES
int NODE_NUMBER = 16; // * 4 
int CPU_AMOUNT_MIN = 50; 
int CPU_AMOUNT_MAX = 100; 
int MEMORY_AMOUNT_MIN = 1024; 
int MEMORY_AMOUNT_MAX = 4096;

#define HOUR 3600
#define MIN 60
//##############################################################################
// the driver
//##############################################################################

string calc_HHMMSS (int x) {
    int time_target=x;
    int hour=time_target/HOUR;
    int second=time_target % HOUR;
    int minute=second/MIN;
    second = second % MIN;
    string h,m,s;
    if (hour <10) // single digit
    	h = "0"+std::to_string(hour) ; 
    else
	h = std::to_string(hour);
    if (minute<10) 
	m = "0"+std::to_string(minute);
    else
	m = std::to_string(minute); 
    if (second <10) 
	s = "0"+std::to_string(second);
    else
	s = std::to_string(second); 

    string format= h+":"+m+":"+s+".00";
    return format; 
}

int main(int argc, char *argv[])
{
 
    for (int x=50; x<=500 ; x+=50){
	// (x) used for num of Pnodes which is 10 values: 50 , 100 , 150 , 200 , 250 , 300 , 350, 400, 450 , 500
	int IT = 0 , temp=0 ; 
	ofstream of[7] , off[7] ;

/*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*/

	for (int i=0 ; i<7 ; i++) {
		// (i) used for Interarrival time which is 7 values : 32, 64, 128, 256, 512, 1024, 2048 
		// also (i) is used as file index within the same Pnode number 
    		of[i].open ("/home/ubuntu/share/pure/Experiment_Pnodes="+std::to_string(x)+"_IT="+std::to_string(int(pow(2,(i) + 5)))+".xml",ios::out);
		of[i]<<"<lease-workload name=\"Experiment_Pnodes="+std::to_string(x)+"_IT="+std::to_string(int(pow(2,(i) + 5)))+"\">\n";
		of[i]<<"	<description>\n";
		of[i]<<"	This experiment tests pure haizea scheduler under different parameters, PNode# ="+std::to_string(x)+", IT="+ std::to_string(int(pow(2,(i) + 5)))+"\n";
		of[i]<<"	</description>\n";
		// Writing nodes descriptions
		of[i]<<"<site> \n";
		of[i]<<"<resource-types names=\"CPU Memory Disk\"/> \n";
		of[i]<<"   <nodes> \n";
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		of[i]<<"<node-set numnodes=\""+std::to_string(x)+"\"> \n";
		of[i]<<"<res type=\"CPU\" amount=\"100\"/> \n";
		of[i]<<"<res type=\"Memory\" amount=\"4096\"/> \n";
		of[i]<<"</node-set> \n";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		of[i]<<"   </nodes> \n";
		of[i]<<"</site> \n";
		of[i]<<" <lease-requests>\n";
//////////****************************************************************************************************
/*				SAME VALUES USED FOR COGNITIVE HAIZEA					     */
//////////****************************************************************************************************
    		off[i].open ("/home/ubuntu/share/cog/Experiment_Pnodes="+std::to_string(x)+"_IT="+std::to_string(int(pow(2,(i) + 5)))+".xml",ios::out);
		off[i]<<"<lease-workload name=\"Experiment_Pnodes="+std::to_string(x)+"_IT="+std::to_string(int(pow(2,(i) + 5)))+"\">\n";
		off[i]<<"	<description>\n";
		off[i]<<"	This experiment tests cognitive haizea scheduler under different parameters, PNode# ="+std::to_string(x)+", IT="+std::to_string(int(pow(2,(i) + 5)))+"\n";
		off[i]<<"	</description>\n";
		// Writing nodes descriptions
		off[i]<<"<site> \n";
		off[i]<<"<resource-types names=\"CPU Memory Disk\"/> \n";
		off[i]<<"   <nodes> \n";
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		off[i]<<"<node-set numnodes=\""+std::to_string(x)+"\"> \n";
		off[i]<<"<res type=\"CPU\" amount=\"100\"/> \n";
		off[i]<<"<res type=\"Memory\" amount=\"4096\"/> \n";
		off[i]<<"</node-set> \n";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		off[i]<<"   </nodes> \n";
		off[i]<<"</site> \n";
		off[i]<<" <lease-requests>\n";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

/*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*//*/*/

//////////////////////////////////////////////////////////////////////////
    // read and parse command line args. if an SWFfile name was given, parse
    // and load it. args contains:
    // 1 maxest:    the maximal allowed estimate
    // 2 jobs:      if an SWFfile this vector hold all the jobs.
    // 3 njobs:     the number of jobs = the number of estimates to produce.
    // 4 userbins:  vector containing user give <estimate,job#> pairs (optional)
    // 5 precision: number of precision digits for flaoting-point SWF fields
    // 6 seed:      to the randome number generator.
    Args_t args(argc,argv);

    
    // this structure encapsulates all the parameters of the model.
    // - its constructor gets only the two mandatory parameters (njobs and
    //   maxest) along with the optional (and possibly empty) user bins that
    //   allow the user to specify site specific/unique information.
    // - howerver, all the other parameters can also be set explicitly e.g.
    //   model_params.binnum = 200;         // num of different estimate values
    //   model_params.binsiz_head_sum = 85; // % of "head" estimates
    EstParams_t model_params(args.njobs, args.maxest, args.userbins);

    
    // generate an estimate distribution according to the above parameters.
    // the model uses the drand48/lrand48 standard random generators.
    vector<EstBin_t> estimate_distribution;
    srand48(args.seed);
    est_gen_dist(model_params, &estimate_distribution);
    // print result.
    if( args.jobs.size() > 0 ) {
	//
	// SWFfile was given, jobs are held by the vector args.jobs; need to
	// assign the estimates to jobs (such that runtimes are never bigger
	// than estimates) and print resulting SWFfile.
	//
	vector<Job_t>& jobs = args.jobs;
	est_assign(estimate_distribution, &jobs);
	int id[7]= {1,1,1,1,1,1,1};
	// Start filling each file :
	for (int i=0; i<7; i++) {
	int temp = 0 ;   
	srand(time(NULL));
	int unble_H = 0 ;
	for(vector<Job_t>::iterator j = jobs.begin() ; j != jobs.end() ; ++j){ 
 
int JOB_MEMORY_AMOUNT = rand() % 4096 ;
int JOB_CPU_AMOUNT = rand()%100 ;

	if (id[i] <=400){ // one experment has 400 jobs 

	    int r2 = int (pow(2,(i) + 5)); // jobs interarrival time 
	    j->submit = r2 + temp ;  // temp tracks last job IT 
			string JOB_INTERARRIVAL = calc_HHMMSS (j->submit);
			string JOB_DURATION = calc_HHMMSS (j->estimate);
			// Writing leases contents: 
			of[i]<<"<lease-request arrival=\""+JOB_INTERARRIVAL+"\">\n";
			of[i]<<"<lease id=\""+std::to_string(id[i])+"\" preemptible=\"true\" > \n";	
			of[i]<<"<nodes>\n";
			int vnode = (rand() %1000)+1;
			int limit = x/10 ; 
			if (vnode >= x && unble_H  < limit)
				{of[i]<<"<node-set numnodes=\""+std::to_string(vnode+1)+"\">\n";
				unble_H  ++ ; }
			else
				of[i]<<"<node-set numnodes=\""+std::to_string((vnode % x)+1)+"\">\n";
			of[i]<<"<res amount=\""+std::to_string(JOB_CPU_AMOUNT)+"\" type=\"CPU\" />\n";
			of[i]<<"<res amount=\""+std::to_string(JOB_MEMORY_AMOUNT)+"\" type=\"Memory\" />\n";
			of[i]<<"</node-set>\n";
			of[i]<<"</nodes>	\n";
			of[i]<<"<start> </start>\n";
			of[i]<<"<duration time=\""+JOB_DURATION+"\"/>\n";
			of[i]<< "<software>\n";
			of[i]<< "<disk-image id=\"foobar.img\" size=\"1024\"/>\n";
			of[i]<< "</software>\n";
			of[i]<<"</lease>\n";
			of[i]<<"</lease-request>\n" << "\n";
		} // end if (id[i] <=400)
//////////****************************************************************************************************
/*				SAME VALUES USED FOR COGNITIVE HAIZEA					     */
//////////****************************************************************************************************
	    if (id[i] <=400){ // one experment 
	    int r2 = int (pow(2,(i) + 5)); // jobs interarrival time 
	    j->submit = r2 + temp ;  // temp tracks last job IT 
			string JOB_INTERARRIVAL = calc_HHMMSS (j->submit);
			string JOB_DURATION = calc_HHMMSS (j->estimate);
// Setting Job Memory amount
			// Writing leases contents: 
			off[i]<<"<lease-request arrival=\""+JOB_INTERARRIVAL+"\">\n";
			off[i]<<"<lease id=\""+std::to_string(id[i]++)+"\" preemptible=\"true\" > \n";	
			off[i]<<"<nodes>\n";
			int vnode = 1;
			off[i]<<"<node-set numnodes=\""+std::to_string(vnode)+"\">\n";
			off[i]<<"<res amount=\""+std::to_string(JOB_CPU_AMOUNT)+"\" type=\"CPU\" />\n";
			off[i]<<"<res amount=\""+std::to_string(JOB_MEMORY_AMOUNT)+"\" type=\"Memory\" />\n";
			off[i]<<"</node-set>\n";
			off[i]<<"</nodes>	\n";
			off[i]<<"<start> <cognitive/> </start>\n";
			off[i]<<"<duration time=\""+JOB_DURATION+"\"/>\n";
			off[i]<< "<software>\n";
			off[i]<< "<disk-image id=\"foobar.img\" size=\"1024\"/>\n";
			off[i]<< "</software>\n";
			off[i]<<"</lease>\n";
			off[i]<<"</lease-request>\n" << "\n";
		
	 	} // end if (id[i] <=400)
		temp = j->submit ; // IT of inserted job
		}  // end for(vector<Job_t>::iterator j = jobs.begin() ; j != jobs.end() ; ++j)
	}// end for (int i=0; i<7; i++) 
    } // end if (args.jobs.size() > 0)
    else {
	//
	// no SWFfile: just print the estimate distribution (both PDF and CDF)
	//
	double            cdf  = 0;
	vector<EstBin_t>& dist = estimate_distribution;

	printf("#%7s %7s %10s %10s\n", "seconds", "njobs", "PDF", "CDF");
	
	for(vector<EstBin_t>::iterator i=dist.begin() ; i != dist.end() ; ++i) {
	    
	    double pdf = 100.0 * i->njobs / args.njobs;
	    cdf       += pdf;
		
	    printf("%8d %7d %10.6f %10.6f\n",
		   i->time,	// the estimate in seconds
		   i->njobs,	// number of jobs using this estimate
		   pdf,		// % of jobs using estimate
		   cdf);	// % of jobs using <= estimate
	}
    }
  
    for (int i=0 ; i<7 ; i++) {    
    of[i]<<"  </lease-requests>\n";
    of[i]<<"</lease-workload>\n\n\n\n";
    of[i].close();
    off[i]<<"  </lease-requests>\n";
    off[i]<<"</lease-workload>\n\n\n\n";
    off[i].close();
    }
 }
    return 0;
}


//##############################################################################
//                                   EOF
//##############################################################################
