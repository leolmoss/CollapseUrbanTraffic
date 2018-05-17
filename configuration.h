// configuration.h
// define's and include's
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <iomanip>
#include <list>
#include <utility>
#include<algorithm>
#include<cmath>
#include<string>
#include <stdlib.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "random.h"

#define LCELL 5.50
#define v_max 7
#define nVehsperRoad 20
#define den_Vehs 0.0
#define lado 9
#define p 0.00
#define h 0.50
#define lrndbouts 9
#define tinyst 6
#define tauwait 96
#define tautestcars 48
#define per_testcars 0.07

int insertados=0;
int carrosquietos=0;

typedef double Len;
typedef double Spd;
typedef unsigned Id;
typedef double Coord;

const char* INFILE_NODES = "boston_nodes_algbformat.txt";
const char* INFILE_EDGES = "boston_edges_algbformat_ftt_ttuser_ttsoc_mod.txt";
const char* OD_INFILE = "boston_trips_wpath_1himproved.txt";
const char* OD_INFILE_ORDEN = "boston_trips_olmos_ordenadas_routing_1h.txt";
const char* RANDOMOD_INFILE_ORDEN = "b_randomod_olmos_routing.txt";

#define PI 3.14159265359
using namespace std;
using namespace boost;


#include "vehicle.h"
#include "street.h"
#include "node.h"
#include "simworld.h"
#include "readingfunctions.h"
