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
* commander for imps data accessor. AO.      
*
*/


#ifndef CImpsFundCommand_H
#define CImpsFundCommand_H

//  INCLUDES
#include "impsdataaccessorapi.h"
#include "impsclientsrv.h"

// FORWARD DECLARATIONS
class CImpsKey;
class CImpsDataAccessor;


    class CImpsFundCommand2 : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aClient a client session
        */
        static CImpsFundCommand2* NewL( RImpsFundClient2& aClient );
        
        /**
        * Destructor.
        */
        virtual ~CImpsFundCommand2();

    public: // New functions
        
        /** 
        * Start the execution. 
        * @param aType Type of the request
        */
        void StartRunL();
        
        /*
        * Reset all member variables that the object owns
        */
        void ResetMembers();
  
        /** 
        * Initialise a request
        * @param aUserIDs User IDs to be inserted
        */
        void InitialiseL( const TImpsContent* aElementArray,
                          const TUint aArraySize );
        
        /** 
        * Set the user IDs of the request
        * @param aUserIDs User IDs to be inserted
        */
        void SetUserIDsL( const MDesCArray* aUserIDs );
        
        /** 
        * Construct a search pair list 
        * @param aSearchPairs
        */
        void MakeSearchPairListL( const CSearchPairs& aSearchPairs );
        
        /** 
        * Set the screen names of the request
        * @param aScreenNames Screen names
        * @param aGroupNames GroupIDs
        */
        void SetScreenNamesL( const MDesCArray* aScreenNames,
                              const MDesCArray* aGroupNames );
        
        /** 
        * Insert a descriptor element to the request
        * @param aElementValue Value of the element
        * @param aElementName Name of the element
        * @param aIndex Index of the element
        */
        void InsertDescriptorElementL( const TDesC& aElementValue,
                                       const TImpsContent aElementName,
                                       const TInt aIndex );

        /** 
        * Insert an integer element to the request
        * @param aElementValue Value of the element
        * @param aElementName Name of the element
        * @param aIndex Index of the element
        */
        void InsertIntegerElementL( const TInt aElementValue,
                                    const TImpsContent aElementName,
                                    const TInt aIndex );

        /*
        * Insert an empty element to the request
        * @param aElementName Name of the element
        * @param aIndex Index of the element
        */
        void InsertEmptyElementL( const TImpsContent aElementName,
                                  const TInt aIndex );
        
        /** 
        * Insert a boolean element to the request
        * @param aElementValue Value of the element
        * @param aElementName Name of the element
        * @param aIndex Index of the element
        */
        void InsertBooleanElementL( const TBool aElementValue, 
                                    const TImpsContent aElementName,
                                    const TInt aIndex );
        
        /** 
        * Pack the data for sending to the server thread 
        * @param aMessageType Type of the request
        * @param aOpId OpId of the request
        */
        void PackAndSendL( const TImpsMessageType aMessageType,
                           const TInt aOpId );

        /** 
        * Pop one element from the data accessor key
        * @param aCount How many to Pop();
        */
        inline void PopElementL( const TInt aCount = 0 );

    private:

        /**
        * C++ default constructor is private
        */
        CImpsFundCommand2( TInt aPriority, RImpsFundClient2& aClient );

        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
        /*
        * Map requests from TImpsMessageType to TImpsServRequest
        * @param aMessageType TImpsMessageType
        * @return TImpsServRequest Type of the message
        */
        TImpsServRequest ServerRequestTypeL( 
                                        const TImpsMessageType aMessageType );

        // By default, prohibit copy constructor
        CImpsFundCommand2( const CImpsFundCommand2& );
        // Prohibit assigment operator
        CImpsFundCommand2& operator= ( const CImpsFundCommand2& );

    private:    // Data
        HBufC8*                         iMessage;
        CImpsKey*                       iKey;
        CImpsFields*                    iImpsFields;
        CImpsDataAccessor*              iDataAccessor;
        RImpsFundClient2&                iClient;
        TUint                           iArraySize;
        TInt                            iOpId;
        TInt                            iState;
        TPtrC8                          iMessagePtr;
    };
#include "ImpsFundCommand.inl"

#endif  
            
// End of File
