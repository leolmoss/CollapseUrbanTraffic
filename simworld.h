double haversine2(double lat1, double lon1,double lat2, double lon2){
  double R = 6372800.0;// Earth radius in kilometers
  double  dLat = PI*(lat2 - lat1)/180.0;
  double dLon = PI*(lon2 - lon1)/180.0;
  double a = sin(dLat/2.0)*sin(dLat/2.0) + cos(PI*lat1/180.0)*cos(PI*(lat2)/180.0)*sin(dLon/2)*sin(dLon/2);
  double c=2*asin(sqrt(a));
  return R * c;
}

struct Simworld{
  Simworld(){}
  ///Reding functions
  void read_streets(City &city,const char *nodes,const char *edges);//lee bien la red pues no completa automáticamente.
  void recalculetiempoorejas(City &city);
  void read_ca_tt(City& city);
  void addsourcetarget(City &city);

  ///inserting vehicles
  void InsertingCarstoCola(City &city, vector< pair<int,int> > &od,vector< pair<double,double> > &pathfeatures, Random &random, int& numero,int nVehicles, int t_depar);
  void InsertingCarsinEveryStreet(City &city,int& numero, int t_depar);

  //MOVE THE AUTOMATA
  void moveCA_rndbc(City &city,Random& ran, int global);
  void moveonNode_prob_rerouting2(City & city, Random &ran, int globaltime,int &outcars);

  //OTHER
  void computetraveltime(City & city, Random &ran, int globaltime,int &outcars);
  void routables_realODs_ordenadas_t_wpath(City& city);
  
  //some print functions
  void print(City &city);
  void print_snapshot(City &city, int aa);
};

void Simworld::read_streets(City& city,const char *nodes, const char *edges){
 
  int idon_road=0;
  string headerline;
  unsigned idnode;

  //read nodes
  double xx,yy;
  ifstream in_nodes(nodes); 
  getline(in_nodes, headerline);
  
  while (in_nodes>>idnode){
      c2v[idnode]=idon_road;
      // hacer el puto asssert, verificar si el archivo es bueno.
      in_nodes>>xx;in_nodes>>yy;
      boost::add_vertex(Node(idnode,xx,yy),city); 
      idon_road++;
    }
  // cout<<c2v.size()<<" "<<c2v[200]<<endl;
  in_nodes.close();

  double capacity,speed_mph,cost_time,tt_ue,tt_so;
  int eid,source, target,dir;
  int edgeid,avtt,sigmatt;
  Len longitud;
  
  //cout<<num_vertices(city)<<endl;
  ifstream in_edges(edges);
  getline(in_edges, headerline);
    // inFile_edges.open(EDGES_FILE_NAME);
  while (in_edges>>eid){
    in_edges>>source; in_edges>>target;
    in_edges>>dir; in_edges>>capacity; 
    in_edges>>speed_mph; in_edges>>cost_time;
    in_edges>>tt_ue;in_edges>>tt_so;
    
    if(source==target){
      //  cout<<eid<<endl;
      continue;
      }

    longitud=double(1609*speed_mph*cost_time/60.0);
    
    int speed_sim=int(speed_mph*1.609/16.0);
    ///cout<<speed_sim<<endl;
    boost::add_edge(c2v[source],c2v[target],Street(eid, longitud ,speed_sim,cost_time,tt_ue,tt_so,capacity),city); 
    }
  in_edges.close();
  //cout<<num_vertices(city)<<" "<<num_edges(city)<<endl;
}


void Simworld::recalculetiempoorejas(City &city){
  vertex_descriptor inicio;
  vertex_descriptor final;
  boost::graph_traits<City>::edge_iterator ei,eend;

  for (tie(ei,eend)=edges(city);ei!=eend;++ei){
      inicio=source(*ei,city);
      final=target(*ei,city);
      double distancia=haversine(city[inicio].lat,city[inicio].lon,city[final].lat,city[final].lon);
      if(city[*ei].len_>=3*distancia){
	city[*ei].t_user_eq=city[*ei].t_user_eq/3.0;
	city[*ei].t_soc_opt=city[*ei].t_soc_opt/3.0;
	city[*ei].cost_t=city[*ei].cost_t/3.0;
      }  
    }
  }

void Simworld::read_ca_tt(City& city){

  ifstream trtime("boston_catt.txt");
  string headerline;
  getline(trtime, headerline);
  double eid,catt,stt;

  boost::graph_traits<City>::edge_iterator ei,eend;
  for (tie(ei,eend)=edges(city);ei!=eend;ei++){
    trtime>>eid>>catt>>stt;
    assert(eid==city[*ei].id_);
    city[*ei].CA_tt=catt;
    city[*ei].sigmaCA_tt=stt;
  }
  trtime.close();
  //cout<<num_vertices(city)<<" "<<num_edges(city)<<endl;
}



void Simworld::addsourcetarget(City &city){
  boost::graph_traits<City>::edge_iterator e,e_end;
  for (tie(e,e_end)=edges(city);e!=e_end;e++)
    city[*e].set_coordinates(city[source(*e,city)].lat,city[source(*e,city)].lon,city[target(*e,city)].lat,city[target(*e,city)].lon);
  //cout<<vehicles<<endl;
}

void Simworld::routables_realODs_ordenadas_t_wpath(City& city){

   int origin, destiny;
   int old_origin=-1;
   double flow;
   vertex_iter vert,vertend;
   std::vector<double> distances(num_vertices(city));//store distances
   std::vector<vertex_descriptor> predecessors(boost::num_vertices(city)); // To store parents
   // map<int,o_d_f> od;
   string headerline;
   ifstream od_file(OD_INFILE_ORDEN);
   //   ofstream tripwpath("boston_trips_wpath_1hmod.txt");
   ofstream tripwpath("boston_trips_wpath_1hmod.txt");
   
   tripwpath<<"source"<<" "<<"destination"<<" "<<"longitud"<<" "<<"costime"<<endl;
   getline(od_file, headerline);
   
   while (od_file >> origin){
     od_file >> destiny;
     // od_file >> flow;
     if(destiny==origin) continue;
     vertex_descriptor v_origin = vertex(c2v[origin], city);
       // ALGORITMO DIJKSTRA PARA EL ORIGEN
     if(origin!=old_origin){
       distances.clear();
       predecessors.clear();

       dijkstra_shortest_paths(city, v_origin,
			       weight_map(get(&Street::t_user_eq, city)).
			       distance_map(make_iterator_property_map(distances.begin(),
								       get(vertex_index, city))).predecessor_map((make_iterator_property_map(predecessors.begin(),
																	     get(vertex_index, city))))
			       );

     }

     // AQUI DEBO RECORRER EL CAMINO DEL FINAL PARA ATRAS
       typedef std::vector<City::edge_descriptor> PathType;
       PathType path;
       double tiempotrip=0.0;
       
       vertex_descriptor v_destiny2 = vertex(c2v[destiny], city);
       vertex_descriptor v_destiny3 = vertex(c2v[destiny], city);

       for(vertex_descriptor ux = predecessors[v_destiny2]; ux!=v_destiny2; v_destiny2=ux, ux=predecessors[v_destiny2]){
	  /////////////
	 std::pair<City::edge_descriptor, bool> edgePair = boost::edge(ux, v_destiny2, city);
	 City::edge_descriptor edge = edgePair.first;
	 path.push_back( edge );
	 if(out_degree(ux,city)!=1)/// Aquí estoy optimizando
	   city[get(vertex_index,city,ux)].route_table[get(vertex_index,city,v_destiny3)]=get(vertex_index,city,v_destiny2);
	 //	 cout<< city[get(vertex_index,city,ux)].route_table.size()<<endl;
       }

       double longitud=0.0;
       double costime=0.0;
       double catt=0.0;
       //   int num_nodos=0;

       for(PathType::reverse_iterator pathIterator = path.rbegin(); pathIterator != path.rend(); ++pathIterator){
	 //	 num_nodos+=1;
	 city[*pathIterator].used+=1.0;
	 longitud+=city[*pathIterator].len_;
	 costime+=city[*pathIterator].cost_t;
	 catt+=city[*pathIterator].CA_tt;
	 //*pathIterator.used=1;
       }

       tripwpath<<city[v_origin].id_<<" "<<city[v_destiny3].id_<<" "<<longitud<<" "<<costime<<endl;
       //tripwpath<<city[v_origin].id_<<" "<<city[v_destiny3].id_<<" "<<longitud<<" "<<costime<<" "<<catt<<endl;
       old_origin=origin;
   }
   
   od_file.close();
}


void Simworld::moveCA_rndbc(City& city,Random& ran, int global){
  
  boost::graph_traits<City>::edge_iterator ei,eend;
  for (tie(ei,eend)=edges(city);ei!=eend;ei++)
    city[*ei].moveNaSch_rndbc(ran, global);
     //cout<<vehicles<<endl;
 }


void Simworld::InsertingCarstoCola(City& city, vector <pair<int,int> > & od, vector <pair<double,double> > & pathfeatures, Random &ran, int &numero,int nVehicles, int t_depar){
  int trip;
  double pp;
  boost::graph_traits<City>::out_edge_iterator e, e_end;
  boost::graph_traits<City>::vertex_iterator v, v_end;
   Veh *carro;
 
  for(int nVehs=0; nVehs<nVehicles; nVehs++){
    trip=ran.r()*od.size();
 
    int origin=od[trip].first;
    int destination=od[trip].second;
    double longitudpath=pathfeatures[trip].first;
    double timepath=pathfeatures[trip].second;

    boost::graph_traits<City>::vertex_descriptor  s = vertex(origin, city);
    int NextNode= city[s].route_table[destination];
    carro=new Veh(numero,1,0);
    carro->originNodeId_=origin;
    carro->set_path(longitudpath,timepath);
    carro->destinationNodeId_=destination;
     carro->destinationNodeId_copy=destination;
    carro->startTime_=t_depar;
    carro->StopTime_=0;

      if(out_degree(s,city)==1){
	boost::tie(e, e_end) = out_edges(s, city);
	city[*e].addToCola(carro);
	numero++;
	od.erase(od.begin()+trip);
	pathfeatures.erase(pathfeatures.begin()+trip);
      }
      else{
       for (boost::tie(e, e_end) = out_edges(s, city); e!= e_end; ++e){
	 if(target(*e,city)==NextNode){
	   city[*e].addToCola(carro);
	   numero++;
	   od.erase(od.begin()+trip);
	   // cout<<"entro"<<endl;
	   pathfeatures.erase(pathfeatures.begin()+trip);
	   break;
	 }
       }
     }
  }
}


void Simworld::InsertingCarsinEveryStreet(City& city,int& numero, int t_depar){
  int trip;
  double pp;
  boost::graph_traits<City>::edge_iterator ll, ll_end;
  Veh *carro;
  int origin,destination;
  double longitudpath,timepath;
  //int chooseSt;
  
  for (boost::tie(ll, ll_end) = edges(city); ll != ll_end; ++ll){
    origin=get(vertex_index,city,source(*ll,city));
    // origin=city[source(*ll,city)];
    destination=get(vertex_index,city,target(*ll,city));
    longitudpath=100.0;
    timepath=100.0;
    carro=new Veh(numero,1,0);
    carro->originNodeId_=origin;
    carro->set_path(longitudpath,timepath);
    carro->destinationNodeId_=destination;
    carro->destinationNodeId_copy=destination;
    carro->startTime_=t_depar;
    city[*ll].addToCola(carro);
  }
}


void Simworld::moveonNode_prob_rerouting2(City & city, Random &ran, int globaltime, int &outcars){
  int NextNode;
  typedef multimap <double, Street* >RndStreets;
  RndStreets rnd_streets;// esto se puede hacer con la funcion sort() de STL cuando haya tiempo hay que hacerlo.
  RndStreets reroute_streets;
  boost::graph_traits<City>::in_edge_iterator i, i_end;
  boost::graph_traits<City>::out_edge_iterator e, e_end;
  boost::graph_traits<City>::out_edge_iterator reroute, reroute_end;
  boost::graph_traits<City>::vertex_iterator v, v_end;
  //boost::graph_traits<City>::out_edge_iterator re, re_end;
  //boost::graph_traits<City>::in_edge_iterator pi, pi_end;
  //boost::graph_traits<City>::out_edge_iterator pe, pe_end;
    
  for (boost::tie(v, v_end) = vertices(city); v != v_end; ++v){
    assert(in_degree(*v,city)!=0);
    int nOutLinks = out_degree(*v,city);
    for (boost::tie(i, i_end) = in_edges(*v, city); i != i_end; ++i){
      Street* inLink=&city[*i];
      rnd_streets.insert(make_pair(ran.r(),inLink));
      //	cout<<(*inLink).id()<<" "<<city[*i].id()<<" "<<endl;
    }
    for ( RndStreets::iterator ll = rnd_streets.begin();ll != rnd_streets.end(); ll++ ) {
      Street* inLink = ll->second ;
      //assert((*inLink)!=NULL);
      Veh* veh=  (*inLink).firstOnStreet();
  
      if(veh!= NULL) {
	int bandera=0;
	//FIRST OPTION ->  es que llegue al nodo.
	if(veh->destinationNodeId_copy==get(vertex_index,city,*v)){
	  (*inLink).rmFirstOnStreet();
	  (*inLink).nCars-=1;
	  bandera=1;
	  outcars+=1;
	  continue;
	}

	else if(nOutLinks==1){
	  boost::tie(e, e_end) = out_edges(*v, city);
	  
	  if(city[*e].hasSpace() && ran.r()<(*inLink).p_cap && city[*e].firstInCola()==NULL){
	    (*inLink).rmFirstOnStreet();
	    veh->set_speed(1);
	    veh->t_on_st=0;
	    city[*e].addToStreet(veh);
	    city[*e].nCars+=1;
	    (*inLink).nCars-=1;
	    // (*inLink).tiempo_quieta=0;
	  }
	    else  veh->set_speed(0);
	    //else  (*inLink).tiempo_quieta+=1;
	  //  }
	    bandera=1;
	    // continue;
	}
	    
	else{
	    ////// si está perdido
	      if(veh->destinationNodeId_==-1){
	      vertex_iter vert,vertend;
	      std::vector<double> distances(num_vertices(city));//store distances
	      std::vector<vertex_descriptor> predecessors(boost::num_vertices(city)); // To store parents
	      distances.clear();
	      //cout<<"entro"<<endl;
	      predecessors.clear();
	      vertex_descriptor v_origin = vertex(*v, city);
	      // ALGORITMO DIJKSTRA PARA EL ORIGEN
	      dijkstra_shortest_paths(city, *v,
				      weight_map(get(&Street::t_user_eq, city)).
				      distance_map(make_iterator_property_map(distances.begin(),
									      get(vertex_index, city))).predecessor_map((make_iterator_property_map(predecessors.begin(),
																		    get(vertex_index, city))))
				      );
	      
	      
	      // AQUI DEBO RECORRER EL CAMINO DEL FINAL PARA ATRAS
		vertex_descriptor v_destiny = vertex(veh->destinationNodeId_copy, city);
		vertex_descriptor v_destiny2 = vertex(veh->destinationNodeId_copy, city);
		vertex_descriptor v_destiny3 = vertex(veh->destinationNodeId_copy, city);
		for(vertex_descriptor ux = predecessors[v_destiny2]; ux!=v_destiny2; v_destiny2=ux, ux=predecessors[v_destiny2]){
		  if(out_degree(ux,city)!=1)/// Aquí estoy optimizando
		    city[get(vertex_index,city,ux)].route_table[get(vertex_index,city,v_destiny3)]=get(vertex_index,city,v_destiny2);
		}
		
		veh->destinationNodeId_=veh->destinationNodeId_copy;
				   
    	}
	    //////////////////////////////////////////////////////////////
		    	    
	    NextNode= city[*v].route_table[veh->destinationNodeId_];
	    
	    for (boost::tie(e, e_end) = out_edges(*v, city); e != e_end; ++e){
	      //  cout<<NextNode<<endl;
	      if(target(*e,city)==NextNode){
	
		if(city[*e].hasSpace() && ran.r()<(*inLink).p_cap && city[*e].firstInCola()==NULL){
		  (*inLink).rmFirstOnStreet();
		  veh->set_speed(1);
		  veh->t_on_st=0;
		  city[*e].addToStreet(veh);
		  city[*e].nCars+=1;
		  (*inLink).nCars-=1;
		  (*inLink).tiempo_quieta=0;
		  //	}
		}
		else {(*inLink).tiempo_quieta+=1; veh->set_speed(0);}
		
		bandera=1;
		break;
	      }
	      
	     	    }
	   	  
	if((*inLink).tiempo_quieta>=tauwait){
	  	  	  
	  for (boost::tie(reroute, reroute_end) = out_edges(*v, city);reroute != reroute_end; ++reroute){
	    Street* rerouteLink=&city[*reroute];
	    reroute_streets.insert(make_pair(rerouteLink->density,rerouteLink));
	  }
	  Street* rerouteLink = reroute_streets.begin()->second ;
	  //NextNode= city[*v].route_table[veh->destinationNodeId_];
	  if((*rerouteLink).hasSpace()){
	  
	    (*inLink).rmFirstOnStreet();
	    veh->set_speed(1);
	     veh->t_on_st=0;
	    (*rerouteLink).addToStreet(veh);
	    (*rerouteLink).nCars+=1;
	    (*inLink).nCars-=1;
	    veh->destinationNodeId_=-1;
	    (*inLink).tiempo_quieta=0;
	   
	  }

	  else veh->set_speed(0);
	  bandera=1;
	  // cout<<"está trancado"<< (*inLink).id_<<" "<<"tiempo :"<<" "<<globaltime<<endl;
	  }
	 }
	//	}	
	if(bandera==0){cout<<"no encontró"<< nOutLinks <<" "<<get(vertex_index,city,*v)<<" "<<" "<<NextNode<<" "<<veh->destinationNodeId_<<endl;
	  for (boost::tie(e, e_end) = out_edges(*v, city); e != e_end; ++e){
	    cout<<target(*e,city)<<" ";
	  }
	  cout<<endl;
	}	
      }	    
    }
    rnd_streets.clear();
  }	  	

}


void Simworld::computetraveltime(City & city, Random &ran, int globaltime, int &outcars){
  int NextNode;
  typedef multimap <double, Street* >RndStreets;
  RndStreets rnd_streets;// esto se puede hacer con la funcion sort() de STL cuando haya tiempo hay que hacerlo.
  RndStreets reroute_streets;
  boost::graph_traits<City>::in_edge_iterator i, i_end;
  boost::graph_traits<City>::out_edge_iterator e, e_end;
  boost::graph_traits<City>::out_edge_iterator re, re_end;
  boost::graph_traits<City>::out_edge_iterator reroute, reroute_end;
  boost::graph_traits<City>::in_edge_iterator pi, pi_end;
  boost::graph_traits<City>::out_edge_iterator pe, pe_end;
  boost::graph_traits<City>::vertex_iterator v, v_end;
  /// hay que recorrer los vertices desordenadamentes? si hay muchas calles con una sola celda, ésta si afectaria.
  
  for (boost::tie(v, v_end) = vertices(city); v != v_end; ++v){
    assert(in_degree(*v,city)!=0);
    int nOutLinks = out_degree(*v,city);
    for (boost::tie(i, i_end) = in_edges(*v, city); i != i_end; ++i){
      Street* inLink=&city[*i];
      rnd_streets.insert(make_pair(ran.r(),inLink));
      //	cout<<(*inLink).id()<<" "<<city[*i].id()<<" "<<endl;
    }
    for ( RndStreets::iterator ll = rnd_streets.begin();ll != rnd_streets.end(); ll++ ) {
      Street* inLink = ll->second ;
      //assert((*inLink)!=NULL);
      Veh* veh=  (*inLink).firstOnStreet();
      //  cout<<(*inLink).id()<<" ";
      if(veh!= NULL) {
	int bandera=0;
	//FIRST OPTION ->  es que llegue al nodo.
	if(veh->destinationNodeId_copy==get(vertex_index,city,*v)){
	  if(ran.r()<(*inLink).p_cap){
	   //   cout<<"entro"<<endl;
	  (*inLink).rmFirstOnStreet();
	  (*inLink).nCars-=1;
	  }
	  bandera=1;
	  outcars+=1;
	  continue;
	} 
    }
   
    }
     rnd_streets.clear();
  }
}	  	


void Simworld::print_snapshot(City &city, int aa){
  char name[22];
  sprintf(name, "qgis_131_%03d.txt", aa);
  ofstream in_n(name);
  vertex_descriptor inicio;
  vertex_descriptor final;
  boost::graph_traits<City>::edge_iterator ei,eend;
  double rat=0.0;
  for (tie(ei,eend)=edges(city);ei!=eend;++ei){
    inicio=source(*ei,city);
    final=target(*ei,city);
    city[*ei].averageTT();
    if(city[*ei].avTT>0.0001)rat=city[*ei].CA_tt/city[*ei].avTT;
    else rat=0.0;
    if(city[*ei].p_cap>=0.78)
      in_n<<city[*ei].id_<<" "<<city[inicio].id_<<" "<<city[final].id_<<" "<<rat<<" "<<double(city[*ei].NVehs)/485.0/city[*ei].p_cap<<endl;
    else
      in_n<<city[*ei].id_<<" "<<city[inicio].id_<<" "<<city[final].id_<<" "<<0.0<<" "<<0.0<<endl;
    
  }
  in_n.close();
}

void Simworld::print(City &city){
    boost::graph_traits<City>::edge_iterator ei,eend;
    //tie(ei,eend)=edges(city);
    for (tie(ei,eend)=edges(city);ei!=eend;++ei)
      city[*ei].printStreet();
  }
