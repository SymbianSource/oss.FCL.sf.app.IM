/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS SAP Settings Store implementation.
*
*/


// INCLUDE FILES

#include 	<e32std.h>
#include    <centralrepository.h>
//#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
//#include <centralrepositoryinternal.h>
//#endif
#include    "CIMPSSAPCenRep.h"
#include	"cimpssapsettings.h"
#include	"CIMPSSAPSettingsBody.h"
#include	"cimpssapsettingslist.h"
#include	"CIMPSSAPKeyValuePairs.h"
#include	"CIMPSSAPKeyValuePair.h"
#include	"CIMPSSAPNotifier.h"

#include	"IMPSSAPSettingsStoreCenRepUids.h"
#include    "IMPSSAPSettingsStoreDebugPrint.h"
#include    "IMPSSAPSettingsStorePanics.h"

#include	"RIMPSReleaseArray.h"

#include 	"TIMPSSAPSettingsListItemNameKey.h"
#include	"TIMPSCenRepOperations.h"

// This implementation uses the following way of dividing the
// central repository id range for storing
// SAP Settings data:
//
// - 6 bits for base id(defines the SAP access group)
// - 10 bits for sap uid(unique within all groups)
// - 16 bits for settings, which is further divided into 256
// subsets each containing 256 items. Currently 2 subsets are in use.
// First of the subsets contains settings that have fixed identifiers
// and the second one key-value pairs.


/*
Example:
------------------------------

cenrep entry: 3221356546 int 8081 0

=

110000 		| 0000000010 	| 0000000000000010
------ 		  ----------   	  ----------------
SAP			| SAP Uid    	| Setting Id
Base

6 bits, 	  10 bits	  	  16 bits
110000		 ..010 = 	 	 (10 = 2 dec = ESAPPort in TFixedSAPSetting)
(0x30)		   2 dec
PEC base
id

*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::CIMPSSAPCenRep
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIMPSSAPCenRep::CIMPSSAPCenRep()
    {
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIMPSSAPCenRep::ConstructL( TInt aPriority )
    {
    iRepository = CRepository::NewL( KCRUidIMPSStore );
    iNotifier = CIMPSSAPNotifier::NewL( aPriority, KCRUidIMPSStore );
    iCommsDatabase = CCommsDatabase::NewL( EDatabaseTypeIAP );
    SetUpDefaultsL();
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPCenRep* CIMPSSAPCenRep::NewL( TInt aPriority )
    {
    CIMPSSAPCenRep* self = CIMPSSAPCenRep::NewLC( aPriority );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIMPSSAPCenRep* CIMPSSAPCenRep::NewLC( TInt aPriority )
    {
    CIMPSSAPCenRep* self = new( ELeave ) CIMPSSAPCenRep();

    CleanupStack::PushL( self );
    self->ConstructL( aPriority );

    return self;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::~CIMPSSAPCenRep
// Destructor
// -----------------------------------------------------------------------------
//
CIMPSSAPCenRep::~CIMPSSAPCenRep()
    {
    delete iRepository;
    delete iNotifier;
    delete iCommsDatabase;
    }

// HELPER FUNCTIONS

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::MaskedId()
// Calculates masked central repository id for setting
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPCenRep::MaskedId( TInt aSetting, TUint32 aUid,
                                  TIMPSAccessGroup aGroup )
    {
    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

    TUint32 maskBase( 0 );
    if ( aGroup & EIMPSIMAccessGroup )
        {
        maskBase += KSAPSettingsIMBaseId << KBaseOffset;
        }
    else
        {
        maskBase += KSAPSettingsPECBaseId << KBaseOffset;
        }
    maskBase += aUid << KSAPOffset;
    maskBase += aSetting;

    return maskBase;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SAPNameExistsL()
// Checks if SAP with given name already exists in given access group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CIMPSSAPCenRep::SAPNameExistsL( const TDesC& aSAPName, TUint32 aUid,
                                      TIMPSAccessGroup aGroup )
    {
    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();

    PopulateSAPSettingsListL( *list, aGroup );

    TInt count( list->Count() );
    TBool alreadyExists( EFalse );
    TInt index( 0 );
    if ( list->FindNameL( aSAPName, index ) == 0 )
        {
        if ( list->UidForIndex( index ) != aUid )
            {
            alreadyExists = ETrue;
            }
        }


    CleanupStack::PopAndDestroy( list );
    return alreadyExists;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SAPProtectionL()
// Checks SAP protection level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TIMPSSAPProtection CIMPSSAPCenRep::SAPProtectionL( TUint32 aUid,
                                                   TIMPSAccessGroup aGroup )
    {
    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

    TInt ret( ESAPNoProtection );
    User::LeaveIfError( iRepository->Get( EProtection + aUid, ret ) );
    return static_cast< TIMPSSAPProtection >( ret );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::NewUidL()
// Gets new Uid for new SAP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TUint32 CIMPSSAPCenRep::NewUidL()
    {
    TInt newUid;
    TInt ret;
    ret = iRepository->Get( KSAPHighestUid, newUid );
    if ( ret == KErrNotFound ) // Highest uid not found, create it
        {
        // Check if we have existing SAPs despite not having the highest uid.
        // This may happen if we have older settings.
        if ( SAPCountL( EIMPSAccessFilterAll ) > 0 ) // existing SAPs
            {
            RArray<TUint32> foundSAPs;
            CleanupClosePushL( foundSAPs );
            // FindL return value is currently undocumented,
            // thus silently consumed for now
            iRepository->FindL( ESAPName, KSAPPopulateAllMask, foundSAPs );
            TInt count( foundSAPs.Count() );
            for ( TInt i( 0 ); i < count; ++i )
                {
                //transform found cenrep uids to SAP uids
                foundSAPs[i] &= KSAPUidMask;
                foundSAPs[i] = foundSAPs[i] >> KSAPOffset;
                }
            foundSAPs.SortUnsigned();
            // Find the highest existing uid
            for ( TUint32 i( KLastSAPId ); i >= KFirstSAPId;i-- )
                {
                if ( foundSAPs.Find( i ) == KErrNone )
                    {
                    if ( i < KLastSAPId )
                        {
                        newUid = i + 1;
                        }
                    else
                        {
                        newUid = KFirstSAPId;
                        }
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(); // foundSAPs
            }
        else // No existing SAPs
            {
            newUid = KFirstSAPId;
            }
        User::LeaveIfError( iRepository->Create( KSAPHighestUid, newUid ) );
        }

    else // Otherwise if it's an error situation
        {
        User::LeaveIfError( ret );
        }

    if ( newUid < KFirstSAPId )
        {
        // Something is wrong if we get here
        User::Leave( KErrGeneral );
        }
    else if ( newUid >= KLastSAPId ) // If at the end of the range
        {
        // start from the beginning
        newUid = KFirstSAPId;
        }
    // Check if there are ANY free uids
    if ( ( KLastSAPId - KFirstSAPId ) <= SAPCountL( EIMPSAccessFilterAll ) )
        {
        User::Leave( KErrInUse ); // All UIDs in use
        }

    // Check if there is the next uid is unused.
    TBool okId( EFalse );
    TInt err( KErrNone );
    while ( !okId )
        {
        newUid++;
        TUint32 maskedUid = newUid << KSAPOffset;
        RArray<TUint32> foundSAPs;
        CleanupClosePushL( foundSAPs );
        err = iRepository->FindL( maskedUid, KSAPExistsMask, foundSAPs );
        if ( err == KErrNotFound ) // The uid is free
            {
            okId = ETrue;
            }
        else if ( err == KErrNone ) // There was a SAP with that uid
            {
            // Check range
            if ( newUid >= KLastSAPId )
                {
                newUid = KFirstSAPId; // Start from the beginning.
                }
            }
        else
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy(); //foundSAPs
        }
    // Save the new highest
    User::LeaveIfError( iRepository->Set( KSAPHighestUid, newUid ) );
    return newUid; // return the new uid
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SAPTypeL()
// Gets SAP type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TIMPSAccessGroup CIMPSSAPCenRep::SAPTypeL( TUint32 aUid )
    {
    TIMPSAccessGroup type( EIMPSNoAccessGroup );
    aUid &= KSAPBaseMask;
    aUid = aUid >> KBaseOffset;
    if ( aUid == KSAPSettingsPECBaseId )
        {
        type = EIMPSPECAccessGroup;
        }
    else if ( aUid == KSAPSettingsIMBaseId )
        {
        type = EIMPSIMAccessGroup;
        }
    else
        {
        // aUid was found in unknown access group. Should not happen.
        User::Leave( KErrGeneral );
        }
    return type;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SAPExistsL()
// Checks if SAP exists in central repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::SAPExistsL( TUint32 aUid )
    {
    RArray<TUint32> foundSAPs;
    CleanupClosePushL( foundSAPs );
    // FindL return value is currently undocumented,
    // thus silently consumed for now
    iRepository->FindL( aUid, KSAPExistsMask, foundSAPs );
    if ( foundSAPs.Count() != 1 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy(); //foundSAPs
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SetUpDefaultsL()
// Sets up default SAP ids if they do not exist in the repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::SetUpDefaultsL()
    {
    TInt uid( 0 );
    TInt err ( iRepository->Get( KDefaultIMSAPId, uid ) );
    if ( err == KErrNotFound )
        {
        User::LeaveIfError( iRepository->Create( KDefaultIMSAPId,
                                                 KIMPSSettingsNullUid ) );
        }
    else
        {
        User::LeaveIfError( err );
        }

    err = iRepository->Get( KDefaultPECSAPId, uid );
    if ( err == KErrNotFound )
        {
        User::LeaveIfError( iRepository->Create( KDefaultPECSAPId,
                                                 KIMPSSettingsNullUid ) );
        }
    else
        {
        User::LeaveIfError( err );
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::FindSAPsFromAccessGroupL()
// Finds and populates SAP ids to an array from desired access group
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::FindSAPsFromAccessGroupL( TIMPSAccessGroup aGroup,
                                               RArray<TUint32>& aFoundSAPs )
    {
    if ( aGroup == EIMPSAccessFilterAll )
        {
        iRepository->FindL( ESAPAddress, KSAPPopulateAllMask, aFoundSAPs );
        }
    else if ( aGroup == EIMPSIMAccessGroup )
        {
        iRepository->FindL( MaskedId( ESAPAddress, KIMPSSettingsNullUid,
                                      EIMPSIMAccessGroup ), KSAPPopulateGroupMask, aFoundSAPs );
        }
    else if ( aGroup == EIMPSPECAccessGroup )
        {
        iRepository->FindL( MaskedId( ESAPAddress, KIMPSSettingsNullUid,
                                      EIMPSPECAccessGroup ), KSAPPopulateGroupMask, aFoundSAPs );
        }
    }


// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::StartOwnTransaction()
// Starts new transaction if there is no ongoing transaction
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CIMPSSAPCenRep::StartOwnTransactionL( TInt aMode )
    {
//    TInt transaction( iRepository->TransactionState() );

 	if( !iInTransaction )
 		{
 		iInTransaction = ETrue;
 		CleanupStack::PushL( TCleanupItem( ReSetTransactionState, this ));
 		iRepository->StartTransaction( static_cast<CRepository::TTransactionMode>( aMode ) );
 		return ETrue;
 		}
	return EFalse;
    }

// OBSERVER SUPPORT

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::AddObserverL()
// Adds SAP observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSAPCenRep::AddObserverL( MIMPSSAPObserver* aObserver,
                                   TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::AddObserverL()" );
    iNotifier->AddObserverL( aObserver, aGroup );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::RemoveObserverL()
// Removes SAP observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSAPCenRep::RemoveObserver( MIMPSSAPObserver* aObserver )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::RemoveObserver()" );
    iNotifier->RemoveObserver( aObserver );
    }

// SAP SETTINGS

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::StoreNewSAPL()
// Stores new SAP in central repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CIMPSSAPCenRep::StoreNewSAPL( CIMPSSAPSettings* aSAPSettings,
                                      TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::StoreNewSAPL()" );

    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

    // SAP name must be defined
    if ( aSAPSettings->SAPName().Length() == 0 )
        {
        User::Leave( KErrArgument );
        }

    // no documentation for StartTransaction() return values, leave if any error
	iInTransaction = ETrue;
	CleanupStack::PushL(TCleanupItem( ReSetTransactionState, this ));
    User::LeaveIfError( iRepository->StartTransaction( CRepository::EReadWriteTransaction ) );

    iRepository->CleanupCancelTransactionPushL();

    //Check if SAP with given name already exists

    // KIMPSSettingsNullUid cannot exist so this checks for all duplicates
    if ( SAPNameExistsL( aSAPSettings->SAPName(), KIMPSSettingsNullUid,
                         aGroup ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    RIMPSReleaseArray tempArray;
    CleanupClosePushL( tempArray );

    // Assign Uid for the new entry, leave if all Uids are in use
    TUint32 newUid( NewUidL() );

    //Encrypt passwords before storing has been removed
    //401-1808 - Ease of Instant Messaging SERVER customization
    HBufC* tmpPasswd = aSAPSettings->SAPUserPassword().AllocLC();
    HBufC* tmpHTTPPasswd = aSAPSettings->HTTPProxyUserPassword().AllocLC();
    TPtr passwdPtr( tmpPasswd->Des() );
    TPtr httpPasswdPtr( tmpHTTPPasswd->Des() );

    //get the first stored setting ID as the returnable ID for the client
    TUint32 returnId( MaskedId( ESAPAddress, newUid, aGroup ) );

    // If this is the first SAP to be stored in either access group,
    // set it to default
    if ( aGroup == EIMPSIMAccessGroup )
        {
        if ( SAPCountL( EIMPSIMAccessGroup ) == 0 )
            {
            User::LeaveIfError( iRepository->Set( KDefaultIMSAPId,
                                                  static_cast< TInt >( returnId ) ) );
            }
        }
    else
        {
        if ( SAPCountL( EIMPSPECAccessGroup ) == 0 )
            {
            User::LeaveIfError( iRepository->Set( KDefaultPECSAPId,
                                                  static_cast< TInt >( returnId ) ) );
            }
        }

    TIMPSCenRepOperations op( returnId, *iRepository );

    op.CreateL( ESAPAddress, aSAPSettings->SAPAddress() );
    op.CreateL( ESAPName, aSAPSettings->SAPName() );
    op.CreateL( ESAPPort, static_cast< TInt >( aSAPSettings->SAPPort() ) );
    op.CreateL( ESAPUserId, aSAPSettings->SAPUserId() );
    op.CreateL( ESAPUserPassword, passwdPtr );
    op.CreateL( EHTTPProxyUserId, aSAPSettings->HTTPProxyUserId() );
    op.CreateL( EHTTPProxyUserPassword, httpPasswdPtr );
    op.CreateL( EHTTPProxyAddress, aSAPSettings->HTTPProxyAddress() );
    op.CreateL( EHTTPProxyPort, static_cast< TInt >( aSAPSettings->HTTPProxyPort() ) );

    HBufC* apName = DoGetAccessPointsNameL(
                        static_cast< TInt >( aSAPSettings->AccessPoint() ) );
    CleanupStack::PushL( apName );
    op.CreateL( EAccessPoint, *apName );
    CleanupStack::PopAndDestroy( apName );

    op.CreateL( EClientId, aSAPSettings->ClientId() );
    op.CreateL( EServerAuthenticationName, aSAPSettings->ServerAuthenticationName() );
    op.CreateL( EServerAuthenticationPassword, aSAPSettings->ServerAuthenticationPassword() );
    op.CreateL( EServerAcceptedContentType, aSAPSettings->ServerAcceptedContentType() );
    op.CreateL( EHighLevelServices, static_cast< TInt >( aSAPSettings->HighLevelServices() ) );
    op.CreateL( EAuthorizationMode, static_cast< TInt >( aSAPSettings->AuthorizationMode() ) );
    op.CreateL( EProtection, static_cast< TInt >( aSAPSettings->Protection() ) );

    const RPointerArray< CIMPSSAPKeyValuePair >& pairs( aSAPSettings->KeyValuePairs().Pairs() );

    TInt count( pairs.Count() );

    for ( TInt i( 0 );i < count; ++i )
        {
        //HBufC ownership transfers here
        HBufC* valuePairFlat = pairs[i]->KeyValuePairFlatLC();
        User::LeaveIfError( iRepository->Create( EKeyValuePairBase +
                                                 returnId + i,
                                                 *valuePairFlat ) );
        // CRepository->Create() is not copying the pointed data during transaction
        // so these must be stored until the transaction has completed
        tempArray.AppendL( valuePairFlat );
        CleanupStack::Pop( valuePairFlat );
        }

    TUint32 err( KErrNone );
    //passed err will be silently consumed because the value is of no use to client
    User::LeaveIfError( iRepository->CommitTransaction( err ) );
	iInTransaction = EFalse;
	CleanupStack::PopAndDestroy( 5 ); //cleanupitem, Transaction, tempArray, tmpPasswd, tmpHTTPPasswd

    aSAPSettings->Body().DoSetUid( returnId );
    aSAPSettings->Body().DoSetAccessGroup( aGroup );
    return returnId;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::GetSAPL()
// Gets SAP from central repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CIMPSSAPCenRep::GetSAPL( TUint32 aUid, CIMPSSAPSettings* aSAPSettings )
    {
    TRAPD( err, DoGetSAPL( aUid, aSAPSettings ) );
    if ( err == KErrArgument )
        {
        aSAPSettings->Reset();
        }
    else if ( err )
        {
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::DoGetSAPL()
// Does the actual get operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------

void CIMPSSAPCenRep::DoGetSAPL( TUint32 aUid, CIMPSSAPSettings* aSAPSettings )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::GetSAPL()" );

	TBool transaction( StartOwnTransactionL( CRepository::EReadTransaction ) );
    if ( transaction )
        {
        iRepository->CleanupCancelTransactionPushL();
        }

    HBufC* tmpBuffer = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr tmpPtr( tmpBuffer->Des() );

    TInt tmpInteger( 0 );

    SAPExistsL( aUid );

    TIMPSAccessGroup type( SAPTypeL( aUid ) );

    TIMPSCenRepOperations op( aUid, *iRepository );

    op.GetL( ESAPAddress, tmpPtr );
    aSAPSettings->SetSAPAddressL( tmpPtr );

    op.GetL( ESAPName, tmpPtr );
    aSAPSettings->SetSAPNameL( tmpPtr );

    op.GetL( ESAPPort, tmpInteger );
    aSAPSettings->SetSAPPort( tmpInteger );

    op.GetL( ESAPUserId, tmpPtr );
    aSAPSettings->SetSAPUserIdL( tmpPtr );

    op.GetL( ESAPUserPassword, tmpPtr );
    //Decrypting of passwrd has been removed
    //401-1808 - Ease of Instant Messaging SERVER customization
    aSAPSettings->SetSAPUserPasswordL( tmpPtr );

    op.GetL( EHTTPProxyUserId, tmpPtr );
    aSAPSettings->SetHTTPProxyUserIdL( tmpPtr );

    //Decrypting of passwrd has been removed
    //401-1808 - Ease of Instant Messaging SERVER customization
    op.GetL( EHTTPProxyUserPassword, tmpPtr );
    aSAPSettings->SetHTTPProxyUserPasswordL( tmpPtr );

    op.GetL( EHTTPProxyAddress, tmpPtr );
    aSAPSettings->SetHTTPProxyAddressL( tmpPtr );

    op.GetL( EHTTPProxyPort, tmpInteger );
    aSAPSettings->SetHTTPProxyPort( tmpInteger );

    //op.GetL( EAccessPoint, tmpInteger );
    op.GetL( EAccessPoint, tmpPtr );
    tmpInteger = static_cast< TInt >( DoGetAccessPointsL( tmpPtr ) );
    aSAPSettings->SetAccessPoint( tmpInteger );

    op.GetL( EClientId, tmpPtr );
    aSAPSettings->SetClientIdL( tmpPtr );

    op.GetL( EServerAuthenticationName, tmpPtr );
    aSAPSettings->SetServerAuthenticationNameL( tmpPtr );

    op.GetL( EServerAuthenticationPassword, tmpPtr );
    aSAPSettings->SetServerAuthenticationPasswordL( tmpPtr );

    op.GetL( EServerAcceptedContentType, tmpPtr );
    aSAPSettings->SetServerAcceptedContentTypeL( tmpPtr );

    op.GetL( EHighLevelServices, tmpInteger );
    aSAPSettings->SetHighLevelServices( tmpInteger );

    op.GetL( EAuthorizationMode, tmpInteger );
    aSAPSettings->SetAuthorizationMode( tmpInteger );

    op.GetL( EProtection, tmpInteger );
    aSAPSettings->ProtectL( static_cast<TIMPSSAPProtection>( tmpInteger ) );

    RPointerArray< CIMPSSAPKeyValuePair >& pairs( aSAPSettings->KeyValuePairs().Pairs() );
    pairs.ResetAndDestroy();

    RArray<TUint32> foundPairs;
    CleanupClosePushL( foundPairs );
    iRepository->FindL( EKeyValuePairBase + aUid, KSAPPairsMask, foundPairs );
    TInt count( foundPairs.Count() );

    for ( TInt i( 0 );i < count; ++i )
        {
        TPtr valuePairFlatPtr( tmpBuffer->Des() );

        User::LeaveIfError(
            iRepository->Get( foundPairs[ i ],
                              valuePairFlatPtr ) );

        // parse the key-value pair descriptor

        CIMPSSAPKeyValuePair* pair = CIMPSSAPKeyValuePair::NewLC( valuePairFlatPtr );

        pairs.AppendL( pair );

        CleanupStack::Pop( pair );
        }

    if ( transaction )
        {
        TUint32 err( KErrNone );
        User::LeaveIfError( iRepository->CommitTransaction( err ) );
		iInTransaction = EFalse;
		CleanupStack::PopAndDestroy(2); // cleanupitem, transaction
        }
    CleanupStack::PopAndDestroy( 2 ); // foundPairs, tmpBuffer
    aSAPSettings->Body().DoSetAccessGroup( type );
    aSAPSettings->Body().DoSetUid( aUid );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SAPCountL()
// Counts the number of SAPs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIMPSSAPCenRep::SAPCountL( TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::SAPCountL()" );

    __ASSERT_ALWAYS(    aGroup & EIMPSAccessFilterAll,
                        Panic( EIMPSInvalidAccessGroup ) );

    RArray<TUint32> foundSAPs;
    CleanupClosePushL( foundSAPs );
    FindSAPsFromAccessGroupL( aGroup, foundSAPs );
    TInt count( foundSAPs.Count() );
    CleanupStack::PopAndDestroy(); //foundSAPs
    return count;
    }


// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::DeleteSAPL()
// Deletes SAP from central repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSAPCenRep::DeleteSAPL( TUint32 aUid )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::DeleteSAPL()" );

    iInTransaction = ETrue;
    CleanupStack::PushL( TCleanupItem( ReSetTransactionState, this ));
    User::LeaveIfError( iRepository->StartTransaction( CRepository::EReadWriteTransaction ) );
    iRepository->CleanupCancelTransactionPushL();

    SAPExistsL( aUid );
    TIMPSAccessGroup type( SAPTypeL( aUid ) );

    RArray<TUint32> foundIds;
    CleanupClosePushL( foundIds );
    iRepository->FindL( aUid, KSAPUidMask, foundIds );
    TInt count( foundIds.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        User::LeaveIfError( iRepository->Delete( foundIds[ i ] ) );
        }

    CleanupStack::PopAndDestroy(); //foundIds

    TInt sapCount( SAPCountL( type ) );

    if ( sapCount > 1 )	// deletion not commited yet, but nowadays the API
        // seems to give up-to-date counts regardless
        {
        TUint32 defaultUid;
        GetDefaultL( defaultUid, type );
        // default SAP id is set to KIMPSSettingsNullUid if the
        // default SAP was deleted
        if ( type == EIMPSIMAccessGroup )
            {
            if ( aUid == defaultUid )
                {
                User::LeaveIfError( iRepository->Set( KDefaultIMSAPId,
                                                      KIMPSSettingsNullUid ) );
                }
            }
        else
            {
            if ( aUid == defaultUid )
                {
                User::LeaveIfError( iRepository->Set( KDefaultPECSAPId,
                                                      KIMPSSettingsNullUid ) );
                }
            }
        }
    // If there is only one SAP left after the deletion, set it to default.
    if ( sapCount == 1 )
        {
        CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();
        PopulateSAPSettingsListL( *list, type );
        TUint32 newDefault( list->UidForIndex( 0 ) );
        SetToDefaultL( newDefault, type );
        CleanupStack::PopAndDestroy( list );
        }
    // If there are no SAPs left after the deletion, default SAP id is
    // set to KIMPSSettingsNullUid
    if ( sapCount == 0 )
        {
        if ( type == EIMPSIMAccessGroup )
            {
            User::LeaveIfError( iRepository->Set( KDefaultIMSAPId,
                                                  KIMPSSettingsNullUid ) );
            }
        else
            {
            User::LeaveIfError( iRepository->Set( KDefaultPECSAPId,
                                                  KIMPSSettingsNullUid ) );
            }
        }
    TUint32 err;
    User::LeaveIfError( iRepository->CommitTransaction( err ) );
	iInTransaction = EFalse;
  	CleanupStack::PopAndDestroy(2); //cleanupitem, Transaction
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::UpdateOldSAPL()
// Updates existing SAP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMPSSAPCenRep::UpdateOldSAPL( CIMPSSAPSettings* aSAPSettings, TUint32 aUid )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::UpdateOldSAPL()" );

    iInTransaction = ETrue;
    CleanupStack::PushL( TCleanupItem( ReSetTransactionState, this ));
    User::LeaveIfError( iRepository->StartTransaction( CRepository::EReadWriteTransaction ) );
    iRepository->CleanupCancelTransactionPushL();

    SAPExistsL( aUid );
    TIMPSAccessGroup type( SAPTypeL( aUid ) );
    TIMPSSAPProtection protection( SAPProtectionL( aUid, type ) );

    if ( protection == ESAPBrandProtection )
        {
        CIMPSSAPSettings* settings = CIMPSSAPSettings::NewLC();
        GetSAPL( aUid, settings );
        if ( ( settings->SAPName().Compare( aSAPSettings->SAPName() ) != 0 ) )
            {
            // User is trying to modify protected values, leave
            User::Leave( KErrAccessDenied );
            }
        CleanupStack::PopAndDestroy( settings );
        }

    //Check for duplicate SAP names. SAPName in SAP with aUid is permitted to be
    //same as it is the one we are updating.
    if ( SAPNameExistsL( aSAPSettings->SAPName(), aUid, type ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    RIMPSReleaseArray tempArray;
    CleanupClosePushL( tempArray );

    //Encrypt password before storing has been removed
    //401-1808 - Ease of Instant Messaging SERVER customization
    HBufC* tmpPasswd = aSAPSettings->SAPUserPassword().AllocLC();
    HBufC* tmpHTTPPasswd = aSAPSettings->HTTPProxyUserPassword().AllocLC();
    TPtr passwdPtr( tmpPasswd->Des() );
    TPtr httpPasswdPtr( tmpHTTPPasswd->Des() );

    TIMPSCenRepOperations op( aUid, *iRepository );

    op.SetL( ESAPAddress, aSAPSettings->SAPAddress() );
    op.SetL( ESAPName, aSAPSettings->SAPName() );
    op.SetL( ESAPPort, static_cast< TInt >( aSAPSettings->SAPPort() ) );
    op.SetL( ESAPUserId, aSAPSettings->SAPUserId() );
    op.SetL( ESAPUserPassword, passwdPtr );
    op.SetL( EHTTPProxyUserId, aSAPSettings->HTTPProxyUserId() );
    op.SetL( EHTTPProxyUserPassword, httpPasswdPtr );
    op.SetL( EHTTPProxyAddress, aSAPSettings->HTTPProxyAddress() );
    op.SetL( EHTTPProxyPort, static_cast< TInt >( aSAPSettings->HTTPProxyPort() ) );

    HBufC* apName = DoGetAccessPointsNameL(
                        static_cast< TInt >( aSAPSettings->AccessPoint() ) );
    CleanupStack::PushL( apName );
    op.SetL( EAccessPoint, *apName );
    CleanupStack::PopAndDestroy( apName );

    op.SetL( EClientId, aSAPSettings->ClientId() );
    op.SetL( EServerAuthenticationName, aSAPSettings->ServerAuthenticationName() );
    op.SetL( EServerAuthenticationPassword, aSAPSettings->ServerAuthenticationPassword() );
    op.SetL( EServerAcceptedContentType, aSAPSettings->ServerAcceptedContentType() );
    op.SetL( EHighLevelServices, static_cast< TInt >( aSAPSettings->HighLevelServices() ) );
    op.SetL( EAuthorizationMode, static_cast< TInt >( aSAPSettings->AuthorizationMode() ) );
    op.SetL( EProtection, static_cast< TInt >( aSAPSettings->Protection() ) );

    //First delete old key-value pairs
    RArray<TUint32> foundPairs;
    CleanupClosePushL( foundPairs );
    iRepository->FindL( EKeyValuePairBase + aUid, KSAPPairsMask, foundPairs );
    TInt oldCount( foundPairs.Count() );

    for ( TInt i( 0 ); i < oldCount; ++i )
        {
        User::LeaveIfError( iRepository->Delete( foundPairs[i] ) );
        }

    //Create new pairs
    const RPointerArray< CIMPSSAPKeyValuePair >& pairs( aSAPSettings->KeyValuePairs().Pairs() );

    TInt count( pairs.Count() );

    for ( TInt i( 0 );i < count; ++i )
        {
        //this creates a new hbufc and transfers ownership here
        HBufC* valuePairFlat = pairs[i]->KeyValuePairFlatLC();
        TPtr valuePairFlatPtr( valuePairFlat->Des() );

        User::LeaveIfError( iRepository->Create( EKeyValuePairBase + i + aUid,
                                                 valuePairFlatPtr ) );

        // CRepository->Set() is not copying the pointed data during transaction
        // so these must be stored until the transaction has completed
        tempArray.AppendL( valuePairFlat );
        CleanupStack::Pop( valuePairFlat );
        }

    TUint32 err( KErrNone );
    User::LeaveIfError( iRepository->CommitTransaction( err ) );
    iInTransaction = EFalse;
	CleanupStack::PopAndDestroy( 6 ); // cleanupitem, Transaction, tmpPasswd, tmpHTTPPasswd, tempArray, foundPairs

    aSAPSettings->Body().DoSetUid( aUid );
    aSAPSettings->Body().DoSetAccessGroup( type );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::PopulateSAPSettingsListL()
// Populates SAP settings list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::PopulateSAPSettingsListL( CIMPSSAPSettingsList& aList,
                                               TIMPSAccessGroup aGroup, TBool aAscending )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::PopulateSAPSettingsListL()" );

    __ASSERT_ALWAYS(    aGroup & EIMPSAccessFilterAll,
                        Panic( EIMPSInvalidAccessGroup ) );

	TBool transaction( StartOwnTransactionL( CRepository::EReadTransaction ) );
    if ( transaction )
        {
        iRepository->CleanupCancelTransactionPushL();
        }

    CIMPSSAPSettingsListItem* listItem = NULL;
    TIMPSSAPSettingsListItemNameKey key( aAscending );

    RArray<TUint32> foundSAPs;
    CleanupClosePushL( foundSAPs );

    FindSAPsFromAccessGroupL( aGroup, foundSAPs );

    TInt count( foundSAPs.Count() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        HBufC16* name = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
        TPtr namePtr( name->Des() );

        TIMPSAccessGroup type( SAPTypeL( foundSAPs[i] ) );

        TInt err( iRepository->Get( foundSAPs[i] + ESAPName, namePtr ) );
        if ( err && ( err != KErrArgument ) )
            {
            User::Leave( err );
            }

        TInt protectionInt( ESAPNoProtection );
        err = iRepository->Get( foundSAPs[i] + EProtection, protectionInt );

        if ( err && ( err != KErrArgument ) )
            {
            User::Leave( err );
            }

        listItem = CIMPSSAPSettingsListItem::NewLC( *name, foundSAPs[ i ], type,
                                                    static_cast<TIMPSSAPProtection>( protectionInt ) );
        aList.InsertIsqAllowDuplicatesL( listItem, key ); //takes ownership of listitem
        CleanupStack::Pop( listItem );
        CleanupStack::PopAndDestroy( name );
        }
    CleanupStack::PopAndDestroy(); // foundSAPs
    if ( transaction )
        {
        TUint32 err( KErrNone );
        User::LeaveIfError( iRepository->CommitTransaction( err ) );
		iInTransaction = EFalse;
		CleanupStack::PopAndDestroy(2); // cleanupitem, transaction
        }
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::GetDefaultL()
// Gets Uid of default SAP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::GetDefaultL( TUint32& aUid, TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::GetDefaultL()" );

    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );
	TBool transaction( StartOwnTransactionL( CRepository::EReadTransaction ) );
    if ( transaction )
        {
        iRepository->CleanupCancelTransactionPushL();
        }

    TInt uid( 0 );
    if ( aGroup & EIMPSIMAccessGroup )
        {
        User::LeaveIfError( iRepository->Get( KDefaultIMSAPId, uid ) );
        }
    else
        {
        User::LeaveIfError( iRepository->Get( KDefaultPECSAPId, uid ) );
        }
    if ( transaction )
        {
        TUint32 err( KErrNone );
        User::LeaveIfError( iRepository->CommitTransaction( err ) );
		iInTransaction = EFalse;
		CleanupStack::PopAndDestroy(2); // cleanupitem, transaction
        }
    aUid = uid;
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::GetDefaultL()
// Gets default SAP to settings container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::GetDefaultL( CIMPSSAPSettings* aSAPSettings,
                                  TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::GetDefaultL() #2" );

    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

    TUint32 uid;
    GetDefaultL( uid, aGroup );
    GetSAPL( uid, aSAPSettings );
    }

// -----------------------------------------------------------------------------
// CIMPSSAPCenRep::SetToDefaultL()
// Sets given SAP to default SAP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIMPSSAPCenRep::SetToDefaultL( TUint32 aUid, TIMPSAccessGroup aGroup )
    {
    SSS_DP_TXT( "CIMPSSAPCenRep::SetToDefaultL()" );

    __ASSERT_ALWAYS( ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSIMAccessGroup ) ||
                     ( ( aGroup & EIMPSAccessFilterAll ) == EIMPSPECAccessGroup ),
                     Panic( EIMPSInvalidAccessGroup ) );

	TBool transaction( StartOwnTransactionL( CRepository::EReadWriteTransaction ) );
    if ( transaction )
        {
        iRepository->CleanupCancelTransactionPushL();
        }

    // check that aUid exists
    SAPExistsL( aUid );

    //and it belongs to the given group
    if ( SAPTypeL( aUid ) & aGroup )
        {
        if ( aGroup & EIMPSIMAccessGroup )
            {
            User::LeaveIfError( iRepository->Set( KDefaultIMSAPId,
                                                  static_cast<TInt>( aUid ) ) );
            }
        else
            {
            User::LeaveIfError( iRepository->Set( KDefaultPECSAPId,
                                                  static_cast<TInt>( aUid ) ) );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( transaction )
        {
        TUint32 err( KErrNone );
        User::LeaveIfError( iRepository->CommitTransaction( err ) );
		iInTransaction = EFalse;
		CleanupStack::PopAndDestroy(2); // cleanupitem, transaction
        }
    }

//401-1808 - Ease of Instant Messaging SERVER customization

// ---------------------------------------------------------
// CIMPSSAPCenRep::DoGetAccessPointsL
// ---------------------------------------------------------
//
TUint32 CIMPSSAPCenRep::DoGetAccessPointsL( const TDesC& aAPName )
    {
    TUint32 ap = 0;
    TInt err( KErrNone );

    // Iterate the IAP table from CommsDat
    CCommsDbTableView* table = NULL;

    table = iCommsDatabase->OpenViewMatchingTextLC( TPtrC( IAP ), TPtrC( COMMDB_NAME ), aAPName );

    err = table->GotoFirstRecord();

    if ( !err )
        {
        // Read IAP's ID
        //
        table->ReadUintL( TPtrC( COMMDB_ID ), ap );
        }

    CleanupStack::PopAndDestroy( table );

    return ap;
    }


// ---------------------------------------------------------
// CIMPSSAPCenRep::DoGetAccessPointsNameL
// ---------------------------------------------------------
//
HBufC* CIMPSSAPCenRep::DoGetAccessPointsNameL( const TUint32 aAP )
    {
    TInt err( KErrNone );
    HBufC* buf1 = NULL;

    // Iterate the IAP table from CommsDat
    CCommsDbTableView* table = NULL;
    table = iCommsDatabase->OpenViewMatchingUintLC( TPtrC( IAP ), TPtrC( COMMDB_ID ), aAP );
    err = table->GotoFirstRecord();

    if ( !err )
        {
        TInt    length1( 0 );

        // Read IAP's connection name
        //
        table->ReadColumnLengthL( TPtrC( COMMDB_NAME ), length1 );
        buf1 = HBufC::NewLC( length1 );
        TPtr ptr1( buf1->Des() );
        table->ReadTextL( TPtrC( COMMDB_NAME ), ptr1 );
        }
    else
        {
        buf1 = KNullDesC().AllocLC();
        }

    CleanupStack::Pop( buf1 );
    CleanupStack::PopAndDestroy( table );

    return buf1;
    }

// ---------------------------------------------------------
// CIMPSSAPCenRep::ReSetTransactionState
// ---------------------------------------------------------
//
void CIMPSSAPCenRep::ReSetTransactionState(TAny* ptr)
	{
	reinterpret_cast<CIMPSSAPCenRep*>( ptr )->ReSetTransactionFlag();
	}

// ---------------------------------------------------------
// CIMPSSAPCenRep::ReSetTransactionFlag
// ---------------------------------------------------------
//
void CIMPSSAPCenRep::ReSetTransactionFlag()
	{
	iInTransaction = EFalse;
	}
//End 401-1808 - Ease of Instant Messaging SERVER customization

//  End of File
