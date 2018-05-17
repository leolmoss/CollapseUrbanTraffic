# CollapseUrbanTraffic
This repository contains code sufficient to replicate all findings from "Macroscopic dynamics and the collapse of urban traffic" a in-submission paper on data-driven simulations of urban city traffic.
Some details:

1. catt.cpp computes the travel time in free flow conditions and it generates the file boston_catt.txt already included in the repository.
2. routingtable.cpp will generate the routing tables for all the trips in "boston_trips_wpath_1hmod.txt". The latter file is the OD table for the real traffic conditions in Boston, i.e. ODs generated from the CDRs. The routes are ccalculated as the shortest path in User Equilibrium Travel Time. This information comes from a previous static route assignment and it is included in the road network data, i.e. "boston_edges_algbformat_ftt_ttuser_ttsoc_mod.txt". 
3. simular.cpp code simulates the load and the unloading process for the city of Boston.
4. File "boston_nodes_algbformat.txt" is thenodes information of the road netwok.
