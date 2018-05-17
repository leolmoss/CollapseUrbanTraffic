#include "configuration.h"

//------------------MAIN----------------------
/////// This code allows to compute travel time under Cellular Automata model.
int main(void){
 Random ran2(4);
  City boston;
  Simworld simWorld;
  
  boost::graph_traits<City>::edge_iterator ll, ll_end;
  boost::graph_traits<City>::out_edge_iterator out, out_end;
   int nTotalCars=0;
   int nTotalCarsOut=0;
  simWorld.read_streets(boston,"boston_nodes_algbformat.txt","boston_edges_algbformat_ftt_ttuser_ttsoc_mod.txt");

  
  cout<<"ID"<<" "<<"avtt"<<" "<<"sigmatt"<<endl;
  simWorld.InsertingCarsinEveryStreet(boston,nTotalCars,0);
    for(int t_global=1; t_global<1000;t_global++){
      if(t_global%20==0 && t_global<500)
      simWorld.InsertingCarsinEveryStreet(boston,nTotalCars,t_global);
      simWorld.moveCA_rndbc(boston,ran2,t_global);
      simWorld.moveonNode_prob_rerouting2(boston,ran2,t_global, nTotalCarsOut);
      
    }

    for (boost::tie(ll, ll_end) = edges(boston); ll != ll_end; ++ll){
	    boston[*ll].averageTT();
	    cout<<boston[*ll].id_<<" "<<boston[*ll].avTT<<" "<<boston[*ll].sigmaTT<<endl;
	    boston[*ll].NVehs=0;
	    boston[*ll].Ttravel=0;
	    boston[*ll].TravelTime.clear();
    }
    return 0;
}

