/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This is a class handling sending data
*
*/

#ifndef __CIMPSPLUGINSENDDATAOPERATION_H__
#define __CIMPSPLUGINSENDDATAOPERATION_H__

//  INCLUDES
#include    <E32Std.h>

#include 	"impsdebugprint.h"

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
NONSHARABLE_CLASS( CImpsPluginSendDataOperation ) :
        public CBase
    {
public:
    /**
     *  Default constructor
     */
    static inline CImpsPluginSendDataOperation* NewLC();

    inline CImpsPluginSendDataOperation()
            : iOpId( KErrNone ),
            iData( NULL )
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::CImpsPluginSendDataOperation Start End" ) );
        }

    /**
     *  Destructor
     */
    inline ~CImpsPluginSendDataOperation()
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::~CImpsPluginSendDataOperation Start" ) );
        delete iData;
        if ( iStatus )
            {
            User::RequestComplete( iStatus, KErrCancel );
            }
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::~CImpsPluginSendDataOperation End" ) );
        }

public: // new function of the class


    /**
     * Set ids, operation and log id
     * @since 3.0
     */
    inline void SetIds( TInt aOperationId, TInt aLogId )
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::SetIds Start" ) );
        iOpId = aOperationId;
        iLogId = aLogId;
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::SetIds End" ) );
        };

    inline void SetRequestStatus( TRequestStatus& aStatus )
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::SetRequestStatus Start" ) );
        iStatus = &aStatus;
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::SetRequestStatus End" ) );
        };

    /**
     * Trequest status of this operation
     *
     * @since 3.0
     */
    inline TRequestStatus* Status()
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::Status Start-End" ) );
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
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::OperationId Start-End" ) );
        return iOpId;
        }

    inline TInt LogId() const
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::LogId Start-End" ) );
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
inline CImpsPluginSendDataOperation* CImpsPluginSendDataOperation::NewLC()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::NewLC Start-End" ) );
    CImpsPluginSendDataOperation* self = new( ELeave )
    CImpsPluginSendDataOperation();

    CleanupStack::PushL( self );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::NewLC End" ) );
    return self;
    }

/**
 * Complete operation
 */
inline void CImpsPluginSendDataOperation::CompletedOperation( TInt aCode )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::CompletedOperation Start :id:%d,code:%d" ),
             iOpId,
             aCode );
    if ( !iStatus )
        {
        IMPS_DP(
            D_IMPS_LIT( "CImpsPluginSendDataOperation::CompletedOperation:!!Double Completion!!!" ) );
        return;
        }
    User::RequestComplete( iStatus, aCode );
    iStatus = NULL;
    }

/**
 * Set response
 */
inline void CImpsPluginSendDataOperation::SetResponse( HBufC8* aBuff )
    {
    IMPS_DP( D_IMPS_LIT( "CPEngPureDataHandler::SetResponse - id:  %d" ), iOpId );
    delete iData;
    iData = aBuff;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginSendDataOperation::SetResponse End" ) );
    }

/**
 * Get response
 */
inline HBufC8* CImpsPluginSendDataOperation::Response()
    {
    IMPS_DP( D_IMPS_LIT( "CPEngPureDataHandler::Response Start" ) );
    HBufC8* b = iData;
    iData = NULL;
    IMPS_DP( D_IMPS_LIT( "CPEngPureDataHandler::Response End" ) );
    return b;
    }

#endif      // __CIMPSPLUGINSENDDATAOPERATION_H__

// End of File
