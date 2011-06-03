/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Peter Kazanzides
Created on: 2009

(C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <string.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaSocketServer.h>

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("osaSocket", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("osaSocketServer", CMN_LOG_ALLOW_ALL);

    osaSocketServer socketServer;
    osaSocket * socket = 0;
    unsigned short port = 1234;
    std::string response;
    bool isServerConnected = false;
    char buffer[512];
    int bytesRead;

    std::cout << "> Listen on port 1234? [y/n] ";
    std::cin >> response;
    if (response.compare("n") == 0) {
        std::cout << "> Enter port: ";
        std::cin >> port;
    }
    while (!socketServer.AssignPort(port)) {
        std::cout << "Will try again in 5 seconds" << std::endl;
        osaSleep(5.0 * cmn_s);
    }
    isServerConnected=socketServer.Listen();

    if (!isServerConnected) {
        std::cout << "Could not set socketserver to listen mode" << std::endl;
        exit (0);
    }

    std::cout << std::endl
        << "Started server on "
        << osaSocket::GetLocalhostIP() << ":" << port << std::endl
        << std::endl;


    while (isServerConnected) {
        socket = socketServer.Accept();
        osaSleep(100.0 * cmn_ms);
        if (socket != 0) {
            socket->Send("Server has accepted your request");
        }

        //nothing connected yet.
        if (socket == 0) {
            continue;
        }

        //if we are connected that send back whatever we get
        while (socket->IsConnected()) {

            // receive
            bytesRead = socket->Receive(buffer, sizeof(buffer));
            if (bytesRead > 0) {
                for (int i = 0; i < bytesRead; i++) {
                    buffer[i] = toupper(buffer[i]);
                }
                buffer[bytesRead] = 0;

                if (strcmp(buffer, "EXITCLIENT") == 0) {
                    socket->Close();

                } else if (strcmp(buffer, "EXIT") == 0) {
                    socket->Close();
                    isServerConnected = false;
                }
                else {
                    // send
#ifdef OSA_SOCKET_WITH_STREAM
                    *socket << buffer;
#else
                    socket->Send(buffer);
#endif // OSA_SOCKET_WITH_STREAM
                }
            }
            osaSleep(100.0 * cmn_ms);
            //socket->Send("");
        }
        //No connection so there is no need for this socket anymore.
        delete socket;
        socket=0;

    }

    if (socket != 0) {
        delete socket; //this closes the socket.
        socket = 0;
    }

    socketServer.Close();

    return 0;
}
