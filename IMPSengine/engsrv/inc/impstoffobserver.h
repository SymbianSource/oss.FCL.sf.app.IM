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
* Description:  Publish & Subscribe property change observer
*
*/


#ifndef IMPSTOFFOBSERVER_H
#define IMPSTOFFOBSERVER_H


#include <e32base.h>
#include <e32property.h>
#include "impspropertyobserverapi.h"

// FORWARD DECLARATIONS
class CImpsServer;
class CImpsPropertyObserver;

/**
* CImpsTOffObserver
* Observer for terminal off line state events
*/
class CImpsTOffObserver: public CBase, public MImpsPropertyObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aServer WV engine core
        */
        static CImpsTOffObserver* NewL(
            CImpsServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CImpsTOffObserver();

        /**
        * Is the GPRS Connection allowed
        * @return ETrue if not in a flight mode
        */
        TBool CheckConnAllowed();

    public: // from derived classes

        /**
        * This is a callback function which is called when a notify event
        * completes. User must derive his class from MImpsPropertyObserver
        * and implement this method if he is to use notify services.
        *
        * @param aValue     the new value involved with the keyword
        */
        void HandlePropertyChangeL( TInt aValue );

    private:

        /**
        * By default constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aServer WV engine core
        */
        CImpsTOffObserver( CImpsServer& aServer );

        // By default, prohibit copy constructor
        CImpsTOffObserver( const CImpsTOffObserver& );
        // Prohibit assigment operator
        CImpsTOffObserver& operator= ( const CImpsTOffObserver& );

    private: //data
        CImpsServer&            iServer;
        CImpsPropertyObserver*  iObserver;
    };


#endif      // IMPSTOFFOBSERVER_H  

// End of File
