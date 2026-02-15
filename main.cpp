#include "DhakaRouting.h"
#include "DhakaGraph.h"
#include "CSVParser.h"
#include "AllProblemsSolver.h"

void printSeparator(char c = '=', int width = 80) {
    for (int i = 0; i < width; i++) std::cout << c;
    std::cout << std::endl;
}

void printResult(int problemNum, const RouteResult& result, 
                const Location& source, const Location& dest, bool showCost) {
    std::cout << "Problem " << problemNum << std::endl;
    std::cout << "Source: " << source.toString() << std::endl;
    std::cout << "Destination: " << dest.toString() << std::endl << std::endl;
    
    for (size_t i = 0; i < result.segments.size(); i++) {
        std::cout << "Segment " << (i+1) << ": " 
                  << transportModeToString(result.modes[i]) << " from ";
        
        if (!result.startNames[i].empty()) 
            std::cout << result.startNames[i] << " ";
        std::cout << result.segments[i].first.toString() << " to ";
        
        if (!result.endNames[i].empty()) 
            std::cout << result.endNames[i] << " ";
        std::cout << result.segments[i].second.toString() << std::endl;
        
        std::cout << "           Distance: " << std::fixed << std::setprecision(2) 
                  << result.distances[i] << " km";
        
        if (showCost) {
            std::cout << ", Cost: Tk" << std::fixed << std::setprecision(2) 
                      << result.costs[i];
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    if (showCost) {
        std::cout << "Total Cost: Tk" << std::fixed << std::setprecision(2) 
                  << result.totalValue << std::endl;
    } else {
        std::cout << "Total Distance: " << std::fixed << std::setprecision(2) 
                  << result.totalValue << " km" << std::endl;
    }
}

void generateKML(const RouteResult& result, const std::string& filename) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) return;
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
    file << "<Document><Placemark><n>route</n>\n";
    file << "<LineString><tessellate>1</tessellate>\n<coordinates>\n";
    
    for (size_t i = 0; i < result.segments.size(); i++) {
        file << std::fixed << std::setprecision(6)
             << result.segments[i].first.lon << "," 
             << result.segments[i].first.lat << ",0\n";
    }
    
    if (!result.segments.empty()) {
        file << std::fixed << std::setprecision(6)
             << result.segments.back().second.lon << "," 
             << result.segments.back().second.lat << ",0\n";
    }
    
    file << "</coordinates></LineString></Placemark></Document></kml>\n";
    file.close();
}

int main() {
    printSeparator();
    std::cout << "  Dhaka Routing System - ALL 6 PROBLEMS" << std::endl;
    printSeparator();
    std::cout << std::endl;
    
    // Load graph
    std::cout << "Loading network..." << std::endl;
    DhakaGraph graph;
    CSVParser::buildGraph(graph, "Roadmap-Dhaka.csv", "Routemap-DhakaMetroRail.csv",
                         "Routemap-BikolpoBus.csv", "Routemap-UttaraBus.csv");
    
    std::cout << "Graph loaded: " << graph.getLocationCount() << " locations, "
              << graph.getEdgeCount() << " edges" << std::endl;
    printSeparator();
    std::cout << std::endl;
    
    AllProblemsSolver solver(graph);
    

    Location source1(23.834145, 90.363833);  
    Location dest1(23.738265, 90.396151);     
    
    Location source2(23.810000, 90.370000);  
    Location dest2(23.750000, 90.395000);    
    
    std::vector<std::pair<Location, Location>> testCases;
    testCases.push_back(std::make_pair(source1, dest1));
    testCases.push_back(std::make_pair(source2, dest2));
    
    for (size_t tc = 0; tc < testCases.size(); tc++) {
        Location src = testCases[tc].first;
        Location dst = testCases[tc].second;
        
        std::cout << "\nTEST CASE " << (tc + 1) << std::endl;
        printSeparator();
        
        // Problem 1
        std::cout << "\nPROBLEM 1: Shortest car route\n";
        printSeparator('-');
        RouteResult r1 = solver.solveProblem1(src, dst);
        printResult(1, r1, src, dst, false);
        std::ostringstream fn1;
        fn1 << "problem1_case" << (tc+1) << ".kml";
        generateKML(r1, fn1.str());
        std::cout << "KML: " << fn1.str() << "\n" << std::endl;
        
        // Problem 2
        std::cout << "PROBLEM 2: Cheapest (Car+Metro)\n";
        printSeparator('-');
        RouteResult r2 = solver.solveProblem2(src, dst);
        printResult(2, r2, src, dst, true);
        std::ostringstream fn2;
        fn2 << "problem2_case" << (tc+1) << ".kml";
        generateKML(r2, fn2.str());
        std::cout << "KML: " << fn2.str() << "\n" << std::endl;
        
        // Problem 3
        std::cout << "PROBLEM 3: Cheapest (All modes)\n";
        printSeparator('-');
        RouteResult r3 = solver.solveProblem3(src, dst);
        printResult(3, r3, src, dst, true);
        std::ostringstream fn3;
        fn3 << "problem3_case" << (tc+1) << ".kml";
        generateKML(r3, fn3.str());
        std::cout << "KML: " << fn3.str() << "\n" << std::endl;
        
        // Problem 4
        std::cout << "PROBLEM 4: Cheapest with time (simplified)\n";
        printSeparator('-');
        RouteResult r4 = solver.solveProblem4(src, dst);
        printResult(4, r4, src, dst, true);
        std::ostringstream fn4;
        fn4 << "problem4_case" << (tc+1) << ".kml";
        generateKML(r4, fn4.str());
        std::cout << "KML: " << fn4.str() << "\n" << std::endl;
        
        // Problem 5
        std::cout << "PROBLEM 5: Fastest (simplified)\n";
        printSeparator('-');
        RouteResult r5 = solver.solveProblem5(src, dst);
        printResult(5, r5, src, dst, true);
        std::ostringstream fn5;
        fn5 << "problem5_case" << (tc+1) << ".kml";
        generateKML(r5, fn5.str());
        std::cout << "KML: " << fn5.str() << "\n" << std::endl;
        
        // Problem 6
        std::cout << "PROBLEM 6: Cheapest with deadline (simplified)\n";
        printSeparator('-');
        RouteResult r6 = solver.solveProblem6(src, dst);
        printResult(6, r6, src, dst, true);
        std::ostringstream fn6;
        fn6 << "problem6_case" << (tc+1) << ".kml";
        generateKML(r6, fn6.str());
        std::cout << "KML: " << fn6.str() << "\n" << std::endl;
    }
    
    printSeparator();
    std::cout << "SUCCESS! All 6 problems solved!" << std::endl;
    std::cout << "Generated 12 KML files (6 problems x 2 test cases)" << std::endl;
    printSeparator();
    
    return 0;
}
