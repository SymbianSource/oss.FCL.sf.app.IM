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
* Description:  SAP Notifier.
*
*/


#ifndef CIMPSSAPNOTIFIER_H
#define CIMPSSAPNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include "cimpssapsettingsstore.h"
#include "TIMPSSAPSettingsListItemNameKey.h"
#include "mimpssapobserver.h"
#include "TIMPSObserverTypePair.h"

// FORWARD DECLARATIONS

class CRepository;
class CIMPSSAPSettings;
class CIMPSSAPSettingsList;

// CLASS DECLARATION

/**
*  SAP Notifier
*  Sends notifys to registered observers about SAP events
*
*  @lib WVSAPSettingsStore.dll
*  @since 3.0
*/
NONSHARABLE_CLASS( CIMPSSAPNotifier ): public CActive
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CIMPSSAPNotifier* NewL( TInt aPriority, TUid aRepositoryUid );

    /**
    * Destructor.
    */
    virtual ~CIMPSSAPNotifier();

public: // New functions

    /**
    * Adds SAP observer to registered observers
    *
    * @param aObserver The observer to be added
    * @param aGroup Group of SAPs to be observed
    * @since 3.0
      */

    void AddObserverL( MIMPSSAPObserver* aObserver, TIMPSAccessGroup aGroup );

    /**
    * Removes SAP observer from registered observers
    *
    * @param aObserver The observer to be removed
    * @since 3.0
      */

    void RemoveObserver( MIMPSSAPObserver* aObserver );

    /**
     * Requests notification for SAP events.
     *
     * @since 3.0
     */
    void RequestNotificationL();

    /**
     * Notifies observers when SAP event occurs.
     *
     * @since 3.0
     */
    void NotifyObservers();

protected:  // Functions from CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:

    /**
    * C++ default constructor.
    */
    CIMPSSAPNotifier( TInt aPriority );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( TUid aRepositoryUid );

private:    // Data

    CRepository* 						iRepository; // Owns. central repository handler
    RArray< TIMPSObserverTypePair >  	iObservers; //Owns. Array of registered observers

    };

#endif      // CIMPSSAPNOTIFIER_H

// End of File
