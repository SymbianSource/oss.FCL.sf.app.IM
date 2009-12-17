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
* handler class for imps fundemental feature.
*
*/


// INCLUDE FILES
#include "impsfundcli.h"
#include "impsfundhandler.h"
#include "impsutils.h"
#include "impscdatautils.h"
#include "impserrors.h"
#include "impscommonenums.h"

// MACROS
#ifndef _DEBUG
#define _NO_IMPS_LOGGING_
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImpsFundHandler2::CImpsFundHandler2
// ----------------------------------------------------------------------------
CImpsFundHandler2::CImpsFundHandler2( TInt aPriority, RImpsFundClient2& aClient ):
    // The base class adds this to the scheduler
    CImpsHandler2( aPriority, aClient), iClient( aClient )
    {
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::NewL
// ----------------------------------------------------------------------------
CImpsFundHandler2* CImpsFundHandler2::NewL( RImpsFundClient2& aClient,
                                          TInt aPriority )
     {
     CImpsFundHandler2* self = new (ELeave) CImpsFundHandler2( aPriority , 
                                                             aClient);
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop( self );
     return self;
     }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::~CImpsFundHandler2
// ----------------------------------------------------------------------------
CImpsFundHandler2::~CImpsFundHandler2()
     {
     delete iDataAccessor;
     delete iKey;
     }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleEventL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleEventL( CImpsFields* aErrorFields )
    {
    // get the event data from server thread.
    SImpsEventData* event = (SImpsEventData*)iEventData.Ptr();
#ifndef _NO_IMPS_LOGGING_
     CImpsClientLogger::Log(_L("CImpsFundHandler2::HandleEventL opid=%d cli=%d"), 
         event->iOpCode, (TInt)&iClient );
#endif
    if ( event->iStatus != ( Imps_ERROR_BASE - 200 ) &&
             event->iStatus != KErrNone )
        {
        HandleErrorEventL( event->iOpCode, event->iStatus, aErrorFields );
        }
        // Call regular observer
        // iStatus is error code or message size
     else if ( iStatus >= 0 )
        {
        HandleFundamentalEventL( ); 
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleFundamentalEventL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleFundamentalEventL( )
    {
    // get the event data from server thread.

    MImpsSearchHandler2* searchhandler = NULL; 
    SImpsEventData* event = ( SImpsEventData* )iEventData.Ptr();
    TInt err( KErrNone );    
    //This must be a "response" to search stop
    if( event->iMessageType == EImpsStatus &&
        event->iRequestType == EImpsServFundSearch )
        {
        searchhandler = iClient.SearchHandler();
        //Notice: Status message does not include a search ID, which is the 
        //third parameter in HandleSearchStoppedL() method. In future, it
        //might be a good idea to expand the event structure so that a
        //response could always be mapped to the correct request regardless
        //of the message type. For now, just send a meaningless magic number 0.
        if( searchhandler != NULL )
            {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsFundHandler2:: calls HandleSearchStoppedL opid=%d"), 
                event->iOpCode );
#endif
            TRAP( err, searchhandler->HandleSearchStoppedL( event->iOpCode, 
                                          /*event->iStatus,*/ KErrNone, *iClient.CspIdentifier() ));
            }
        else
            {
            #ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log(_L("Search handler not instantiated!"));
            #endif
            return;
            }
        }
    else
        {
        MImpsInviteHandler2* invitehandler = iClient.InviteHandler();
        switch( event->iMessageType )
            {
            case EImpsSearchRes:
                HandleSearchResponseL( event->iOpCode );
                break;
            case EImpsInviteUserReq:
                HandleInviteRequestL();
                break;
            case EImpsInviteRes:
                HandleInviteResponseL();
                break;
            case EImpsCancelInviteUserReq:
                HandleInviteCancelL();
                break;
            default:
                if( invitehandler != NULL)
                {
#ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "CImpsFundHandler2:: calls HandleCompleteL opid=%d"), 
                event->iOpCode );
#endif
                    TRAP( err, invitehandler->HandleCompleteL( event->iOpCode, /*event->iStatus,*/ *iClient.CspIdentifier() ));
                }
                else
                    {
                    #ifndef _NO_IMPS_LOGGING_
                        CImpsClientLogger::Log
                            ( _L( "Invite handler not instantiated!" ) );
                    #endif
                    return;
                    }
                break;
            }
        iKey->Reset();
        }
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundHandler2::HandleFundamentalEventL ends" ) );
    #endif
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleSearchResponseL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleSearchResponseL( const TInt aOpId )
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log
                ( _L( "CImpsFundHandler2::HandleSearchResponseL()" ) );
    #endif
    MImpsSearchHandler2* search = iClient.SearchHandler();
    if( search != NULL )
        {
        TImpsSearchResultType resultType = EImpsUserResult;
        const TImpsContent* content = KSearchResponseElements;
        TImpsDataUtils::AddValuesFromArrayL( iKey, content,
                                        sizeof( KSearchResponseElements ) /
                                        sizeof( KSearchResponseElements[0] ) );
        TInt searchId = IntegerElementL( EImpsKeySearchID, 0 ); //search ID
        TInt index = IntegerElementL( EImpsKeySearchIndex, 0 );
        TInt results = IntegerElementL( EImpsKeySearchFindings, 0 );
        TBool complete = BooleanElementL( EImpsKeyCompletionFlag, 0 );
        CPtrC16Array* resultArray = results > 0 ? SearchResultsLC( resultType ) : NULL;

       	TInt err( KErrNone );
        TRAP( err, search->HandleSearchL( aOpId, 
                               searchId, index,
                               results, complete,
                               resultType, resultArray, *iClient.CspIdentifier() ));
        if( resultArray != NULL )
            CleanupStack::PopAndDestroy();  //resultArray
        }
    else
        {
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( " Search handler not instantiated!" ) );
        #endif
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleInviteRequestL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleInviteRequestL()
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log
            ( _L( "CImpsFundHandler2::HandleInviteRequestL()" ) );
    #endif
    MImpsInviteHandler2* invite = iClient.InviteHandler();
    if( invite != NULL )
        {
		TInt err( KErrNone );

        const TImpsContent* content = KInviteUserRequestElements;
        TImpsDataUtils::AddValuesFromArrayL( iKey, content,
                                    sizeof( KInviteUserRequestElements ) /
                                    sizeof( KInviteUserRequestElements[0] ) );
      
        TInt inviteType = IntegerElementL( EImpsKeyInviteType, 0 );
        TPtrC inviteID = DescriptorElementL( EImpsKeyInviteID, 0 );
        TPtrC reason = DescriptorElementL( EImpsKeyInviteNote, 0 );
        TInt validity = IntegerElementL( EImpsKeyValidity, 0 );
        TPtrC groupName;
        groupName.Set( DescriptorElementL( EImpsKeyGroupID, 0 ) );//gr   
            
        iKey->AddL( CREATEKEY( EImpsKeySender, 0 ) );           
        iKey->AddL( CREATEKEY( EImpsKeyUser, 0 ) );            
        TPtrC userID = DescriptorElementL( EImpsKeyUserID, 0 );
        if ( inviteType == EImpsGR )
            {
            TPtrC sName;
            if( userID.Length() == 0 )
                {
                iKey->PopL();                                       //user
                iKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );        // group
                iKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );   // ScreenName //gr
                sName.Set( DescriptorElementL( EImpsKeySName, 0 ) );
                groupName.Set( DescriptorElementL( EImpsKeyGroupID, 0 ) );
                }
	        TRAP( err, invite->HandleGroupInviteL( inviteID, userID, sName, groupName,
                                    reason, validity, 
	                                    *iClient.CspIdentifier() ) );  
        }
            else if ( inviteType == EImpsIM )
                {
                invite->HandleImInviteL( inviteID, userID, reason, 
                                         validity, *iClient.CspIdentifier());
                }
            else if ( inviteType == EImpsSC )
                {
                CDesCArrayFlat* urlList = new ( ELeave ) CDesCArrayFlat( 4 );
                CleanupStack::PushL( urlList );
                TImpsCDataUtils::GetUrlListL( iDataAccessor, urlList );
                TRAP( err, invite->HandleContentInviteL( inviteID, userID, urlList,
                                          reason, validity, *iClient.CspIdentifier()));
                CleanupStack::PopAndDestroy();
                }
            else
                {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsFundHandler2: ERROR Unknown invite type %d" ), inviteType );
#endif
                User::Leave( KErrCorrupt );
                }
        

        }
    else
        {
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Invite handler not instantiated!" ) );
        #endif
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleInviteResponseL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleInviteResponseL()
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log
            ( _L( "CImpsFundHandler2::HandleInviteResponseL()" ) );
    #endif
    MImpsInviteHandler2* invite = iClient.InviteHandler();
    if( invite != NULL )
        {
        TPtrC sName;
        TPtrC groupName;
        const TImpsContent* content = KInviteResponseElements;
        TImpsDataUtils::AddValuesFromArrayL( iKey, content,
                                        sizeof( KInviteResponseElements ) /
                                        sizeof( KInviteResponseElements[0] ) );
        TPtrC inviteID = DescriptorElementL( EImpsKeyInviteID, 0 );
        TBool accept = BooleanElementL( EImpsKeyAcceptance, 0 );
        TPtrC response = DescriptorElementL( EImpsKeyResponseNote, 0 );
        iKey->AddL( CREATEKEY( EImpsKeySender, 0 ) );           // sender
        iKey->AddL( CREATEKEY( EImpsKeyUser, 0 ) );             // user
        TPtrC userID = DescriptorElementL( EImpsKeyUserID, 0 );
        if( userID.Length() == 0 )
            {
            iKey->PopL();                                      //user
            iKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );       // group
            iKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );  //screenName
            sName.Set( DescriptorElementL( EImpsKeySName, 0 ) );
            groupName.Set( DescriptorElementL( EImpsKeyGroupID, 0 ) );
            } 
        TInt err( KErrNone );
        TRAP( err, invite->HandleInviteResponseL( inviteID, accept, userID, 
                                       sName, groupName, response, *iClient.CspIdentifier() ));
        }
    else
        {
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Invite handler not instantiated!" ) );
        #endif
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::HandleInviteCancelL
// ----------------------------------------------------------------------------
void CImpsFundHandler2::HandleInviteCancelL()
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log
            ( _L( "CImpsFundHandler2::HandleInviteCancelL()" ) );
    #endif
    MImpsInviteHandler2* invite = iClient.InviteHandler();
    if( invite != NULL )
        {
        TPtrC sName;
        TPtrC groupName;
        const TImpsContent* content = KInviteCancelUserRequestElements;
        TImpsDataUtils::AddValuesFromArrayL( iKey, content,
                                sizeof( KInviteCancelUserRequestElements ) /
                                sizeof( KInviteCancelUserRequestElements[0] ) );
        TPtrC inviteID = DescriptorElementL( EImpsKeyInviteID, 0 );
        TPtrC response = DescriptorElementL( EImpsKeyInviteNote, 0 );
        iKey->AddL( CREATEKEY( EImpsKeySender, 0 ) );           // sender 
        iKey->AddL( CREATEKEY( EImpsKeyUser, 0 ) );             // user
        TPtrC userID = DescriptorElementL( EImpsKeyUserID, 0 );
        if( userID.Length() == 0 )
            {
            iKey->PopL();                                      //user
            iKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );       //group
            iKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );  //screenName
            sName.Set( DescriptorElementL( EImpsKeyGroupID, 0 ) );
            //screenName, group
            groupName.Set( DescriptorElementL( EImpsKeySName, 0 ) );    
            }
        TInt err( KErrNone );    
        TRAP( err, invite->HandleInviteCancelL( inviteID, userID, sName, 
                                     groupName, response,
                                     *iClient.CspIdentifier() ));  
        }
    else
        {
        #ifndef _NO_IMPS_LOGGING_
            CImpsClientLogger::Log( _L( "Invite handler not instantiated!" ) );
        #endif
        }
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::SearchResultsLC
// ----------------------------------------------------------------------------
CPtrC16Array* CImpsFundHandler2::SearchResultsLC( 
                                        TImpsSearchResultType& aResultType )
    {
    #ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "   CImpsFundHandler2::SearchResultsLC()" ) );
    #endif
    TBool moreResults = ETrue;
    const TInt KArraySize = 5;
    CPtrC16Array* results = new ( ELeave ) CPtrC16Array( KArraySize );
    CleanupStack::PushL( results );
    iKey->AddL( CREATEKEY( EImpsKeySearchResult, 0 ) );     //*** search result
    iKey->AddL( CREATEKEY( EImpsKeyUserList, 0 ) );         //*** user list
    if( iDataAccessor->CheckBranchExistenceL( iKey ) )
        {
        aResultType = EImpsUserResult;
        for( TInt i = 0;moreResults;i++ )
            {
            iKey->AddL( CREATEKEY( EImpsKeyUser, i ) );     //*** user
            TPtrC userId = DescriptorElementL( EImpsKeyUserID, 0 );
            if( userId.Length() > 0 )
                {
                results->AppendL( userId );
                #ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "        Result no. %d: \"%S\"" ), i, &userId );
                #endif
                }
            else
                {
                moreResults = EFalse;
                }
            iKey->PopL();                                   //user
            }
        iKey->PopL( 2 );                                    //user list, search result
        }
    else
        {
        aResultType = EImpsGroupResult;
        iKey->PopL();                                       //user list
        iKey->AddL( CREATEKEY( EImpsKeyGroupList, 0 ) );    //*** group list
        for( TInt i(0); moreResults; ++i )
            {
            TPtrC groupId = DescriptorElementL( EImpsKeyGroupID, i );
            if( groupId.Length() > 0 )
                {
                results->AppendL( groupId );
                #ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log( _L( "        Result no. %d: \"%S\"" ), i, &groupId );
                #endif
                }
            else
                {
                moreResults = EFalse;
                }
            }
        iKey->PopL( 2 );                                    //group list, search result
        }
    return results;
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::DescriptorElementL
// ----------------------------------------------------------------------------
TPtrC CImpsFundHandler2::DescriptorElementL( const TImpsContent aContent,
                                            const TInt aIndex )
    {
    TDesC* ret = NULL;
    iKey->AddL( CREATEKEY( aContent, aIndex ) );
    iDataAccessor->RestoreDescL( iKey, ret );
    iKey->PopL();
    return ret != NULL ? TPtrC( *ret ) : TPtrC();
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::BooleanElementL
// ----------------------------------------------------------------------------
TBool CImpsFundHandler2::BooleanElementL( const TImpsContent aContent,
                                         const TInt aIndex )
    {
    TBool ret;
    iKey->AddL( CREATEKEY( aContent, aIndex ) );
    iDataAccessor->RestoreBooleanL( iKey, ret );
    iKey->PopL();
    return ret;
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::IntegerElementL
// ----------------------------------------------------------------------------
TInt CImpsFundHandler2::IntegerElementL( const TImpsContent aContent,
                                        const TInt aIndex )
    {
    TInt ret = 0;
    iKey->AddL( CREATEKEY( aContent, aIndex ) );
    iDataAccessor->RestoreIntegerL( iKey, ret );
    iKey->PopL();
    return ret;
    }

// ----------------------------------------------------------------------------
// CImpsFundHandler2::ConstructL
// ----------------------------------------------------------------------------

void CImpsFundHandler2::ConstructL()
    {
    CImpsHandler2::ConstructL( );
    iDataAccessor = CImpsDataAccessor::NewL( iFields );
    iKey = CImpsKey::NewL();
    }

//  End of File  


