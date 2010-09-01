/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Command Manager Factory
*
*/



#ifndef CCACOMMANDMANAGERFACTORY_H
#define CCACOMMANDMANAGERFACTORY_H

//  INCLUDES
#include "CCASingletonCmd.h"


//FORWARD DECLARATIONS
class MCACommandFactory;
class MCAProcessManager;
class CCASessionHandlerCmd;
class CCACommandManager;
class MCAUiLoginCmdCB;

// CLASS DECLARATION

/**
*  Command Manager Factory
*  This class is singleton
*
*/
NONSHARABLE_CLASS( CCACommandManagerFactory ) : public CCASingletonCmd
    {
public:  // Constructors

    static CCACommandManagerFactory* NewL();

public: // New functions

    /**
    * Return pointer to the singleton instance.
    *
    * @return pointer to the singleton instance
    */
    IMPORT_C static CCACommandManagerFactory* InstanceL();


    /* Delete singleton instance.
    *
    */
    IMPORT_C static void ReleaseResources();

    /**
    * This method is used to get the command manager
    */
    IMPORT_C CCACommandManager* GetCommandManager();


    /**
    * This method constructs the session handler and also initialises
    * the Command Manager's session handler
    */
    IMPORT_C void InitializeSessionHandlerL( MCAProcessManager& aProcessManager,
                                             MCAUiLoginCmdCB& aUiLoginCmdCB );




private:

    /**
    * Destructor.
    */
    ~CCACommandManagerFactory();

    /**
    * C++ default constructor.
    */
    inline CCACommandManagerFactory();

    /**
    * Symbian 2nd phase constructor.
    */
    inline void ConstructL();

private:     // Data
    // Command manager, owns
    CCACommandManager* iCommandManager;
    //SessionHandler, owns
    CCASessionHandlerCmd* iSessionHandlerCmd;

    };

#endif      // CCACommandManagerFactory_H

// End of File
