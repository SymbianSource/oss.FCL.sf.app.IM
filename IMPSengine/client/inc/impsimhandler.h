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
* Description: 
*     WV interface observer handler.
*
*/


#ifndef CImpsImHandler_H
#define CImpsImHandler_H

// INCLUDES
#include <e32base.h>
#include "impshandler.h"

// FORWARD DECLARATIONS
class RImpsImClient2;
class MImpsImHandler2;
class CImpsDataAccessor;

// CLASS DECLARATION

class CImpsImHandler2 :public CImpsHandler2
    {
    public:  // Constructors and destructor

      
        /**
        * Two-phase constructor.
        * @param aClient a client session
        * @param aPriority active object priority
        */
        static CImpsImHandler2* NewL( 
            RImpsImClient2& aClient,
            TInt aPriority );
       
        /**
        * Destructor.
        */
        virtual ~CImpsImHandler2();

    protected:
        /**
        * By default constructor is private.
        */
        void ConstructL();

    private:

        /**
        * C++ default constructor. 
        * @param aPriority active object priority
        * @param aClient Im Client
        */
        CImpsImHandler2( TInt aPriority, RImpsImClient2& aClient );

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        */
        void HandleEventL( CImpsFields* aErrorFields );

        /**
        * Handle the Im events
        */
        void HandleImEventL( );

        // By default, prohibit copy constructor
        CImpsImHandler2( const CImpsImHandler2& );
        // Prohibit assigment operator
        CImpsImHandler2& operator= ( const CImpsImHandler2& );

    private:    // Data

        MImpsImHandler2*     iHandlerCallBack;

    };

#endif      // CImpsImHandler_H  

            
// End of File
