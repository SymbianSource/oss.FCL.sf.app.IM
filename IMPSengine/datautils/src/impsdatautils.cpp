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
* Description: Util classes for imps data accessor
* 
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <centralrepository.h>
#include    <imcvcodc.h>
#include	<miutconv.h>
#include    "impskey.h"
#include    "impsdatautils.h"
#include    "impsdataaccessor.h"
#include    "impsliterals.h"
#include    "wvengineinternalcrkeys.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TImpsDataUtils::TImpsDataUtils(
// -----------------------------------------------------------------------------
EXPORT_C TImpsDataUtils::TImpsDataUtils()
    {
    }
   
// -----------------------------------------------------------------------------
// TImpsDataUtils::GetSenderL
// -----------------------------------------------------------------------------
EXPORT_C void TImpsDataUtils::GetSenderL(
            MImpsKey* aKey, 
            MImpsDataAccessor* aAc,
            TPtrC& aSender,      // Sender(User(UserID) or ScreenName if a GroupID
            TPtrC& aGroup)       // Sender(ScreenName(SName, GroupID))
    {

    aKey->AddL( CREATEKEY( EImpsKeySender, 0 ) );

    // Get User:UserID
    TDesC* myPtr;
    aKey->AddL( CREATEKEY( EImpsKeyUser, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
    if ( aAc->RestoreDescL( aKey, myPtr ) )
        {
        aSender.Set( *myPtr );
        aGroup.Set( KNullDesC );
        aKey->PopL( 2 );       // UserID, User
        }
    else
        {
        aKey->PopL( 2 );       // UserID, User
        aKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );    
        aKey->AddL( CREATEKEY( EImpsKeySName, 0 ) ); 
        if ( aAc->RestoreDescL( aKey, myPtr ) )        
            {
			if( myPtr )
                aSender.Set( *myPtr );
            else
                aSender.Set( KNullDesC );
            aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) ); 
            aAc->RestoreDescL( aKey, myPtr );
            if( myPtr )
                aGroup.Set( *myPtr );
            else
                aGroup.Set( KNullDesC );
            }
        aKey->PopL( 3 );
        }

    aKey->PopL();   // Sender
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetSenderL
// -----------------------------------------------------------------------------
EXPORT_C void TImpsDataUtils::SetSenderL(
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC* aSender,
            const TDesC* aGroupId )
    {
    aKey->AddL( CREATEKEY( EImpsKeySender, 0 ) );

    if ( aSender && !aGroupId )
        {
        // Pure UserID
        aKey->AddL( CREATEKEY( EImpsKeyUser, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        aAc->StoreDescL( aKey, *aSender );
        aKey->PopL( 2 );
        }
    else if ( aSender && aGroupId )
        {
        // Screen Name
        aKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeySName, 0 ) );
        aAc->StoreDescL( aKey, *aSender );
        aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        aAc->StoreDescL( aKey, *aGroupId );
        aKey->PopL( 3 );
        }
    else if ( !aSender && aGroupId )
        {
        // Sender is Group, well ... maybe it is possible in principle
        aKey->AddL( CREATEKEY( EImpsKeyGroup, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        aAc->StoreDescL( aKey, *aGroupId );
        aKey->PopL( 2 );
        }

    aKey->PopL();
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetContentDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetContentDataL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const TDesC& aContent )
    {
	if ( aContent.Length() > 0 )
		{		
    aKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
	    aAc->StoreDescL( aKey, aContent );
    aKey->PopL();
		}
	else
		{
		TPtrC p;
		p.Set( KNullDesC );
	    aKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
    	aAc->StoreDescL( aKey, p );
    	aKey->PopL();
		}
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetContentDataL
// version for 16-bit
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::GetContentDataL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    TPtrC& aContent )
    {
    TDesC* myPtr;
    aKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
    if ( aAc->RestoreDescL( aKey, myPtr ) )
        {
        aContent.Set( *myPtr );
        }
    else
        {
        aContent.Set( KNullDesC );
        }
    aKey->PopL();
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetResultStatusL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetResultStatusL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    TInt aCode )
    {
    aKey->Reset();  // OPa

    TImpsDataUtils::AddValuesFromArrayL( 
        aKey, 
        KSetResultCodeElements, 
        sizeof( KSetResultCodeElements ) /
        sizeof( KSetResultCodeElements[0] ) );
    aAc->StoreIntegerL( aKey, aCode );
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetResultStatusL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TImpsDataUtils::GetResultStatusL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    TInt& aCode )
    {

    aKey->Reset();  // OPa

    TImpsDataUtils::AddValuesFromArrayL( 
        aKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    TBool ret(EFalse);
    TInt enumi(0);
    TInt index(0);
    TImpsKeyType type;
    if( aAc->RestoreAlternativeL( aKey, enumi, index, type ) )
        {
        switch ( enumi )
            {
            case EImpsKeyStatus:
            case EImpsKeyLogin_Response:
            case EImpsKeyDisconnect:
            case EImpsKeyKeepAlive_Response:
            case EImpsKeyGetPresence_Response:
            case EImpsKeyListManage_Response:
            case EImpsKeyGetAttributeList_Response:
            case EImpsKeySendMessage_Response:
            case EImpsKeyDeliveryReport_Request:
            case EImpsKeyLeaveGroup_Response:
                aKey->AddL( enumi, type );
                aKey->AddL( EImpsKeyResult );
                aKey->AddL( EImpsKeyCode );
                if( aAc->RestoreIntegerL( aKey, aCode ) )
                    ret = ETrue;
                break;
            default:
              break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetNewMessageIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TImpsDataUtils::GetNewMessageIdL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            TPtrC& aId )
    {

    aKey->Reset();

    TImpsDataUtils::AddValuesFromArrayL( 
        aKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    aKey->AddL( CREATEKEY( EImpsKeyNewMessage, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyMessageID, 0 ) );

    TDesC* myPtr;
    if ( aAc->RestoreDescL( aKey, myPtr ) )
        {
        aId.Set( *myPtr );
        return ETrue;
        }
    else
        {
        aId.Set( KNullDesC );
        }

    return EFalse;

    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::CreateClientIdL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::CreateClientIdL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const TDesC& aClientId )
    {
    aKey->AddL( CREATEKEY( EImpsKeyClientID, 0 ) );
    // '+' prefix or all numbers means MSISDN, other is URL

    TInt len = aClientId.Length( );
    TInt i(0);
    TBool isMSISDN( ETrue );
    for( i = 0; i < len; i++)
        {
        TChar cha = aClientId[i];
        if( !i && !( cha == '+' || cha.IsDigit( ) ) )
            {
            isMSISDN = EFalse;
            break;
            }
        if( i && !cha.IsDigit( ) )
            {
            isMSISDN = EFalse;
            break;
            }
        }
    if ( isMSISDN )
        {
        aKey->AddL( CREATEKEY( EImpsKeyMSISDN, 0 ) );
        }
    else
        {
        aKey->AddL( CREATEKEY( EImpsKeyURL, 0 ) );
        }
    aAc->StoreDescL( aKey, aClientId );
    aKey->PopL( 2 );
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetMessageTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TImpsDataUtils::GetMessageTypeL( MImpsDataAccessor* aAc )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );
    TInt enumi( 0 );
    TInt index( 0 );
    TImpsKeyType type( EImpsKeyTypeIM );
    if( aAc->RestoreAlternativeL( myKey, enumi, index, type ) )
        {
        CleanupStack::PopAndDestroy( );  //myKey
        return enumi;
        }

    CleanupStack::PopAndDestroy( );  //myKey
    return EImpsMessageNone;

    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetTransactionIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::GetTransactionIDL( 
    MImpsDataAccessor* aAc, TPtrC& aID  )
    {
    TDesC* myPtr;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionID, 0 ) );
    if( aAc->RestoreDescL( myKey, myPtr ) )
        aID.Set( *myPtr );
    else
        aID.Set( KNullDesC );
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetTransactionIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetTransactionIDL( 
    MImpsDataAccessor* aAc, const TDesC& aID  )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionID, 0 ) );
    aAc->StoreDescL( myKey, aID );
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetSessionIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::GetSessionIDL( MImpsDataAccessor* aAc, TPtrC& aID  )
    {
    TDesC* myPtr;
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeySessionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeySessionID, 0 ) );
    if( aAc->RestoreDescL( myKey, myPtr ) )
        aID.Set( *myPtr );
    else
        aID.Set( KNullDesC );
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetSessionIDL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetSessionIDL( MImpsDataAccessor* aAc, const TDesC& aID  )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeySessionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeySessionID, 0 ) );
    aAc->StoreDescL( myKey, aID );
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetSessionTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::GetSessionTypeL( 
    MImpsDataAccessor* aAc, TImpsSessionType& aID  )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeySessionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeySessionType, 0 ) );
    TInt id;
    if( aAc->RestoreIntegerL( myKey, id ) )
        aID = ( TImpsSessionType ) id;
    else
        aID = EImpsUndefSes;
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::SetSessionTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetSessionTypeL( 
    MImpsDataAccessor* aAc, const TImpsSessionType aType )
    {
    CImpsKey* myKey = CImpsKey::NewLC();
    myKey->AddL( CREATEKEY( EImpsKeySession, 0) );
    myKey->AddL( CREATEKEY( EImpsKeySessionDescriptor, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeySessionType, 0 ) );
    aAc->StoreIntegerL( myKey, aType );
    CleanupStack::PopAndDestroy( );  //myKey
    }

// -----------------------------------------------------------------------------
// TImpsDataUtils::GetPollL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TImpsDataUtils::GetPollL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    TBool& aBool )
    {
    CImpsFields* fields = aAc->GetImpsFields();
    if ( fields->CspVersion() == EImpsCspVersion11 )
        {
        TImpsDataUtils::AddValuesFromArrayL( 
        aKey, 
        KGetPollElements, 
        sizeof( KGetPollElements ) /
        sizeof( KGetPollElements[0] ) );
        }
    else
        {
        TImpsDataUtils::AddValuesFromArrayL( 
            aKey, 
            KGet12PollElements, 
            sizeof( KGet12PollElements ) /
            sizeof( KGet12PollElements[0] ) );
        }
    if( aAc->RestoreBooleanL( aKey, aBool ))
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// TImpsDataUtils::AddValuesFromArrayL
// Add values to a key straight from an array
// -----------------------------------------------------------------------------
//											    
EXPORT_C void TImpsDataUtils::AddValuesFromArrayL( 
    MImpsKey* aImpsKey, const TImpsContent* aElementArray, TInt aSize )
    {
    for( TInt i = 0; i < aSize; i++ )
        {
        aImpsKey->AddL( CREATEKEY( aElementArray[i], 0 ) );
        }
    }

// ---------------------------------------------------------
// TImpsDataUtils::CopyNewUsersL
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::CopyNewUsersL( 
            MImpsDataAccessor& aSource,
            MImpsDataAccessor& aDestination)
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
   
    myKey->AddL( CREATEKEY( EImpsKeyGroupChangeNotice, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyJoined, 0 ) );
        
    // copy joined users
    aDestination.CopyDataL(aSource, myKey);

    CleanupStack::PopAndDestroy( );           // >> myKey
    }
// ---------------------------------------------------------
// TImpsDataUtils::CopyLeftUsersL
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::CopyLeftUsersL( 
            MImpsDataAccessor& aSource,
            MImpsDataAccessor& aDestination)
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
   
    myKey->AddL( CREATEKEY( EImpsKeyGroupChangeNotice, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyLeft, 0 ) );
        
    // copy left users
    aDestination.CopyDataL(aSource, myKey);

    CleanupStack::PopAndDestroy( );           // >> myKey
    }
    
// ---------------------------------------------------------
// TImpsDataUtils::CopyGroupPropertiesL
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::CopyGroupPropertiesL( 
            MImpsDataAccessor& aSource,
            MImpsDataAccessor& aDestination)
    {
    CImpsKey* myKey = STATIC_CAST (CImpsKey*, aSource.NewKeyL()); 
    CleanupStack::PushL(myKey);         // << myKey

    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    TInt primitive(0);
    TInt index(0);
    TImpsKeyType type;
    if( aSource.RestoreAlternativeL( myKey, primitive, index, type ) )
        {
        myKey->AddL( CREATEKEY( primitive, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyGroupProperties, 0 ) );
        
        // copy left users
        aDestination.CopyDataL(aSource, myKey);

        CleanupStack::PopAndDestroy( );           // >> myKey
        }
    }
    
// ---------------------------------------------------------
// TImpsDataUtils::CopyOwnPropertiesL
// ---------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::CopyOwnPropertiesL( 
            MImpsDataAccessor& aSource,
            MImpsDataAccessor& aDestination)
    {
    CImpsKey* myKey = STATIC_CAST (CImpsKey*, aSource.NewKeyL()); 
    CleanupStack::PushL(myKey);         // << myKey

    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    TInt primitive(0);
    TInt index(0);
    TImpsKeyType type;
    if( aSource.RestoreAlternativeL( myKey, primitive, index, type ) )
        {
        myKey->AddL( CREATEKEY( primitive, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyOwnProperties, 0 ) );
        
        // copy left users
        aDestination.CopyDataL(aSource, myKey);

        CleanupStack::PopAndDestroy( );           // >> myKey
        }
    }

// ---------------------------------------------------------
// TImpsDataUtils::GetCenRepIntValueL
// ---------------------------------------------------------
//
EXPORT_C TInt TImpsDataUtils::GetCenRepIntValueL( 
                TUid aRepositoryUid,
                TUint32 aId )
    {
    TInt value( 0 );
    CRepository* repository = CRepository::NewLC( aRepositoryUid ); // << repository    
    User::LeaveIfError( repository->Get( aId, value ) );    
    CleanupStack::PopAndDestroy(); // >> repository
    return value;
    }
    
// ---------------------------------------------------------
// TImpsDataUtils::GetCenRepRealValueL
// ---------------------------------------------------------
//
EXPORT_C TReal TImpsDataUtils::GetCenRepRealValueL( 
                TUid aRepositoryUid,
                TUint32 aId )
    {
    TReal value( 0 );
    CRepository* repository = CRepository::NewLC( aRepositoryUid ); // << repository    
    User::LeaveIfError( repository->Get( aId, value ) );    
    CleanupStack::PopAndDestroy(); // >> repository
    return value;
    }
    
// -----------------------------------------------------------------------------
// TImpsDataUtils::SetBinaryContentDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::SetBinaryContentDataL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const TDesC8& aContent )
    {
    // first set the content encoding
    aKey->AddL( CREATEKEY( EImpsKeyContentEncoding, 0 ) );
    aAc->StoreIntegerL( aKey, EImpsBASE64 );
    aKey->PopL( 2 );
    // then manipulate the data    
    aKey->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
    if( aContent.Length() != 0 )
        {
        // BASE64 encoding
        TImCodecB64 codec;
        codec.Initialise();
        // Encoder doesn't itself cope with buffer overflows, 
        // so allocate twice big buffer to be sure that there is enough 
        // space for B64 data
        HBufC8* base64Buf = HBufC8::NewLC( aContent.Size() * 2 ); // << base64Buf
        TPtr8 base64Data( base64Buf->Des() );
        codec.Encode( aContent, base64Data );
        // finally, store the data
        aAc->StoreDesc8L( aKey, base64Data );
        CleanupStack::PopAndDestroy(1); // >> base64Buf
        }
    else
        { 
        TPtrC8 content( KNullDesC8 );
        aAc->StoreDesc8L( aKey, content );
        }
    aKey->PopL();
    }

    
// -----------------------------------------------------------------------------
// TImpsDataUtils::GetContentTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TImpsDataUtils::GetContentTypeL( 
    MImpsDataAccessor* aAc,
    TPtrC& aContentType )
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KNewMessageElements, 
        sizeof( KNewMessageElements ) /
        sizeof( KNewMessageElements[0] ) );
        
    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyContentType, 0 ) );
    
    TDesC* myPtr;
    if ( aAc->RestoreDescL( myKey, myPtr ) )
        {
        aContentType.Set( *myPtr );
        }
    else
        {
        aContentType.Set( KNullDesC );
        }
    CleanupStack::PopAndDestroy(); //  >>> myKey
    }  
    
    
// -----------------------------------------------------------------------------
// TImpsDataUtils::GetContentDataTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C TImpsDataType TImpsDataUtils::GetContentDataTypeL( 
            MImpsDataAccessor* aAc, TInt aIndex )
    {
    TImpsDataType dataType ( EImpsDataTypeDesc );
    
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    myKey->AddL( CREATEKEY( EImpsKeySession, 0 ) );
    // find the right transaction
    myKey->AddL( CREATEKEY( EImpsKeyTransaction, aIndex ) );
    myKey->AddL( CREATEKEY( EImpsKeyTransactionContent, 0 ) );

    // find the primitive
    TInt primitive(0);
    TInt index(0);
    TImpsKeyType keyType;
    if( aAc->RestoreAlternativeL( myKey, primitive, index, keyType ) )
        {
        switch ( primitive )
            {
            case EImpsKeyNewMessage:
                myKey->AddL( CREATEKEY( EImpsKeyNewMessage, 0 ) );
            break;
            case EImpsKeySendMessage_Request:
                myKey->AddL( CREATEKEY( EImpsKeySendMessage_Request, 0 ) );
            break;
            default:
            break;
            }
        
        myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
        myKey->AddL( CREATEKEY( EImpsKeyContentType, 0 ) );
    
        TDesC* myPtr;
        if ( aAc->RestoreDescL( myKey, myPtr ) )
            {
            if ( !myPtr->Left(KImpsContentType().Length()).CompareF( KImpsContentType ) || myPtr->Length() == 0 )
                {
                dataType = EImpsDataTypeDesc;
                }
            else
                {    
                dataType = EImpsDataTypeDesc8;         
                }
            }
        }
    CleanupStack::PopAndDestroy(); //  >>> myKey
    return dataType;
    }
            
            
               
//  End of File  
