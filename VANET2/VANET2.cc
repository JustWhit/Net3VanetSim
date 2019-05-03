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
#include "ns3/core-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-helper.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-bsm-helper.h"


//    Network Topology
//
//      Wave 10.1.3.0     point-to-point    LAN 10.1.2.0
//                  ^      10.1.1.0         ^
//       v*          
//       e*                  
//       h*        
//       i*        
//       c*        *AP1  --------------   n2
//       l*                             
//       e*        *AP0   --------------   n1
//       s*                                n0  SERVER (VC)
//                                     


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VANET_Whitlock");





int 
main (int argc, char *argv[]) 
{
    bool verbose = false;
    uint32_t nVH = 10;
    uint32_t nAP = 2;
    bool tracing = false;
    std::string phyMode ("OfdmRate27MbpsBW10MHz");
    uint32_t packetSize = 1024;
    uint32_t numPackets = 300;
    double interval = 1.0;
    bool useCourseChangeCallback = false;
    uint32_t stopTime = 20;
    std::string m_lossModelName ("ns3::TwoRayGroundPropagationLossModel");
    double freq = 5.9e9;
    double m_txp = 20;
    double m_gpsAccuracyNs = 40;
    std::vector<double> m_txSafetyRanges = {50,100,150,200,250,300,350,400,450,500,550};
    double m_txMaxDelayMs = 10;
    double m_waveInterval = 0.1;
    int chAccessMode = 0;
    
    CommandLine cmd;
    cmd.AddValue ("nVH", "Number of Vehicles", nVH);
    cmd.AddValue ("nAP", "Number of Access Points devices", nAP);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
    cmd.AddValue ("phyMode","Wifi Phy mode", phyMode);
    cmd.AddValue ("packetSize","size of application packet sent", packetSize);
    cmd.AddValue ("numPackets","Number of packets sent", numPackets);
    cmd.AddValue ("interval","Interval between packets", interval);
    cmd.AddValue ("useCourseChangeCallback", "whether to enable course change tracing", useCourseChangeCallback);
    cmd.AddValue ("stopTime", "simulation stop time (seconds)", stopTime);
    
    cmd.Parse (argc,argv);
    
    Time interPacketInterval = Seconds(interval);
    
    if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    
    NodeContainer apNodes;
    NodeContainer vhNodes;
    NodeContainer csmaNodes;
    
    /*/////////////////////////////////////////////////////////////////
     Creation of vhNodes of number nVH                                /
     Creation of CSMA node 0 which will be the access point for the VC/
     */////////////////////////////////////////////////////////////////
    
    vhNodes.Create(nVH);
    csmaNodes.Create(1);
    
    /*///////////////////////////////////////////////////////////////////////////////////////////
     * Creating Point to Point connection pairs which will serve as Access Points and CSMA nodes/
     *///////////////////////////////////////////////////////////////////////////////////////////
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    NetDeviceContainer p2pDevices;  
    for(uint32_t i = 0; i<nAP; ++i){
        NodeContainer p2pNodes;
        p2pNodes.Create(2);
        p2pDevices = pointToPoint.Install (p2pNodes);
        apNodes.Add(p2pNodes.Get(0));
        csmaNodes.Add(p2pNodes.Get(1));
        
    }
    /*///////////////////////////////////////
     * Installing CSMA devices on CSMA nodes/
     *///////////////////////////////////////
    
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes);
    
    /*///////////////////////////////////////////////////////
     * Installing WAVE devices on Access Points and Vehicles/
     *///////////////////////////////////////////////////////
    
    NetDeviceContainer waveDevices;
    // I took this from the Documentation for Wave. 
    
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    // two-ray requires antenna height (else defaults to Friss)
    wifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (freq), "HeightAboveZ", DoubleValue (1.5));
    wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");

    Ptr<YansWifiChannel> channel = wifiChannel.Create ();
    // Setup Wave Phy and MAC
    YansWavePhyHelper wavePhy = YansWavePhyHelper::Default();
    wavePhy.SetChannel(channel);
    // Generate a Pcap trace
    wavePhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11);
    QosWaveMacHelper waveMac = QosWaveMacHelper::Default();
    WaveHelper waveHelper = WaveHelper::Default();
    if(verbose){
        // Turn on all wifi logging components
        waveHelper.EnableLogComponents();
    }
    waveHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager","DataMode",StringValue(phyMode),"ControlMode",StringValue(phyMode));
    
    wavePhy.Set ("TxPowerStart",DoubleValue (m_txp));
    wavePhy.Set ("TxPowerEnd", DoubleValue (m_txp));
    
    waveDevices.Add(waveHelper.Install(wavePhy, waveMac, apNodes));
    waveDevices.Add(waveHelper.Install(wavePhy, waveMac, vhNodes));

    wavePhy.EnablePcap("VANET",waveDevices);
    
    
    
    
     //Mobility for fixed Wave APs
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> APpositionAlloc = CreateObject<ListPositionAllocator> ();
    for(double i=0.0; i< double(nAP); ++i){
        APpositionAlloc->Add (Vector (i+5,25,0.0));
    }
    mobility.SetPositionAllocator (APpositionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    
    /*
     * Mobility for fixed CSMA nodes
     */
    Ptr<ListPositionAllocator> CSMApositionAlloc = CreateObject<ListPositionAllocator>();
    for(double i=0.0; i<csmaNodes.GetN (); ++i){
        CSMApositionAlloc->Add(Vector (i+5,10,0.0));
    }
    mobility.SetPositionAllocator (CSMApositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csmaNodes);
    
    /*
     * Mobility for Vehicle nodes
     */
   mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
    
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install(vhNodes);
    
    /*
     * Internet setup
     */
    
    InternetStackHelper internet;
    internet.Install(vhNodes);
    internet.Install(csmaNodes);
    internet.Install(apNodes);
    
    Ipv4AddressHelper address;

    address.SetBase ("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    Ipv4InterfaceContainer csmaInterfaces;
    Ipv4InterfaceContainer waveInterfaces;
    p2pInterfaces = address.Assign (p2pDevices);
    address.NewNetwork();
    address.SetBase ("172.16.1.0", "255.255.255.0");
    csmaInterfaces = address.Assign(csmaDevices);
    address.NewNetwork();
    
    address.SetBase ("10.0.0.0", "255.255.255.0");
    
    waveInterfaces = address.Assign(waveDevices);
    
    address.NewNetwork();
   
    WaveBsmHelper m_waveBsmHelper;
    NodeContainer waveNodes;
    waveNodes.Add(vhNodes);
    waveNodes.Add(apNodes);
    m_waveBsmHelper.Install (waveNodes);
    m_waveBsmHelper.Install(waveDevices);
    m_waveBsmHelper.Install(waveInterfaces, Seconds (stopTime), packetSize, Seconds (m_waveInterval), m_gpsAccuracyNs, m_txSafetyRanges, chAccessMode, MilliSeconds (m_txMaxDelayMs));
    /*void Install (Ipv4InterfaceContainer & i,
                Time totalTime,          // seconds
                uint32_t wavePacketSize, // bytes
                Time waveInterval,       // seconds
                double gpsAccuracyNs,    // clock drift range in number of ns
                std::vector <double> ranges,          // m
                int chAccessMode,        // channel access mode (0=continuous; 1=switching)
                Time txMaxDelay);        // max delay prior to transmit*/
    
    //Populating routing tables for initial setup
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    Simulator::Stop (Seconds (20.0));
    
    //csma.EnablePcap ("VANET", csmaDevices.Get (0), true);
    
    AnimationInterface anim ("VANETanimation.xml");
    for (uint32_t i = 0; i < apNodes.GetN(); ++i)
    {
      anim.UpdateNodeDescription (apNodes.Get (i), "AP"); // Optional
      anim.UpdateNodeColor (apNodes.Get (i), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < nVH; ++i)
    {
      anim.UpdateNodeDescription (vhNodes.Get (i), "VH"); // Optional
      anim.UpdateNodeColor (vhNodes.Get (i), 0, 255, 0); // Optional
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

