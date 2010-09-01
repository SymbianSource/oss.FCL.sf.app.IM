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
* Description: mplementation of the IM Client interface (imclient.h)
*
*/



// INCLUDE FILES
#include "imclientimpl.h"
#include "apiutils.h"
#include "imerrors.h"
#include "imconnectionimpl.h"
#include "cntdbreader.h"


// CONSTANTS
const TInt KDetailedListGranularity = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImClient::CImClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImClient::CImClient( CImConnectionImpl& aImConnection )
        :   iImConnection( aImConnection )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Construction" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CImClient::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImClient::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CImClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImClient* CImClient::NewL( CImConnectionImpl& aImConnection )
    {
    CImClient* self = new( ELeave ) CImClient( aImConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// Destructor
CImClient::~CImClient()
    {

#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Destruction" ) );
#endif


    // This should be already empty
    __ASSERT_DEBUG( iOutstandingOpIds.Count() == 0,
                    User::Panic( KTxtImOpenApiPanic, EListNotEmpty ) );

    iOutstandingOpIds.Close();

    }


// -----------------------------------------------------------------------------
// CImClient::RegisterL
// Starts the registration sequence
// -----------------------------------------------------------------------------
//
void CImClient::RegisterObserverL( MImObserver* aObserver )
    {

#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: RegisterL" ) );
#endif

    if ( iClientObserver )
        {
        User::Leave( KImApiErrAlreadyRegistered );
        }

    iImClient.RegisterL(
        ImpsEngine(),
        ImpsImHandler(),
        NULL,
        iImConnection.ApplicationId(),
        ETrue );

    iImClient.RegisterErrorObserverL( ImpsErrorHandler() );

    iClientObserver = aObserver;

    }

// -----------------------------------------------------------------------------
// CImClient::UnregisterL
// Unregisters the client from the WV engine
// -----------------------------------------------------------------------------
//
void CImClient::UnregisterObserver()
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: UnregisterL" ) );
#endif

    // Clear the pointers to the client
    iClientObserver = NULL;

    // Unregister from Imps engine
    iImClient.Unregister();
    TInt err;
    TRAP( err, iImClient.UnregisterErrorObserverL() );
    }


// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const TContactItemId& aContactItem,
                                  const TDesC16&        aContent )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Sending PToP text Message (ContactID)" ) );
#endif
    // Check the observer
    if ( !iClientObserver )
        {
        User::Leave( KImApiErrNotRegistered );
        }
    // Get the user IDs from the contact DB
    CDesCArraySeg* userIds = new( ELeave ) CDesCArraySeg( 1 );
    CleanupStack::PushL( userIds );
    iImConnection.ContactDBReader().GetWVIdL( aContactItem, *userIds );

    // check the userIds
    if ( userIds->Count() == 0 )
        {
        // no valid user to send
        CleanupStack::PopAndDestroy(); // >> userIds
        User::Leave( KImApiErrInvalidContactId );
        }

#ifdef _DEBUG
    for ( TInt i( 0 ); i < userIds->MdcaCount(); ++i )
        {
        TPtrC	tmp = ( *userIds )[i];
        CImApiLogger::Log( _L( "    User ID: %S" ), &tmp );
        }
#endif

    TInt opId( DoSendMessageL( *userIds, aContent ) );
    CleanupStack::PopAndDestroy(); // >> userIds
    return opId;

    }

// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const CContactIdArray&   aContactIds,
                                  const TDesC16&           aContent )

    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Sending PToP text Message (ContactID)" ) );
#endif
    // Check the observer
    if ( !iClientObserver )
        {
        User::Leave( KImApiErrNotRegistered );
        }

    // Get the user IDs from the contact DB
    CDesCArraySeg* userIds = new( ELeave ) CDesCArraySeg( 1 );
    CleanupStack::PushL( userIds );
    iImConnection.ContactDBReader().ReadWVIdsL( aContactIds, *userIds );

    // check the userIds
    if ( userIds->Count() == 0 )
        {
        // no valid user to send
        CleanupStack::PopAndDestroy(); // >> userIds
        User::Leave( KImApiErrInvalidContactId );
        }

#ifdef _DEBUG
    for ( TInt i( 0 ); i < userIds->MdcaCount(); ++i )
        {
        TPtrC	tmp = ( *userIds )[i];
        CImApiLogger::Log( _L( "    User ID: %S" ), &tmp );
        }
#endif

    TInt opId( DoSendMessageL( *userIds, aContent ) );
    CleanupStack::PopAndDestroy(); // >> userIds
    return opId;

    }


// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const TDesC&      aUserId,
                                  const TDesC16&    aContent )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Sending PToP text Message (UserID)" ) );
    CImApiLogger::Log( _L( "    User ID: %S" ), &aUserId );
#endif

    // Check the observer
    if ( !iClientObserver )
        {
        User::Leave( KImApiErrNotRegistered );
        }

    // Sanity checking
    if ( aUserId.Length() == 0  ) // empty user ID
        {
        User::Leave( KImApiErrInvalidUserId );
        }

    CDesCArraySeg* userIds = new( ELeave ) CDesCArraySeg( 1 );
    CleanupStack::PushL( userIds );
    userIds->AppendL( aUserId );

    TInt opId( DoSendMessageL( *userIds, aContent ) );
    CleanupStack::PopAndDestroy(); // >> userIds
    return opId;
    }

// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const MDesCArray& aUserIds,
                                  const TDesC16&    aContent )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: Sending PToP text Message (UserID)" ) );

    for ( TInt i( 0 ); i < aUserIds.MdcaCount(); ++i )
        {
        TPtrC	tmp = aUserIds.MdcaPoint( i );
        CImApiLogger::Log( _L( "    User ID: %S" ), &tmp );
        }
#endif

    // Check the observer
    if ( !iClientObserver )
        {
        User::Leave( KImApiErrNotRegistered );
        }

    // sanity checking
    if ( aUserIds.MdcaCount() == 0 ) // empty list
        {
        User::Leave( KImApiErrInvalidUserId );
        }
    else if ( ( aUserIds.MdcaCount() == 1 ) &&
              ( aUserIds.MdcaPoint( 0 ).Length() == 0 ) ) // empty user ID
        {
        User::Leave( KImApiErrInvalidUserId );
        }

    TInt opId( DoSendMessageL( aUserIds, aContent ) );
    return opId;
    }


// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const CContactIdArray& /*aContactIds*/,
                                  const TDesC&           /*aContentType*/,
                                  const TDesC8&          /*aContent*/ )
    {
    User::Leave( KErrNotSupported );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImClient::SendPToPMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::SendPToPMessageL( const MDesCArray& /*aUserIds*/,
                                  const TDesC&      /*aContentType*/,
                                  const TDesC8&     /*aContent*/ )
    {
    User::Leave( KErrNotSupported );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImClient::HandleNewTextMessageL
//
// -----------------------------------------------------------------------------
//
void CImClient::HandleNewTextMessageL( TInt              /*aOpId*/,
                                       const TDesC&      /*aMessageId*/,
                                       const TDesC&      aSender,
                                       const TDesC&      /*aGroupId*/,
                                       const MDesCArray& /*aRecipients*/,
                                       const MDesCArray& /*aScreenNames*/,
                                       const TDesC&      aText,
                                       TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log(
        _L( "CImClient: HandleNewTextMessageL" ) );
#endif
    _LIT( KMessageType, "text/plain" );

    if ( iClientObserver )
        {
        iClientObserver->HandleNewPToPMessageL(
            KErrNone,
            KNullContactId ,
            aSender,
            KMessageType,
            aText );
        }
    }

// -----------------------------------------------------------------------------
// CImClient::HandleSendCompleteL
//
// -----------------------------------------------------------------------------
//
void CImClient::HandleSendCompleteL(
    TInt  aOpId,
    TBool /*aDeliveryReportOrdered*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: HandleSendCompleteL" ) );
#endif


    RemoveOpId( aOpId );

    if ( iClientObserver )
        {
        iClientObserver->HandleMessageSentL(
            aOpId,
            KErrNone );
        }
    }




// -----------------------------------------------------------------------------
// CImClient::HandleNewContentMessageL
//
// -----------------------------------------------------------------------------
//
void CImClient::HandleNewContentMessageL(
    TInt /*aOpId*/,
    const TDesC& /*aMessageId*/,
    const TDesC& /*aSender*/,
    const TDesC& /*aGroupId*/,
    const MDesCArray& /*aRecipients*/,
    const MDesCArray& /*aScreenNames*/,
    const TDesC& /*aContentType*/,
    const TDesC8& /*aContent*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CImClient::HandleDeliveryReportL
//
// -----------------------------------------------------------------------------
//
void CImClient::HandleDeliveryReportL(
    const TDesC& /*aMessageId*/,
    TInt /*aResult*/,
    const TDesC* /*aDescription*/,
    TImpsCspIdentifier& /*aCspId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CImClient::HandleErrorL
//
// -----------------------------------------------------------------------------
//
void CImClient::HandleErrorL( TInt                 aStatus,
                              TInt                 aOpId,
                              const TDesC*         /*aDescription*/,
                              const CImpsDetailed* aDetailedRes,
                              TImpsCspIdentifier&  /*aCspId*/ )
    {
#ifdef _DEBUG
    CImApiLogger::Log( _L( "CImClient: IM error received: %d" ), aStatus );
#endif

    CImClientDetailedErrorImpl* detailedResult = NULL;

    if ( aDetailedRes )
        {
        detailedResult = CImClientDetailedErrorImpl::NewL( aDetailedRes );
        CleanupStack::PushL( detailedResult ); // << detailedResult
        }

    if ( iClientObserver )
        {
        TInt err;
        TRAP( err, iClientObserver->HandleSendErrorL(
                  aOpId,
                  iImConnection.ConvertImpsEngineErrorCode( aStatus ),
                  detailedResult ) );
        }


    // Delete the OpID from the list
    RemoveOpId( aOpId );
    if ( detailedResult )
        {
        CleanupStack::PopAndDestroy(); // >> detailedResult
        }

    }

// -----------------------------------------------------------------------------
// CImClient::DoSendMessageL
//
// -----------------------------------------------------------------------------
//
TInt CImClient::DoSendMessageL( const MDesCArray&    aUserIds,
                                const TDesC&         aContent )
    {

    TInt opId( 0 );

    TRAPD( err, opId = iImClient.SendTextMessageL( NULL,
                                                   &aUserIds,
                                                   NULL,
                                                   NULL,
                                                   aContent,
                                                   EFalse /*No delivery report*/ ) );

    if ( err != KErrNone )
        {
        // Failed to send
        User::Leave ( KImApiGeneralError );
        }

    // Put the OpID to the Outstanding OpIds list
    iOutstandingOpIds.Append( opId );

    return opId;
    }

// -----------------------------------------------------------------------------
// CImClient::RemoveOpId
//
// -----------------------------------------------------------------------------
//
void CImClient::RemoveOpId( TInt aOpId )
    {
    TInt index = iOutstandingOpIds.Find( aOpId );

    // OpID is not found from the list there is a problem somewhere
    __ASSERT_DEBUG( ( index != KErrNotFound ),
                    User::Panic( KTxtImOpenApiPanic, EListCorrupted ) );

    iOutstandingOpIds.Remove( index );
    }


// -----------------------------------------------------------------------------
// CImClient::ImpsImHandler
//
// -----------------------------------------------------------------------------
//
MImpsImHandler2* CImClient::ImpsImHandler()
    {
    return this;
    }

// -----------------------------------------------------------------------------
// CImClient::ImpsErrorHandler
//
// -----------------------------------------------------------------------------
//
MImpsErrorHandler2& CImClient::ImpsErrorHandler()
    {
    return *this;
    }

// -----------------------------------------------------------------------------
// CImClient::ImpsEngine
//
// -----------------------------------------------------------------------------
//
RImpsEng& CImClient::ImpsEngine()
    {
    return iImConnection.ImpsEngine();
    }


// -----------------------------------------------------------------------------
// CImClient::ClientObserver
//
// -----------------------------------------------------------------------------
//
MImObserver* CImClient::ClientObserver()
    {
    return iClientObserver;
    }


//////////////////////////////////////////////////////////////////////////
// TDetailedResultItem

// -----------------------------------------------------------------------------
// TDetailedResultItem::TDetailedResultItem
//
// -----------------------------------------------------------------------------
//
TDetailedResultItem::TDetailedResultItem()
    {
    }

// -----------------------------------------------------------------------------
// TDetailedResultItem::TDetailedResultItem
//
// -----------------------------------------------------------------------------
//
TDetailedResultItem::~TDetailedResultItem()
    {
    }

// -----------------------------------------------------------------------------
// TDetailedResultItem::SetErrorCode
//
// -----------------------------------------------------------------------------
//
void TDetailedResultItem::SetErrorCode( TInt aErroCode )
    {
    iErrorCode = aErroCode;
    }

// -----------------------------------------------------------------------------
// TDetailedResultItem::ErrorCode
//
// -----------------------------------------------------------------------------
//
TInt TDetailedResultItem::ErrorCode() const
    {
    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// TDetailedResultItem::SetUserId
//
// -----------------------------------------------------------------------------
//
void TDetailedResultItem::SetUserId( TDesC& aUserId )
    {
    iUserId.Set( aUserId );
    }

// -----------------------------------------------------------------------------
// TDetailedResultItem::UserId
//
// -----------------------------------------------------------------------------
//
const TDesC& TDetailedResultItem::UserId() const
    {
    return iUserId;
    }


//////////////////////////////////////////////////////////////////////////
// CImClientDetailedErrorImpl
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::CImClientDetailedErrorImpl
//
// -----------------------------------------------------------------------------
//
CImClientDetailedErrorImpl::CImClientDetailedErrorImpl()
    {
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::CImClientDetailedErrorImpl
//
// -----------------------------------------------------------------------------
//
void CImClientDetailedErrorImpl::ConstructL(
    const CImpsDetailed* aImpsDetailed )
    {
    iDetailedList = new( ELeave ) CArrayFixFlat<TDetailedResultItem>(
        KDetailedListGranularity );
    DoInitL( aImpsDetailed );
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::CImClientDetailedErrorImpl
//
// -----------------------------------------------------------------------------
//
CImClientDetailedErrorImpl* CImClientDetailedErrorImpl::NewL(
    const CImpsDetailed* aImpsDetailed )
    {
    CImClientDetailedErrorImpl* self = new( ELeave ) CImClientDetailedErrorImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aImpsDetailed );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::~CImClientDetailedErrorImpl
//
// -----------------------------------------------------------------------------
//
CImClientDetailedErrorImpl::~CImClientDetailedErrorImpl()
    {
    delete iDetailedList;
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::Count
//
// -----------------------------------------------------------------------------
//
TInt CImClientDetailedErrorImpl::Count()
    {
    return iDetailedList->Count();
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::UserId
//
// -----------------------------------------------------------------------------
//
const TDesC& CImClientDetailedErrorImpl::UserId( TInt aIndex )
    {
    // out-of-bounds will panic: E32USER-CBase 21
    return iDetailedList->At( aIndex ).UserId();
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::ErrorCode
//
// -----------------------------------------------------------------------------
//
TInt CImClientDetailedErrorImpl::ErrorCode( TInt aIndex )
    {
    // out-of-bounds will panic: E32USER-CBase 21
    return iDetailedList->At( aIndex ).ErrorCode();
    }

// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::DoInitL
// Initializes the detailed list from the ImpsDetailed
// -----------------------------------------------------------------------------
//
void CImClientDetailedErrorImpl::DoInitL( const CImpsDetailed* aImpsDetailed )
    {
    TInt detailedCount = aImpsDetailed->Count();
    for ( TInt i( 0 ); i < detailedCount; ++i )
        {
        // Get all the failed user IDs
        // and make a list of those IDs
        CPtrC16Array* userIds = aImpsDetailed->At( i )->UserIds();
        if ( userIds )
            {
            TInt userCount = userIds->Count();
            for ( TInt j( 0 ); j < userCount; ++j )
                {
                TDetailedResultItem resultItem;
                resultItem.SetUserId( userIds->At( j ) );
                resultItem.SetErrorCode( ConvertWVErrorCode(
                                             aImpsDetailed->At( i )->Code() ) );
                iDetailedList->AppendL( resultItem );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CImClientDetailedErrorImpl::ConvertWVErrorCode
// Converts the WV error code to an API error code
// -----------------------------------------------------------------------------
//
TInt CImClientDetailedErrorImpl::ConvertWVErrorCode( TInt aErrorCode )
    {
    // Do not try to convert KErrNone
    if ( aErrorCode == KErrNone )
        {
        return KErrNone;
        }

    TInt returnValue;
    switch ( aErrorCode )
        {
        case 531: // Recipient does not exist
            returnValue = KImApiErrInvalidUserId;
            break;
        case 532: //Recipient user blocked the sender
            returnValue = KImApiErrSenderBlocked;
            break;

        case 533: // Recipient user is not logged in
            returnValue = KImApiErrRecipientNotLogged;
            break;

        default:
            returnValue = KImApiGeneralError;
            break;

        }
    return returnValue;
    }
//  End of File
