#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "DhakaRouting.h"
#include "DhakaGraph.h"



class CSVParser {
public:
    static std::vector<std::string> parseLine(const std::string& line) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(line);
        
        while (std::getline(tokenStream, token, ',')) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    static void parseRoadmap(const std::string& filename, DhakaGraph& graph) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        std::string line;
        int edgeCount = 0;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            auto tokens = parseLine(line);
            if (tokens.size() < 3) continue;
            
            // Parse coordinates
            std::vector<Location> coords;
            for (size_t i = 1; i + 1 < tokens.size() - 2; i += 2) {
                try {
                    double lon = std::stod(tokens[i]);
                    double lat = std::stod(tokens[i + 1]);
                    coords.push_back(Location(lat, lon));
                } catch (...) {
                    break;
                }
            }
            
            if (coords.size() < 2) continue;
            
            
            double totalDistance = 0.0;
            try {
                totalDistance = std::stod(tokens[tokens.size() - 1]);
            } catch (...) {
                continue;
            }
            
            
            double segmentDist = totalDistance / (coords.size() - 1);
            for (size_t i = 0; i + 1 < coords.size(); i++) {
                graph.addEdge(Edge(coords[i], coords[i+1], segmentDist, TransportMode::CAR));
                graph.addEdge(Edge(coords[i+1], coords[i], segmentDist, TransportMode::CAR));
                edgeCount += 2;
            }
        }
        
        file.close();
        std::cout << "  ✓ " << edgeCount << " road segments loaded" << std::endl;
    }
    
    static void parseTransitRoute(const std::string& filename, DhakaGraph& graph, 
                                  TransportMode mode) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        std::string line;
        int edgeCount = 0;
        int stationCount = 0;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            auto tokens = parseLine(line);
            if (tokens.size() < 4) continue;
            
            
            std::vector<Location> coords;
            for (size_t i = 1; i + 1 < tokens.size() - 2; i += 2) {
                try {
                    double lon = std::stod(tokens[i]);
                    double lat = std::stod(tokens[i + 1]);
                    coords.push_back(Location(lat, lon));
                } catch (...) {
                    break;
                }
            }
            
            if (coords.size() < 2) continue;
            
    
            std::string startName = tokens[tokens.size() - 2];
            std::string endName = tokens[tokens.size() - 1];
            

            if (!coords.empty()) {
                if (mode == TransportMode::METRO) {
                    graph.addMetroStation(coords.front(), startName);
                    graph.addMetroStation(coords.back(), endName);
                } else if (mode == TransportMode::BUS_BIKOLPO) {
                    graph.addBikolpoStop(coords.front(), startName);
                    graph.addBikolpoStop(coords.back(), endName);
                } else if (mode == TransportMode::BUS_UTTARA) {
                    graph.addUttaraStop(coords.front(), startName);
                    graph.addUttaraStop(coords.back(), endName);
                }
                stationCount += 2;
            }
            
            
            for (size_t i = 0; i + 1 < coords.size(); i++) {
                double dist = haversineDistance(coords[i], coords[i+1]);
                graph.addEdge(Edge(coords[i], coords[i+1], dist, mode));
                graph.addEdge(Edge(coords[i+1], coords[i], dist, mode));
                edgeCount += 2;
            }
        }
        
        file.close();
        
        std::string modeName = transportModeToString(mode);
        std::cout << "  ✓ " << edgeCount << " " << modeName << " segments loaded";
        if (stationCount > 0) {
            std::cout << " (" << stationCount/2 << " unique stations)";
        }
        std::cout << std::endl;
    }
    
    static void buildGraph(DhakaGraph& graph,
                          const std::string& roadmapFile,
                          const std::string& metroFile,
                          const std::string& bikolpoFile,
                          const std::string& uttaraFile) {
        std::cout << "Loading road network..." << std::endl;
        parseRoadmap(roadmapFile, graph);
        
        std::cout << "Loading metro network..." << std::endl;
        parseTransitRoute(metroFile, graph, TransportMode::METRO);
        
        std::cout << "Loading Bikolpo bus network..." << std::endl;
        parseTransitRoute(bikolpoFile, graph, TransportMode::BUS_BIKOLPO);
        
        std::cout << "Loading Uttara bus network..." << std::endl;
        parseTransitRoute(uttaraFile, graph, TransportMode::BUS_UTTARA);
    }
};

#endif 
