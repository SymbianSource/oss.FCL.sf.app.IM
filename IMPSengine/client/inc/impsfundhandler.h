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
* imps fundemental feature handler for imps engine.       
*
*/


#ifndef CImpsFundHandler_H
#define CImpsFundHandler_H

// INCLUDES
#include "impshandler.h"
#include "impsfundamental.h"

// FORWARD DECLARATIONS
class RImpsFundClient2;
class MImpsFundHandler2;
class CImpsDataAccessor;

// CLASS DECLARATION

    class CImpsFundHandler2 :public CImpsHandler2
    {
    public:  // Constructors and destructor

        /**
        * Two-phase constructor.
        * @param aClient a client session
        * @param aPriority active object priority
        */
        static CImpsFundHandler2* NewL( RImpsFundClient2& aClient, 
                                       TInt aPriority );

        /**
        * Destructor.
        */
        virtual ~CImpsFundHandler2();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CImpsFundHandler2( TInt aPriority, RImpsFundClient2& aClient );

        /**
        * Call appropriate observer
        * @param aErrorFields used if error case. NULL or ImpsFields structure
        */
        void HandleEventL( CImpsFields* aErrorFields );
        
        /**
        * Handle the fundamental events
        */
        void HandleFundamentalEventL( );

        /**
        * Handle a search response
        * @param aOpId OpId of the initial request
        */
        void HandleSearchResponseL( const TInt aOpId );
        
        /**
        * Handle an invitation coming our way
        */
        void HandleInviteRequestL();
        
        /**
        * Handle invitation responses from the recipients
        */
        void HandleInviteResponseL();
        
        /**
        * Handle the cancellation of invitations
        */
        void HandleInviteCancelL();
        
        /**
        * Fetch a descriptor type of element from the response
        * @param aContent Name of the element to fetch
        * @param aIndex Index of the element
        * @return TPtrC
        */
        TPtrC DescriptorElementL( const TImpsContent aContent,
                                  const TInt aIndex );
        
        /**
        * Fetch an integer type of element from the response
        * @param aContent Name of the element to fetch
        * @param aIndex Index of the element
        * @return TInt
        */
        TInt IntegerElementL( const TImpsContent aContent,
                              const TInt aIndex );
        
        /**
        * Fetch a boolean type of element from the response
        * @param aContent Name of the element to fetch
        * @param aIndex Index of the element
        * @return TBool
        */
        TBool BooleanElementL( const TImpsContent aContent,
                               const TInt aIndex );
        
        /**
        * Returns the search results from received from the remote host
        * @param aResultType Upon return, contains the type of the results
        * @return CPtrC16Array Pointer to an array of strings
        */
        CPtrC16Array* SearchResultsLC( TImpsSearchResultType& aResultType );

        // By default, prohibit copy constructor
        CImpsFundHandler2( const CImpsFundHandler2& );
        // Prohibit assigment operator
        CImpsFundHandler2& operator= ( const CImpsFundHandler2& );

    private:    // Data

        RImpsFundClient2&               iClient;
        CImpsDataAccessor*              iDataAccessor;
        CImpsKey*                       iKey;
    };

#endif   
            
// End of File
