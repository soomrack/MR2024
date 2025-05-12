#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


class Flight {
public:
    double time;
    double carrier_id;
    std::string carrier_name;
    double city_from_id;
    std::string city_from;
    double city_to_id;
    std::string city_to;

    Flight(double air_time, double airline_id, std::string origin_carrier_name,
        double origin_city_id, std::string origin_city, double destination_city_id,
        std::string destination_city);

};


Flight::Flight(double air_time, double airline_id, std::string origin_carrier_name,
    double origin_city_id, std::string origin_city, double destination_city_id,
    std::string destination_city)
{
    time = air_time;
    carrier_id = airline_id;
    carrier_name = origin_carrier_name;
    city_from_id = origin_city_id;
    city_from = origin_city;
    city_to_id = destination_city_id;
    city_to = destination_city;
}


void Save_to_file(std::ofstream& logfile, const std::vector<Flight>& Flights)
{
    logfile << "fr.id" << "\t" << "to.id" << "\t" << "time" << "\t"
        << "car.id" << "\t" << "car.name" << "\t" << "ci.fr." << "\t"
        << "ci.to" << std::endl;

    for (const auto& flight : Flights)
    {
        logfile << flight.city_from_id << "\t" << flight.city_to_id << "\t" << flight.time << "\t"
            << flight.carrier_id << "\t" << flight.carrier_name << "\t" << flight.city_from << "\t"
            << flight.city_to << std::endl;
    }
}


int main()
{
    std::ifstream datafile;
    datafile.open("T_T100_SEGMENT_ALL_CARRIER.csv");

    if (!datafile.is_open())
    {
        std::cout << "Unable to open data file1\n";
        return 0;
    }

    std::ofstream logfile;
    logfile.open("logfile.txt");

    if (!logfile.is_open())
    {
        std::cout << "Unable to open data file2\n";
        return 0;
    }

    std::vector<Flight> Flights;
    std::string csvline;

    while (getline(datafile, csvline))
    {
        std::stringstream inputString(csvline);

        std::string tempString;
        int departures_performed;
        std::string carrier_name;
        std::string city_from;
        std::string city_to;
        double time, carrier_id, city_from_id, city_to_id;

        size_t add = 0;

        for (size_t idx = 0; idx < 2; ++idx) { getline(inputString, tempString, ','); }
        departures_performed = atoi(tempString.c_str());

        for (size_t idx = 0; idx < 8; ++idx) { getline(inputString, tempString, ','); }
        time = atof(tempString.c_str());

        for (size_t idx = 0; idx < 2; ++idx) { getline(inputString, tempString, ','); }
        carrier_id = atof(tempString.c_str());

        getline(inputString, carrier_name, ',');
        if (carrier_name[0] == '\"')
        {
            carrier_name.erase(0, 1);
            add = 2;
        }

        for (size_t idx = 0; idx < 9 + add; ++idx) { getline(inputString, tempString, ','); }
        city_from_id = atof(tempString.c_str());
        getline(inputString, city_from, ',');

        for (size_t idx = 0; idx < 11; ++idx) { getline(inputString, tempString, ','); }
        city_to_id = atof(tempString.c_str());
        getline(inputString, city_to, ',');


        if (time > 0 && departures_performed != 0)
        {
            Flight flight(time, carrier_id, carrier_name, city_from_id, city_from, city_to_id, city_to);
            Flights.push_back(flight);
        }
    }

    Save_to_file(logfile, Flights);

    datafile.close();
    logfile.close();
    return 0;
}
