/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>


mtsFunctionWrite::~mtsFunctionWrite()
{}


bool mtsFunctionWrite::Detach(void) {
    if (this->IsValid()) {
        Command = 0;
        return true;
    }
    return false;
}


bool mtsFunctionWrite::IsValid(void) const {
    return (this->Command != 0);
}


bool mtsFunctionWrite::Bind(CommandType * command) {
    Command = command;
    return (command != 0);
}


mtsExecutionResult mtsFunctionWrite::Execute(const mtsGenericObject & argument) const {
    return Command ? Command->Execute(argument, MTS_NOT_BLOCKING) : mtsExecutionResult::NO_INTERFACE;
}


mtsExecutionResult mtsFunctionWrite::ExecuteBlocking(const mtsGenericObject & argument) const {
    return Command ? Command->Execute(argument, MTS_BLOCKING) : mtsExecutionResult::NO_INTERFACE;
}


mtsFunctionWrite::CommandType * mtsFunctionWrite::GetCommand(void) const {
    return Command;
}


const mtsGenericObject * mtsFunctionWrite::GetArgumentPrototype(void) const
{
    if (this->Command) {
        return this->Command->GetArgumentPrototype();
    }
    return 0;
}


void mtsFunctionWrite::ToStream(std::ostream & outputStream) const
{
    if (this->Command) {
        outputStream << "mtsFunctionWrite for " << *Command;
    } else {
        outputStream << "mtsFunctionWrite not initialized";
    }
}
