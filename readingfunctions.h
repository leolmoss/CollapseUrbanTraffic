struct o_d_f{
  map<int,double> d_f;
};

map<int,o_d_f> od; 

map<int,o_d_f> leer_matrix_old_od(){
  int origin, destiny;
  double flow;
  // map<int,o_d_f> od;
  string headerline;
  ifstream od_file(OD_INFILE);
  getline(od_file, headerline);
  while (od_file >> origin){
    od_file >> destiny;
    od_file >> flow;
    if(destiny==origin) continue;
    od[c2v[origin]].d_f[c2v[destiny]]=flow;
    //    cout<<origin<<" "<<destiny<<" "<<endl;
    }
  //cout<<"total # of trips: "<< totflow<<endl;
  return od;
  }

vector< od_travelt >olmos_OD_tt;


vector< pair<int,int> >olmos_OD;
vector< pair<double,double> >leng_ft;
void leer_olmos_od_l_ft(const char *name){
  int origin, destiny;
  double largo,freetime;
  string headerline;
  // ifstream od_file(OD_INFILE);
  //ifstream od_file("boston_trips_wpath_1himproved.txt");
  ifstream od_file(name);
  
  getline(od_file, headerline);
  pair<int,int> pair_o_d;
  pair<double,double> pair_l_ft;
  //pair=new pair<,>
  int numbers=0;
  while (od_file >> origin){
       od_file >> destiny;
       od_file >> largo;
       od_file >> freetime;
       
    if(destiny==origin){
      // cout<<"eyy "<<endl;
      continue; 
    }
    else{
  
      pair_o_d.first=c2v[origin];
      pair_o_d.second=c2v[destiny];
      pair_l_ft.first=largo;
      pair_l_ft.second=freetime;
      olmos_OD.push_back(pair_o_d);
      leng_ft.push_back(pair_l_ft);
    }
  
  }
  od_file.close();
}


void upload_routable_olmos(City& city,const char *name){
  ifstream route(name);
  int nexthop,destiny, node;
  while (route >> destiny){
    route >>nexthop;
    if(nexthop==-1){
      node=destiny;
      continue;
    }
    else{
      city[node].route_table[destiny]=nexthop;
      //route >>d;
    }
  }
  route.close();
}

void print_routable_olmos(City& city){

  boost::graph_traits<City>::vertex_iterator vt, vt_end; 
  int r=0;
  
  for (boost::tie(vt, vt_end) = vertices(city); vt != vt_end; ++vt){
    r+=city[*vt].route_table.size();
  
    city[*vt].print_routingtable();
  }
}

