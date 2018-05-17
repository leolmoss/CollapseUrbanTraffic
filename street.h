////This function allows to estimate the geographical location of a vehicle.
void getposition(double f, double lat1, double lon1,double lat2, double lon2){
  double R = 6372800.0;// Earth radius in kilometers
  double dist=haversine(lat1,lon1,lat2,lon2)/R;
  double A=sin(PI*(1-f)/180.0)*dist/sin(PI*dist/180.0);
  double B=sin(PI*f*dist/180.0)/sin(PI*dist/180.0);
  double x=A*cos(PI*lat1/180.0)*cos(PI*lon1/180.0)+B*cos(PI*lat2/180.0)*cos(PI*lon2/180.0);
  double y=A*cos(PI*lat1/180.0)*sin(PI*lon1/180.0)+B*cos(PI*lat2/180.0)*sin(PI*lon2/180.0);
  double z=A*sin(PI*lat1/180.0)+B*sin(PI*lat2/180.0);
  double lat,lon;
  lat=atan2(z,sqrt(x*x+y*y));
  lon=atan2(y,x);
  std::cout << std::setprecision(8);
  cout<<lat*180.0/PI<<','<<lon*180.0/PI;
}

//////////////////////////
/////Definition of STREET structure
//////////////////////////
struct Street{
  Id id_; // Edge ID
  Len len_; // Edge Length
  Spd spd_; // Edge speed in CA units
  int capacity; // Real road capacity
  double p_cap; //CA road capacity
  double lat_s,lon_s,lat_t,lon_t; // coordinates source and target of the road segment
  bool enzona;// to track roads inside an specific zone of the city.
  //////below: freetraveltime, UserEqTravelTime,SocOptTravelTime
  double cost_t, t_user_eq, t_soc_opt;
  double CA_tt; //CA road segment travel time under free flow traffic conditions.
  double sigmaCA_tt; // Standard deviation of CA_tt, usually is 0.
  double avTT;//Average speed during certain period of time (estimpualte later)
  double sigmaTT; // Standard deviation of avTT

  int NVehs;// Number of vehicles that have crossed the street during a period of time
  int Ttravel;// Travel time of a vehicle who crosses the street
  int nCars; //Vehicle inside the street at an especific time.
  int nCells; // Numer of cells in the road segment.
  int tiempo_quieta; // Time stopped, for rerouting.
  
  vector<int> TravelTime;// Storage of TravelTimeduring a certain period of time,
  typedef vector<Veh*> Cells;
  Cells cells_;// Vector of cells in the street
  typedef deque<Veh*> WaitCola;
  WaitCola waitCola_; // Vector of Vehicles waiting to be introduced in the network
 
  float used; //For routing tables, betweeness?

  double density;// Vehcile density of the road
  double av_speed; // average speed of the road
  double flow; // flow
  
  Street(){}
  Street(Id i,Len l, Spd k, double time, double tue, double tso, int cap):id_(i),len_(l),spd_(k),cost_t(time),t_user_eq(tue),t_soc_opt(tso),capacity(cap){
    used=0;/// se usa en el computo de escalamiento de autos.
    nCars=0;//vehiculos que están en la calle
    NVehs=0;// vehicles que pasan
    Ttravel=0;//este es el travel time en el automata
    tiempo_quieta=0;
    av_speed=0.0;
    flow=0.0;
    nCells= int(len_/LCELL);
    if(nCells<=tinyst)nCells=tinyst;
    for(int ii=0; ii<nCells;ii++){
      cells_.push_back(NULL);
    }
    ///definition of the road capacities in the CA model.
    if(capacity==950)p_cap=0.35185;
    else if(capacity==1900)p_cap=0.70370;//scundary.
    else if(capacity==6900)p_cap=0.85185;//primary roads
    else p_cap=1.00;//motorways
  }
  /// set functions
  void set_vel(double dd){av_speed=dd;}
  void set_flow(double dd){flow=dd;}
  void set_den(double dd){density=dd;}
  void set_id(Id val){id_=val;}
  void set_length(Len val){len_=val;}
  void set_coordinates(double lat1,double lon1,double lat2,double lon2);
  //get data functions
  double den(){return density;}
  double avspeed(){return av_speed;}
  Id id(){return id_;}
  Len length(){return len_;}

  void addToStreet(Veh* veh){
    assert(cells_[0]== NULL );
    //    veh->TimeinRed_=global;
    cells_[0] = veh ;
  }
  
  Veh* firstOnStreet(){
    return cells_.back();
  }
  
  void averageTT();
  void colaToStreet (int global);//Insert a vehicle into the road network.
  void moveOnStreetNaSch_rndbc(); // move the street on time step in the automata.
  void compute_gap_rndbc();// functions of the CA model.
  void compute_speed_rndbc(Random& ran);
  void printStreet();
  void printStreet_speed();
  void moveNaSch_rndbc(Random& ran, int global);


  bool hasSpace(){
    return cells_.front()==NULL;
  }  

  void addToCola(Veh* veh){
    waitCola_.push_back( veh );
  }

  Veh* firstInCola(){
    if ( waitCola_.size()>=1 ) {
      return waitCola_.front() ;
    } else {
      return NULL ;
    }
  }

  void rmFirstOnStreet(){
    assert(cells_.back()!=NULL);
     NVehs+=1;
    //cout<<cells_.back()->t_on_st<<endl;
    TravelTime.push_back(cells_.back()->t_on_st+1);
    Ttravel+=cells_.back()->t_on_st+1;
    //cout<<Ttravel<<endl;
    cells_.back()=NULL;
  }
  
  void rmFirstInWait() {
    assert( waitCola_.size() >= 1 ) ;
    waitCola_.pop_front() ;
    }
     
};

 
void Street::set_coordinates(double lat1,double lon1,double lat2,double lon2){
  // for tracking vehicles inside and especific zone of the city.
    lat_s=lat1;
    lon_s=lon1;
    lat_t=lat2;
    lon_t=lon2;
    if(haversine(lat_s,lon_s,42.345619, -71.003689)<=17000 || haversine(lat_s,lon_s,42.345619, -71.003689)<= 17000)
	enzona=1;
    else {enzona=0;
      spd_=4;
    }
  }


void Street::averageTT(){
      //assert(cells_.back()!=NULL);
    double average=0.0;
    double sigma=0.0;
    assert(NVehs==TravelTime.size());
    if(NVehs==0){
      avTT=0.0;
      sigmaTT=0.0;
    }
    else if(NVehs==1){
      avTT=TravelTime[0];
      sigmaTT=0.0;
    }

    else {
      int last = cells_.size();
     
      for( int ii=0; ii<NVehs ; ii++) {
	average+=TravelTime[ii];
      }
       avTT=average/double(NVehs);
       for(int ii=0; ii<NVehs ; ii++) {
	 sigma+=(TravelTime[ii]-avTT)*(TravelTime[ii]-avTT);
      }
       sigmaTT=sqrt(sigma/(NVehs-1));
    }
  }

void Street::colaToStreet (int global) {
    Veh* veh = firstInCola() ;
    int band=0;
 
    while( hasSpace() && veh != NULL ) {
      insertados++;
      veh->TimeinRed_=global;
      rmFirstInWait() ;
      addToStreet(veh) ;
      nCars+=1;
      veh = firstInCola() ;
    }
  
  }

 void Street::moveNaSch_rndbc(Random& ran, int global){
    colaToStreet(global);
    compute_gap_rndbc();
    compute_speed_rndbc(ran);
    moveOnStreetNaSch_rndbc();
    };

void Street::printStreet(){ 
  int last = cells_.size();
  cout<<id()<<endl;
  for( int ii=0; ii<last;ii++) {
    Veh* veh = cells_[ii] ;
    if ( veh != NULL ) {
      cout<<cells_[ii]->id()<<" ";
    }
    else cout<<"."<<" ";
    
  }
  cout<<endl;
}

void Street::printStreet_speed(){ 
  int last = cells_.size();
  cout<<id()<<endl;
  for( int ii=0; ii<last ; ii++ ) {
    Veh* veh = cells_[ii] ;
    if ( veh != NULL ) {
      cout<<cells_[ii]->spd()<<" ";
    }
    else cout<<"."<<" ";
    
  }
  cout<<endl;
}

    
void Street::compute_gap_rndbc(){
  int gap;
  int last = cells_.size();
  double counter=0.0;
  nCars=0;
  for( int ii=0; ii<last ; ii++){
    Veh* veh = cells_[ii] ;
    if ( veh != NULL ) {
      nCars+=1;
      gap=0;
   
      for( int kk=1; kk<last;kk++ ){
	if(cells_[ii+kk]==NULL){
	  gap+=1;

	  if((kk+ii)>=last-1){
	    cells_[ii]->set_gap(gap);
	    // cout<< cells_[ii]->id()<<" entro "<<gap<<endl;
	    break;
	  }
	}
	else{
	  cells_[ii]->set_gap(gap);
	  break;
	}	
	
      }
      // cout<< gap<<" ";
    }
  }
  set_den(double(nCars)/nCells);
}


void Street::compute_speed_rndbc(Random &ran){
 
  int gap=0;
  int vel=0;
  int velocity=0;
  int last = cells_.size();
  for( int ii=0; ii<last ; ii++ ){
    Veh* veh = cells_[ii] ;
    if ( veh != NULL ) {
      vel=cells_[ii]->spd();
      velocity+=vel;
      gap=cells_[ii]->gap();
      if(vel<gap){
	//if(vel<v_max)
	  veh->set_speed(min(vel+1,spd_));
	  //  cout<<cells_[ii]->id()<<" sisi "<< cells_[ii]->spd()<<endl;
      }
      else cells_[ii]->set_speed(min(gap,vel));
      
      vel=cells_[ii]->spd();
      if(ran.r()<p) veh->set_speed(max(vel-1,0));
      veh->t_on_st+=1;
      if(cells_[ii]->spd()==0)carrosquietos++;
    }
    //
  }

  if(nCars!=0)set_vel(double(velocity)/nCars);
  else set_vel(0.0);
  set_flow(density*av_speed);
}


void Street::moveOnStreetNaSch_rndbc(){
  int last = cells_.size()-1;
  int vel;
  for( int ii=0; ii<last; ii++) {
    Veh* veh = cells_[ii];
    // vel=veh->spd(); 
    if ( veh != NULL ) {
      int vel=cells_[ii]->spd(); 
      // cout<<vel<<endl;
      if(vel!=0){
	assert(cells_[ii+vel]==NULL);
	cells_[ii+vel] = veh;
	cells_[ii] = NULL ;
	ii+=vel;
    }
    }
    //    cout<<endl;
  }
  }
