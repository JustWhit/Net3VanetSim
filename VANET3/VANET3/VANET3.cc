/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VANET.cc
 * Author: Justin Whitlock
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
//#include "MyClient.h"
//#include "MyServer.h"
//#include "MyHelper.h"
//#include "Vehicle.h"




//    Network Topology
//
//      Wave 10.0.0.0     point-to-point            LAN 172.16.1.0
//                  ^      192.168.2.0              ^
//       v*         *AP1  --------------   n2 
//       e*                                         -
//       h*                                         -
//       i*                                         -
//       c*                                         -
//       l*                                         -
//       e*         *AP0   --------------   n1
//       s*                                         n0  SERVER (VC)
//                                     


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VANET_Whitlock");


int 
main (int argc, char *argv[]) 
{
    bool verbose = false;
    uint32_t nVH = 5;//this is the number of vehicles generated by SUMO
    //Time vhFinish[479]; // this tracks the expected finish time of each vehicle
    uint32_t nAP = 2;//this is the number of Access Points
    bool tracing = false;//whether or not to display trace
    std::string phyMode ("OfdmRate27MbpsBW10MHz");
    uint32_t packetSize = 1024;
    uint32_t numPackets = 300;
    double interval = 1.0;
    
    uint32_t stopTime = 8000;
    std::string m_lossModelName ("ns3::TwoRayGroundPropagationLossModel");
    double freq = 5.9e9;
    double m_txp = 20;
    double m_gpsAccuracyNs = 40;
    std::vector<double> m_txSafetyRanges = {50,100,150,200,250,300,350,400,450,500,550};
    double m_txMaxDelayMs = 10;
    double m_waveInterval = 0.1;
    int chAccessMode = 0;
    std::string m_traceFile ("/home/ninjagaiden/source/ns-3.29/scratch/VANET3/VANETmobility.tcl");
    WaveBsmHelper m_waveBsmHelper;
    bool useTrace = false;
    
    
    CommandLine cmd;
    cmd.AddValue ("nVH", "Number of Vehicles", nVH);
    cmd.AddValue ("nAP", "Number of Access Points devices", nAP);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
    cmd.AddValue ("phyMode","Wifi Phy mode", phyMode);
    cmd.AddValue ("packetSize","size of application packet sent", packetSize);
    cmd.AddValue ("numPackets","Number of packets sent", numPackets);
    cmd.AddValue ("interval","Interval between packets", interval);  
    cmd.AddValue ("stopTime", "simulation stop time (seconds)", stopTime);
    
    
    cmd.Parse (argc,argv);
    
    
    if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    
    /*////////////////////////////
     * Creation of and addition of Mobility for Vehicle nodes/
     *////////////////////////////
    
    
    NodeContainer vhNodes;
    vhNodes.Create(nVH);// nVH is the number of Vehicles
    
    if(useTrace){
        Ns2MobilityHelper ns2 = Ns2MobilityHelper (m_traceFile);
        ns2.Install (); // configure movements for each node, while reading trace file
        // initially assume all nodes are not moving
        WaveBsmHelper::GetNodesMoving ().resize (nVH, 0);
    }
    else{
        MobilityHelper mobility;
        mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
        mobility.Install(vhNodes);
        Ptr<UniformRandomVariable> rvar = CreateObject<UniformRandomVariable>();
        for (NodeContainer::Iterator i = vhNodes.Begin(); i != vhNodes.End();++i){
            Ptr<Node> node = (*i);
            double speed = rvar->GetValue(50,70);
            node->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(speed,0,0));
            
        }
        //Initially assume all nodes are moving
        WaveBsmHelper::GetNodesMoving().resize (nVH,1);
    }
    
    
    
    /*///////////////////////////////////////////////////////////////////
     * Creation of CSMA node 0 which will be the access point for the VC/
     *///////////////////////////////////////////////////////////////////
    
    NodeContainer apNodes;  //Access Points
    NodeContainer csmaNodes; //LAN Nodes, Node 0 is VC
    csmaNodes.Create(1); // Create Node 0
    
    /*///////////////////////////////////////////////////////////////////////////////////////////
     * Creating Point to Point connection pairs which will serve as Access Points and CSMA nodes/
     *///////////////////////////////////////////////////////////////////////////////////////////
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps")); // Simulating Fiber Optic
    pointToPoint.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (0.82))); // at speed of light in a fiber optic cable(122,000 mps), csma nodes are about 100miles away from the highway
    NetDeviceContainer p2pDevices;  
    for(uint32_t i = 0; i<nAP; ++i){
        NodeContainer p2pNodes;
        p2pNodes.Create(2);
        p2pDevices = pointToPoint.Install (p2pNodes);
        apNodes.Add(p2pNodes.Get(0)); // One of each pair becomes an Access Point
        csmaNodes.Add(p2pNodes.Get(1)); // The other becomes a LAN node
        
    }
    /*///////////////////////////////////////
     * Installing CSMA devices on CSMA nodes/
     *///////////////////////////////////////
    
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps")); // Simulating Fiber Optic
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560))); //distance is assumed to be shorter

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes);
    
    /*///////////////////////////////////////////////////////
     * Installing WAVE devices on Access Points and Vehicles/
     *///////////////////////////////////////////////////////
    
    NetDeviceContainer waveDevices;
     
    
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    // antenna height , This is how the propogation model was setup in the VANET example in NS3 documentation. I put the heigh at 25m.  
    wifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (freq), "HeightAboveZ", DoubleValue (2.0));
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
        // Turn on all wifi logging components, I haven't figured out how the logging really works. At least, I'm not seeing any output
        waveHelper.EnableLogComponents();
    }
    waveHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager","DataMode",StringValue(phyMode),"ControlMode",StringValue(phyMode));
    
    wavePhy.Set ("TxPowerStart",DoubleValue (m_txp));
    wavePhy.Set ("TxPowerEnd", DoubleValue (m_txp));
    
    waveDevices.Add(waveHelper.Install(wavePhy, waveMac, apNodes));
    waveDevices.Add(waveHelper.Install(wavePhy, waveMac, vhNodes));
    /* This section was intended to be the receive callback, but it needs to be implemented in Vehicle.cc 
    for (uint32_t i=0; i<waveDevice.GetN(); i++){
        Ptr<WaveNetDevice> wd = DynamicCast<WaveNetDevice> (waveDevices.Get(i));
        wd->SetReceiveCallback (MakeCallback (&vhReceive, this));
        wd->SetWaveVsaCallback(MakeCallback (&vhReceiveVsa,this));
    }
     */
    wavePhy.EnablePcap("VANET",waveDevices);
                                            
    
     //Mobility for fixed Wave APs
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> APpositionAlloc = CreateObject<ListPositionAllocator> ();
    
    APpositionAlloc->Add (Vector (500,25,0.0));
    APpositionAlloc->Add (Vector (500,475,0.0));
    
    mobility.SetPositionAllocator (APpositionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    
    /*///////////////////////////////
     * Mobility for fixed CSMA nodes/
     *///////////////////////////////
    Ptr<ListPositionAllocator> CSMApositionAlloc = CreateObject<ListPositionAllocator>();
    
    CSMApositionAlloc->Add(Vector (550,10,0.0));
    CSMApositionAlloc->Add(Vector (550,15,0.0));
    CSMApositionAlloc->Add(Vector (550,485,0.0));
    
    mobility.SetPositionAllocator (CSMApositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csmaNodes);
    
    
    
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
    
    address.SetBase ("10.0.0.0", "255.255.252.0");
    
    waveInterfaces = address.Assign(waveDevices);
    
    address.NewNetwork();
   
    
    NodeContainer waveNodes;
    waveNodes.Add(vhNodes);
    waveNodes.Add(apNodes);
    
    m_waveBsmHelper.Install(waveInterfaces, Seconds (stopTime), packetSize, Seconds (m_waveInterval), m_gpsAccuracyNs, m_txSafetyRanges, chAccessMode, MilliSeconds (m_txMaxDelayMs));
    
    
    //Populating routing tables for initial setup
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    //UdpServerHelper server (port);
    
    

    Simulator::Stop (Seconds (stopTime));
    
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

