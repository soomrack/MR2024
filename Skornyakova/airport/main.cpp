#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <sstream>
#include <unordered_map>

#include "Flights.hpp"

 


void printf_path(Path& path ){

    std::cout<< "Shortest path:\n"<< path.first[0];

    for (size_t i = 1; i< path.first.size()-1; ++i){
        std::cout<<"->"<< path.first[i];
    }

    std::cout<<"->"<< path.first.back()<< std::endl;
    std::cout<<"Total duration of the path:"<< path.second.back();

}

int main(){
    
    std::string file_path;
    Flights flights;
    int origin_index;
    int dest_index;
    Path path;
    
   std::cout << "Please enter the path to the file"<< std::endl;
    flights.extract_flight_data_csv();
    std::cout << "Please enter the origin airport indexes"<< std::endl;
    std::cin >> origin_index;
    std::cout <<"Please enter the destination airport indexes"<< std::endl;
    std::cin >> dest_index;


/*using GraphEdge = std::vector<std::pair<AirportID, AirTime>>;
using Graph = std::unordered_map<AirportID, GraphEdge>;

std::pair<AirportID, AirTime> one (2,1);
std::pair<AirportID, AirTime> two (3,5);
std::pair<AirportID, AirTime> three (4,30);
std::pair<AirportID, AirTime> four (5,35);
std::pair<AirportID, AirTime> five (4,15);
std::pair<AirportID, AirTime> six (5,20);
std::pair<AirportID, AirTime> seven (6,20);
std::pair<AirportID, AirTime> eight (6,10);

GraphEdge ed1 = {one,two};
GraphEdge ed2 = {three,four};
GraphEdge ed3 = {five,six};
GraphEdge ed4 = {seven};
GraphEdge ed5 = {eight};


Graph gra;
   gra[1] = ed1;
   gra[2] = ed2;
   gra[3] = ed3;
   gra[4] = ed4;
   gra[5] = ed5;

   using Node = std::unordered_set<AirportID>;
   Node node = {1,2,3,4,5,6};
   Flights myflights(gra,node);
   //myflights.printGraph();
*/
  
   
  // Path mypath  ({1,2,3},{23,34,56});
    path = flights.find_the_shortest_path(1,5);
    printf_path(path);

    return 0;

     
} 