#ifndef DHAKA_GRAPH_H
#define DHAKA_GRAPH_H

#include "DhakaRouting.h"


class DhakaGraph {
private:
    std::map<Location, std::vector<Edge>> adjacencyList;
    std::set<Location> locations;
    std::map<Location, std::string> metroStations;
    std::map<Location, std::string> bikolpoStops;
    std::map<Location, std::string> uttaraStops;
    
public:
    void addEdge(const Edge& edge) {
        locations.insert(edge.start);
        locations.insert(edge.end);
        adjacencyList[edge.start].push_back(edge);
    }
    
    const std::vector<Edge>& getNeighbors(const Location& loc) const {
        static const std::vector<Edge> empty;
        auto it = adjacencyList.find(loc);
        return (it != adjacencyList.end()) ? it->second : empty;
    }
    
    void addMetroStation(const Location& loc, const std::string& name) {
        metroStations[loc] = name;
    }
    
    void addBikolpoStop(const Location& loc, const std::string& name) {
        bikolpoStops[loc] = name;
    }
    
    void addUttaraStop(const Location& loc, const std::string& name) {
        uttaraStops[loc] = name;
    }
    
    std::string getStationName(const Location& loc) const {
        auto it = metroStations.find(loc);
        if (it != metroStations.end()) return it->second;
        
        it = bikolpoStops.find(loc);
        if (it != bikolpoStops.end()) return it->second;
        
        it = uttaraStops.find(loc);
        if (it != uttaraStops.end()) return it->second;
        
        return "";
    }
    
    Location findNearestLocation(const Location& target) const {
        if (locations.empty()) return target;
        
        double minDist = std::numeric_limits<double>::max();
        Location nearest = target;
        
        for (const auto& loc : locations) {
            double dist = haversineDistance(target, loc);
            if (dist < minDist) {
                minDist = dist;
                nearest = loc;
            }
        }
        
        return nearest;
    }
    
    double distanceToNearestLocation(const Location& target) const {
        Location nearest = findNearestLocation(target);
        return haversineDistance(target, nearest);
    }
    
    size_t getLocationCount() const {
        return locations.size();
    }
    
    size_t getEdgeCount() const {
        size_t count = 0;
        for (const auto& pair : adjacencyList) {
            count += pair.second.size();
        }
        return count;
    }
};

#endif // DHAKA_GRAPH_H
