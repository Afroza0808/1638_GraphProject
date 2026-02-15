#ifndef ALL_PROBLEMS_SOLVER_H
#define ALL_PROBLEMS_SOLVER_H

#include "DhakaRouting.h"
#include "DhakaGraph.h"
#include <sstream>


struct RouteResult {
    std::vector<std::pair<Location, Location>> segments;
    std::vector<TransportMode> modes;
    std::vector<double> distances;
    std::vector<double> costs;
    std::vector<std::string> startNames;
    std::vector<std::string> endNames;
    double totalValue; 
    
    RouteResult() : totalValue(0.0) {}
};

struct DijkstraNode {
    Location location;
    double cost;
    
    DijkstraNode(const Location& loc, double c) : location(loc), cost(c) {}
    
    bool operator>(const DijkstraNode& other) const {
        return cost > other.cost;
    }
};

class AllProblemsSolver {
private:
    const DhakaGraph& graph;
    
    std::vector<Edge> dijkstra(const Location& source, const Location& destination,
                               const std::set<TransportMode>& allowedModes,
                               std::function<double(const Edge&)> costFunc) const {
        std::map<Location, double> distances;
        std::map<Location, Location> previous;
        std::vector<Edge> allEdges;
        std::map<Location, size_t> edgeIndices;
        std::set<Location> visited;
        
        std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;
        
        distances[source] = 0.0;
        pq.push(DijkstraNode(source, 0.0));
        
        while (!pq.empty()) {
            DijkstraNode current = pq.top();
            pq.pop();
            
            if (visited.count(current.location)) continue;
            visited.insert(current.location);
            
            if (current.location == destination) break;
            
            for (const auto& edge : graph.getNeighbors(current.location)) {
                if (allowedModes.find(edge.mode) == allowedModes.end()) continue;
                
                double cost = costFunc(edge);
                double newDist = current.cost + cost;
                
                if (distances.find(edge.end) == distances.end() || newDist < distances[edge.end]) {
                    distances[edge.end] = newDist;
                    previous[edge.end] = current.location;
                    allEdges.push_back(edge);
                    edgeIndices[edge.end] = allEdges.size() - 1;
                    pq.push(DijkstraNode(edge.end, newDist));
                }
            }
        }
        
        std::vector<Edge> path;
        Location current = destination;
        
        while (current != source) {
            if (previous.find(current) == previous.end()) {
                return std::vector<Edge>();
            }
            if (edgeIndices.find(current) != edgeIndices.end()) {
                path.push_back(allEdges[edgeIndices[current]]);
            }
            current = previous[current];
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }
    
    RouteResult convertToResult(const std::vector<Edge>& edges, const Location& source, const Location& dest) const {
        RouteResult result;
        
        Location nearestSource = edges.empty() ? source : edges.front().start;
        Location nearestDest = edges.empty() ? dest : edges.back().end;
        
        double walkStart = haversineDistance(source, nearestSource);
        double walkEnd = haversineDistance(nearestDest, dest);
        
        if (walkStart > 0.001) {
            result.segments.push_back(std::make_pair(source, nearestSource));
            result.modes.push_back(TransportMode::WALK);
            result.distances.push_back(walkStart);
            result.costs.push_back(0.0);
            result.startNames.push_back("");
            result.endNames.push_back("");
        }
        
        
        if (!edges.empty()) {
            TransportMode currentMode = edges[0].mode;
            Location segStart = edges[0].start;
            Location segEnd = edges[0].end;
            double segDist = edges[0].distance;
            
            for (size_t i = 1; i < edges.size(); i++) {
                if (edges[i].mode == currentMode && edges[i].start == segEnd) {
                    segEnd = edges[i].end;
                    segDist += edges[i].distance;
                } else {
                    result.segments.push_back(std::make_pair(segStart, segEnd));
                    result.modes.push_back(currentMode);
                    result.distances.push_back(segDist);
                    result.costs.push_back(0.0);
                    result.startNames.push_back(graph.getStationName(segStart));
                    result.endNames.push_back(graph.getStationName(segEnd));
                    
                    currentMode = edges[i].mode;
                    segStart = edges[i].start;
                    segEnd = edges[i].end;
                    segDist = edges[i].distance;
                }
            }
            
            result.segments.push_back(std::make_pair(segStart, segEnd));
            result.modes.push_back(currentMode);
            result.distances.push_back(segDist);
            result.costs.push_back(0.0);
            result.startNames.push_back(graph.getStationName(segStart));
            result.endNames.push_back(graph.getStationName(segEnd));
        }
        
        if (walkEnd > 0.001) {
            result.segments.push_back(std::make_pair(nearestDest, dest));
            result.modes.push_back(TransportMode::WALK);
            result.distances.push_back(walkEnd);
            result.costs.push_back(0.0);
            result.startNames.push_back("");
            result.endNames.push_back("");
        }
        
        return result;
    }

public:
    AllProblemsSolver(const DhakaGraph& g) : graph(g) {}
    
    // PROBLEM 1
    RouteResult solveProblem1(const Location& source, const Location& dest) const {
        Location nearestSrc = graph.findNearestLocation(source);
        Location nearestDst = graph.findNearestLocation(dest);
        
        auto costFunc = [](const Edge& e) { return e.distance; };
        std::set<TransportMode> modes;
        modes.insert(TransportMode::CAR);
        
        auto edges = dijkstra(nearestSrc, nearestDst, modes, costFunc);
        RouteResult result = convertToResult(edges, source, dest);
        
        for (size_t i = 0; i < result.distances.size(); i++) {
            result.totalValue += result.distances[i];
        }
        
        return result;
    }
    
    RouteResult solveProblem2(const Location& source, const Location& dest) const {
        const double CAR_COST = 20.0;
        const double METRO_COST = 5.0;
        
        Location nearestSrc = graph.findNearestLocation(source);
        Location nearestDst = graph.findNearestLocation(dest);
        
        auto costFunc = [CAR_COST, METRO_COST](const Edge& e) {
            if (e.mode == TransportMode::CAR) return e.distance * CAR_COST;
            if (e.mode == TransportMode::METRO) return e.distance * METRO_COST;
            return 0.0;
        };
        
        std::set<TransportMode> modes;
        modes.insert(TransportMode::CAR);
        modes.insert(TransportMode::METRO);
        
        auto edges = dijkstra(nearestSrc, nearestDst, modes, costFunc);
        RouteResult result = convertToResult(edges, source, dest);
        
        for (size_t i = 0; i < result.distances.size(); i++) {
            if (result.modes[i] == TransportMode::CAR) {
                result.costs[i] = result.distances[i] * CAR_COST;
            } else if (result.modes[i] == TransportMode::METRO) {
                result.costs[i] = result.distances[i] * METRO_COST;
            }
            result.totalValue += result.costs[i];
        }
        
        return result;
    }
    
    
    RouteResult solveProblem3(const Location& source, const Location& dest) const {
        const double CAR_COST = 20.0;
        const double METRO_COST = 5.0;
        const double BUS_COST = 7.0;
        
        Location nearestSrc = graph.findNearestLocation(source);
        Location nearestDst = graph.findNearestLocation(dest);
        
        auto costFunc = [CAR_COST, METRO_COST, BUS_COST](const Edge& e) {
            if (e.mode == TransportMode::CAR) return e.distance * CAR_COST;
            if (e.mode == TransportMode::METRO) return e.distance * METRO_COST;
            if (e.mode == TransportMode::BUS_BIKOLPO || e.mode == TransportMode::BUS_UTTARA)
                return e.distance * BUS_COST;
            return 0.0;
        };
        
        std::set<TransportMode> modes;
        modes.insert(TransportMode::CAR);
        modes.insert(TransportMode::METRO);
        modes.insert(TransportMode::BUS_BIKOLPO);
        modes.insert(TransportMode::BUS_UTTARA);
        
        auto edges = dijkstra(nearestSrc, nearestDst, modes, costFunc);
        RouteResult result = convertToResult(edges, source, dest);
        
        for (size_t i = 0; i < result.distances.size(); i++) {
            if (result.modes[i] == TransportMode::CAR) {
                result.costs[i] = result.distances[i] * CAR_COST;
            } else if (result.modes[i] == TransportMode::METRO) {
                result.costs[i] = result.distances[i] * METRO_COST;
            } else if (result.modes[i] == TransportMode::BUS_BIKOLPO || 
                       result.modes[i] == TransportMode::BUS_UTTARA) {
                result.costs[i] = result.distances[i] * BUS_COST;
            }
            result.totalValue += result.costs[i];
        }
        
        return result;
    }
    
    RouteResult solveProblem4(const Location& source, const Location& dest) const {
        return solveProblem3(source, dest);
    }
    
    RouteResult solveProblem5(const Location& source, const Location& dest) const {
        return solveProblem3(source, dest);
    }
    
    RouteResult solveProblem6(const Location& source, const Location& dest) const {
        return solveProblem3(source, dest);
    }
};

#endif
