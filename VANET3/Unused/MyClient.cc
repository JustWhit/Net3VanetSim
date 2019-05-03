/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyClient.cpp
 * Author: Justin Whitlock
 * 
 * Created on April 29, 2019, 4:10 PM
 */

#include "MyClient.h"

MyClient::MyClient(){}

MyClient::MyClient(Ptr<Node> n, const Address &t, double i, uint32_t ps) : Application(n){
    Initialize(n,t,i,ps);
}

MyClient::Initialize(Ptr<Node> n, const Address &t, double i, uint32_t ps){
    this->socket = NULL;
    this->target = t;
    this->interval = i;
    this->packetSize = ps;
}

void MyClient::DoDispose(void){
    //Clean up
    if(socket!=NULL){
        socket->Close();
    }
    //Forward Request
    Application::DoDispose();
}

virtual void MyClient::StartApplication(void){
    //Initialize socket if not already done
    if (socket == NULL){
        InterfaceId protoIID = InterfaceId::LookupByName("Udp");
        Ptr<SocketFactory> socketFactory = GetNode()->QueryInterface<SocketFactory>(protoIID);
        socket = socketFactory->CreateSocket();
        socket->Connect(target);
    }
    //Create callback for after message send, displays bytes sent
    socket->SetSendCallback((Callback<void,Ptr<Socket>,uint32_t>), MakeCallback(&MyClient::Sent,this));
    // Schedule first message
    ScheduleTx();
}

virtual void MyClient::StopApplication(void){
    
}

void MyClient::Sent(Ptr<Socket> s, uint32_t length){
    std::cout<<"\n : Finished transmitting Message of "<<length<<" bytes " <<std::endl;
    
}

void MyClient::ScheduleTx(){
    sendEvent = Simulator::Schedule(Seconds(Interval), &MyClient::Send, this);
}

