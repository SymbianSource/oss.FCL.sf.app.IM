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
* Description: Accessor handler
* Active object to handle access events from server thread
* 
*/


#ifndef CImpsAccessHandler_H
#define CImpsAccessHandler_H

// INCLUDES
#include <e32base.h>
#include "impshandler.h"


// FORWARD DECLARATIONS
class RImpsAccessClient2;
class MImpsAccessHandler2;

// CLASS DECLARATION

/**
*  Active object to handle access events from server thread
*/
class CImpsAccessHandler2 :public CImpsHandler2
    {
    public:  // Constructors and destructor
       
        /**
        * Two-phase constructor.
        * @param aClient a client session
        * @param aPriority active object priority
        */
        static CImpsAccessHandler2* NewL( 
            RImpsAccessClient2& aClient,
            TInt aPriority = EPriorityStandard );
       
        /**
        * Destructor.
        */
        virtual ~CImpsAccessHandler2();
        
    private:

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        */
        void HandleEventL( CImpsFields* aErrorFields );

        /**
        * Handle the access events
        */
        void HandleAccessEventL( );

        /**
        * C++ default constructor.
        */
        CImpsAccessHandler2( TInt aPriority, RImpsAccessClient2& aClient );

        // By default, prohibit copy constructor
        CImpsAccessHandler2( const CImpsAccessHandler2& );
        // Prohibit assigment operator
        CImpsAccessHandler2& operator= ( const CImpsAccessHandler2& );

    private:    // Data

        MImpsAccessHandler2*      iHandlerCallBack;

    };


#endif      // ?INCLUDE_H   
            
// End of File
