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
* Description: Header file for im commander
* 
*/



#ifndef ImpsImCommand_H
#define ImpsImCommand_H

//  INCLUDES
#include <e32base.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impsdataaccessorapi.h"
#include "impsclientsrv.h"


// FORWARD DECLARATIONS
class CImpsKey;


// CLASS DECLARATION

class CImpsImCommand2 :public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aClient a client session
        */
        static CImpsImCommand2* NewL( RImpsImClient2& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CImpsImCommand2();

    public: // New functions
        
        /** 
        * Start the execution. 
        * @param aOpId operation-id
        * @param aType message type
        * @param aMessage message
        */
        void StartRunL( 
            TInt                aOpId,
            TImpsServRequest    aType );

        /**
        * Set request parameters.
        */
        void SetParametersL( 
            const TDesC* aSenderSn,
            //const TDesC* aSendeGroup,
            const MDesCArray* aUserIds,
            const TDesC* aGroupId,         
            const MDesCArray* aScreenNames, 
            const TDesC& aContent,
            TBool aDeliveryReportWanted );

        /**
        * Set request parameters.
        */
        void SetParametersL( 
            const TDesC* aSenderSn,
            const MDesCArray* aUserIds,
            const TDesC* aGroupId,         
            const MDesCArray* aScreenNames, 
            const TDesC& aContentType,
            const TDesC8& aContent,
            TBool aDeliveryReportWanted );


        /*
        * Set request parameyter for blocking
        */
        void SetParametersL(
            const MDesCArray* aBlockEntity,
            const MDesCArray* aUnBlockEntity,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedEntityList,
            const MDesCArray* aUnGrantEntity,
            TBool aGrantedListInUse );


    private:

        /**
        * C++ default constructor.
        */
        CImpsImCommand2( TInt aPriority, RImpsImClient2& aClient );


        // from CActive
        void RunL();
        void DoCancel();

        /**
        * By default constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsImCommand2( const CImpsImCommand2& );
        // Prohibit assigment operator
        CImpsImCommand2& operator= ( const CImpsImCommand2& );

    private:    // Data
        RImpsImClient2&  iClient;
        TInt                iOpId;
        TImpsServRequest    iType;
        TInt                iState;

        // Local copies of data to enable internally asyncronous request
        HBufC8*     iMessageStream;    // streaming buffer for packed messages

        CImpsDetailed* iDetailed; 

    private:    // Friend classes
        friend class RImpsImClient2;

    };

#endif   
            
// End of File
