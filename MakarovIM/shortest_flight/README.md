### Start Project

- `git clone <repo_url>`

- `mkdir{data,logs,out}`

The data directory must contain flight data in csv format.

- Take it for editing main.cpp. In line 19, you need the absolute path to the data file.

- `cd out`

- `cmake ..`

- `cmake --build .`

- `./shortest_flight [origin_city_name] [destination_city_name]` (example: ./shortest_flight "Dover, DE" "Tampa, Fl")
