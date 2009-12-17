/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This class handles the data sending
*
*/

#ifndef __CPENGSENDDATAOPERATION_H__
#define __CPENGSENDDATAOPERATION_H__

//  INCLUDES
#include    <E32Std.h>

#include    "PresenceDebugPrint.h"

// WV Engine walk around
#include    <impspurecli.h>

// FORWARD DECLARATION
class RImpsEng;

// CONSTANTS
/**
 * Send operation repeat time
 */
const TInt KSendOperationRepeatTime = 100000;


// CLASS DECLARATION

/**
 * Send data operation holder
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSendDataOperation ) :
        public CBase
    {
public:
    /**
     *  Default constructor
     */
    static inline CPEngSendDataOperation* NewLC();

    inline CPEngSendDataOperation()
            : iOpId( KErrNone ),
            iData( NULL )
        {
        }

    /**
     *  Destructor
     */
    inline ~CPEngSendDataOperation()
        {
        delete iData;
        if ( iStatus )
            {
            User::RequestComplete( iStatus, KErrCancel );
            }
        }

public: // new function of the class


    /**
     * Set ids, operation and log id
     * @since 3.0
     */
    inline void SetIds( TInt aOperationId, TInt aLogId )
        {
        iOpId = aOperationId;
        iLogId = aLogId;
        };

    inline void SetRequestStatus( TRequestStatus& aStatus )
        {
        iStatus = &aStatus;
        };

    /**
     * Trequest status of this operation
     *
     * @since 3.0
     */
    inline TRequestStatus* Status()
        {
        return iStatus;
        }

    /**
     * Get Ids
     *
     * @since 3.0
     * @return operation Id
     */
    inline TInt OperationId() const
        {
        return iOpId;
        }

    inline TInt LogId() const
        {
        return iLogId;
        }

    /**
     * Complete operation
     *
     * @since 3.0
     * @param aCode complete code
     */
    inline void CompletedOperation( TInt aCode );

    /**
     * Set response buffer
     * Owner ship is transfered
     *
     * @since 3.0
     * @param aBuff response buffer
     */
    inline void SetResponse( HBufC8* aBuff );

    /**
     * Response buffer,
     * owner ship is transfered
     *
     * @since 3.0
     * @return transfer buffer
     */
    inline HBufC8* Response();

private: // Public data

    /// Status of the operation
    TRequestStatus*     iStatus;

    /// OWN: Operation Id
    TInt                iOpId;

    /// OWN: Response buffer
    HBufC8*             iData;

    /// OWN: Id used for logs writting
    TInt                iLogId;
    };

/**
 * NewLC
 */
inline CPEngSendDataOperation* CPEngSendDataOperation::NewLC()
    {
    CPEngSendDataOperation* self = new( ELeave )
    CPEngSendDataOperation();

    CleanupStack::PushL( self );
    return self;
    }

/**
 * Complete operation
 */
inline void CPEngSendDataOperation::CompletedOperation( TInt aCode )
    {
    PENG_DP( D_PENG_LIT( "CPEngSendDataOperation::CompletedOperation:id:%d,code:%d" ),
             iOpId,
             aCode );
    if ( !iStatus )
        {
        PENG_DP(
            D_PENG_LIT( "CPEngSendDataOperation::CompletedOperation:!!Double Completion!!!" ) );
        return;
        }
    User::RequestComplete( iStatus, aCode );
    iStatus = NULL;
    }

/**
 * Set response
 */
inline void CPEngSendDataOperation::SetResponse( HBufC8* aBuff )
    {
    PENG_DP( D_PENG_LIT( "CPEngPureDataHandler::HandlePureDataL - id:  %d" ), iOpId );
    delete iData;
    iData = aBuff;
    }

/**
 * Get response
 */
inline HBufC8* CPEngSendDataOperation::Response()
    {
    HBufC8* b = iData;
    iData = NULL;
    return b;
    }

#endif      // __CPENGSENDDATAOPERATION_H__

// End of File
