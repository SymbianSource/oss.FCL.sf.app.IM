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
* imps access commander. AO.
*
*/


#ifndef ImpsAccessCommand_H
#define ImpsAccessCommand_H

//  INCLUDES
#include <e32base.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impsclientsrv.h"


// FORWARD DECLARATIONS
class RImpsAccessClient2;

// CLASS DECLARATION

/**
*  Active object to control access client request
*/
class CImpsAccessCommand2 :public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aClient a client session
        */
        static CImpsAccessCommand2* NewL( RImpsAccessClient2& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CImpsAccessCommand2();

    public: // New functions
        
        /** 
        * Start the execution. 
        * @param aOpId operation-id
        * @param aType message type
        */
        void StartRunL( 
            TInt                aOpId,
            TImpsServRequest    aType );

        /**
        * Set login parameters.
        * Data is copied.
        * @param aServer SAP URI
        * @param aUser user id
        * @param aPsw password
        * @param aClientId CSP clientId
        * @param aAP CommDb id for access point
        * @param aKey1 future option
        * @param aKey2 future option
        */
        void SetParametersL( 
            const TDesC& aServer,
            const TDesC& aUser,
            const TDesC& aPsw,
            const TDesC& aClientId,
            TUint32 aAP,
            const TDesC* aKey1,
            const TDesC* aKey2 );

       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CImpsAccessCommand2( TInt aPriority, RImpsAccessClient2& aClient );

        // from CActive
        void RunL();
        void DoCancel();

        /**
        * By default constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsAccessCommand2( const CImpsAccessCommand2& );
        // Prohibit assigment operator
        CImpsAccessCommand2& operator= ( const CImpsAccessCommand2& );

    private:    // Data
        RImpsAccessClient2&  iClient;
        TInt                iOpId;
        TImpsServRequest    iType;
        TInt                iState;

        // Local copies of data to enable internally asyncronous request
        HBufC8*             iCookie;        // session cookie
        HBufC8*             iMessageStream; // streaming buffer
        TPtrC8              iMessagePtr;
        TPtrC8              iCookiePtr;

    private:    // Friend classes
        friend class RImpsAccessClient2;

    };

#endif      // ?INCLUDE_H   
            
// End of File
