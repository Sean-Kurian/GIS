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
#include "m4.h"

#include <unittest++/UnitTest++.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>

TEST(extreme_multi_toronto_canada) {
    load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
    std::vector<DeliveryInfo> deliveries;
    std::vector<IntersectionIndex> depots;
    float turn_penalty;
    float truck_capacity;
    std::vector<CourierSubpath> result_path;

    deliveries = {DeliveryInfo(128368, 90666, 52.16571), DeliveryInfo(24143, 105845, 90.84896), DeliveryInfo(20756, 96363, 170.39661), DeliveryInfo(148729, 105845, 14.74155), DeliveryInfo(26866, 90455, 189.19469), DeliveryInfo(56686, 31736, 17.00520), DeliveryInfo(110603, 102507, 57.90324), DeliveryInfo(7107, 148377, 47.28886), DeliveryInfo(98631, 36531, 80.65243), DeliveryInfo(70300, 24130, 173.28859), DeliveryInfo(46252, 141588, 13.10448), DeliveryInfo(13180, 106439, 166.36078), DeliveryInfo(116876, 150526, 0.65950), DeliveryInfo(53224, 76483, 34.62863), DeliveryInfo(134849, 54575, 158.46289), DeliveryInfo(140662, 131385, 162.42998), DeliveryInfo(128368, 32796, 57.20420), DeliveryInfo(43487, 40382, 104.01823), DeliveryInfo(106836, 87192, 63.87435), DeliveryInfo(26866, 134786, 78.70642), DeliveryInfo(36361, 939, 37.46352), DeliveryInfo(141888, 125530, 1.36804), DeliveryInfo(58215, 112018, 36.81779), DeliveryInfo(36647, 41900, 31.73566), DeliveryInfo(131450, 86473, 112.34334), DeliveryInfo(50187, 86452, 125.32245), DeliveryInfo(110603, 11043, 39.80464), DeliveryInfo(36420, 112018, 9.97357), DeliveryInfo(78160, 103584, 34.01591), DeliveryInfo(43380, 97802, 182.03966), DeliveryInfo(128368, 45967, 35.91187), DeliveryInfo(92846, 14795, 101.83276), DeliveryInfo(122846, 19897, 168.86510), DeliveryInfo(58939, 150526, 87.13423), DeliveryInfo(141140, 43999, 56.31700), DeliveryInfo(26717, 29171, 77.97560), DeliveryInfo(84826, 112018, 160.20862), DeliveryInfo(92846, 147285, 30.13062), DeliveryInfo(67977, 87871, 35.03835), DeliveryInfo(57606, 37718, 136.10197), DeliveryInfo(145524, 67954, 178.42542), DeliveryInfo(70817, 84449, 172.69223), DeliveryInfo(26866, 115384, 26.76720), DeliveryInfo(26932, 103360, 79.41059), DeliveryInfo(6852, 56741, 28.55339), DeliveryInfo(26717, 24416, 100.89793), DeliveryInfo(26866, 127147, 43.87358), DeliveryInfo(30325, 939, 21.70639), DeliveryInfo(130141, 51166, 188.95027), DeliveryInfo(89818, 63527, 18.89853), DeliveryInfo(123564, 97802, 51.15905), DeliveryInfo(24659, 113999, 98.57288), DeliveryInfo(38918, 109846, 11.59753), DeliveryInfo(24143, 126928, 24.21732), DeliveryInfo(128368, 84723, 194.11719), DeliveryInfo(128368, 96559, 30.65087), DeliveryInfo(105909, 77616, 147.90996), DeliveryInfo(36147, 67293, 161.03790), DeliveryInfo(116379, 126304, 60.46647), DeliveryInfo(25854, 41900, 176.82239), DeliveryInfo(26866, 188, 193.57985), DeliveryInfo(1878, 10325, 93.29756), DeliveryInfo(61564, 110628, 166.75243), DeliveryInfo(51786, 96192, 105.97484), DeliveryInfo(135047, 30781, 111.33770), DeliveryInfo(41046, 23347, 182.04376), DeliveryInfo(125120, 74133, 34.73820), DeliveryInfo(69793, 67797, 47.85841), DeliveryInfo(131450, 101057, 173.20018), DeliveryInfo(21116, 14055, 145.52446), DeliveryInfo(120472, 20747, 11.68320), DeliveryInfo(36147, 84449, 159.88330), DeliveryInfo(138275, 115172, 127.36371), DeliveryInfo(9037, 105845, 173.52748), DeliveryInfo(7107, 57036, 145.23422), DeliveryInfo(65531, 110605, 1.14073), DeliveryInfo(48535, 147285, 64.26728), DeliveryInfo(29698, 67954, 45.76415), DeliveryInfo(24143, 51681, 81.30389), DeliveryInfo(25868, 29593, 20.28520), DeliveryInfo(50955, 115090, 186.98189), DeliveryInfo(78677, 70669, 3.11204), DeliveryInfo(56464, 33086, 21.39054), DeliveryInfo(63457, 42351, 142.56639), DeliveryInfo(131937, 11004, 22.22210), DeliveryInfo(906, 86436, 83.04862), DeliveryInfo(26717, 28731, 40.01344), DeliveryInfo(56819, 10325, 88.56308), DeliveryInfo(75909, 130777, 28.46552), DeliveryInfo(114599, 87508, 59.15406), DeliveryInfo(2532, 123927, 25.49306), DeliveryInfo(120157, 10335, 175.61475), DeliveryInfo(45463, 114192, 99.68723), DeliveryInfo(88995, 21042, 2.24697), DeliveryInfo(2526, 34766, 63.38704), DeliveryInfo(24143, 136875, 155.72171), DeliveryInfo(106836, 84449, 42.68796), DeliveryInfo(128368, 63527, 190.45369), DeliveryInfo(131450, 100320, 189.92645), DeliveryInfo(99914, 56545, 139.67902), DeliveryInfo(65531, 30253, 45.70741), DeliveryInfo(143226, 112018, 142.14262), DeliveryInfo(56819, 67954, 166.03258), DeliveryInfo(37399, 101057, 158.49792), DeliveryInfo(56819, 147285, 52.88765), DeliveryInfo(142661, 5207, 139.54852), DeliveryInfo(25095, 136251, 146.97165), DeliveryInfo(91799, 101057, 68.28905), DeliveryInfo(99475, 43398, 180.86418), DeliveryInfo(98300, 81333, 94.69167), DeliveryInfo(34109, 71355, 61.21972), DeliveryInfo(12428, 135399, 137.25305), DeliveryInfo(91575, 132462, 142.48959), DeliveryInfo(110603, 106654, 114.96309), DeliveryInfo(27668, 939, 183.25032), DeliveryInfo(135696, 147285, 76.44156), DeliveryInfo(36647, 142693, 110.56858), DeliveryInfo(148729, 138833, 56.08653), DeliveryInfo(26866, 51681, 152.71448), DeliveryInfo(26384, 142983, 80.78411), DeliveryInfo(74187, 120392, 129.76117), DeliveryInfo(20410, 141588, 101.30692), DeliveryInfo(147775, 53214, 185.83119), DeliveryInfo(115112, 29593, 57.19205), DeliveryInfo(51489, 138833, 117.31264), DeliveryInfo(89542, 58413, 197.90628), DeliveryInfo(131450, 128282, 133.81400), DeliveryInfo(113085, 79166, 134.87607), DeliveryInfo(131450, 30781, 2.40866), DeliveryInfo(41902, 121238, 68.20745), DeliveryInfo(36647, 142693, 41.75874), DeliveryInfo(48026, 95462, 136.19992), DeliveryInfo(123274, 51681, 106.33556), DeliveryInfo(110603, 138833, 123.90230), DeliveryInfo(148090, 143577, 87.81820), DeliveryInfo(131450, 100320, 72.34099), DeliveryInfo(24143, 147045, 171.90059), DeliveryInfo(148729, 128605, 12.86886), DeliveryInfo(125785, 97802, 192.83534), DeliveryInfo(30987, 83283, 154.84070), DeliveryInfo(69063, 88136, 115.00397), DeliveryInfo(51708, 69120, 156.49841), DeliveryInfo(47950, 77703, 135.02206), DeliveryInfo(19753, 125346, 50.37898), DeliveryInfo(99914, 36345, 43.65586), DeliveryInfo(101614, 63527, 111.96758), DeliveryInfo(38522, 149206, 187.09366), DeliveryInfo(9037, 100813, 197.46153), DeliveryInfo(32474, 29788, 65.67228), DeliveryInfo(92846, 55983, 66.47141), DeliveryInfo(53919, 58731, 41.26199), DeliveryInfo(131450, 51009, 157.82123), DeliveryInfo(18453, 150526, 188.15334), DeliveryInfo(26866, 96192, 149.74525), DeliveryInfo(120125, 143313, 153.09782), DeliveryInfo(4402, 124360, 184.94472), DeliveryInfo(115825, 149206, 66.36335), DeliveryInfo(57606, 11004, 25.48172), DeliveryInfo(30019, 143933, 28.32197), DeliveryInfo(46762, 125346, 129.61548), DeliveryInfo(26717, 30434, 69.01270), DeliveryInfo(65531, 104246, 112.01280), DeliveryInfo(24143, 112018, 170.29016), DeliveryInfo(99914, 94563, 76.12354), DeliveryInfo(13864, 109154, 159.08685), DeliveryInfo(43730, 125346, 81.69088), DeliveryInfo(44019, 42351, 100.36263), DeliveryInfo(36647, 125869, 4.53757), DeliveryInfo(31713, 106827, 33.04659), DeliveryInfo(97971, 43107, 192.05797), DeliveryInfo(6100, 96192, 29.90458), DeliveryInfo(57606, 7799, 124.01443), DeliveryInfo(131450, 40382, 100.95026), DeliveryInfo(65531, 151245, 169.73723), DeliveryInfo(26717, 84449, 186.04260), DeliveryInfo(99914, 149072, 62.29188), DeliveryInfo(65531, 142693, 91.39250), DeliveryInfo(147042, 73517, 7.75393), DeliveryInfo(99939, 49477, 89.98482), DeliveryInfo(72299, 44104, 56.93989), DeliveryInfo(99914, 40382, 145.20457), DeliveryInfo(106836, 28182, 170.25377), DeliveryInfo(36147, 97802, 119.63917), DeliveryInfo(65531, 29593, 102.42642), DeliveryInfo(100371, 30781, 25.59460), DeliveryInfo(96421, 939, 76.98265), DeliveryInfo(116429, 4299, 29.30084), DeliveryInfo(18428, 106738, 181.08267), DeliveryInfo(68916, 147219, 42.22037), DeliveryInfo(92798, 150526, 19.62985), DeliveryInfo(38619, 29593, 152.69012), DeliveryInfo(110603, 119498, 104.85538), DeliveryInfo(139081, 141588, 74.19072), DeliveryInfo(7107, 145004, 60.03025), DeliveryInfo(92846, 141515, 120.19324), DeliveryInfo(99914, 138772, 48.23465), DeliveryInfo(69053, 125346, 134.83749), DeliveryInfo(56819, 40382, 140.66013), DeliveryInfo(26866, 8713, 4.58767), DeliveryInfo(26717, 28182, 51.41176), DeliveryInfo(26717, 115422, 97.20940), DeliveryInfo(56819, 125346, 194.54651), DeliveryInfo(120497, 102117, 154.73692), DeliveryInfo(12990, 80976, 106.19604), DeliveryInfo(31506, 101057, 188.78438), DeliveryInfo(130773, 74975, 11.00346), DeliveryInfo(88823, 83752, 147.96347), DeliveryInfo(99914, 54353, 145.08344), DeliveryInfo(142663, 109447, 122.79404), DeliveryInfo(92846, 84449, 191.24759), DeliveryInfo(5709, 100320, 112.70341), DeliveryInfo(104773, 101538, 106.43143), DeliveryInfo(87009, 69793, 151.16974), DeliveryInfo(92846, 77660, 114.74046), DeliveryInfo(22635, 70068, 37.23694), DeliveryInfo(40478, 42351, 64.49900), DeliveryInfo(103247, 82323, 31.35605), DeliveryInfo(7107, 130842, 187.82190), DeliveryInfo(80441, 30781, 151.04733), DeliveryInfo(89818, 108377, 5.38100), DeliveryInfo(147775, 133611, 146.48691), DeliveryInfo(92846, 116425, 69.97147), DeliveryInfo(9037, 67954, 193.05806), DeliveryInfo(120817, 65518, 105.59691), DeliveryInfo(65531, 102129, 68.79330), DeliveryInfo(126555, 31736, 121.11068), DeliveryInfo(41647, 96192, 133.26616), DeliveryInfo(110047, 75478, 90.69925), DeliveryInfo(142333, 51681, 143.60081), DeliveryInfo(148729, 96508, 155.14073), DeliveryInfo(9037, 129770, 198.19899), DeliveryInfo(148729, 41900, 63.43557), DeliveryInfo(115364, 87624, 98.39468), DeliveryInfo(131450, 66779, 9.57014), DeliveryInfo(99914, 28182, 160.33247), DeliveryInfo(147089, 17540, 8.83452), DeliveryInfo(36923, 71793, 50.89398), DeliveryInfo(73209, 146782, 103.41691), DeliveryInfo(69208, 73438, 37.51317), DeliveryInfo(128368, 1708, 198.25780), DeliveryInfo(57606, 29593, 101.96731), DeliveryInfo(24143, 5207, 64.09848), DeliveryInfo(105380, 130867, 97.65171), DeliveryInfo(108487, 84696, 145.69057), DeliveryInfo(146131, 47441, 29.24379), DeliveryInfo(63536, 113999, 27.03202), DeliveryInfo(148729, 108646, 62.47343), DeliveryInfo(36147, 137044, 55.18011), DeliveryInfo(145225, 40382, 199.15579), DeliveryInfo(36647, 91784, 49.15639), DeliveryInfo(131450, 64437, 28.09111), DeliveryInfo(55591, 30781, 186.11127), DeliveryInfo(57606, 93624, 177.25247), DeliveryInfo(47599, 97802, 153.86888), DeliveryInfo(7107, 93368, 4.84624), DeliveryInfo(120679, 30702, 133.00502), DeliveryInfo(89864, 42351, 85.63468), DeliveryInfo(124878, 129899, 71.93238), DeliveryInfo(3042, 137826, 47.89386), DeliveryInfo(105659, 28182, 84.29709)};
    depots = {13, 51601, 61505, 85936, 132265};
    turn_penalty = 15.000000000;
    truck_capacity = 7768.253417969;
    {
        result_path = traveling_courier(deliveries, depots, turn_penalty, truck_capacity);
    }
} //extreme_multi_toronto_canada


TEST(easy_toronto_canada) {
    load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
    std::vector<DeliveryInfo> deliveries;
    std::vector<IntersectionIndex> depots;
    float turn_penalty;
    float truck_capacity;
    std::vector<CourierSubpath> result_path;

    deliveries = {DeliveryInfo(106896, 96006, 75.05315), DeliveryInfo(65038, 87076, 90.76368), DeliveryInfo(135221, 28262, 53.31088), DeliveryInfo(95563, 109313, 169.31749), DeliveryInfo(82672, 101772, 124.38162), DeliveryInfo(74966, 56035, 25.56255), DeliveryInfo(134204, 51749, 129.98366), DeliveryInfo(103341, 37007, 41.05344), DeliveryInfo(105969, 10229, 90.72692), DeliveryInfo(102380, 42614, 79.29823), DeliveryInfo(42199, 117913, 4.35320), DeliveryInfo(119658, 32615, 133.52373), DeliveryInfo(86334, 72388, 123.70870), DeliveryInfo(113281, 81132, 142.89053), DeliveryInfo(121985, 133821, 69.00546), DeliveryInfo(29435, 48326, 162.74165), DeliveryInfo(145491, 82634, 102.08238), DeliveryInfo(81186, 87321, 18.80107), DeliveryInfo(137127, 125686, 146.41916), DeliveryInfo(34975, 45024, 198.81140), DeliveryInfo(27101, 140389, 24.81209), DeliveryInfo(40756, 122694, 104.19133), DeliveryInfo(117691, 82535, 19.50759), DeliveryInfo(149474, 121164, 50.89673), DeliveryInfo(136515, 38216, 36.02774)};
    depots = {10, 38701};
    turn_penalty = 15.000000000;
    truck_capacity = 2427.750000000;
//    result_path = v2traveling_courier(deliveries, depots, turn_penalty, truck_capacity);
    result_path = traveling_courier(deliveries, depots, turn_penalty, truck_capacity);
}


//TEST(pathFindingTest) {
//    load_map("/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin");
//    
//    unsigned startInt = 657;
//    unsigned endInt = 443;
//    
//    std::vector<int> segsOfInt = find_street_segments_of_intersection(startInt);
//    std::cout << "Segs of start: \n";
//    for (const int& seg : segsOfInt) {
//        std::cout << "SegID: " << seg << "\n";
//    }
//    
//    std::vector<int> segsOfEnd = find_street_segments_of_intersection(endInt);
//    std::cout << "Segs of end: \n";
//    for (const int& seg : segsOfEnd) {
//        std::cout << "SegID: " << seg << "\n";
//    }
//    
//    auto startFind = std::chrono::high_resolution_clock::now();
//    
//    std::vector<int> path = find_path_between_intersections(startInt, endInt, 0.00000000000000000);
//    
//    auto endFind = std::chrono::high_resolution_clock::now();
//    std::cout << "Time taken to find path: "
//              << std::chrono::duration_cast<std::chrono::microseconds>(endFind - startFind).count() << " ns\n";
//    
//    auto startCalc = std::chrono::high_resolution_clock::now();
//    
//    double time = compute_path_travel_time(path, 0.00000000000000000);
//    
//    auto endCalc = std::chrono::high_resolution_clock::now();
//    std::cout << "Time taken to calc time: "
//              << std::chrono::duration_cast<std::chrono::microseconds>(endCalc - startCalc).count() << " ns\n";
//    
//    std::cout << "Time: " << time << "\n";
//    std::cout << "Path: \n";
//    for (const int& seg : path) {
//        std::cout << "SegID: " << seg << "\n";
//    }
//    
//    startFind = std::chrono::high_resolution_clock::now();
//    
//    std::pair<std::vector<StreetSegmentIndex>, std::vector<StreetSegmentIndex>> walkPath = find_path_with_walk_to_pick_up(109, 1086, 22.58759087598575732, 1.39999999999999991, 0.00000000000000000);
//    
//    endFind = std::chrono::high_resolution_clock::now();
//    std::cout << "Time taken to find path: "
//              << std::chrono::duration_cast<std::chrono::microseconds>(endFind - startFind).count() << " ns\n";
//    
//    startCalc = std::chrono::high_resolution_clock::now();
//    
//    time = compute_path_travel_time(path, 0.00000000000000000);
//    
//    endCalc = std::chrono::high_resolution_clock::now();
//    std::cout << "Time taken to calc time: "
//              << std::chrono::duration_cast<std::chrono::microseconds>(endCalc - startCalc).count() << " ns\n";
//    
//    std::cout << "Time: " << time << "\n";
//    std::cout << "Path: \n";
//    for (const int& seg : path) {
//        std::cout << "SegID: " << seg << "\n";
//    }
//}

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

