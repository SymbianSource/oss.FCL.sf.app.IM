/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection Settings notifier.
*
*/

#ifndef __CIMPSCONNUICONNECTIONSETTINGSNOTIFIER_H
#define __CIMPSCONNUICONNECTIONSETTINGSNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include <impspresenceconnectionuiconstsng.h>


//FORWARD DECLARATIONS
class MIMPSConnUiConnectionSettingsObserver;
class CIMPSConnUiConnectionSettingsNotifierImp;


/**
 * Connection mode notifier.
 *
 * Notifies registered observers from client
 * specific connection settings changes.
 *
 * For more information about connection mode
 * changes, see TIMPSConnectionSettingsEvent settings
 * event enumeration.
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
class CIMPSConnUiConnectionSettingsNotifier : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 2.1
         * @param aClient The client for which to deliver
         * the connection mode events.
         */
        IMPORT_C static CIMPSConnUiConnectionSettingsNotifier* NewL(
            TIMPSConnectionClient aClient );

        /**
         * Two-phased constructor.
         * Leaves CIMPSConnUiConnectionSettingsNotifier object on the
         * CleanupStack.
         *
         * @since 2.1
         * @param aClient The client for which to deliver
         * the connection mode events.
         */
        IMPORT_C static CIMPSConnUiConnectionSettingsNotifier* NewLC(
            TIMPSConnectionClient aClient );

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSConnUiConnectionSettingsNotifier();


    public: // New functions

        /**
         * Start listening connection settings events.
         *
         * Starts to deliver notifications from connection
         * setting changes (e.g. user settings selections that inpact to
         * connection state).
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
         * @see TIMPSConnectionSettingsEvent
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
        IMPORT_C virtual void AddObserverL( MIMPSConnUiConnectionSettingsObserver* aObserver );


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
        IMPORT_C virtual TInt RemoveObserver( MIMPSConnUiConnectionSettingsObserver* aObserver );



        /**
         * Gets current/last connection settting event for in the
         * construction registered client type.
         *
         * Events are devided to different event groups
         * and last event for each group is maintained by the
         * notification system. It is possible to query
         * each group last event by giving the event identifying
         * the group as method parameter.
         *
         * @since 2.1
         * @param aSettingsEventCateqory Event identifying the category
         * for which to get the last event.
         * @return Current/last connection settings event for the requested
         * event category. If there isn't any last/current event recognized
         * for the asked event group, EIMPSCSEUnknown is returned.
         */
        IMPORT_C virtual TIMPSConnectionSettingsEvent ConnectionSettingByCategoryL(
            TIMPSConnectionSettingsEvent aSettingsEventCateqory );




    private: //Constructors.

        /**
         * C++ default constructor.
         */
        CIMPSConnUiConnectionSettingsNotifier();


        /**
         * Symbian OS constructor.
         */
        void ConstructL( TIMPSConnectionClient aClient );



    private: //Data
        ///<Implementation, owned
        CIMPSConnUiConnectionSettingsNotifierImp*   iImp;

        ///<Reserved, owned
        TAny*                                       iReserved1;
    };

#endif      // __CIMPSCONNUICONNECTIONSETTINGSNOTIFIER_H

// End of File
