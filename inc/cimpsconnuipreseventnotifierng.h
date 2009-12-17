/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence event notifier.
*
*/

#ifndef __CIMPSCONNUIPRESEVENTNOTIFIERNG_H
#define __CIMPSCONNUIPRESEVENTNOTIFIERNG_H

//  INCLUDES
#include <e32base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATIONS
class MIMPSConnUiPresenceEventObserver;
class CIMPSConnUiPresEventNotifierImpBase;
class CPEngNWSessionSlotID2;

/**
 * Presence event notifier.
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
class CIMPSConnUiPresEventNotifier : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CIMPSConnUiPresEventNotifier* NewL();

        /**
         * Two-phased constructor.
         * Leaves CIMPSConnUiPresEventNotifier object on the
         * CleanupStack.
         */
        IMPORT_C static CIMPSConnUiPresEventNotifier* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSConnUiPresEventNotifier();


    public: // New functions

        /**
         * Start listening presence events.
         *
         * Starts to deliver notifications to reqistered
         * observers. Notifiaction is requsted from all
         * presence events, and those are reported
         * asynchronously to observers when they happen.
         *
         * Starting fails with leave if:
         * - already started (KErrInUse)
         * - no observers registered (KErrNotReady)
         * - if running out of resourcies (KErrNoMemory)
         * - or with other standard Symbian OS errorcodes
         *   coming from underlying PEC engine.
         * @since 2.1
         */
        IMPORT_C virtual void StartL();


        /**
         * Stop listening presence events.
         *
         * Stop listening presence events.
         * KErrCancel error is notified
         * synchronously to reqistered observers.
         *
         * @since 2.1
         */
        IMPORT_C virtual void Stop();


        /**
         * Add observer.
         *
         * Add observer to receive notifications about
         * presence events.
         *
         * @since 2.1
         */
        IMPORT_C virtual void AddObserverL( MIMPSConnUiPresenceEventObserver* aObserver );


        /**
         * Remove observer.
         *
         * Removes observer from getting the presence events.
         *
         * @since 2.1
         * @return KErrNotFound if the observer wasn't previously added.
         * Otherwise KErrNone.
         */
        IMPORT_C virtual TInt RemoveObserver( MIMPSConnUiPresenceEventObserver* aObserver );


        /**
         * Gets current service status as application
         * level enumeration.
         *
         * @since 3.0
         * @return  Result:
         *  - KErrNone if state succesfully get.
         *  - Else one of system wide error codes.
         */
        IMPORT_C virtual const CPEngNWSessionSlotID2* AppServiceStatusL(
            TIMPSConnectionClient aClient );



    protected: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSConnUiPresEventNotifier();

        /**
         * Symbian OS constructor.
         *
         * Symbian OS constructor for
         * derived implementations.
         */
        void ConstructL( CIMPSConnUiPresEventNotifierImpBase* aImp );


    private: //Symbian constructor

        /**
         * Symbian OS constructor.
         * For class internal vonstruction.
         */
        void ConstructL();



    private: //Data
        ///<Implementation, owned
        CIMPSConnUiPresEventNotifierImpBase*   iImp;

        ///<Reserved, owned
        TAny*                                  iReserved1;
    };

#endif      // __CIMPSCONNUIPRESEVENTNOTIFIERNG_H

// End of File
