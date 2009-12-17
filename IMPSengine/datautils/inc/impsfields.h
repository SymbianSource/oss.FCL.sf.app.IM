/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Internal engine data structure for IMPS messages
*
*/



#ifndef CImpsFields_H
#define CImpsFields_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "impsconst.h"
#include "impsservercommon.h"
#include "impscommonenums.h"
#include "impsdata.h"

// CONSTANTS
// Maximum size for Mmessage field string value in internalize.
const TInt KMaxHeaderStringLength = 100;

// constant for TImpsPackedEntity, Notice: change always when data members change!
// size of mem::copy for CImpsFields
// 2*4.   iPoll and iSessionType are not copied.
const TInt KImpsPackedCopyData = 8;
// Nbr of fields that have streamed
const TInt KImpsPackedFieldsNbr = 4;

// DATA TYPES
// Values for recipient types
enum TImpsRecipients
    {
    EImpsUser = 1,
    EImpsGroup,
    EImpsList,
    EImpsScreenName
    };

// FORWARD DECLARATIONS
class RWriteStream;
class CImpsPresenceArray;
class CImpsFields;
class CImpsCapability;
class CImpsServices;
class CImpsGroupFields;


/**
*  CImpsFields
*/
class CImpsFields : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CImpsFields* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CImpsFields();

    public: // New functions

        /**
        * Reset.
        */
        IMPORT_C void Reset( );

        /**
        * Size of the object in memory.
        * @return object size
        */
        IMPORT_C TInt Size() const;

        /**
        * Set Transaction-ID
        * @param aCid transaction ID
        */
        IMPORT_C void SetTidL( const TDesC& aId );

        /**
        * Get Transcation-ID
        * @return transaction id
        */
        IMPORT_C TPtrC TidL();

        /**
        * Set Session-ID
        * @param aId session ID
        */
        IMPORT_C void SetSessionIdL( const TDesC& aId );

        /**
        * Get Session-ID
        * @return session id
        */
        IMPORT_C TPtrC SessionIdL();

        /**
        * Get LoginSession-ID
        * @return session id
        */
        // TPtrC LoginSessionIdL();

        /**
        * Set CSP Message-Type
        * @param aType message type, values defined in IMPSCONST.H
        */
        inline void SetMessageType( TInt aType );

        /**
        * Get CSP Message-Type
        * @return message type.
        */
        inline TInt MessageType() const;

        /**
        * Response-Status accessor
        * @return Response-Status field value
        */
        IMPORT_C TInt32 ResponseStatusL();


        /**
        * Status mutator
        * @param aStatus status value. See Impsconst.h.
        */
        inline void SetStatus( const TInt aStatus );

        /**
        * Status accessor
        * @return status value. See Impsconst.h.
        */
        inline TInt Status() const;

        /**
        * PollL
        * @return ETrue if Poll
        */
        IMPORT_C TBool PollL();

        /**
        * SetSessionTypeL
        */
        IMPORT_C void SetSessionTypeL( TImpsSessionType aType );

        /**
        * SessionTypeL
        */
        IMPORT_C TImpsSessionType SessionTypeL();

        /**
        * accessor for fundamental specific data
        */
        inline CImpsData* Data( ) const;

        /**
        * Set CSP version
        */
        inline void SetCspVersion( TImpsCspVersion aCspVersion );

        /**
        * CSP version
        */
        inline TImpsCspVersion CspVersion();

#ifdef _DEBUG
        /**
        * Data element creator.
        * This function takes the ownership of the aData
        * The object then belongs to CImpsData
        */
        void SetDataL( CImpsData& aData );


        /**
        * Dumps the internal data to file
        */
        void DumpToFileL( RFs& aFs, const TDesC& aFilename );
#endif

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CImpsFields();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CImpsFields( const CImpsFields& );
        // Prohibit assigment operator
        CImpsFields& operator= ( const CImpsFields& );


    public:     // Data

    protected:  // Data

    private:    // Data
        TInt32          iMsgType;       // Message-Type;    // This must be first!
        TInt32          iStatus;        // Status
        CImpsData*      iData;          // All other data which is not in some member of impsfields
        TImpsCspVersion iCspVersion;    // CSP version in use


    public:     // Friend classes
        friend class TImpsPackedEntity;
        friend class CImpsSession;

    };

#include "impsfields.inl"

#endif      // ?INCLUDE_H   

// End of File
