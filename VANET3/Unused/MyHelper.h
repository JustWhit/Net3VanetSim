/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyHelper.h
 * Author: Justin Whitlock
 *
 * Created on April 29, 2019, 6:12 PM
 */

#ifndef MYHELPER_H
#define MYHELPER_H
#include "ns3/address.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"

using namespace ns3;

class MyHelper {
public:
    MyHelper (std::string protocol, Address remote);
    
    ApplicationContainer Install (NodeContainer nodes);
private:
    
    
};
#endif /* MYHELPER_H */
