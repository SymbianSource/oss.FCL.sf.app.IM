/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* imps data util class.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <miutconv.h>
#include    "impskey.h"
#include    "impscdatautils.h"
#include    "impsdataaccessor.h"
#include    "impsgrouppropsint.h"
#include    "impsservices.h"
#include    "impsliterals.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TImpsCDataUtils::TImpsCDataUtils()
    {
    }
   

// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetRecipientsL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetRecipientsL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    CPtrC16Array* aGroups,
    CPtrC16Array* aUsers,  
    CPtrC16Array* aSnGroups,      // ScreenName( GroupId )
    CPtrC16Array* aScreenNs )     // ScreenName( SName );
    {

    aKey->AddL( CREATEKEY( EImpsKeyRecipient, 0 ) );

    // Get User*:UserID
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index
    for ( i = 0; more; i++)
        {
        aKey->AddL( CREATEKEY( EImpsKeyUser, i ) );
        aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aKey, myPtr ) )
            {
            aUsers->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aKey->PopL( 2 );
        }

    // get Group*:GroupID
    more = ETrue;
    for ( i = 0; more; i++)
        {
        aKey->AddL( CREATEKEY( EImpsKeyGroup, i ) );
        aKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
        if ( aAc->RestoreDescL( aKey, myPtr ) )
            {
            aGroups->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aKey->PopL( 2 );
        }

    // Group*:ScreenName
    more = ETrue;
    for ( i = 0; more; i++)
        {
        aKey->AddL( CREATEKEY( EImpsKeyGroup, i ) );
        aKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );
        aKey->AddL( CREATEKEY( EImpsKeySName, 0 ) );
        if ( aAc->RestoreDescL( aKey, myPtr ) )
            {
            aScreenNs->AppendL( *myPtr );
            aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            if ( aAc->RestoreDescL( aKey, myPtr ) )
                {
                aSnGroups->AppendL( *myPtr );
                } // else error in data!
            }
        
            {
            more = EFalse;
            }
        aKey->PopL( 3 );
        }

    aKey->PopL();

    // notice: no contact list supported yet 

    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::SetRecipientsL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::SetRecipientsL(
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    const MDesC16Array* aGroups,
    const MDesC16Array* aUserIds,  
    const MDesC16Array* aSnGroups,         // ScreenName( GroupId )
    const MDesC16Array* aScreenNames )     // ScreenName( SName );
    {

    aKey->AddL( CREATEKEY( EImpsKeyRecipient, 0 ) );

    TInt i = 0;

  // User recipients
    if ( aUserIds )
        {
        TInt count = aUserIds->MdcaCount();


        for ( i = 0; i < count; i++ )
            {            
            aKey->AddL( CREATEKEY( EImpsKeyUser, i ) );
            aKey->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
            aAc->StoreDescL( aKey, aUserIds->MdcaPoint( i ) );
            aKey->PopL( 2 );
            }
        } // userids

    if ( aGroups )
        {
        TInt count = aGroups->MdcaCount();

        for ( i = 0; i < count; i++ )
            {            
            aKey->AddL( CREATEKEY( EImpsKeyGroup, i ) );
            aKey->AddL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            aAc->StoreDescL( aKey, aGroups->MdcaPoint( i ) );
            aKey->PopL( 2 );
            }          
        }  // groupid

    if ( aScreenNames )
        {
        TInt count = aScreenNames->MdcaCount();

        for ( i = 0; i < count; i++ )
            {            
            aKey->AddL( CREATEKEY( EImpsKeyGroup, i ) );
            aKey->AddL( CREATEKEY( EImpsKeyScreenName, 0 ) );
            aKey->AddL( CREATEKEY( EImpsKeySName, 0 ) );
            aAc->StoreDescL( aKey, aScreenNames->MdcaPoint( i ) );
            aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
            // myAccess->StoreDescL( aKey, *aGroupId );
            aAc->StoreDescL( aKey, aSnGroups->MdcaPoint( i ) );
            aKey->PopL( 3 );
            }
        } // screenNames

    aKey->PopL();   // EImpsKeyRecipient

    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::SetSenderL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::SetSenderL(
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

// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetUserListL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetUserListL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers,
            CDesCArray* aScreenNames )
    {
    // Get first the users
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyUser, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aUsers->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    // Then get the screennames
    more = ETrue;
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyScreenName, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeySName, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aScreenNames->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetUrlListL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetUrlListL(
            MImpsDataAccessor* aAc,
            CDesCArray* aUrlList )
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KInviteUserRequestElements, 
        sizeof( KInviteUserRequestElements ) /
        sizeof( KInviteUserRequestElements[0] ) );
        
    myKey->AddL( CREATEKEY( EImpsKeyURLList, 0 ) );
   
    // Get the urls
    TBool more( ETrue );
    TDesC* myPtr;
    TInt i = 0;         // aux index
    for ( i = 0; more; i++)
        {
        myKey->AddL( CREATEKEY( EImpsKeyURL, i ) );
        if ( aAc->RestoreDescL( myKey, myPtr ) )
            {
            aUrlList->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        myKey->PopL( );
        }
    CleanupStack::PopAndDestroy();
    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetModAndAdmL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetModAndAdmL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aModerators,
            CDesCArray* aAdmins )
    {
    // Get first the moderators
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index

    aPredecessor->AddL( CREATEKEY( EImpsKeyMod, 0 ) );
    aPredecessor->AddL( CREATEKEY( EImpsKeyUserList, 0 ) );
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyUser, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aModerators->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    aPredecessor->PopL( 2 );  // Pop out UserList,Pop out mod
    // Then get the admins
    aPredecessor->AddL( CREATEKEY( EImpsKeyAdmin, 0 ) );
    aPredecessor->AddL( CREATEKEY( EImpsKeyUserList, 0 ) );
    more = ETrue;
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyUser, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aAdmins->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    aPredecessor->PopL( 2 );  // Pop out UserList,out Admin
    }
 
// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetUsersL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetUsersL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers )
    {
    // Get first the moderators
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index

    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyUser, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aUsers->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetGroupPropertiesL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetGroupPropertiesL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CImpsCommonGroupPropsInt* aCommonP,
            CImpsPrivateGroupPropsInt* aPrivateP )
    {
    // Get first the common properties
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index

    aPredecessor->AddL( CREATEKEY( EImpsKeyGroupProperties, 0 ) );
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyProperty, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyName, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aPredecessor->PopL( ); // Pop out Name
            HBufC* newString = myPtr->AllocLC( );
            HBufC* mystring = newString;
            mystring->Des( ).UpperCase( );
            if( mystring->CompareF( KImpsName ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    aCommonP->SetGroupNameL( *myPtr );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsAccessType ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    HBufC* newString = myPtr->AllocLC( );
                    HBufC* mystring = newString;
                    mystring->Des( ).UpperCase( );
                    if( mystring->CompareF( KImpsOpen ) == 0 )
                        aCommonP->SetOpen( EImpsPropYes );
                    else
                        aCommonP->SetOpen( EImpsPropNo );
                    CleanupStack::PopAndDestroy(1);
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsPrivateMessaging ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    if( myPtr->CompareF( KImpsTrue ) == 0 )
                        aCommonP->SetPrivateAllowed( EImpsPropYes  );
                    else
                        aCommonP->SetPrivateAllowed( EImpsPropNo );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsType ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    HBufC* newString = myPtr->AllocLC( );
                    HBufC* mystring = newString;
                    mystring->Des( ).UpperCase( );
                    if( mystring->CompareF( KImpsPublic ) == 0 )
                        aCommonP->SetPublic( EImpsPropYes  );
                    else
                        aCommonP->SetPublic( EImpsPropNo );
                    CleanupStack::PopAndDestroy(1);
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsSearchable ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    if( myPtr->CompareF( KImpsTrue ) == 0 )
                        aCommonP->SetSearchable( EImpsPropYes );
                    else
                        aCommonP->SetSearchable( EImpsPropNo );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsTopic ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    aCommonP->SetTopicL( *myPtr );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsMaxActiveUsers ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    TLex lex;
                    TUint32 myValue = 0;

                    lex.Assign( *myPtr );
                    lex.Val(myValue, EDecimal, 100000);
                    aCommonP->SetMaxNbrOfUsers( myValue );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsNumberOfUsers ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    TLex lex;
                    TUint32 myValue = 0;

                    lex.Assign( *myPtr );
                    lex.Val(myValue, EDecimal, 100000);
                    aCommonP->SetNbrOfUsers( myValue );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            // AutoDelete
            if( mystring->CompareF( KImpsAutoDelete ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    if( myPtr->CompareF( KImpsTrue ) == 0 )
                        aCommonP->SetAutoDelete( EImpsPropYes );
                    else
                        aCommonP->SetAutoDelete( EImpsPropNo );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            CleanupStack::PopAndDestroy(1);
            }
        else
            {
            more = EFalse;
            aPredecessor->PopL( );  // PopOut Name
            }
        aPredecessor->PopL( ); // Popout EImpsKeyProperty
        }
    aPredecessor->AddL( CREATEKEY( EImpsKeyWelcomeNote, 0 ) );
    aPredecessor->AddL( CREATEKEY( EImpsKeyContentData, 0 ) );
    TDesC* myPtrWelcome;
    if( aAc->RestoreDescL( aPredecessor, myPtrWelcome ) )
        {
        aCommonP->SetWelcomeL( *myPtrWelcome );
        }
    aPredecessor->PopL( 3 ); // Popout EImpsKeyGroupProperties

    // Get own properties
    more = ETrue ;
    aPredecessor->AddL( CREATEKEY( EImpsKeyOwnProperties, 0 ) );
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyProperty, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyName, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aPredecessor->PopL( ); // Pop out Name
            HBufC* newString = myPtr->AllocLC( );
            HBufC* mystring = newString;
            mystring->Des( ).UpperCase( );
            if( mystring->CompareF( KImpsPrivateMessaging ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    if( myPtr->CompareF( KImpsTrue ) == 0 )
                        aPrivateP->SetPrivateAllowed( EImpsPropYes );
                    else
                        aPrivateP->SetPrivateAllowed( EImpsPropNo );      
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsIsMember ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    if( myPtr->CompareF( KImpsTrue ) == 0 )
                        aPrivateP->SetMember( EImpsPropYes );
                    else
                        aPrivateP->SetMember( EImpsPropNo );
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            if( mystring->CompareF( KImpsPriviledgeLevel ) == 0 )
                {
                aPredecessor->AddL( CREATEKEY( EImpsKeyValue, 0 ) );
                if( aAc->RestoreDescL( aPredecessor, myPtr ) )
                    {
                    HBufC* newString = myPtr->AllocLC( );
                    HBufC* mystring = newString;
                    mystring->Des( ).UpperCase( );
                    if( mystring->CompareF( KImpsUser ) == 0 )
                        aPrivateP->SetPrivileges( EImpsOrdinary );
                    if( mystring->CompareF( KImpsMod ) == 0 )
                        aPrivateP->SetPrivileges( EImpsModerator );
                    if( mystring->CompareF( KImpsAdmin ) == 0 )
                        aPrivateP->SetPrivileges( EImpsAdmin );
                    CleanupStack::PopAndDestroy(1);
                    }
                aPredecessor->PopL( ); // Pop out Value
                }
            CleanupStack::PopAndDestroy(1);
            }
        else
            {
            more = EFalse;
            aPredecessor->PopL( );  // PopOut Name
            }
        aPredecessor->PopL( );      // Popout EImpsKeyProperty
        }
    aPredecessor->PopL( );           // Popout EImpsKeyOwnProperties
    }


// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetBlockedListResponseL
// ----------------------------------------------------------------------------
//
void TImpsCDataUtils::GetBlockedListResponseL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            CPtrC16Array*                 blockedList,
            TBool&                        blockedInUse,
            CPtrC16Array*                 grantedList,
            TBool&                        grantedInUse
            )
    {
    aKey->Reset();
    blockedList->Reset();
    grantedList->Reset();
    blockedInUse = EFalse;
    grantedInUse = EFalse;
    
    TImpsCDataUtils::AddValuesFromArrayL( 
        aKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    aKey->AddL( CREATEKEY( EImpsKeyGetBlockedList_Response, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyBlockList, 0 ) );
    if ( aAc->CheckBranchExistenceL( aKey ) )
        {
        // CSP says this is mandatory, DTD not
        aKey->AddL( CREATEKEY( EImpsKeyInUse, 0 ) );  
        aAc->RestoreBooleanL( aKey, blockedInUse );
        aKey->ReplaceLastL( CREATEKEY( EImpsKeyEntityList, 0 ) );
        // Search only UserIDs here in this version!!!
        // Get User*:UserID
        TBool more(ETrue);
        TDesC* myPtr;
        TInt i = 0;         // aux index
        for ( i = 0; more; i++)
            {
            aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
            if ( aAc->RestoreDescL( aKey, myPtr ) )
                {
                blockedList->AppendL( *myPtr );
                }
            else
                {
                more = EFalse;
                }
            aKey->PopL();
            }
        aKey->PopL();
        }  // EImpsKeyBlockList
    aKey->ReplaceLastL( CREATEKEY( EImpsKeyGrantList, 0 ) );

    if ( aAc->CheckBranchExistenceL( aKey ) )
        {
        // CSP says this is mandatory, DTD not
        aKey->AddL( CREATEKEY( EImpsKeyInUse, 0 ) );  
        aAc->RestoreBooleanL( aKey, grantedInUse );
        aKey->ReplaceLastL( CREATEKEY( EImpsKeyEntityList, 0 ) );
        // Search only UserIDs here in this version!!!
        // Get User*:UserID
        TBool more(ETrue);
        TDesC* myPtr;
        TInt i = 0;         // aux index
        for ( i = 0; more; i++)
            {
            aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
            if ( aAc->RestoreDescL( aKey, myPtr ) )
                {
                grantedList->AppendL( *myPtr );
                }
            else
                {
                more = EFalse;
                }
            aKey->PopL();
            }
        aKey->PopL();
        }  // EImpsKeyGrantList


    }

// ----------------------------------------------------------------------------
// TImpsCDataUtils::SetBlockedListRequestL
// ----------------------------------------------------------------------------
//
void TImpsCDataUtils::SetBlockedListRequestL( 
            MImpsKey* aKey,
            MImpsDataAccessor* aAc,
            const MDesCArray* aBlockEntity,
            const MDesCArray* aUnBlockEntity,
            TBool aBlockedListInUse,
            const MDesCArray* aGrantedEntityList,
            const MDesCArray* aUnGrantEntity,
            TBool aGrantedListInUse
            )
    {

    TImpsCDataUtils::AddValuesFromArrayL( 
        aKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );
    aKey->AddL( CREATEKEY( EImpsKeyBlockEntity_Request, 0 ) );
    aKey->AddL( CREATEKEY( EImpsKeyBlockList, 0 ) );        // blocklist
    aKey->AddL( CREATEKEY( EImpsKeyInUse, 0 ) );            // inuse
    aAc->StoreBooleanL( aKey, aBlockedListInUse );
    aKey->ReplaceLastL( CREATEKEY( EImpsKeyAddList, 0 ) ); // block: addlist
    TInt i = 0;
    TInt count = 0;
    if ( aBlockEntity )
        {
        count = aBlockEntity->MdcaCount();
        }
    for ( i = 0; i < count; i++ )
        {
        aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
        aAc->StoreDescL( aKey, aBlockEntity->MdcaPoint( i ) );
        aKey->PopL();
        }
    // block: renovelist
    aKey->ReplaceLastL( CREATEKEY( EImpsKeyRemoveList, 0 ) ); 
    count = 0;
    if ( aUnBlockEntity )
        {
        count = aUnBlockEntity->MdcaCount();
        }
    for ( i = 0; i < count; i++ )
        {
        aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
        aAc->StoreDescL( aKey, aUnBlockEntity->MdcaPoint( i ) );
        aKey->PopL();
        }
    aKey->PopL();   // addlist
    aKey->ReplaceLastL( CREATEKEY( EImpsKeyGrantList, 0 ) );   // grantList
    aKey->AddL( CREATEKEY( EImpsKeyInUse, 0 ) ); 
    aAc->StoreBooleanL( aKey, aGrantedListInUse );
    aKey->ReplaceLastL( CREATEKEY( EImpsKeyAddList, 0 ) );    // grant:addlist
    count = 0;
    if ( aGrantedEntityList )
        {
        count = aGrantedEntityList->MdcaCount();
        }
    for ( i = 0; i < count; i++ )
        {
        aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
        aAc->StoreDescL( aKey, aGrantedEntityList->MdcaPoint( i ) );
        aKey->PopL();
        }

    aKey->ReplaceLastL( CREATEKEY( EImpsKeyRemoveList, 0 ) ); // grant: removelist
    count = 0;
    if ( aUnGrantEntity )
        {
        count = aUnGrantEntity->MdcaCount();
        }
    for ( i = 0; i < count; i++ )
        {
        aKey->AddL( CREATEKEY( EImpsKeyUserID, i ) );
        aAc->StoreDescL( aKey, aUnGrantEntity->MdcaPoint( i ) );
        aKey->PopL();
        }
    }





// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetDetailedResultL
// ----------------------------------------------------------------------------
//
TBool TImpsCDataUtils::GetDetailedResultL( 
    MImpsKey* aKey,
    MImpsDataAccessor* aAc,
    TPtrC& aDescription,
    CImpsDetailed*& aDetailed )
    {

    TImpsCDataUtils::AddValuesFromArrayL( 
        aKey, 
        KTransContentElements, 
        sizeof( KTransContentElements ) /
        sizeof( KTransContentElements[0] ) );

    TBool ret(EFalse);
    TInt enumi(0);
    TInt index(0);
    TInt error(0);
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
                if( aAc->RestoreIntegerL( aKey, error ) )
                    ret = ETrue;
                break;
            default:
              break;
            }
        }
    if( ret )
        {
        // Key allready contains code and result
        // Pop out the code
        aKey->PopL( );

        TDesC* myPtr;   
        aKey->AddL( CREATEKEY( EImpsKeyDescription, 0 ) ); 
        if ( aAc->RestoreDescL( aKey, myPtr ) )
            {
            // description found
            aDescription.Set( *myPtr );
            }
        
        // Handle actual detailed part on if
        // error is  201 or 900

        if ( error == 201 || error == 900 )        
            {
            TBool more(ETrue);
            TInt myInt;
            for ( TInt i = 0; more; i++ )
                {
                aKey->ReplaceLastL( CREATEKEY( EImpsKeyDetailedResult, i ) );
                if ( aAc->CheckBranchExistenceL( aKey ) )
                    {
                    CImpsDetailedResult* oneRes = CImpsDetailedResult::NewL();
                    CleanupStack::PushL( oneRes );      // <<< oneRes
                    // Code
                    aKey->AddL( CREATEKEY( EImpsKeyCode, 0 ) );
                    // Notice: if we have validator then we do not need to check that
                    if ( aAc->RestoreIntegerL( aKey, myInt ) )
                        {
                        oneRes->SetCode( myInt );
                        }
                    // Description?
                    aKey->ReplaceLastL( CREATEKEY( EImpsKeyDescription, 0 ) );
                    if ( aAc->RestoreDescL( aKey, myPtr ) )
                        {
                        oneRes->SetDescriptorL( *myPtr );
                        }
                    // UserID*
                    TBool more2(ETrue);

                    TInt j = 0;
                    for ( j = 0; more2; j++ )
                        {
                        aKey->ReplaceLastL( CREATEKEY( EImpsKeyUserID, j ) );
                        if ( aAc->RestoreDescL( aKey, myPtr ) )
                            {
                            CPtrC16Array* users = oneRes->UserIds();
                            users->AppendL( *myPtr );
                            }
                        else
                            {
                            more2 = EFalse;
                            }
                        }

                    // GroupId*
                    more2 = ETrue;
                    for ( j = 0; more2; j++ )
                        {
                        aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, j ) );
                        if ( aAc->RestoreDescL( aKey, myPtr ) )
                            {
                            CPtrC16Array* grs = oneRes->GroupIds();
                            grs->AppendL( *myPtr );
                            }
                        else
                            {
                            more2 = EFalse;
                            }
                        }

                    // ScreenName*
                    more2 = ETrue;
                    for ( j = 0; more2; j++ )
                        {
                        aKey->ReplaceLastL( CREATEKEY( EImpsKeyScreenName, j ) );
                        if ( aAc->RestoreDescL( aKey, myPtr ) )
                            {
                            // This bastard has two elements SName, GroupId
                            CPtrC16Array* sn = oneRes->SNames();
                            CPtrC16Array* sng = oneRes->SNameGroups();
                            aKey->AddL( CREATEKEY( EImpsKeySName, 0 ) );
                            if ( aAc->RestoreDescL( aKey, myPtr ))
                                {
                                sn->AppendL( *myPtr );
                                }
                            aKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupID, 0 ) );
                            if ( aAc->RestoreDescL( aKey, myPtr ) )
                                {
                                sng->AppendL( *myPtr );
                                }
                            aKey->PopL();
                            }
                        else
                            {
                            more2 = EFalse;
                            }
                        }

                    // MessageId*
                    more2 = ETrue;
                    for ( j = 0; more2; j++ )
                        {
                        aKey->ReplaceLastL( CREATEKEY( EImpsKeyMessageID, j ) );
                        if ( aAc->RestoreDescL( aKey, myPtr ) )
                            {
                            CPtrC16Array* msgs = oneRes->MessageIds();
                            msgs->AppendL( *myPtr );
                            }
                        else
                            {
                            more2 = EFalse;
                            }
                        }

                    aDetailed->AppendL( oneRes );   // myDet destructor should delete oneRes
                    CleanupStack::Pop(1);       // >>> oneRes
                    // Pop out the lasd one
                    aKey->PopL( );
                    }
                else
                    {
                    more = EFalse;
                    break;
                    }
                }       // for
            }  // 201/900
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TImpsCDataUtils::SetContentTypeL
// -----------------------------------------------------------------------------
//
void TImpsCDataUtils::SetContentTypeL( 
    MImpsDataAccessor* aAc,
    TPtrC& aContentType )
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KSendMessageElements, 
        sizeof( KSendMessageElements ) /
        sizeof( KSendMessageElements[0] ) );
        
    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyContentType, 0 ) );    
    aAc->StoreDescL( myKey, aContentType );
    CleanupStack::PopAndDestroy(); //  >>> myKey
    }      

// -----------------------------------------------------------------------------
// TImpsCDataUtils::GetContentEncodingL
// -----------------------------------------------------------------------------
//
void TImpsCDataUtils::GetContentEncodingL( 
    MImpsDataAccessor* aAc,
    TInt& aEncoding )
    {
    CImpsKey* myKey = CImpsKey::NewLC(); // << myKey
    TImpsDataUtils::AddValuesFromArrayL( 
        myKey, 
        KNewMessageElements, 
        sizeof( KNewMessageElements ) /
        sizeof( KNewMessageElements[0] ) );
        
    myKey->AddL( CREATEKEY( EImpsKeyMessageInfo, 0 ) );
    myKey->AddL( CREATEKEY( EImpsKeyContentEncoding, 0 ) );    
    aAc->RestoreIntegerL( myKey, aEncoding );
    CleanupStack::PopAndDestroy(); //  >>> myKey
    }   
    
// ----------------------------------------------------------------------------
// TImpsCDataUtils::GetUserMapListL
// ----------------------------------------------------------------------------
void TImpsCDataUtils::GetUserMapListL(
            MImpsKey* aPredecessor,
            MImpsDataAccessor* aAc,
            CDesCArray* aUsers,
            CDesCArray* aScreenNames )
    {
    aPredecessor->AddL( CREATEKEY( EImpsKeyUserMapping, 0 ) );
    // Get first the users
    TBool more(ETrue);
    TDesC* myPtr;
    TInt i = 0;         // aux index
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyMapping, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeyUserID, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aUsers->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    // Then get the screennames
    more = ETrue;
    for ( i = 0; more; i++)
        {
        aPredecessor->AddL( CREATEKEY( EImpsKeyMapping, i ) );
        aPredecessor->AddL( CREATEKEY( EImpsKeySName, 0 ) );
        if ( aAc->RestoreDescL( aPredecessor, myPtr ) )
            {
            aScreenNames->AppendL( *myPtr );
            }
        else
            {
            more = EFalse;
            }
        aPredecessor->PopL( 2 );
        }
    aPredecessor->PopL( ); // UserMapping
    }
   
//  End of File  

