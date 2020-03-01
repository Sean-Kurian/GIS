//==============================================================================
// File Description: Defines a struct and functions to load JSON data and parse it
//
//==============================================================================

#ifndef LIBCURL_H
#define LIBCURL_H
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wredundant-decls"

typedef struct MyCustomStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} MyCustomStruct;

void PrintTTCVehicleInfo(ptree &ptRoot, ezgl::renderer* rend); 
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp); 



#endif /* LIBCURL_H */


