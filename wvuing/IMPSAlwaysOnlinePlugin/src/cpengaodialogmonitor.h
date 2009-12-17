/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for dialog monitor
 *
*/



#ifndef CPENGAODIALOGMONITOR_H
#define CPENGAODIALOGMONITOR_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MPEngAODialogStatusNotifier;

// CLASS DECLARATION
/**
*  CPEngAODialogMonitor.
*  Monitors the dialog status
*/
class CPEngAODialogMonitor : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CPEngAODialogMonitor* NewL( MPEngAODialogStatusNotifier& aNotifier );

        /**
        * Destructor.
        */
        ~CPEngAODialogMonitor();

    public: // New functions


        /**
        * StartMonitoring.
        * @return void
        */
        void Start();


    private: // Functions from base classes

        /**
        * From CActive Handles an active object’s request completion event.
        * @return void
        */
        void RunL();

        /**
        * From CActive Cancels any outstanding request.
        * @return void
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CPEngAODialogMonitor( MPEngAODialogStatusNotifier& aNotifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        MPEngAODialogStatusNotifier& iNotifier;
    };

#endif      // CPENGAODIALOGMONITOR_H   

// End of File
