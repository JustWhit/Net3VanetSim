/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vehicles.cpp
 * Author: ninjagaiden
 * 
 * Created on April 13, 2019, 6:10 PM
 */

#include "Vehicles.h"


Vehicles::Vehicles(){
        verbose = true;
       
        std::string phyMode ("OfdmRate27MbpsBW10MHz");
        Node.Create(1);
        // I took this from the Documentation for Wave. 
        wavePhy = YansWavePhyHelper::Default();
        wifiChannel = YansWifiChannelHelper::Default();
        
        channel = wifiChannel.Create ();
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
        device = waveHelper.Install(wavePhy, waveMac, Node);
        
        wavePhy.EnablePcap("VANET",device);
        //wavePhy.EnablePcapAll("VANET",device);
        
        
    };
    
    Vehicles::Vehicles(bool verbose){
        this->verbose = verbose;
        
        std::string phyMode ("OfdmRate27MbpsBW10MHz");
        Node.Create(1);
        // I took this from the Documentation for Wave. 
        wavePhy = YansWavePhyHelper::Default();
        wifiChannel = YansWifiChannelHelper::Default();
        
        channel = wifiChannel.Create ();
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
        device = waveHelper.Install(wavePhy, waveMac, Node);
        
        wavePhy.EnablePcap("VANET",device);
        //wavePhy.EnablePcapAll("VANET",device);
        
    };