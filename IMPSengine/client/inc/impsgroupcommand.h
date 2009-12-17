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
*     A part of WV engine group feature interface. 
*     Handles asyncronous requests. 
*
*/


#ifndef CImpsGroupCommand_H
#define CImpsGroupCommand_H

//  INCLUDES
#include <e32base.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impsdataaccessorapi.h"
#include "impsclientsrv.h"

// FORWARD DECLARATIONS
class RImpsGroupClient2;
class CImpsFields;
class CImpsKey;
class CImpsDataAccessor;

// CLASS DECLARATION

    class CImpsGroupCommand2 :public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aClient a client session
        */
        static CImpsGroupCommand2* NewL( RImpsGroupClient2& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CImpsGroupCommand2();

    public: // New functions
        
        /** 
        * Start the execution. 
        * @param aOpId operation-id
        * @param aType message type
        */
        void StartRunL( 
            TInt                aOpId,
            TImpsServRequest    aType = EImpsServGroup); 

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
        * @param aElementValue Value is inserted to the dataaccesor
        * @param aElementName name of the key
        */
        void InsertElementL( const TDesC& aElementValue,
                             const TImpsContent aElementName );

        /**
        * Insert boolean element to the command
        * @param aBool Boolean Value is inserted to the dataaccesor
        * @param aElementName name of the key
        */
        void InsertElementL( TBool aBool,
                             const TImpsContent aElementName );

        /**
        * Insert boolean element to the command
        * @param aInt Integer Value is inserted to the dataaccesor
        * @param aElementName name of the key
        */
        void InsertElementIntL( TInt aInt,
                             const TImpsContent aElementName );

        /**
        * Insert string element to the command
        * @param aElementValue Value is inserted to the dataaccesor
        * @param aElementName name of the key
        * @param aIndex index of the key
        */
        void InsertElementL( const TDesC& aElementValue,
                             const TImpsContent aElementName,
                             TInt aIndex );

        /**
        * Insert empty element to the command
        * @param aElementName name of the key
        */
        void InsertEmptyL( const TImpsContent aElementName );

        /**
        * Add key to command
        * @param aKey to be inserted
        */
        inline void AddKeyL( TUint aKey );

        /**
        * Pop Key from command
        */
        inline void PopKeyL( );

        /**
        * Set message type to the command
        * @param aMsgType Messagetype of the command
        */
        inline void SetMessageType( TImpsMessageType aMsgType );

        /**
        * Pack the command
        */
        void PackMessageL( );
        
        /**
        * Send the packed data to the server
        * @param messagetype 
        * @param operatioid
        */
        void PackAndSendL( TImpsMessageType aMsgType, TInt aOpId );

    private:

        /**
        * C++ default constructor.
        * @param aPriority Pririty of the command
        * @param aClient a client session
        */
        /**
        */
        CImpsGroupCommand2( TInt aPriority, RImpsGroupClient2& aClient );

        // from CActive
        void RunL();
        void DoCancel();

        // By default, prohibit copy constructor
        CImpsGroupCommand2( const CImpsGroupCommand2& );
        // Prohibit assigment operator
        CImpsGroupCommand2& operator= ( const CImpsGroupCommand2& );

    private:    // Data
        RImpsGroupClient2&               iClient;
        TInt                            iOpId;
        TImpsServRequest                iType;
        TImpsMessageType                iMessageType;
        HBufC8*                         iMessage;

        CImpsFields*                    iImpsFields;
        CImpsDataAccessor*              iAccessor;
        MImpsKey*                       iKey;
        TPtrC8                          iMessagePtr;
    };


#include "ImpsGroupCommand.inl"

#endif      // CImpsGroupCommand_H

// End of File
