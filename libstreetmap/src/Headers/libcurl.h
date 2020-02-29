/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   libcurl.h
 * Author: kurianse
 *
 * Created on February 29, 2020, 1:43 PM
 */

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

typedef struct MyCustomStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} MyCustomStruct;

void PrintTTCVehicleInfo(ptree &ptRoot, ezgl::renderer* rend); 
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp); 



#endif /* LIBCURL_H */


