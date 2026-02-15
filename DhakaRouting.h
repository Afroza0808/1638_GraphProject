#ifndef DHAKA_ROUTING_H
#define DHAKA_ROUTING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <functional>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const double EARTH_RADIUS_KM = 6371.0;
const double WALK_SPEED_KMH = 2.0;
const double EPSILON = 1e-6;

enum class TransportMode {
    WALK,
    CAR,
    METRO,
    BUS_BIKOLPO,
    BUS_UTTARA
};

struct Location {
    double lat;
    double lon;
    
    Location() : lat(0.0), lon(0.0) {}
    Location(double latitude, double longitude) : lat(latitude), lon(longitude) {}
    
    bool operator==(const Location& other) const {
        return std::abs(lat - other.lat) < EPSILON && 
               std::abs(lon - other.lon) < EPSILON;
    }
    
    bool operator!=(const Location& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Location& other) const {
        if (std::abs(lat - other.lat) >= EPSILON) {
            return lat < other.lat;
        }
        return lon < other.lon;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << "(" << lon << "," << lat << ")";
        return oss.str();
    }
};

struct Edge {
    Location start;
    Location end;
    double distance;
    TransportMode mode;
    std::string name;
    
    Edge() : start(), end(), distance(0.0), mode(TransportMode::WALK), name("") {}
    Edge(const Location& s, const Location& e, double d, TransportMode m, const std::string& n = "")
        : start(s), end(e), distance(d), mode(m), name(n) {}
};

inline std::string transportModeToString(TransportMode mode) {
    switch (mode) {
        case TransportMode::WALK: return "Walk";
        case TransportMode::CAR: return "Car";
        case TransportMode::METRO: return "Metro";
        case TransportMode::BUS_BIKOLPO: return "Bikolpo Bus";
        case TransportMode::BUS_UTTARA: return "Uttara Bus";
        default: return "Unknown";
    }
}

inline double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

inline double haversineDistance(const Location& loc1, const Location& loc2) {
    double lat1 = toRadians(loc1.lat);
    double lon1 = toRadians(loc1.lon);
    double lat2 = toRadians(loc2.lat);
    double lon2 = toRadians(loc2.lon);
    
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    
    double a = std::sin(dlat/2) * std::sin(dlat/2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon/2) * std::sin(dlon/2);
    double c = 2 * std::asin(std::sqrt(a));
    
    return EARTH_RADIUS_KM * c;
}

class DhakaGraph;

#endif
