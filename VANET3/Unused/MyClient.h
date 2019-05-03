/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyClient.h
 * Author: Justin Whitlock
 *
 * Created on April 29, 2019, 4:10 PM
 */

#ifndef MYCLIENT_H
#define MYCLIENT_H

#include "ns3/application.h"




using namespace ns3;

class MyClient : public Application{

    
private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    void DoGenerate (void);
    void Initialize(Ptr<Node> n, const Address &t, double i, uint32_t ps);
    void Sent(Ptr<Socket> s, uint32_t length);
    void ScheduleTx();
    
    Ptr<Socket> m_socket;
    Address target;
    double interval;
    uint32_t packetSize;
    EventId sendEvent;
    EventId rcvEvent;
    
    
public:
    MyClient();
    MyClient(Ptr<Node> n, const Address &t, double i, uint32_t ps);
        
};

#endif /* MYCLIENT_H */
