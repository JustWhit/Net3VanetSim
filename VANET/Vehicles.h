/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vehicles.h
 * Author: ninjagaiden
 *
 * Created on April 13, 2019, 6:10 PM
 */

#ifndef VEHICLES_H
#define VEHICLES_H


#include "ns3/core-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-helper.h"
//#include "ns3/yans-wave-helper.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"

using namespace ns3;


class Vehicles{
public:
    NodeContainer Node;
    YansWavePhyHelper wavePhy;
    Ptr<YansWifiChannel> channel;
    YansWifiChannelHelper wifiChannel;
    bool verbose;
    NetDeviceContainer device;
    
    Vehicles();
    Vehicles(bool verbose);
    
    
};

#endif /* VEHICLES_H */
