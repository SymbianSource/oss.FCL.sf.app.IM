/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client logins status based filterd presence event notifier.
*
*/

#ifndef __CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERNG_H
#define __CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERNG_H

//  INCLUDES
#include <E32Base.h>
#include <cimpsconnuipreseventnotifierng.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATIONS
class MIMPSConnUiPresenceEventObserver;


/**
 * Client filtered presence event notifier.
 *
 * Extends standard CIMPSConnUiPresEventNotifier
 * behaviour by filtering delivered presence events
 * based the requested client login status.
 *
 * Also this class service status getter behaves so that
 * it returns service status filtered according the
 * requested client online status.
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
class CIMPSConnUiClntFilteredPresEventNotifier : public CIMPSConnUiPresEventNotifier
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 2.1
         * @param aClient The client according to filter
         * delivered presence events.
         */
        IMPORT_C static CIMPSConnUiClntFilteredPresEventNotifier* NewL(
            TIMPSConnectionClient aClient );


        /**
         * Two-phased constructor.
         * Leaves CIMPSConnUiClntFilteredPresEventNotifier object on the
         * CleanupStack.
         *
         * @since 2.1
         * @param aClient The client according to filter
         * delivered presence events.
         */
        IMPORT_C static CIMPSConnUiClntFilteredPresEventNotifier* NewLC(
            TIMPSConnectionClient aClient );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSConnUiClntFilteredPresEventNotifier();



    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSConnUiClntFilteredPresEventNotifier();


        /**
         * Symbian OS constructor.
         */
        void ConstructL( TIMPSConnectionClient aClient );
    };

#endif      // __CIMPSCONNUICLNTFILTEREDPRESEVENTNOTIFIERNG_H

// End of File
