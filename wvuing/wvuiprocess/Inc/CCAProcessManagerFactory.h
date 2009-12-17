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
* Description:  Process Manager Factory for creating the Process Manager
*
*/




#ifndef CCAPROCESSMANAGERFACTORY_H
#define CCAPROCESSMANAGERFACTORY_H


//  INCLUDES
#include    <e32base.h>
#include	"CCASingletonPC.h"


//FORWARD DECLARATIONS
class MCAProcessManager;

class CCAProcessManagerFactory: public CCASingletonPC
    {
    public:

        //Two Phase Constructor
        static CCAProcessManagerFactory* NewL();

        /**
        * Destructor.
        */

        ~CCAProcessManagerFactory();

    private:

        /**
        * C++ default constructor.
        */
        inline CCAProcessManagerFactory();

        /**
        * Symbian 2nd phase constructor.
        */
        inline void ConstructL();

    public: // operations

        /**
        * Return pointer to the singleton instance.
        *
        * @return pointer to the singleton instance
        */
        IMPORT_C static CCAProcessManagerFactory* InstanceL();

        /**
        * This method is used to get the process manager
        */
        IMPORT_C MCAProcessManager* GetProcessManager();

        /**
        * Delete singleton instance.
        */
        IMPORT_C static void ReleaseResources();

    private:

        // Data
        MCAProcessManager* iProcessManager;

    };

#endif // CCAPROCESSMANAGERFACTORY_H

// End of File