/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   P2P.h
 * Author: ninjagaiden
 *
 * Created on April 12, 2019, 4:00 PM
 */

#ifndef P2P_H
#define P2P_H

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

class P2P{
    
public:
    NodeContainer p2pNodes;
    NetDeviceContainer p2pDevices;
    PointToPointHelper pointToPoint;
    
    P2P(){
        
        p2pNodes.Create (2);

        
        pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
        pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
        
        p2pDevices = pointToPoint.Install (p2pNodes);
        pointToPoint.EnablePcapAll("VANET");
        
    }
    
    P2P(int numNodes){
        p2pNodes.Create (numNodes);

        
        pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
        pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
        
        p2pDevices = pointToPoint.Install (p2pNodes);
        pointToPoint.EnablePcapAll("VANET");
    }
    
    
    
    
};

#endif /* P2P_H */
