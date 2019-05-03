/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vehicle.h
 * Author: ninjagaiden
 *
 * Created on May 1, 2019, 4:33 PM
 */

#ifndef VEHICLE_H
#define VEHICLE_H

using namespace ns3;

static const uint32_t writeSize = 1024;
uint8_t data[writeSize];
static const uint16_t sPort = 50000;

class Vehicle{
private:
    int vehicleID;
    Ptr<Node> node;
    Ptr<NetDevice> device;
    Ptr<Socket> sink;
    Time finished; // Expected Finish time
    bool started; // time VM is received
    uint32_t totalBytes; // VM size
    uint32_t currentBytes; // bytes tx out.
    
    
public:
    //initializes variables
    Vehicle();
    ~Vehicle(){
        // nothing, Ptr's handle their own memory cleanup.
    }
    bool sendTo(Address trgt, Ptr<Packet> pkt);
    bool receive(Ptr<NetDevice> dev, Ptr<const Packet> pkt,uint16_t mode, const Address& sender);
    bool trxVM(Address trgt);
    void startFLOW(Ptr<Socket> localskt, Ipv4Address trgt, uint16_t tPort);
    void writeFull(Ptr<Socket> localskt,uint32_t txSpace);
};

#endif /* VEHICLE_H */
