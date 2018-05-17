#include "configuration.h"

//------------------MAIN----------------------

int main(void){
  //45
  Random ran2(4);
  City boston;
  Simworld simWorld;
  int num_rdb=0;
 
  simWorld.read_streets(boston,"boston_nodes_algbformat.txt","boston_edges_algbformat_ftt_ttuser_ttsoc_mod.txt");/// read road networks
  simWorld.recalculetiempoorejas(boston);//Virtual reduction of travel time of entrances/exit ramps in the bridges. This urges the vehicles to take correctly the entrances/exit ramp in the bridges instead of doing prohibited maneuvers inside the bridge. It is important in the precomputation of the routing tables.
  simWorld.routables_realODs_ordenadas_t_wpath(boston);//compute routing tables by using UserEquilibrium Travel time


  ///Print Routing Tables
  boost::graph_traits<City>::vertex_iterator vt, vt_end; 
  for (boost::tie(vt, vt_end) = vertices(boston); vt != vt_end; ++vt){
    cout<<get(vertex_index,boston,*vt)<<" "<<-1<<endl;
    boston[*vt].print_routingtable();
    }

 return 0;
}

