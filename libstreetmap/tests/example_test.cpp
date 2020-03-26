/* 
 * Copyright 2020 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pathfinding.h"
#include "m1.h"

#include <unittest++/UnitTest++.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>

TEST(pathFindingTest) {
    load_map("/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin");
    
    unsigned startInt = 657;
    unsigned endInt = 443;
    
    std::vector<int> segsOfInt = find_street_segments_of_intersection(startInt);
    std::cout << "Segs of start: \n";
    for (const int& seg : segsOfInt) {
        std::cout << "SegID: " << seg << "\n";
    }
    
    std::vector<int> segsOfEnd = find_street_segments_of_intersection(endInt);
    std::cout << "Segs of end: \n";
    for (const int& seg : segsOfEnd) {
        std::cout << "SegID: " << seg << "\n";
    }
    
    auto startFind = std::chrono::high_resolution_clock::now();
    
    std::vector<int> path = find_path_between_intersections(startInt, endInt, 0.00000000000000000);
    
    auto endFind = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to find path: "
              << std::chrono::duration_cast<std::chrono::microseconds>(endFind - startFind).count() << " ns\n";
    
    auto startCalc = std::chrono::high_resolution_clock::now();
    
    double time = compute_path_travel_time(path, 0.00000000000000000);
    
    auto endCalc = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to calc time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(endCalc - startCalc).count() << " ns\n";
    
    std::cout << "Time: " << time << "\n";
    std::cout << "Path: \n";
    for (const int& seg : path) {
        std::cout << "SegID: " << seg << "\n";
    }
    
    startFind = std::chrono::high_resolution_clock::now();
    
    std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> walkPath = find_path_with_walk_to_pickup(109, 1086, 22.58759087598575732, 1.39999999999999991, 0.00000000000000000);
    
    endFind = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to find path: "
              << std::chrono::duration_cast<std::chrono::microseconds>(endFind - startFind).count() << " ns\n";
    
    startCalc = std::chrono::high_resolution_clock::now();
    
    time = compute_path_travel_time(path, 0.00000000000000000);
    
    endCalc = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to calc time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(endCalc - startCalc).count() << " ns\n";
    
    std::cout << "Time: " << time << "\n";
    std::cout << "Path: \n";
    for (const int& seg : path) {
        std::cout << "SegID: " << seg << "\n";
    }
}

// Tests how the map stores its data
// Removed comparison function that was being used on compareMap
//TEST(MapComparitorTest) {
//    std::multimap<std::string, unsigned> defaultMap;
//    std::multimap<std::string, unsigned> compareMap;
//    
//    std::string testName = "Saint";
//    unsigned num = 10;   
//    defaultMap.insert(std::make_pair(testName, num));
//    compareMap.insert(std::make_pair(testName, num));
//    
//    testName = "saint";
//    num = 20;   
//    defaultMap.insert(std::make_pair(testName, num));
//    compareMap.insert(std::make_pair(testName, num));
//    
//    testName = "sbint";
//    num = 30;   
//    defaultMap.insert(std::make_pair(testName, num));
//    compareMap.insert(std::make_pair(testName, num));
//    
//    testName = "Saint Toronto";
//    num = 40;   
//    defaultMap.insert(std::make_pair(testName, num));
//    compareMap.insert(std::make_pair(testName, num));
//    
//    testName = "SaintToronto";
//    num = 50;   
//    defaultMap.insert(std::make_pair(testName, num));
//    compareMap.insert(std::make_pair(testName, num));
//    
//    unsigned count = 0;
//    std::cout << "============== Default Map ==============\n";
//    for (auto itrd = defaultMap.begin(); itrd != defaultMap.end(); ++itrd) {
//        std::cout << "======= Element " << count << ": =======\nKey: " << itrd->first
//                  << "\nValue: " << itrd->second << "\n";
//        count++;
//    }
//    
//    count = 0;
//    std::cout << "============== Compare Map ==============\n";
//    for (auto itrc = compareMap.begin(); itrc != compareMap.end(); ++itrc) {
//        std::cout << "======= Element " << count << ": =======\nKey: " << itrc->first
//                  << "\nValue: " << itrc->second << "\n";
//        count++;
//    }
//    std::string name = "Sa";
//    std::vector<int> streetIDs;
//    if (!name.empty()) {
//        auto itr = compareMap.lower_bound(name);
//        auto end = compareMap.end();
//        if (itr != end) {
//            std::string nameMatch = itr->first;
//            std::transform(nameMatch.begin(), nameMatch.end(), nameMatch.begin(), ::tolower);
//            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
//            
//            while (!nameMatch.compare(0, name.size(), name, 0, name.size())) {
//                streetIDs.push_back(itr->second);
//                itr++;
//                if (itr != end) {
//                    nameMatch = itr->first;
//                    std::transform(nameMatch.begin(), nameMatch.end(), nameMatch.begin(), ::tolower);
//                }
//                else if (itr == end)
//                    break;
//            }
//        }
//    }
//    std::cout << "============== IDs Found ==============\n";
//    for (unsigned i = 0; i < streetIDs.size(); ++i) {
//        std::cout << streetIDs[i] << " ";
//    }
//    std::cout << "\n";
//    CHECK(true);
//}

