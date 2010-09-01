/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish and Subscribe implementation of notify services
*
*/



#ifndef CCANOTIFIERIMPL_H
#define CCANOTIFIERIMPL_H

//  INCLUDES
#include "MCANotifierInterface.h"
#include "MCAPropertyNotificationObserver.h"

// FORWARD DECLARATIONS
class MCANotifierKeyObserver;
class CCAPropertyObserver;

// CLASS DECLARATION
/**
*  Class for notifying key changes, reading and changing the values of
*  Pub&Sub keys.
*
*  @since Series 60 Platform 3.0
*/
class CCANotifierImpl : public CBase,
            public MCANotifierInterface,
            public MCAPropertyNotificationObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCANotifierImpl* NewL( MCANotifierKeyObserver* aObserver,
                                      const TUid aUid );

        /**
        * Destructor.
        */
        virtual ~CCANotifierImpl();

    public: // Functions from base classes

        TInt SetInt( TNotifyKey aKey, TInt aValue );
        TInt GetInt( TNotifyKey aKey, TInt& aValue );
        TInt ListenKeyChanges( TNotifyKey aKey );

    private: // From MCAPropertyNotificationObserver

        /**
        * @see MCAPropertyNotificationObserver
        */
        void HandlePropertyNotificationEventL( TUid aCategory, TUint aKey );

    private:

        /**
        * C++ default constructor.
        */
        CCANotifierImpl( MCANotifierKeyObserver* aObserver, const TUid aUid );


    private:    // Data

        MCANotifierKeyObserver* iObserver;
        TUid iCatUID;
        CCAPropertyObserver* iPropWatcher;
    };

#endif      // CCANOTIFIERIMPL_H 

// End of File
