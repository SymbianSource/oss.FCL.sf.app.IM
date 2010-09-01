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
* Description:  Publish & Subscribe system keys change observer
*
*/



#ifndef CIMPSSYSTEMNOTFIER_H
#define CIMPSSYSTEMNOTFIERSYSTEMAGENT_H

//  INCLUDES

#include "MIMPSSystemNotifier.h"
#include "MIMPSPropertyNotificationObserver.h"

// FORWARD DECLARATIONS
class MIMPSSystemNotifierObserver;
class CIMPSPropertyObserver;

// CLASS DECLARATION

/**
*  Implementation of the Publish & Subscribe system keys change observer
*
*  @since 2.6
*/
NONSHARABLE_CLASS( CIMPSSystemNotifierPubSub ) : public MIMPSSystemNotifier,
        public MIMPSPropertyNotificationObserver
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aObserver Observer to be informed when observed system key changes
    */
    static CIMPSSystemNotifierPubSub* NewL( MIMPSSystemNotifierObserver& aObserver,
    const TUid aKey );

    /**
    * Destructor.
    */
    virtual ~CIMPSSystemNotifierPubSub();

protected:     // from MIMPSSystemNotifier

    /**
    * @see MIMPSSystemNotifier
    */
    TInt Subscribe();

    /**
    * @see MIMPSSystemNotifier
    */
    void UnSubscribe();

    /**
    * @see MIMPSSystemNotifier
    */
    TInt GetIntKey( TInt& aValue );

protected:  // from MIMPSPropertyNotificationObserver.h

    void HandlePropertyNotificationEventL( TUid aCategory, TUint aKey );

private:

    /**
    * C++ default constructor.
    */
    CIMPSSystemNotifierPubSub( MIMPSSystemNotifierObserver& aObserver,
                               const TUid aKey );

    /**
    * Symbian 2nd phase constructor..
    */
    void ConstructL();

private:    // Data
    // Observers property sets, owned
    CIMPSPropertyObserver* iPropertyObserver;

    // Observer to be informed when value of system key changes
    MIMPSSystemNotifierObserver& iObserver;

    // the key this observer is attached
    TUid iKey;

    };

#endif      // CIMPSSYSTEMNOTFIERSYSTEMAGENT_H

// End of File
