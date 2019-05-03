/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vehicle.cc
 * Author: ninjagaiden
 * 
 * Created on May 1, 2019, 4:33 PM
 */

#include "Vehicle.h"
#include <sstream>

Vehicle::Vehicle(){
    vehicleID = 1;
    node = CreateObject<Node>();
     ("ns3::TcpSocketFactory");
    sink = Socket::CreateSocket(node,TcpSocketFactory::GetTypeId ()); // This will be attached to the receive function as a call back
    sink->Bind();
    finished= Seconds(0);
    started = false;
    totalBytes = 2000000;
    currentBytes = 0;
    
}



bool 
Vehicle::receive(Ptr<NetDevice> dev, Ptr<const Packet> rpkt,uint16_t mode, const Address& sender){
    SeqTsHeader seqTS;
    rpkt->PeekHeader(seqTS);
    std::ostringstream msg;
    Ptr<Packet> spkt;
    Time time=Simulator::Now().GetSeconds();
    try{
        if(started){// If the Vehicle has already started processing a VM
            if(time>finished){ // If the vehicle has passed the expected finish time, respond with "done"
                msg <<"done"<<'\0';
                spkt=Create<Packet>((uint8_t*)msg.str ().c_str (), msg.str ().length ());

            }
            else{// otherwise return the time left
                msg<<finished-time<<'\0';
                spkt=Create<Packet>((uint8_t*)msg.str ().c_str (), msg.str ().length ());
            }
            sendTo(sender, spkt);
        }
        else{ // the packet contains VM information, get Finish time from packet payload
            uint8_t *buffer = new uint8_t[rpkt->GetSize ()];
            rpkt->CopyData(buffer, rpkt->GetSize ());
            finished = Seconds( std::atoi(std::string((char*)buffer).c_str()));
            started = true;
        }
    }catch(...){
        std::cout<<"PACKET CAUSES EXCEPTION, returned false\n";
        return false;
    }
    
    return true;
}

bool Vehicle::sendTo(Address trgt, Ptr<Packet> pkt){
    // needs filling
}

bool Vehicle::trxVM(Address trgt){
    Ptr<Socket> localskt = Socket::CreateSocket (node, TcpSocketFactory::GetTypeId ());
    Simulator::ScheduleNow (&startFlow, localskt, trgt, tPort);
}

void Vehicle::startFLOW(Ptr<Socket> localskt, Ipv4Address trgt, uint16_t tPort){
    NS_LOG_LOGIC("tx VM at "<< Simulator::Now ().GetSeconds ());
    localskt->Connect(InetSocketAddress(trgt, tPort));
    //create a callback in case the buffer fills, call write full again
    localskt->SetSendCallback(MakeCallback (&writeFull, this));
    writeFull(localskt, localskt->GetTxAvailable());
}
    

void Vehicle::writeFull(Ptr<Socket> localskt,uint32_t txSpace){
    while(currentTxBytes < totalTxBytes && localskt->GetTxAvailable () > 0){
        uint32_t left = totalBytes - currentBytes;
        uint32_t Offset = currentBytes % writeSize;
        uint32_t Write = writeSize - Offset;
        Write = std::min (Write, left);
        Write = td::min (Write, localskt->GetTxAvailable ());
        int Sent = localskt->Send(&data[Offset], Write, 0);
        if (Sent < 0){
            // Callback will occur when TX space is available in
            return;
        }
        currentBytes += Sent;
    }
    localskt->Close();
}