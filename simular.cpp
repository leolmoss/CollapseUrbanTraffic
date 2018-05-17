#include "configuration.h"

//------------------MAIN----------------------

int main(void){
 Random ran2(27);
  City boston;
  Simworld simWorld;
  map <int, vector<double> > density;
  map <int, vector<double> > havlinratio;
  map <int, vector<double> > havlinflujo;
  char name[22];
  char dens[22];
  char flow[22];
  char rati[22];
  boost::graph_traits<City>::edge_iterator ll, ll_end;
  boost::graph_traits<City>::out_edge_iterator out, out_end;

  ////////////////////////////////////////////////////
  /////Simulation, ii= number of configuraitons to simulate
  ////////////////////////////////////////////////////

  for (int ii=0;ii<1; ++ii) {
    int nTotalCars=0; /// Total number of inserted vehicles
    int nTotalCarsOut=0; // Total number of vehicles taht have completed their trips
    int nTotalinNetwork=0; /// Vehicles still in the network
    int nTotalCarsOut_old=0; ///
    insertados=0;
    double rat=0.0; 
    //sb=sin borrar
    sprintf(name, "numb89_%03d.dat", ii);
    sprintf(dens, "dens89_%03d.txt", ii);
    sprintf(flow, "flow89_%03d.txt", ii);
    sprintf(rati, "rati89_%03d.txt", ii);


    simWorld.read_streets(boston,"boston_nodes_algbformat.txt","boston_edges_algbformat_ftt_ttuser_ttsoc_mod.txt");/// read road networks
    simWorld.addsourcetarget(boston); // add geographiical location for the source and target of each road segment.
    simWorld.read_ca_tt(boston); // read travel time for each road segment under free flow conditions and in CA units.
    //simWorld.recalculetiempoorejas(boston);//Virtual reduction of travel time of entrances/exit ramps in the bridges. This urges the vehicles to take correctly the entrances/exit ramp in the bridges instead of doing prohibited maneuvers inside the bridge. It is important in the precomputation of the routing tables.
    leer_olmos_od_l_ft("boston_trips_wpath_1hmod.txt");///Read trips
    upload_routable_olmos(boston,"boston_routable_wpath_1hmod.txt"); // read routing_tables.
    int nTotalCarsOld=0;
   
    ofstream fout(name);
    ofstream den(dens);
    ofstream hflow(flow);
    ofstream hratio(rati);

     ////////////////////////////////////////////////////
    ///// Loading part
    ////////////////////////////////////////////////////
    for(int t_global=1; t_global<2911;t_global++){
      carrosquietos=0;
      simWorld.InsertingCarstoCola(boston, olmos_OD, leng_ft, ran2, nTotalCars,60,t_global);////Insert vehicles with R=60.
      simWorld.moveCA_rndbc(boston,ran2,t_global);//// On simulation step on the road segments
     
      simWorld.moveonNode_prob_rerouting2(boston,ran2,t_global, nTotalCarsOut);//// On simulation step on the nodes.
      if(t_global%194==0){
	fout<<t_global<<" "<<nTotalCars-nTotalCarsOut<<" "<<insertados-nTotalCarsOut<<" "<<nTotalCarsOut-nTotalCarsOld<<endl;
	nTotalCarsOld=nTotalCarsOut;
		for (boost::tie(ll, ll_end) = edges(boston); ll != ll_end; ++ll){
	    boston[*ll].averageTT();
	     if(boston[*ll].avTT>0.0001)rat=boston[*ll].CA_tt/boston[*ll].avTT;//// ratio between average speed and  maximal speed for each road segment.
	     else rat=0.0;
	     havlinratio[boston[*ll].id_].push_back(rat*boston[*ll].spd_);
	     //havlinratio[boston[*ll].id_].push_back(boston[*ll].avTT);
	    
	    havlinflujo[boston[*ll].id_].push_back(boston[*ll].NVehs/194.0/boston[*ll].p_cap);
	    density[boston[*ll].id_].push_back(boston[*ll].density);
	    boston[*ll].NVehs=0;
	    boston[*ll].Ttravel=0;
	    boston[*ll].TravelTime.clear();
	    }
	 
     } 
      }
        
    olmos_OD.clear();
    ////////////////////////////////////////////////////
    ///// Unlooding part
    ////////////////////////////////////////////////////
     for(int t_global=2911; t_global<4600;t_global++){
       carrosquietos=0;
	
       simWorld.moveCA_rndbc(boston,ran2, t_global);//// On simulation step on the road segments
       simWorld.moveonNode_prob_rerouting2(boston,ran2,t_global, nTotalCarsOut);//// On simulation step on the nodes.
      
       if(t_global%194==0){ //194 time steps equals 4 minutes in real units.
	fout<<t_global<<" "<<nTotalCars-nTotalCarsOut<<" "<<insertados-nTotalCarsOut<<" "<<nTotalCarsOut-nTotalCarsOld<<endl;
	nTotalCarsOld=nTotalCarsOut;
	for (boost::tie(ll, ll_end) = edges(boston); ll != ll_end; ++ll){
	  boston[*ll].averageTT();
	  if(boston[*ll].avTT>0.0001)rat=boston[*ll].CA_tt/boston[*ll].avTT;
	  else rat=0.0;
	  //// save road segment properties, average speed, density and flow.
	  havlinratio[boston[*ll].id_].push_back(rat);
	  havlinflujo[boston[*ll].id_].push_back(boston[*ll].NVehs/194.0/boston[*ll].p_cap);
	  density[boston[*ll].id_].push_back(boston[*ll].density);
	  boston[*ll].NVehs=0;
	  boston[*ll].Ttravel=0;
	  boston[*ll].TravelTime.clear();
	}
	
      }
     }

     ////////////////////////////////////////////////////
     /////Print road segment properties for all the simulation
     ////////////////////////////////////////////////////
     
     std::map<int,vector< double > >::iterator mapiter;
     std::vector< double >::iterator veciter;
     
    for(mapiter=havlinflujo.begin();mapiter!=havlinflujo.end();++mapiter){
      hflow<<mapiter->first<<" ";
      for(veciter=mapiter->second.begin();veciter!=mapiter->second.end();++veciter)
	hflow<<*veciter<<" ";
      hflow<<endl;
       }
    for(mapiter=havlinratio.begin();mapiter!=havlinratio.end();++mapiter){
      hratio<<mapiter->first<<" ";
      for(veciter=mapiter->second.begin();veciter!=mapiter->second.end();++veciter)
	hratio<<*veciter<<" ";
      hratio<<endl;
       }
    for(mapiter=density.begin();mapiter!=density.end();++mapiter){
       den<<mapiter->first<<" ";
       for(veciter=mapiter->second.begin();veciter!=mapiter->second.end();++veciter)
	 den<<*veciter<<" ";
       den<<endl;
       }
    
    boston.clear();
    density.clear();
    havlinflujo.clear();
    havlinratio.clear();
    hflow.close();
    hratio.close();
    den.close();
    fout.close();
  }  
 return 0;
}

