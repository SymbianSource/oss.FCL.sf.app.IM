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
*     WV engine pure data command.
*
*/



#ifndef CIMPSPURECOMMAND_H
#define CIMPSPURECOMMAND_H

//  INCLUDES
#include <e32base.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impsdataaccessorapi.h"
#include "impsclientsrv.h"

// FORWARD DECLARATIONS
class CImpsFields;
class CImpsKey;
class CImpsDataAccessor;

// CLASS DECLARATION

    class CImpsPureCommand2 :public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aClient a client session
        */
        static CImpsPureCommand2* NewL( RImpsPureClient2& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CImpsPureCommand2();

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
        * Reset all the member
        */
        void ResetMembers( );

        /**
        * Initialize the command with type and array size and the array address
        * @param aElementArray Key array releated to the command 
        * @param aArraySize size of the array
        */
        void InitialiseL( const TImpsContent* aElementArray,
                          const TUint aArraySize );

        /**
        * Insert string element to the command
        * @param aElementValue to be inserted
        * @param aElementName name of the key
        */
        void InsertElementL( const TDesC8& aElementValue,
                             const TImpsContent aElementName );
        /**
        * Set message type to the command
        * @param aMsgType Messagetype of the command
        */
        void SetMessageType( TImpsMessageType aMsgType );

        /**
        * Pack the command
        * @return  KErrNone if ok, error otherwise
        */
        TInt PackMessageL( );
        
        /**
        * Send the packed data to the server
        * @param messagetype 
        * @param operatioid
        * @return  KErrNone if ok, error otherwise
        */
        TInt PackAndSendL( TImpsMessageType aMsgType, TInt aOpId );

    private:

        /**
        * C++ default constructor.
        */
        CImpsPureCommand2( TInt aPriority, RImpsPureClient2& aClient );

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * By default Symbian constructor is private.
        */
        void ConstructL();

    private:    // Data

        RImpsPureClient2&                iClient;
        TInt                            iOpId;
        TImpsServRequest                iType;
        TImpsMessageType                iMessageType;
        HBufC8*                         iMessage;
        CImpsFields*                    iImpsFields;
        CImpsDataAccessor*              iAccessor;
        MImpsKey*                       iKey;
        TPtrC8                          iMessagePtr;

    private:    // Friend classes

        friend class RImpsPureClient2;

    };

#endif      // CIMPSPURECOMMAND_H
            
// End of File
