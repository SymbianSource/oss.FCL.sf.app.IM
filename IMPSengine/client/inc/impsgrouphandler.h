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
*     WV engine group API notification handler.
*
*/


#ifndef ImpsGroupHandler_H
#define ImpsGroupHandler_H

// INCLUDES
#include <e32base.h>
#include "impshandler.h"


// FORWARD DECLARATIONS
class RImpsGroupClient2;
class MImpsGroupHandler2;

// CLASS DECLARATION

class CImpsGroupHandler2 :public CImpsHandler2
    {
    public:  // Constructors and destructor
       
        /**
        * Two-phase constructor.
        * @param aClient a client session
        * @param aPriority Handler priority
        */
        static CImpsGroupHandler2* NewL( 
            RImpsGroupClient2& aClient,
            TInt aPriority = EPriorityStandard );
       
        /**
        * Destructor.
        */
        virtual ~CImpsGroupHandler2();

    private:

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        */
        void HandleEventL( CImpsFields* aErrorFields );

        /**
        * Handle the Group events
        */
        void HandleGroupEventL( );

        /**
        * C++ default constructor.
        * @param aClient a client session
        * @param aPriority Handler priority
        */
        CImpsGroupHandler2( TInt aPriority, RImpsGroupClient2& aClient );

        // By default, prohibit copy constructor
        CImpsGroupHandler2( const CImpsGroupHandler2& );
        // Prohibit assigment operator
        CImpsGroupHandler2& operator= ( const CImpsGroupHandler2& );


    private:    // Data

        MImpsGroupHandler2*      iHandlerCallBack;

    };

#endif      // ImpsGroupHandler_H
            
// End of File
