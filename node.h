
struct Node{
  Id id_;
  Coord lat;
  Coord lon;
  int tracts_;// census tract
  Veh* vehCorner;
  map<int,int> route_table;
  vector<double> prob_income;
  typedef deque<Veh*> WaitQueue;
  WaitQueue waitQueue_;
  Node(){}
  Node(Id i, Coord x, Coord y):id_(i),lon(x),lat(y){
    vehCorner=NULL;
}
  void print_routingtable();
  void set_id(Id val){id_=val;}
  Id id(){return id_;}
  void rndmove();
  void move(){rndmove();}
  void set_xx(Coord val){lon=val;}
  Coord xx() {return lon;}
  void set_yy(Coord val){lat=val;}
  Coord yy() {return lat;}
  void set_income_probs(vector<double> in){prob_income=in;}
  void addToWait(Veh* veh){
    waitQueue_.push_back( veh );
  }
  Veh* firstInWait(){
    if ( waitQueue_.size()>=1 ) {
      return waitQueue_.front() ;
    } else {
      return NULL ;
    }
    
  }
  void rmFirstInWait() {
    assert( waitQueue_.size() >= 1 ) ;
    waitQueue_.pop_front() ;
  }
 
};

void Node::print_routingtable(){
  // cout<<route_table.size()<<endl;
  for (std::map<int,int>::iterator it=route_table.begin(); it!=route_table.end(); ++it)
    cout << it->first <<" "<< it->second << '\n';
}

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS,Node, Street > City;

//descriptors
typedef boost::graph_traits<City>::vertex_descriptor vertex_descriptor;
typedef std::pair<boost::graph_traits<City>::edge_descriptor, bool> edge_t;

//iterators
typedef graph_traits<City>::vertex_iterator vertex_iter;
typedef graph_traits<City>::edge_iterator edge_iter;
std::map <int,int> c2v;


struct od_travelt{
  pair<int,int> trip_od;
  double travel_time;
  od_travelt(double tt, pair<int,int> odtt):travel_time(tt),trip_od(odtt){}
  bool operator < (const od_travelt& str) const
  {
        return (travel_time < str.travel_time);
    }

  bool operator > (const  od_travelt& str) const
    {
        return (travel_time > str.travel_time);
    }

  
};
