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
* Description:  Publish & Subscribe property change observer
*
*/


#ifndef IMPSPROPERTYOBSERVER_H
#define IMPSPROPERTYOBSERVER_H


#include <e32base.h>
//#include <e32property.h>
#include <centralrepository.h>
#include "impspropertyobserverapi.h"

// FORWARD DECLARATIONS
class MImpsPropertyObserver;


//**********************************
// CImpsPropertyObserver
//**********************************
class CImpsPropertyObserver: public CActive
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CImpsPropertyObserver* NewL( MImpsPropertyObserver& aObserver,
                                            TUid aCategory,
                                            TUint32 aKey );

        /**
        * Destructor
        */
        virtual ~CImpsPropertyObserver();


    protected:

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    private:
        /**
        * C++ default constructor.
        */
        CImpsPropertyObserver( MImpsPropertyObserver& iObserver,
                               TUid aCategory,
                               TUint32 aKey );

        /**
        * 2nd phase constructor.
        */
        void ConstructL( );

    private:
        // Data
        MImpsPropertyObserver&    iObserver;
        TUid                iCategory;
        TUint32             iKey;
        CRepository*        iRep;
    };


#endif      // IMPSPROPERTYOBSERVER_H  

// End of File
