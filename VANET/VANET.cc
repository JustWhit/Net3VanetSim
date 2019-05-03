/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VANET.cpp
 * Author: ninjagaiden
 *
 * Created on April 9, 2019, 5:54 PM
 */

/*
#include "ns3/vector.h"
#include "ns3/string.h"
#include "ns3/socket.h"
#include "ns3/double.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/position-allocator.h"
#include "mobility-helper.h"
*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ssid.h"

#include "AccessPoint.h"
#include "P2P.h"
#include "Vehicles.h"

//    Network Topology
//
//      Wave 10.1.3.0     point-to-point    LAN 10.1.2.0
//                  ^      10.1.1.0         ^
//       v*        *AP9   --------------   n10   
//       e*                  
//       h*        *AP8   --------------   n9
//       i*        *AP7   --------------   n8
//       c*        *AP6   --------------   n7
//       l*        ...                     ...
//       e*        *AP0   --------------   n1
//       s*                                n0 -------------> VC controller
//                                     


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VANET_Whitlock");

int 
main (int argc, char *argv[]) 
{
    bool verbose = true;
    uint32_t nAP = 2;
    uint32_t nVH = 10;
    
    if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    //initialize AccessPoints 1=EntranceAP, 2=ExitAP, 3=MxAP
    AccessPoint APList[nAP];
   
    for(int i=0;i<3; i++){
        APList[i].setType(1);
    }
    for(int i=3;i<6; i++){
        APList[i].setType(2);
    }
    for(int i=6;i<10; i++){
        APList[i].setType(3);
    }
     
    
    P2P P2PList[10];
    NodeContainer csmaNodes;
    csmaNodes.Create (1);
    for(int i=0; i<10; i++){
        csmaNodes.Add (P2PList[i].p2pNodes.Get (1));
        APList[i].Node.Add(P2PList[i].p2pNodes.Get (0));
    }
    
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes);
    
    
    
    //Mobility for fixed Wave APs
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> APpositionAlloc = CreateObject<ListPositionAllocator> ();
    for(double i=0.0; i<50; i+=5){
        APpositionAlloc->Add (Vector (i,25,0.0));
    }
    mobility.SetPositionAllocator (APpositionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    for(int i=0; i<10; i++){
        mobility.Install(APList[i].Node);
    }
    
    
    //Mobile Wave Nodes
    Vehicles VHList[nVH];
    //MobilityHelper VHmobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
    
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    for(int i=0; i<10; i++){
        mobility.Install (VHList[i].Node);
    }
    AnimationInterface::SetConstantPosition(csmaNodes.Get (0), 10, 30);
    //AnimationInterface::SetConstantPostion(p2pNode)
    
    
    /// Internet setup
    
    InternetStackHelper internet;
    for(uint32_t i=0;i<nAP;i++){
        internet.Install(APList[i].Node);
        //internet.Install(P2PList[i].p2pNodes);
    }
    internet.Install(csmaNodes);
    for(uint32_t i=0;i<nVH;i++){
        internet.Install(VHList[i].Node);
    }
    
    /// IPv4 address setup
    
    Ipv4AddressHelper address;

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    Ipv4InterfaceContainer csmaInterfaces;
    for(uint32_t i=0; i<nAP;i++){
        p2pInterfaces.Add(address.Assign (P2PList[i].p2pDevices));
    }
    address.SetBase ("10.1.1.0", "255.255.255.0");
    csmaInterfaces = address.Assign(csmaDevices);
    
    address.SetBase ("10.1.3.0", "255.255.255.0");
    for(uint32_t i=0; i<nAP; i++){
        address.Assign(APList[i].device);
    }
    address.SetBase ("10.1.4.0", "255.255.255.0");
    for(uint32_t i=0; i<nVH; i++){
        address.Assign(VHList[i].device);
    }
    
    //Set up Server Client relationship
    UdpEchoServerHelper echoServer (9);
    ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (0));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (20.0));
    
    ApplicationContainer clientApps;
    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (0), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (320));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.05)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
    for(uint32_t i=0; i<nVH; i++){
        clientApps.Add( echoClient.Install (VHList[i].Node));
    }
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (20.0));
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    Simulator::Stop (Seconds (20.0));
    
    
    csma.EnablePcap ("VANET", csmaDevices.Get (0), true);
    
    AnimationInterface anim ("VANETanimation.xml");
    for (uint32_t i = 0; i < nAP; ++i)
    {
      anim.UpdateNodeDescription (APList[i].Node.Get (0), "AP"); // Optional
      anim.UpdateNodeColor (APList[i].Node.Get (0), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < nVH; ++i)
    {
      anim.UpdateNodeDescription (VHList[i].Node.Get (0), "VH"); // Optional
      anim.UpdateNodeColor (VHList[i].Node.Get (0), 0, 255, 0); // Optional
    }
  for (uint32_t i = 0; i < csmaNodes.GetN (); ++i)
    {
      anim.UpdateNodeDescription (csmaNodes.Get (i), "CSMA"); // Optional
      anim.UpdateNodeColor (csmaNodes.Get (i), 0, 0, 255); // Optional 
    }
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking ("routingtable-VANET.xml", Seconds (0), Seconds (5), Seconds (0.25)); 
    anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); 
    anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); 
    
    Simulator::Run();
    Simulator::Destroy();
    
    
    return 0;
}

