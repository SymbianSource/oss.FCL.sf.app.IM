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
* Description:  Connection mode notifier.
*
*/

#ifndef __CIMPSCONNUICONNECTIONMODENOTIFIERNG_H
#define __CIMPSCONNUICONNECTIONMODENOTIFIERNG_H

//  INCLUDES
#include <e32base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATIONS
class MIMPSConnUiConnectionModeObserver;
class CIMPSConnUiConnectionModeNotifierImp;


/**
 * Connection mode notifier.
 *
 * Notifies registered observers from client
 * specific connection mode changes.
 *
 * For more information about connection mode
 * changes, see TIMPSConnectionModeEvent mode
 * event enumeration.
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
class CIMPSConnUiConnectionModeNotifier : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 2.1
         * @param aClient The client for which to deliver
         * the connection mode events.
         */
        IMPORT_C static CIMPSConnUiConnectionModeNotifier* NewL( TIMPSConnectionClient aClient );

        /**
         * Two-phased constructor.
         * Leaves CIMPSConnUiConnectionModeNotifier object on the
         * CleanupStack.
         *
         * @since 2.1
         * @param aClient The client for which to deliver
         * the connection mode events.
         */
        IMPORT_C static CIMPSConnUiConnectionModeNotifier* NewLC( TIMPSConnectionClient aClient );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSConnUiConnectionModeNotifier();


    public: // New functions

        /**
         * Start listening connection mode events.
         *
         * Starts to deliver notifications from connection
         * mode changes (e.g. user selections that inpact to
         * connection state/modes).
         *
         * Starting fails with leave if:
         * - already started (KErrInUse)
         * - no observers registered (KErrNotReady)
         * - if running out of resourcies (KErrNoMemory)
         * - or with other standard Symbian OS error codes
         *   coming from underlying implementation.
         * @since 2.1
         *
         * @see MIMPSConnUiConnectionModeObserver
         * @see TIMPSConnectionModeEvent
         */
        IMPORT_C virtual void StartL();


        /**
         * Stop listening connection mode events.
         *
         * Stops listening connection mode events.
         * KErrCancel error is notified
         * synchronously to reqistered observers.
         *
         * @since 2.1
         */
        IMPORT_C virtual void Stop();


        /**
         * Add observer.
         *
         * Adds observer to receive notifications about
         * connection mode events.
         *
         * @since 2.1
         */
        IMPORT_C virtual void AddObserverL( MIMPSConnUiConnectionModeObserver* aObserver );


        /**
         * Remove observer.
         *
         * Removes observer from getting
         * connection mode events.
         *
         * @since 2.1
         * @return KErrNotFound if the observer wasn't previously added.
         * Otherwise KErrNone.
         */
        IMPORT_C virtual TInt RemoveObserver( MIMPSConnUiConnectionModeObserver* aObserver );


        /**
         * Gets current/last connection mode event
         * for in  the construction registered client type.
         *
         * Defaults to category to user level login/logout events.
         * (EIMPSCMEUserLevelSelectionEventGroup)
         *
         * @since 2.1
         * @return Current/last connection mode event for the
         * user level login/logout event category.
         * If there isn't any last/current event, return EIMPSCMEUnknown is returned.
         */
        IMPORT_C virtual TIMPSConnectionModeEvent ConnectionModeL();


        /**
         * Gets current/last connection mode event for in the
         * construction registered client type.
         *
         * Events are devided to different event groups
         * and last event for each group is maintained by the
         * notification system. It is possible to query
         * each group last event by giving the event
         * group id as method parameter.
         *
         * @since 2.1
         * @param aModeEventCateqory Event identifying the category
         * for which to get the last event.
         * @return Current/last connection mode event for the requested
         * event category. If there isn't any last/current event recognized
         * for the asked event group, EIMPSCMEUnknown is returned.
         */
        IMPORT_C virtual TIMPSConnectionModeEvent ConnectionModeByCategoryL(
            TIMPSConnectionModeEvent aModeEventCateqory );




    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSConnUiConnectionModeNotifier();


        /**
         * Symbian OS constructor.
         */
        void ConstructL( TIMPSConnectionClient aClient );



    private: //Data
        ///<Implementation, owned
        CIMPSConnUiConnectionModeNotifierImp*   iImp;

        ///<Reserved, owned
        TAny*                                   iReserved1;
    };

#endif      // __CIMPSCONNUICONNECTIONMODENOTIFIERNG_H

// End of File
