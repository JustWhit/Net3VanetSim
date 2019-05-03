/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AccessPoint.h
 * Author: ninjagaiden
 *
 * Created on April 10, 2019, 7:12 PM
 */

#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include "ns3/core-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-helper.h"
//#include "ns3/yans-wave-helper.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"

using namespace ns3;

class AccessPoint{
public:
    NodeContainer Node;
    YansWavePhyHelper wavePhy;
    Ptr<YansWifiChannel> channel;
    YansWifiChannelHelper wifiChannel;
    bool verbose;
    NetDeviceContainer device;
    int Type;
    
    AccessPoint();
    AccessPoint(bool verbose);
    
    void setType(int type){
        this->Type=type;
    }
    int getType(){
        return this->Type;
    }
    
};


#endif /* ACCESSPOINT_H */
