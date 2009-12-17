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
* Description:  Settings manager
*
*/

// INCLUDE FILES
#include "CCASettingsManager.h"			// Own header
#include "MCASettingsObserver.h"
#include "ChatDebugPrint.h"
#include "chatdebugassert.h"
#include "PrivateEngineDefinitions.h"
#include "CCASDCRVariator.h"
#include "ImpsCSPAllErrors.h"
#include "impsbuilddefinitions.h"

#include <e32base.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <centralrepository.h>

// CONSTANTS

// Separators for Alias table, to separate items from each other
// e.g [userid][itemsep][alias][tablesep][userid][itemsep][alias] etc...
//
const TUint KAliasItemSeparator  = 0x10;
const TUint KAliasTableSeparator = 0x11;
const TInt  KSeparatorSize       = 2; // bytes

// Maximum length for alias table.
// See CIMPSSAPSettings::SetOpaqueDesC16 for description of this value
const TInt  KMaxAliasDataLength  =
    NCentralRepositoryConstants::KMaxUnicodeStringLength -
    5 - 10; // 10 == KCAOwnAlias().Length() !

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCASettingsManager* CCASettingsManager::NewL( TUid aIdentifier )
    {
    CCASettingsManager* self = new ( ELeave ) CCASettingsManager;

    CleanupStack::PushL( self );
    self->ConstructL( aIdentifier );
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCASettingsManager::~CCASettingsManager()
    {
    iObserverList.Close();
    delete iSDCRVariator;
    delete iSapSettingsStore;
    delete iDefaultSap;
    }

// ---------------------------------------------------------
// CCASettingsManager::AddObserverL
// ---------------------------------------------------------
//
void CCASettingsManager::AddObserverL( const MCASettingsObserver* aObserver )
    {
    if ( aObserver )
        {
        TInt err( iObserverList.Append( aObserver ) );
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::RemoveObserver
// ---------------------------------------------------------
//
void CCASettingsManager::RemoveObserver( const MCASettingsObserver* aObserver )
    {
    TInt index( iObserverList.Find( aObserver ) );

    if ( index != KErrNotFound )
        {
        iObserverList.Remove( index );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::Value
// ---------------------------------------------------------
//
TBool CCASettingsManager::Value( TCASettingFlags aSetting,
                                 CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt value( 0 );
    TBool ok( EFalse );

    TRAPD( err,
        {
        CIMPSSAPSettings* sap = GetSAPLC( aSap );

        if ( iSettingsFaceLift && !sap )
            {
            ok = ( iSDCRVariator->GetInt( aSetting, value ) == KErrNone );
            }
        else if ( sap )
            {
            TPtrC settingName( ConvertSettingInteger( aSetting ) );
            if ( settingName.Length() > 0 )
                {
                ok = ( sap->GetOpaqueInt( settingName, value ) == KErrNone );
                }
            }

        if ( !ok )
            {
            if ( iSDCRVariator->GetInt( aSetting, value ) != KErrNone )
                {
                value = DefaultBooleanValue( aSetting );
                }
            }

        CleanupStack::PopAndDestroy( sap );
        CHAT_DP( D_CHAT_LIT( "CCASettingsManager::Value( %d ) = %d" ),
                 aSetting, value );
        } );    // TRAPD

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        return EFalse;
        }
    else
        {
        return value;
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::Value
// ---------------------------------------------------------
//
TInt CCASettingsManager::Value( TCASettingIntegers aSetting,
                                CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt value( 0 );
    TBool ok( EFalse );

    TRAPD( err,
        {
        CIMPSSAPSettings* sap = GetSAPLC( aSap );

        if ( iSettingsFaceLift && !sap )
            {
            ok = ( iSDCRVariator->GetInt( aSetting, value ) == KErrNone );
            }
        else if ( sap )
            {
            TPtrC settingName( ConvertSettingInteger( aSetting ) );
            if ( settingName.Length() > 0 )
                {
                ok = ( sap->GetOpaqueInt( ConvertSettingInteger( aSetting ), value ) == KErrNone );
                }
            }

        if ( !ok )
            {
            if ( iSDCRVariator->GetInt( aSetting, value ) != KErrNone )
                {
                value = DefaultIntegerValue( aSetting );
                }
            }

        CleanupStack::PopAndDestroy( sap );
        CHAT_DP( D_CHAT_LIT( "CCASettingsManager::Value( %d ) = %d" ),
                 aSetting, value );
        } );    // TRAPD

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        return EFalse;
        }
    else
        {
        return value;
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::ValueL
// ---------------------------------------------------------
//
HBufC* CCASettingsManager::ValueL( TCASettingStrings aSetting,
                                   CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt length( ( aSetting & EStringsLengthMask ) >> EStringsLengthShift );

    // Check if those enum definitions where reasonable
    if ( !length )
        {
        User::Leave( EInvalidStringEnumDefinition );
        }

    HBufC* temp = HBufC::NewLC( length );
    TPtr str( temp->Des() );

    CIMPSSAPSettings* sap = GetSAPLC( aSap );
    TInt ok( EFalse );
    if ( sap )
        {
        TPtrC settingName( ConvertSettingInteger( aSetting ) );
        if ( settingName.Length() > 0 )
            {
            TPtrC strC( KNullDesC );
            ok = ( sap->GetOpaqueDesC16( settingName, strC ) == KErrNone );

            str.Copy( strC );
            }
        }

    if ( !ok )
        {
        if ( iSDCRVariator->GetString( aSetting, str ) != KErrNone )
            {
            // default to empty
            str.Copy( KNullDesC );
            }
        }

    CHAT_DP( D_CHAT_LIT( "CCASettingsManager::Value( %d ) = %S" ),
             aSetting, temp );

    CleanupStack::PopAndDestroy( sap );
    CleanupStack::Pop( temp );
    return temp;
    }

// ---------------------------------------------------------
// CCASettingsManager::SetValueL
// ---------------------------------------------------------
//
void CCASettingsManager::SetValueL( TCASettingFlags aSetting, TBool aValue,
                                    RArray<TInt>* aIgnoreRollback /*= NULL*/,
                                    CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TBool oldValue( Value( aSetting, aSap ) );
    SetIntValueL( aSetting, aValue, oldValue, aIgnoreRollback, aSap );
    }

// ---------------------------------------------------------
// CCASettingsManager::SetValueL
// ---------------------------------------------------------
//
void CCASettingsManager::SetValueL( TCASettingIntegers aSetting, TInt aValue,
                                    RArray<TInt>* aIgnoreRollback /*= NULL*/,
                                    CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt oldValue( Value( aSetting, aSap ) );
    SetIntValueL( aSetting, aValue, oldValue, aIgnoreRollback, aSap );
    }

// ---------------------------------------------------------
// CCASettingsManager::SetIntValueL
// ---------------------------------------------------------
//
void CCASettingsManager::SetIntValueL( TInt aSetting, TInt aValue,
                                       TInt aOldValue,
                                       RArray<TInt>* aIgnoreRollback /*= NULL*/,
                                       CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    if ( aValue != aOldValue )
        {
        CIMPSSAPSettings* sap = GetSAPLC( aSap );
        StoreL( sap, aSetting, aValue );

        TInt leave( KErrNone );

        TRAP( leave, NotifyObserversL( aSetting ) );

        if ( leave )
            {
            //roll back
            TBool rollback( ETrue );
            if ( aIgnoreRollback )
                {
                // rollback if the error code is not found from the list
                rollback = ( aIgnoreRollback->Find( leave ) == KErrNotFound );
                }

            if ( rollback )
                {
                StoreL( sap, aSetting, aOldValue );
                }
            User::Leave( leave );
            }

        CleanupStack::PopAndDestroy( sap );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::SetValueL
// ---------------------------------------------------------
//
void CCASettingsManager::SetValueL( TCASettingStrings aSetting,
                                    const TDesC& aValue,
                                    RArray<TInt>* aIgnoreRollback /*= NULL*/,
                                    CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt length( ( aSetting & EStringsLengthMask ) >> EStringsLengthShift );

    if ( length < aValue.Length() )
        {
        User::Leave( EStringTooLong );
        }

    HBufC* oldValue = NULL;

    TRAPD( err, oldValue = ValueL( aSetting ) );
    if ( err != EOwnUserIdNotAvailable )
        {
        User::LeaveIfError( err );
        }

    if ( oldValue )
        {
        CleanupStack::PushL( oldValue );
        }

    if ( !oldValue || aValue != *oldValue )
        {
        CHAT_DP( D_CHAT_LIT( "CCASettingsManager::Value( %d ) = %S" ),
                 aSetting, &aValue );
        CIMPSSAPSettings* sap = GetSAPLC( aSap );
        StoreL( sap, aSetting, aValue );

        TInt leave( KErrNone );

        TRAP( leave, NotifyObserversL( aSetting ) );

        if ( leave && oldValue )
            {
            //roll back
            TBool rollback( ETrue );
            if ( aIgnoreRollback )
                {
                // rollback if the error code is not found from the list
                rollback = ( aIgnoreRollback->Find( leave ) == KErrNotFound );
                }

            if ( rollback )
                {
                StoreL( sap, aSetting, *oldValue );
                }
            }
        User::LeaveIfError( leave );
        CleanupStack::PopAndDestroy( sap );
        }

    if ( oldValue )
        {
        CleanupStack::PopAndDestroy( oldValue );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::FlushData
// ---------------------------------------------------------
//
void CCASettingsManager::FlushData()
    {
    TInt err = iSDCRVariator->FlushData();
    if ( err != KErrNone )
        {
        CActiveScheduler::Current()->Error( err );
        }
    }

// Symbian OS default constructor can leave.
void CCASettingsManager::ConstructL( TUid aIdentifier )
    {
    iSDCRVariator = CCASDCRVariator::NewL( aIdentifier );
    iSapSettingsStore = CIMPSSAPSettingsStore::NewL();

    // populate the list of sapsettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSapSettingsStore->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();
    TRAPD( err, iSapSettingsStore->GetDefaultL( sap, EIMPSIMAccessGroup ) );
    if ( err == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( sap );
        sap = NULL;
        }
    else
        {
        CleanupStack::Pop( sap );
        }
    iDefaultSap = sap;

    CleanupStack::PopAndDestroy( sapList );


#ifdef RD_SETTINGS_FACELIFT
    iSettingsFaceLift = ETrue;
#endif
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCASettingsManager::CCASettingsManager()
    {
    }

// ---------------------------------------------------------
// CCASettingsManager::NotifyObserversL
// ---------------------------------------------------------
//
void CCASettingsManager::NotifyObserversL( TInt aEnum )
    {
    TInt observerCount( iObserverList.Count() );

    CHAT_DP( D_CHAT_LIT( "CCASettingsManager::NotifyObserversL(%d), changed \
	                      enum: %d" ), observerCount, aEnum );

    for ( TInt i( 0 ); i < observerCount; ++i )
        {
        ( *iObserverList[i] ).HandleSettingsChangeL( aEnum );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::DefaultIntegerValue
// ---------------------------------------------------------
//
TInt CCASettingsManager::DefaultIntegerValue( TInt aEnum ) const
    {
    switch ( aEnum )
        {
        case EMessageFlowSettingLevel:
            {
            return KDefaultMessageFlowSettingLevel;
            }
        case EFriendsListOrdering :
            {
            return EAlphabetical;
            }
        default:
            {
            // Default default value will be zero for integers
            return 0;
            }
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::DefaultBooleanValue
// ---------------------------------------------------------
//
TBool CCASettingsManager::DefaultBooleanValue( TInt aEnum ) const
    {
    switch ( aEnum )
        {
        case EFirstLoginDone:
            {
            return KDefaultFirstLoginDone;
            }
        case EDefaultScreenNameInUse:
            {
            return KDefaultDefaultScreenNameInUse;
            }
        case EAutomaticPresenceUpdate:
            {
            return KDefaultAutomaticPresenceUpdate;
            }
        case EUpdateSelectedContacts:
            {
            return KDefaultUpdateSelectedContacts;
            }
        case EShowHistory:
            {
            return KDefaultShowHistory;
            }
        case EShowOffline:
            {
            return KDefaultShowOffline;
            }
        case EShowTimeStamps:
            {
            return KDefaultShowTimeStamps;
            }

        default:
            {
            return EFalse;
            }
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::ConvertSettingInteger
// ---------------------------------------------------------
//
const TDesC& CCASettingsManager::ConvertSettingInteger( TInt aEnum ) const
    {
    switch ( aEnum )
        {
            // TCASettingFlags
        case EFirstLoginDone:
            {
            return KCAFirstLoginDone();
            }
        case EDefaultScreenNameInUse:
            {
            return KCADefaultScreenNameInUse();
            }
        case EAutomaticPresenceUpdate:
            {
            return KCAAutomaticPresenceUpdate();
            }
        case EUpdateSelectedContacts:
            {
            return KCAUpdateSelectedContacts();
            }
        case EShowHistory:
            {
            return KCAShowHistory();
            }
        case EShowOffline:
            {
            return KCAShowOffline();
            }
        case EShowTimeStamps:
            {
            return KCAShowTimeStamps();
            }

        // TCASettingIntegers
        case EAuthorizeIMPresence:
            {
            return KCAAuthIMPr();
            }
        case EReceiveIMessages:
            {
            return KCAReceiveIMessages();
            }
        case EReceiveInvitations:
            {
            return KCAReceiveInvitations();
            }
        case EMessageFlowSettingLevel:
            {
            return KCAMsgFlow();
            }
        case EFriendsListOrdering:
            {
            return KCAFriendsListOrdering();
            }

        // TCASettingStrings
        case EDefaultScreenName:
            {
            return KCADefaultNick();
            }
        case EOwnAlias:
            {
            return KCAOwnAlias();
            }
        case EOwnWVUserID:  // flowthrough
        case EServiceAddress:
        case EStatusMsgOnline:
        case EStatusMsgAway:
        case EStatusMsgBusy:
            {
            // these will be saved to cenrep currently
            // as we don't have to support multiple
            // connections
            return KNullDesC();
            }

        default:
            {
            // shouldn't be here
            __CHAT_ASSERT_DEBUG( EFalse );
            return KNullDesC();
            }
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::GetSAPLC
// ---------------------------------------------------------
//
CIMPSSAPSettings* CCASettingsManager::GetSAPLC( CIMPSSAPSettings* aSap )
    {
    if ( aSap )
        {
        CIMPSSAPSettingsList* sapList =
            CIMPSSAPSettingsList::NewLC();
        iSapSettingsStore->PopulateSAPSettingsListL( *sapList,
                                                     EIMPSIMAccessGroup );
        // update information that welcome note was shown,
        // and find our sap... because logged in SAP
        // has UID of zero, we must find the correct UID
        // manually from list
        TInt index( KErrNotFound );
        TInt err = sapList->FindNameL( aSap->SAPName(), index );

        // err would be zero if found else err may be any non zero value
        CIMPSSAPSettings* storedSap = NULL;
        if ( index != KErrNotFound && err == KErrNone )
            {
            // found it, update the correct sap
            storedSap = CIMPSSAPSettings::NewLC();

            TUint32 sapUid = sapList->UidForIndex( index );
            iSapSettingsStore->GetSAPL( sapUid, storedSap );
            CleanupStack::Pop( storedSap );
            }
        CleanupStack::PopAndDestroy( sapList );
        CleanupStack::PushL( storedSap );
        return storedSap;
        }
    else
        {
#ifdef RD_SETTINGS_FACELIFT
        if ( iDefaultSap )
            {
            CIMPSSAPSettings* sap = GetSAPLC( iDefaultSap );
            return sap;
            }
        else
            {
            CleanupStack::PushL( static_cast<CBase*>( NULL ) );
            return NULL;
            }
#else
        CleanupStack::PushL( static_cast<CBase*>( NULL ) );
        return NULL;
#endif //RD_SETTINGS_FACELIFT		
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::StoreL
// ---------------------------------------------------------
//
void CCASettingsManager::StoreL( CIMPSSAPSettings* aSap,
                                 TInt aSetting,
                                 TInt aValue )
    {
    TBool ok( EFalse );
    if ( aSap )
        {
        CIMPSSAPSettingsList* sapList =
            CIMPSSAPSettingsList::NewLC();
        iSapSettingsStore->PopulateSAPSettingsListL( *sapList,
                                                     EIMPSIMAccessGroup );
        // Because logged in SAP hs UID of zero,
        // we must find the correct UID manually from list
        TInt index( KErrNotFound );
        sapList->FindNameL( aSap->SAPName(), index );
        if ( index == KErrNotFound )
            {
            User::Leave( index );
            }

        if ( index != KErrNotFound )
            {
            // found it, update the correct sap
            CIMPSSAPSettings* storedSap =
                CIMPSSAPSettings::NewLC();

            TUint32 sapUid = sapList->UidForIndex( index );
            iSapSettingsStore->GetSAPL( sapUid, storedSap );

            TPtrC settingName( ConvertSettingInteger( aSetting ) );
            if ( settingName.Length() > 0 )
                {
                storedSap->SetOpaqueInt( settingName, aValue );
                // update sap
                iSapSettingsStore->UpdateOldSAPL( storedSap, sapUid );
                ok = ETrue;
                }

            CleanupStack::PopAndDestroy( storedSap );
            }
        CleanupStack::PopAndDestroy( sapList );
        }

    if ( !ok )
        {
        // not saved to SAP, use cenrep
        User::LeaveIfError( iSDCRVariator->SetInt( aSetting, aValue ) );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::StoreL
// ---------------------------------------------------------
//
void CCASettingsManager::StoreL( CIMPSSAPSettings* aSap,
                                 TInt aSetting,
                                 const TDesC& aValue )
    {
    TBool ok( EFalse );
    if ( aSap )
        {
        CIMPSSAPSettingsList* sapList =
            CIMPSSAPSettingsList::NewLC();
        iSapSettingsStore->PopulateSAPSettingsListL( *sapList,
                                                     EIMPSIMAccessGroup );
        // Because logged in SAP hs UID of zero,
        // we must find the correct UID manually from list
        TInt index( KErrNotFound );
        sapList->FindNameL( aSap->SAPName(), index );
        if ( index == KErrNotFound )
            {
            User::Leave( index );
            }

        if ( index != KErrNotFound )
            {
            // found it, update the correct sap
            CIMPSSAPSettings* storedSap =
                CIMPSSAPSettings::NewLC();

            TUint32 sapUid = sapList->UidForIndex( index );
            iSapSettingsStore->GetSAPL( sapUid, storedSap );

            TPtrC settingName( ConvertSettingInteger( aSetting ) );
            if ( settingName.Length() > 0 )
                {
                storedSap->SetOpaqueDesC16( settingName, aValue );
                // update sap
                iSapSettingsStore->UpdateOldSAPL( storedSap, sapUid );
                ok = ETrue;
                }
            CleanupStack::PopAndDestroy( storedSap );
            }
        CleanupStack::PopAndDestroy( sapList );
        }

    if ( !ok )
        {
        // not saved to SAP, use cenrep
        User::LeaveIfError( iSDCRVariator->SetString( aSetting, aValue ) );
        }
    }

// ---------------------------------------------------------
// CCASettingsManager::SetDefaultSapL
// ---------------------------------------------------------
//
void CCASettingsManager::SetDefaultSapL( CIMPSSAPSettings* aSap )
    {
    CIMPSSAPSettings* tempSap = GetSAPLC( aSap );
    CleanupStack::Pop( tempSap );
    delete iDefaultSap;
    iDefaultSap = tempSap;
    }

// ---------------------------------------------------------
// CCASettingsManager::OwnAliasL()
// ---------------------------------------------------------
//
HBufC* CCASettingsManager::OwnAliasL()
    {
    CHAT_DP_FUNC_ENTER( "CCASettingsManager::OwnAliasL" );

    // Read aliasdata from SAP settings
    CIMPSSAPSettings* sap = GetSAPLC( NULL );
    if ( !sap )
        {
        User::Leave( KErrNotReady );
        }
    TPtrC settingName( ConvertSettingInteger( MCASettings::EOwnAlias ) );
    TPtrC alias( KNullDesC );

    // aliasTable has all different aliases for each userID
    // format [userID]\x10[Alias]\0x11[userID]\x10[Alias],...
    if ( sap->GetOpaqueDesC16( settingName, alias ) != KErrNone )
        {
        alias.Set( KNullDesC );
        }

    // current user ID
    HBufC* userId = ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( userId );

    TInt index = KErrNotFound;
    TInt length = 0;
    LocateAliasL( alias, *userId, index, length );

    HBufC* ret = NULL; // if not found -> NULL
    if ( index != KErrNotFound )
        {
        // Alias found
        ret = alias.Mid( index, length ).AllocL();
        }

    CleanupStack::PopAndDestroy( 2, sap ); // sap, userId
    CHAT_DP_FUNC_DONE( "CCASettingsManager::OwnAliasL" );
    return ret; // NULL or ALIAS
    }

// ---------------------------------------------------------
// CCASettingsManager::SetOwnAliasL()
// ---------------------------------------------------------
//
void CCASettingsManager::SetOwnAliasL( const TDesC& aAlias )
    {
    CHAT_DP_FUNC_ENTER( "CCASettingsManager::SetOwnAliasL" );

    // Read aliasdata from SAP settings
    CIMPSSAPSettings* sap = GetSAPLC( NULL );
    if ( !sap )
        {
        User::Leave( KErrNotReady );
        }
    TPtrC settingName( ConvertSettingInteger( MCASettings::EOwnAlias ) );
    TPtrC aliases( KNullDesC );

    // aliasTable has all different aliases for each userID
    // format [userID]\x10[Alias]\0x11[userID]\x10[Alias],...
    if ( sap->GetOpaqueDesC16( settingName, aliases ) != KErrNone )
        {
        aliases.Set( KNullDesC );
        }

    // current user ID
    HBufC* userId = ValueL( MCASettings::EOwnWVUserID );
    CleanupStack::PushL( userId );

    // Search for alias
    TInt index = KErrNotFound;
    TInt length = 0;
    LocateAliasL( aliases, *userId, index, length );

    HBufC* newAliasBuf = NULL;
    if ( index == KErrNotFound )
        {
        // Alias not found -> Append to table.
        newAliasBuf = HBufC::NewLC( aliases.Length() +
                                    userId->Length() +
                                    aAlias.Length() +
                                    KSeparatorSize * 2 );
        TPtr newAliases( newAliasBuf->Des() );
        newAliases.Set( newAliasBuf->Des() );
        newAliases.Copy( aliases );
        if ( newAliases.Length() > 0 )
            {
            // only add separator if this is not the first alias
            newAliases.Append( TChar( KAliasTableSeparator ) );
            }
        newAliases.Append( *userId );
        newAliases.Append( TChar( KAliasItemSeparator ) );

        index = newAliases.Length();
        length = aAlias.Length();
        newAliases.Append( aAlias );
        }
    else
        {
        // Alias Found -> Replace in table.
        newAliasBuf = HBufC::NewLC( aliases.Length() +
                                    aAlias.Length() );
        TPtr newAliases( newAliasBuf->Des() );
        newAliases.Set( newAliasBuf->Des() );
        newAliases.Copy( aliases );
        newAliases.Replace( index, length, aAlias );
        }

    // Trim new data to fit into SAP settings
    TPtr data( newAliasBuf->Des() );
    TInt findPos = 0;
    while ( data.Length() > KMaxAliasDataLength )
        {
        TInt pos = data.Mid( findPos ).Locate( TChar( KAliasTableSeparator ) ) + 1;
        TBool nonRemovable = ( findPos < index ) && ( index < findPos + pos );
        if ( pos != KErrNotFound )
            {
            if ( !nonRemovable )
                {
                // this is not the newly updated alias -> remove it
                data.Delete( findPos, pos );
                if ( index > findPos )
                    {
                    index -= pos;
                    }
                }
            else
                {
                // could not delete -> continue from new pos
                findPos += pos;
                }
            }
        else
            {
            // last item. We have to remove something to fit data into
            // settings!
            TInt delLen = data.Mid( findPos ).Length();
            if ( delLen == 0 )
                {
                // If we end up here, it means we have already deleted
                // everything we can. All we can do now is to clear the whole
                // data and break out. This means we don't define Alias.
                // This case is only possible if userId and Alias exceeds the
                // capasity of SAP setting store.
                //
                __CHAT_ASSERT_DEBUG( ETrue );
                data.Zero();
                break;
                }
            data.Delete( findPos, delLen );
            }
        }

    // Write to store
    StoreL( sap, MCASettings::EOwnAlias, *newAliasBuf );

    CleanupStack::PopAndDestroy( 3, sap ); // sap, userId, newAliasBuf
    CHAT_DP_FUNC_DONE( "CCASettingsManager::SetOwnAliasL" );
    }

// ---------------------------------------------------------
// CCASettingsManager::LocateAliasL()
// ---------------------------------------------------------
//
void CCASettingsManager::LocateAliasL( const TDesC& aAliasTable,
                                       const TDesC& aUserId,
                                       TInt& aIndex,
                                       TInt& aLength )
    {
    CHAT_DP_TXT( "Finding alias from setting table..." );
    aIndex = KErrNotFound;
    aLength = 0;
    if ( aAliasTable.Length() == 0 || aUserId.Length() == 0 )
        {
        // nothing to search!
        return;
        }

    HBufC* findPattern = HBufC::NewLC( aUserId.Length() +
                                       KSeparatorSize );
    TPtr find( findPattern->Des() );
    find.Copy( aUserId );
    TInt len = find.Length();
    find.Append( TChar( KAliasItemSeparator ) );
    TInt len2 = find.Length();

    aIndex = aAliasTable.Find( find );
    if ( aIndex == KErrNotFound )
        {
        // not found
        CHAT_DP_TXT( "...Alias not found" );
        CleanupStack::PopAndDestroy( findPattern );
        return;
        }

    // found it, get length
    aIndex += find.Length();
    TPtrC rest( aAliasTable.Mid( aIndex ) );
    aLength = rest.Locate( TChar( KAliasTableSeparator ) );
    if ( aLength == KErrNotFound )
        {
        // this was last item
        aLength = rest.Length();
        }
    CleanupStack::PopAndDestroy( findPattern );
    CHAT_DP( D_CHAT_LIT( "...Alias found at pos:%d length:%d" ),
             aIndex, aLength );
    return;
    }

//  End of File
