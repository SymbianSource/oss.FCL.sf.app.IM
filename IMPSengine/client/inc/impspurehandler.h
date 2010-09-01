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
*     WV engine pure data API notification handler.
*
*/


#ifndef CIMPSPUREHANDLER_H
#define CIMPSPUREHANDLER_H

// INCLUDES
#include <e32base.h>
#include "impshandler.h"


// FORWARD DECLARATIONS
class RImpsPureClient2;
class MImpsPureHandler2;

// CLASS DECLARATION

class CImpsPureHandler2 :public CImpsHandler2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phase constructor.
        * @param aClient a client session
        */
        static CImpsPureHandler2* NewL( 
            RImpsPureClient2& aClient,
            TInt aPriority = EPriorityStandard );
       
        /**
        * Destructor.
        */
        virtual ~CImpsPureHandler2();
      
    private:

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        */
        void HandleEventL( CImpsFields* aErrorFields );

        /**
        * Handle the Pure events
        */
        void HandlePureEventL( );

        /**
        * C++ default constructor.
        */
        CImpsPureHandler2( TInt aPriority, RImpsPureClient2& aClient );

    private:    // Data

        MImpsPureHandler2*      iHandlerCallBack;

    };


#endif      
            
