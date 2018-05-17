/////////////////////////////
int min(int a,int b){
  if(a<=b) return a;
  else return b;
}

int max(int a,int b){
	if(a<=b) return b;
	else return a;
}

double haversine(double lat1, double lon1,double lat2, double lon2){
  double R = 6372800.0;// Earth radius in kilometers
  double  dLat = PI*(lat2 - lat1)/180.0;
  double dLon = PI*(lon2 - lon1)/180.0;
  double a = sin(dLat/2.0)*sin(dLat/2.0) + cos(PI*lat1/180.0)*cos(PI*(lat2)/180.0)*sin(dLon/2)*sin(dLon/2);
  double c=2*asin(sqrt(a));
  return R * c;
  }
/////////////////////////////////

// Clase Vehiculo como lo cambié a estructura no necesito las funciones. // otra cosa 
// El contador de vehiculos generados debe ser global.
struct Veh{
  Id id_;
  int spd_;
  int gap_;
  int originNodeId_;
  int destinationNodeId_;
  int destinationNodeId_copy;
  int startTime_;
  int TimeinRed_;
  int StopTime_;
  int arrivalTime_;
  int t_on_st;
  double pathlength;
  double pathfreetime;
  bool carroprueba;
  
  Veh(){}
Veh(Id i, int vel, int g):id_(i),spd_(vel),gap_(g){t_on_st=0; carroprueba=0;}
  typedef deque<Id> Route;
  Route route_ ;
  void set_speed(int val){
    spd_=val;
    if(spd_==0)StopTime_+=1;
  }
  void set_path(double len1,double time1){
    pathlength=len1;
    pathfreetime=time1;
    }
  
  void set_gap(int val){gap_=val;}
  //void set_carroprueba(){carroprueba=1;}
  void set_originNodeId(int val){originNodeId_=val;}

  void set_destinationNodeId(int val){
    destinationNodeId_=val;
    destinationNodeId_copy=val;
    }
  
  void set_startTime(int val){
    startTime_=val;
    //  StopTime_=0;
  }
  void set_arrivalTime(int val){arrivalTime_=val;}
 
  Id id(){return id_;}
  int spd(){return spd_;}
  int gap(){return gap_;}
};
