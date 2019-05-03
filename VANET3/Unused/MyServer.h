/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyServer.h
 * Author: Justin Whitlock
 *
 * Created on April 29, 2019, 4:10 PM
 */

#ifndef MYSERVER_H
#define MYSERVER_H

#include "ns3/application.h"


using namespace ns3;

class MyServer : public Application {
public:
    MyServer();
    
private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    void DoGenerate (void);
    
    Ptr<Socket> m_socket;
            
};

#endif /* MYSERVER_H */
