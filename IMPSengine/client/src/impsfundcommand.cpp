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
* fundemental commander class.
*
*/


// INCLUDE FILES
#include "impsfundcli.h"
#include "impsfundCommand.h"
#include "impsutils.h"
#include "impsdatautils.h"
#include "ImpsPacked.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsFundCommand2::CImpsFundCommand2
// ----------------------------------------------------------------------------
CImpsFundCommand2::CImpsFundCommand2( TInt aPriority, RImpsFundClient2& aClient  ):
                                    CActive( aPriority ),
                                    iClient( aClient ),
                                    iMessagePtr()
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::NewL
// ----------------------------------------------------------------------------
CImpsFundCommand2* CImpsFundCommand2::NewL( RImpsFundClient2& aClient )
     {
     CImpsFundCommand2* self = new (ELeave) CImpsFundCommand2( 
                                    EPriorityUserInput, aClient);
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop();
     return self;
     }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::ConstructL
// ----------------------------------------------------------------------------
void CImpsFundCommand2::ConstructL()
     {
     iKey = CImpsKey::NewL();
     iImpsFields = CImpsFields::NewL();
     iDataAccessor = CImpsDataAccessor::NewL( iImpsFields );
     }
 
// ----------------------------------------------------------------------------
// CImpsFundCommand2::~CImpsFundCommand2
// ----------------------------------------------------------------------------
CImpsFundCommand2::~CImpsFundCommand2()
     {
     //Cancel any outstanding requests
     Cancel();
     ResetMembers();
     delete iKey;
     delete iImpsFields;
     delete iDataAccessor;
     }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::StartRunL
// ----------------------------------------------------------------------------
void CImpsFundCommand2::StartRunL()
    {
    iState = 0;
    if ( IsActive() )
        {
        User::Leave( KErrServerBusy );
        }
    // Set this active object active
    iStatus = KRequestPending;
    SetActive();
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::RunL
// ----------------------------------------------------------------------------
void CImpsFundCommand2::RunL()
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundCommand2: RunL %d" ), iStatus.Int() );
    #endif
    MImpsErrorHandler2* ehandler = iClient.ErrorHandler();
    // Errors are handled here. 
    // Successful cases are handled by CImpsFundHandler.
    if ( iStatus == KErrNone )
        {
        return;
        }
    // Handler should be registered
    if ( ehandler )
        {
        TInt err( KErrNone );
        TRAP( err, ehandler->HandleErrorL( iStatus.Int(), iOpId, NULL, NULL, *iClient.CspIdentifier() ));
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::DoCancel
// ----------------------------------------------------------------------------

void CImpsFundCommand2::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::InitialiseL
// ----------------------------------------------------------------------------
void CImpsFundCommand2::InitialiseL( const TImpsContent* aElementArray,
                                    const TUint aArraySize )
    {
    ResetMembers();
    TImpsDataUtils::AddValuesFromArrayL( iKey, aElementArray, aArraySize );
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::SetUserIDsL
// There is no count check here, since client does not
// call this function in case the array is empty.
// ----------------------------------------------------------------------------                                          
void CImpsFundCommand2::SetUserIDsL( const MDesCArray* aUserIDs )
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundCommand2::SetUserIDsL()" ) );
    #endif
    TUint count = aUserIDs->MdcaCount();
    for( TUint i = 0;i < count;i++ )
        {
        TPtrC userID = aUserIDs->MdcaPoint( i );
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( 
                _L( "    User ID no. %d: \"%S\"" ), i, &userID );
        #endif
        iKey->AddL( CREATEKEY( EImpsKeyUser, i ) );
        iKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        iDataAccessor->StoreDescL( iKey, userID );
        iKey->PopL( 2 );
        }
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::MakeSearchPairListL
// -----------------------------------------------------------------------------                                           
void CImpsFundCommand2::MakeSearchPairListL( const CSearchPairs& aSearchPairs )
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundCommand2::MakeSearchPairListL" ) );
    #endif
    TInt index = 0;
    TInt count = aSearchPairs.Count();
    CImpsSearchRequest* request = NULL;
    for( TInt i = 0;i < count;i++ )
        {
        request = aSearchPairs.At( i );
        TPtrC value = request->Value();
        if( value.Length() != 0 )
            {
            #ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log( 
                    _L( "  Value of the search pair no. %d: \"%S\"" ), i, &value );
            #endif
            //Must use a separate index variable in order
            //no to mix up the sequence in the iKey member.
            iKey->AddL( CREATEKEY( EImpsKeySearchPairList, index ) );
            iKey->AddL( CREATEKEY( EImpsKeySearchElement, 0 ) );    
            iDataAccessor->StoreIntegerL( iKey, request->Element() );
            iKey->PopL();  //search element
            iKey->AddL( CREATEKEY( EImpsKeySearchString, 0 ) ); 
            iDataAccessor->StoreDescL( iKey, request->Value() );
            iKey->PopL( 2 );
            index++;
            }
        #ifndef _NO_IMPS_LOGGING_
            else
                {
                CImpsClientLogger::Log( _L( "  Value no. %d is empty!" ), i );
                }
        #endif
        }
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::InsertEmptyElementL
// -----------------------------------------------------------------------------            
void CImpsFundCommand2::InsertEmptyElementL( const TImpsContent aElementName,
                                            const TInt aIndex )
    {
    iKey->AddL( CREATEKEY( aElementName, aIndex ) );
    iDataAccessor->StoreEmptyL( iKey );
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::SetScreenNamesL
// There is no count check here, since client does not
// call this function in case the arrays are empty.
// -----------------------------------------------------------------------------                                            
void CImpsFundCommand2::SetScreenNamesL( const MDesCArray* aScreenNames,
                                        const MDesCArray* aGroupNames )
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundCommand2::SetScreenNamesL()" ) );
    #endif
    TUint count = aScreenNames->MdcaCount();
    for( TUint i = 0;i < count;i++ )
        {
        iKey->AddL( CREATEKEY( EImpsKeyScreenName, i ) );
        iKey->AddL( CREATEKEY( EImpsKeySName, 0 ) );
        //Store the SName
        TPtrC SName = aScreenNames->MdcaPoint( i );
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "    SName no. %d: \"%S\"" ), i, &SName );
        #endif
        iDataAccessor->StoreDescL( iKey, SName );
        iKey->PopL();
        //Store the GroupID
        iKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        #ifndef _NO_IMPS_LOGGING_
            TPtrC groupID = aGroupNames->MdcaPoint( i );
            CImpsClientLogger::Log( 
                _L( "    GroupID no. %d: \"%S\"" ), i, &groupID );
        #endif
        iDataAccessor->StoreDescL( iKey, aGroupNames->MdcaPoint( i ) );
        iKey->PopL();
        }
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::InsertDescriptorElementL
// -----------------------------------------------------------------------------                                           
void CImpsFundCommand2::InsertDescriptorElementL( const TDesC& aElementValue,
                                                 const TImpsContent aElementName,
                                                 const TInt aIndex )
    {
    iKey->AddL( CREATEKEY( aElementName, aIndex ) );
    iDataAccessor->StoreDescL( iKey, aElementValue );
    iKey->PopL();
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::InsertBooleanElementL
// -----------------------------------------------------------------------------                                              
void CImpsFundCommand2::InsertBooleanElementL( const TBool aElementValue, 
                                              const TImpsContent aElementName,
                                              const TInt aIndex )
    {
    iKey->AddL( CREATEKEY( aElementName, aIndex ) );
    iDataAccessor->StoreBooleanL( iKey, aElementValue );
    iKey->PopL();
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::InsertIntegerElementL
// -----------------------------------------------------------------------------                                            
void CImpsFundCommand2::InsertIntegerElementL( const TInt aElementValue,
                                              const TImpsContent aElementName,
                                              const TInt aIndex )
    {
    iKey->AddL( CREATEKEY( aElementName, aIndex ) );
    iDataAccessor->StoreIntegerL( iKey, aElementValue );
    iKey->PopL();
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::ResetMembers
// -----------------------------------------------------------------------------                                             
void CImpsFundCommand2::ResetMembers()
    {
    iArraySize = 0;
    delete iMessage;
    iMessage = NULL;
    iMessagePtr.Set( KNullDesC8 );
    if (iKey != NULL)
        {
        iKey->Reset();
        }
    if (iImpsFields != NULL)
        {
        iImpsFields->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CImpsFundCommand2::PackAndSendL
// -----------------------------------------------------------------------------                                         
void CImpsFundCommand2::PackAndSendL( const TImpsMessageType aMessageType,
                                     const TInt aOpId )
    {
    iOpId = aOpId;
    delete iMessage;
    iMessage = NULL;
    HBufC8* buffer = HBufC8::NewLC( iImpsFields->Size() );  // << buffer
    iImpsFields->SetMessageType( aMessageType  );
    TImpsPackedEntity packedMessage( buffer );
    TInt error = packedMessage.PackEntity( *iImpsFields );
    if( error == KErrNone )
        {
        TImpsServRequest req = ServerRequestTypeL( aMessageType );
        StartRunL();
        iMessage = buffer;
        CleanupStack::Pop();  // >> buffer
        iMessagePtr.Set( iMessage->Des() );
        iClient.SendReceive( req, TIpcArgs( &iMessagePtr, iOpId ), iStatus );
        }
    else
        {
        iImpsFields->Reset();
        User::Leave( error );   // >> buffer deleted from CleanupStack
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundCommand2::ServerRequestTypeL
// ----------------------------------------------------------------------------      
TImpsServRequest CImpsFundCommand2::ServerRequestTypeL( 
                                        const TImpsMessageType aMessageType )
    {
    TImpsServRequest ret = EImpsServNone;
    switch( aMessageType )
        {
        case EImpsSearchReq: 
        case EImpsStopSearchReq:                
        case EImpsSearchRes:
            ret = EImpsServFundSearch;
            break;
        case EImpsInviteReq:                
        case EImpsInviteRes:               
        case EImpsInviteUserReq:                
        case EImpsInviteUserRes:               
        case EImpsCancelInviteReq:             
        case EImpsCancelInviteUserReq:
            ret = EImpsServFundInvite;
            break;
        default:
            User::Leave( ret );
        }
    return ret;
    }
    
    
//  End of File  


