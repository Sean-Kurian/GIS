#ifndef COORDDATA_H
#define COORDDATA_H

struct CoordData {
    CoordData() : minLat(0), minLon(0), maxLat(0), maxLon(0), 
                  avgLat(0), avgLon(0), latAspectRatio(0) { }
    double minLat, minLon;
    double maxLat, maxLon;
    double avgLat, avgLon;
    double latAspectRatio;
};

#endif /* COORDDATA_H */

