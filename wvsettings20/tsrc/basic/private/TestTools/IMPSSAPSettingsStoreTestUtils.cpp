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
* Description: TestUtils for SAP settings store module tests
*
*/


//  Include Files
#include <E32Math.h>
#include "f32file.h"
#include "IMPSSAPSettingsStoreTestUtils.h"
#include "CIMPSSAPSettingsStore.h"
#include "CIMPSSAPSettings.h"
#include "CIMPSSAPSettingsList.h"
#include "CIMPSSAPSettingsListItem.h"


//  CONSTANTS AND MACROS
//SAP strings and consts to use in filling
_LIT( KSAPName, "SAPName" );
_LIT( KDefaultSAPName, "Default SAP" );
_LIT( KSAPAddres, "SAPAddr" );
_LIT( KSAPUserId, "SAPUserId" );
_LIT( KSAPUserPassword, "SAPUserPwd" );
_LIT( KHTTPProxyAddres, "ProxyAddr" );
_LIT( KHTTPProxyUserId, "ProxyUserId" );
_LIT( KHTTPProxyUserPassword, "ProxyUserPwd" );
_LIT( KCLientId, "CLientId" );

_LIT( KServerAuthenticationUsername, "SAUsername" );
_LIT( KServerAuthenticationPassword, "SAPassword" );
_LIT( KServerAcceptedContentType, "application/vnd" );

_LIT( KKey8, "Key8" );
_LIT( KKey16, "Key16" );
_LIT( KKeyInt, "KeyInt" );

_LIT8( KOpaque8, "Value8" );
_LIT( KOpaque16, "Value16" );

const TInt KHighlevelServices = EIMPSServicesIM + EIMPSServicesPEC;

const TInt KAuthorizationMode = EIMPSAuthModeUnspecified;

const TInt KSAPPort = 100;
const TInt KHTTPProxyPort = 200;
const TInt KAP = 300;

const TInt KOpaqueInt = 1234;

//format string for SAP naming
_LIT( KNumberFormat, ": %d" );


//local data types
struct TSAPName
    {
    const TText*    iName;
    };

//Pre defined string array for SAP names
//NOTE! This must have atleast two items
LOCAL_D TSAPName const KSAPNames[] =
    {
        { _S( "abcd1" ) },
    { _S( "AAaaa" ) },
    { _S( "abcd2" ) },
    { _S( "AAAaa" ) },
    { _S( "Aaaaa" ) },
    { _S( "abcd0" ) },
    { _S( "abcde" ) },
    { _S( "kiiii" ) },
    { _S( "+*?=)" ) },
    { _S( "kkkkk" ) },
    { _S( "Oooyy" ) },
    { _S( "Öööyy" ) },
    { _S( "Äääyy" ) },
    { 0 }
    };




//---------------------------------------------------------------
// METHODS
//---------------------------------------------------------------
//
IMPSSAPSettingsStoreTestUtils::IMPSSAPSettingsStoreTestUtils()
    {
    }

IMPSSAPSettingsStoreTestUtils::~IMPSSAPSettingsStoreTestUtils()
    {
    }


//---------------------------------------------------------------
// Store deletion
//---------------------------------------------------------------
//
/*void IMPSSAPSettingsStoreTestUtils::DeleteStoresL( TUint aStoreId )
    {
    DeleteNormalStoreL( aStoreId );
    DeleteDefaultStoresL( aStoreId );
    }


void IMPSSAPSettingsStoreTestUtils::DeleteNormalStoreL( TUint aStoreId )
    {

    {
    TFileName fileName;
    AppendDbName( fileName, aStoreId );
    DoDeleteNormalFileL( fileName );
    }

    {
    TFileName fileName;
    AppendIniName( fileName, aStoreId );
    DoDeleteNormalFileL( fileName );
    }

    }
*/
/*
void IMPSSAPSettingsStoreTestUtils::DeleteDefaultStoresL( TUint aStoreId )
    {
    TFileName fileNameSearch;

        {
        TFileName dbFileName;
        AppendDbName( dbFileName, aStoreId, ETrue );
        fileNameSearch.Append( TParsePtrC( dbFileName ).Path() );
        fileNameSearch.Append( TParsePtrC( dbFileName ).NameAndExt() );
        }
    DoDeleteDefaultFilesL( fileNameSearch );

    fileNameSearch.Zero();
        {
        TFileName iniFileName;
        AppendIniName( iniFileName, aStoreId, ETrue );
        fileNameSearch.Append( TParsePtrC( iniFileName ).Path() );
        fileNameSearch.Append( TParsePtrC( iniFileName ).NameAndExt() );
        }


    DoDeleteDefaultFilesL( fileNameSearch );
    }

*/

/*
void IMPSSAPSettingsStoreTestUtils::UseDbL( TUint aStoreId,
                                          const TDesC* aSapDbToUse,
                                          const TDesC* aIniFileToUse )
    {
    DeleteNormalStoreL( aStoreId );

    if( aSapDbToUse )
        {
        TFileName sourceFile;
        sourceFile.Append( KSapDb_TestFileFolder );
        sourceFile.Append( *aSapDbToUse );

        TFileName destinationFile;
        AppendDbName( destinationFile, aStoreId, ETrue );
        CopyFileToNameL( sourceFile, destinationFile );
        }

    if( aIniFileToUse )
        {
        TFileName sourceFile;
        sourceFile.Append( KSapDb_TestFileFolder );
        sourceFile.Append( *aIniFileToUse );

        TFileName destinationFile;
        AppendIniName( destinationFile, aStoreId, ETrue );
        CopyFileToNameL( sourceFile, destinationFile );
        }
    }


void IMPSSAPSettingsStoreTestUtils::UseDefaultDbL( TUint aStoreId,
                                                 const TDesC& aSapDbToUse,
                                                 const TDesC* aIniFileToUse )
    {
    DeleteDefaultStoresL();

    {
    TFileName sourceFile;
    sourceFile.Append( KSapDb_TestFileFolder );
    sourceFile.Append( aSapDbToUse );

    TFileName destinationFile;
    destinationFile.Append( KDbDefaultFilesFolder );
    AppendDbName( destinationFile, aStoreId, EFalse );
    CopyFileToNameL( sourceFile, destinationFile );
    }

    if( aIniFileToUse )
        {
        TFileName sourceFile;
        sourceFile.Append( KSapDb_TestFileFolder );
        sourceFile.Append( *aIniFileToUse );

        TFileName destinationFile;
        destinationFile.Append( KDbDefaultFilesFolder );
        AppendIniName( destinationFile, aStoreId, EFalse );
        CopyFileToNameL( sourceFile, destinationFile );
        }
    }


//---------------------------------------------------------------
// Store size print
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::PrintStoreSizesL( TUint aStoreId )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TFileName fileName;
    fileName.Append( KDbFileNameHead );
    fileName.AppendNum( aStoreId );
    fileName.Append( KDbFileNameTail );

    TParsePtr parse( fileName );
    TPtrC fileNameAndExt = parse.NameAndExt();

    TEntry entry;
    TInt err;

    err = fs.Entry( fileName, entry );
    if( err == KErrNone )
        {
        RDebug::Print( _L("%S size: %d bytes"), &fileNameAndExt, entry.iSize );
        }

    if( err == KErrNotFound )
        {
        RDebug::Print( _L("File not found.") );
        err = KErrNone;
        }
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy(); //fs
    }

*/

//---------------------------------------------------------------
// SAP name handling
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::UnigueSAPNameL( TUint& aNameIndex,
                                                    CIMPSSAPSettings& aSAP )
    {
    aNameIndex++;
    HBufC* name = HBufC::NewLC( aSAP.SAPName().Length() + 10 );
    TPtr namePtr( name->Des() );

    namePtr.Copy( aSAP.SAPName() );
    if ( namePtr.Length() > 4 )
        {
        namePtr.Delete( namePtr.Length() - 4, 4 );
        }
    namePtr.AppendFormat( KNumberFormat, aNameIndex );

    aSAP.SetSAPNameL( namePtr );

    CleanupStack::PopAndDestroy(); //name
    }



//---------------------------------------------------------------
// SAP add
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( CIMPSSAPSettingsStore& aStoreToAdd,
                                                     TInt aHowMany,
                                                     TUint32& aFirstUid,
                                                     TUint32& aLastUid,
                                                     TIMPSAccessGroup aGroup )
    {
    __ASSERT_ALWAYS( ( aHowMany >= 2 ), User::Panic( _L( "Add count" ), 1 ) );

    CIMPSSAPSettings* settings = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( settings );

    TBuf< 15 > sapName;

    for ( TInt ii( 0 ); ii < aHowMany; ii++ )
        {
        sapName.Zero();
        sapName.Copy( KSAPName );
        sapName.AppendFormat( KNumberFormat, ii );

        settings->Reset();
        IMPSSAPSettingsStoreTestUtils::FillContainerL( *settings, sapName );

        TUint32 uid = aStoreToAdd.StoreNewSAPL( settings, aGroup );

        if ( ii == 0 )
            {
            aFirstUid = uid; //first id
            }

        aLastUid = uid; //last id
        }

    CleanupStack::PopAndDestroy(); //settings
    }



//---------------------------------------------------------------
// SAP add
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( CIMPSSAPSettingsStore& aStoreToAdd,
                                                     TInt aHowMany,
                                                     TIMPSAccessGroup aGroup )
    {
    TUint32 firstUid = 0;
    TUint32 lastUid = 0;

    AddSAPSettingsL( aStoreToAdd, aHowMany, firstUid, lastUid, aGroup );
    }



//---------------------------------------------------------------
// SAP add
//---------------------------------------------------------------
//
TInt IMPSSAPSettingsStoreTestUtils::AddSAPSettingsWithLiteralNamesL( CIMPSSAPSettingsStore& aStoreToAdd,
                                                                     TUint32& aFirstUid,
                                                                     TUint32& aLastUid,
                                                                     TIMPSAccessGroup aGroup )
    {
    CIMPSSAPSettings* settings = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( settings );
    TInt ii;

    for ( ii = 0 ; KSAPNames[ ii ].iName != NULL; ii++ )
        {
        settings->Reset();
        const TPtrC name = KSAPNames[ ii ].iName;
        FillContainerL( *settings, name );

        RDebug::Print( _L( "Storing literal name: %S " ), &name );
        TUint32 uid = aStoreToAdd.StoreNewSAPL( settings, aGroup );

        if ( ii == 0 )
            {
            aFirstUid = uid; //first id
            }

        aLastUid = uid; //last id
        }

    CleanupStack::PopAndDestroy(); //settings
    return ii;
    }


//---------------------------------------------------------------
// SAP deletion
//---------------------------------------------------------------
//
TInt IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( CIMPSSAPSettingsStore& aStoreToClear )
    {
    TInt deleteCount = 0;
    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();
    aStoreToClear.PopulateSAPSettingsListL( *list, EIMPSAccessFilterAll );

    RDebug::Print( _L( "delete all SAPCount: %d" ), aStoreToClear.SAPCountL( EIMPSAccessFilterAll ) );

    if ( aStoreToClear.SAPCountL( EIMPSAccessFilterAll ) >= 1 )
        {
        //clear the store
        const TInt listCount = list->Count();
        for ( TInt ii( 0 ); ii < listCount ; ii++ )
            {
            aStoreToClear.DeleteSAPL( ( *list )[ ii ]->Uid() );
            deleteCount++;
            }
        }

    CleanupStack::PopAndDestroy(); // list
    return deleteCount;
    }



//---------------------------------------------------------------
// SAP fill
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::RandomFillKey( TDes& aBuffer, TInt64& aSeed )
    {
    aBuffer.Zero();
    for ( TInt i( 0 ); i < aBuffer.MaxLength(); i++ )
        {
        TChar ch = Math::Rand( aSeed ) % 200 + 32;
        aBuffer.Append( ch );
        }
    }


void IMPSSAPSettingsStoreTestUtils::FillContainerL( CIMPSSAPSettings& aSettings,
                                                    const TDesC& aName )
    {
    if ( aName.Length() == 0 )
        {
        aSettings.SetSAPNameL( KSAPName );
        }
    else
        {
        aSettings.SetSAPNameL( aName );
        }

    aSettings.SetSAPAddressL( KSAPAddres );
    aSettings.SetSAPPort( KSAPPort );
    aSettings.SetSAPUserIdL( KSAPUserId );
    aSettings.SetSAPUserPasswordL( KSAPUserPassword );

    aSettings.SetHTTPProxyAddressL( KHTTPProxyAddres );
    aSettings.SetHTTPProxyUserIdL( KHTTPProxyUserId );
    aSettings.SetHTTPProxyUserPasswordL( KHTTPProxyUserPassword );
    aSettings.SetHTTPProxyPort( KHTTPProxyPort );

    aSettings.SetAccessPoint( KAP );
    aSettings.SetClientIdL( KCLientId );

    aSettings.SetHighLevelServices( KHighlevelServices );
    aSettings.SetAuthorizationMode( KAuthorizationMode );
    aSettings.SetServerAcceptedContentTypeL( KServerAcceptedContentType );
    aSettings.SetServerAuthenticationNameL( KServerAuthenticationUsername );
    aSettings.SetServerAuthenticationPasswordL( KServerAuthenticationPassword );

    aSettings.SetOpaqueDesC8( KKey8, KOpaque8 );
    aSettings.SetOpaqueDesC16( KKey16, KOpaque16 );
    aSettings.SetOpaqueInt( KKeyInt, KOpaqueInt );
    aSettings.SetAccessGroup( EIMPSIMAccessGroup );

    }

//---------------------------------------------------------------
// SAP fill
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::FillContainerPartiallyL( CIMPSSAPSettings& aSettings,
                                                             const TDesC& aName )
    {
    if ( aName.Length() == 0 )
        {
        aSettings.SetSAPNameL( KSAPName );
        }
    else
        {
        aSettings.SetSAPNameL( aName );
        }

    aSettings.SetSAPAddressL( KSAPAddres );
    aSettings.SetSAPUserPasswordL( KSAPUserPassword );

    aSettings.SetHTTPProxyUserIdL( KHTTPProxyUserId );
    aSettings.SetHTTPProxyPort( KHTTPProxyPort );
    aSettings.SetHTTPProxyUserPasswordL( KHTTPProxyUserPassword );
    aSettings.SetAccessGroup( EIMPSIMAccessGroup );
    }


//---------------------------------------------------------------
// SAP fill
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( CIMPSSAPSettings& aSettings )
    {
    TTime seedtime;
    seedtime.HomeTime();

    TInt64 seed = seedtime.Int64();

    HBufC* buf = HBufC::NewLC( 128 );
    TPtr tmp = buf->Des();
    RandomFillKey( tmp, seed );

    aSettings.SetSAPNameL( *buf );

    aSettings.SetSAPAddressL( *buf );
    aSettings.SetSAPPort( KMaxTInt32 );
    aSettings.SetSAPUserIdL( *buf );
    aSettings.SetSAPUserPasswordL( *buf );

    aSettings.SetHTTPProxyAddressL( *buf );
    aSettings.SetHTTPProxyUserIdL( *buf );
    aSettings.SetHTTPProxyUserPasswordL( *buf );
    aSettings.SetHTTPProxyPort( KMaxTInt32 );

    aSettings.SetAccessPoint( KMaxTUint );
    aSettings.SetClientIdL( *buf );

    aSettings.SetServerAcceptedContentTypeL( *buf );
    aSettings.SetServerAuthenticationNameL( *buf );
    aSettings.SetServerAuthenticationPasswordL( *buf );

    aSettings.SetAccessGroup( EIMPSIMAccessGroup );

    CleanupStack::PopAndDestroy(); //buf
    }


//---------------------------------------------------------------
// SAP fill
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::FillContainerEvenLongerDataL( CIMPSSAPSettings& aSettings )
    {
    TTime seedtime;
    seedtime.HomeTime();
    TInt64 seed = seedtime.Int64();

    HBufC* buf = HBufC::NewLC( 300 );
    TPtr tmp = buf->Des();
    RandomFillKey( tmp, seed );

    aSettings.SetSAPNameL( *buf );
    aSettings.SetSAPAddressL( *buf );
    aSettings.SetSAPPort( KMaxTInt32 );
    aSettings.SetSAPUserIdL( *buf );
    aSettings.SetSAPUserPasswordL( *buf );

    aSettings.SetHTTPProxyAddressL( *buf );
    aSettings.SetHTTPProxyUserIdL( *buf );
    aSettings.SetHTTPProxyUserPasswordL( *buf );
    aSettings.SetHTTPProxyPort( KMaxTInt32 );

    aSettings.SetAccessPoint( KMaxTUint );
    aSettings.SetClientIdL( *buf );

    aSettings.SetServerAcceptedContentTypeL( *buf );
    aSettings.SetServerAuthenticationNameL( *buf );
    aSettings.SetServerAuthenticationPasswordL( *buf );
    aSettings.SetAccessGroup( EIMPSIMAccessGroup );

    CleanupStack::PopAndDestroy(); //buf
    }


//---------------------------------------------------------------
// SAP data check
//---------------------------------------------------------------
//
TBool IMPSSAPSettingsStoreTestUtils::Equals( CIMPSSAPSettings& aFirst,
                                             CIMPSSAPSettings& aSecond )
    {

    if ( aFirst.SAPName().Compare( aSecond.SAPName() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.SAPAddress().Compare( aSecond.SAPAddress() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.HTTPProxyAddress().Compare( aSecond.HTTPProxyAddress() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.SAPUserId().Compare( aSecond.SAPUserId() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.SAPUserPassword().Compare( aSecond.SAPUserPassword() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.HTTPProxyUserId().Compare( aSecond.HTTPProxyUserId() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.HTTPProxyUserPassword().Compare( aSecond.HTTPProxyUserPassword() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.SAPPort() != aSecond.SAPPort() )
        {
        return EFalse;
        }

    if ( aFirst.HTTPProxyPort() != aSecond.HTTPProxyPort() )
        {
        return EFalse;
        }

    if ( aFirst.AccessPoint() != aSecond.AccessPoint() )
        {
        return EFalse;
        }

    if ( aFirst.ClientId().Compare( aSecond.ClientId() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.HighLevelServices() != aSecond.HighLevelServices() )
        {
        return EFalse;
        }

    if ( aFirst.AuthorizationMode() != aSecond.AuthorizationMode() )
        {
        return EFalse;
        }

    if ( aFirst.ServerAuthenticationName().Compare( aSecond.ServerAuthenticationName() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.ServerAuthenticationPassword().Compare( aSecond.ServerAuthenticationPassword() ) != 0 )
        {
        return EFalse;
        }

    if ( aFirst.Protection() != aSecond.Protection() )
        {
        return EFalse;
        }

    return ETrue;
    }


/*

//---------------------------------------------------------------
// Store name access
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::AppendDbName( TFileName& aFileName,
                                                TUint aStoreId,
                                                TBool aFullPath )
    {
    if( aFullPath )
        {
        aFileName.Append( KDbFilesFolder );
        }

    aFileName.Append( KDbFileNameHead );
    aFileName.AppendNum( (TUint) aStoreId );
    aFileName.Append( KDbFileNameTail );
    }



void IMPSSAPSettingsStoreTestUtils::AppendIniName( TFileName& aFileName,
                                                 TUint aStoreId,
                                                 TBool aFullPath )
    {
    if( aFullPath )
        {
        aFileName.Append( KDbFilesFolder );
        }

    aFileName.Append( KDbIniFileNameHead );
    aFileName.AppendNum( (TUint) aStoreId );
    aFileName.Append( KDbIniFileNameTail );
    }


//---------------------------------------------------------------
// Raw file copy & delete
//---------------------------------------------------------------
//
void IMPSSAPSettingsStoreTestUtils::CopyFileToNameL( const TDesC& aSourceFile,
                                                   const TDesC& aDestinationFile )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );

    RDebug::Print( _L("Copying [%S] to [%S]"), &aSourceFile, &aDestinationFile );
    User::LeaveIfError( fileMan->Copy( aSourceFile, aDestinationFile ) );

    //make sure the file is now there
    TEntry entry;
    User::LeaveIfError( fs.Entry( aDestinationFile, entry ) );

    CleanupStack::PopAndDestroy( 2 ); //fileMan, fs
    }



void IMPSSAPSettingsStoreTestUtils::DoDeleteDefaultFilesL( const TDesC& aDefFileSearchName )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );


    TFindFile defaultDbFinder( fs );
    TInt defaultFound = defaultDbFinder.FindByDir( aDefFileSearchName, KNullDesC );
    while( defaultFound == KErrNone )
        {
        //default file found somewhere
        //try to delete it if ok
        TParsePtrC driveCheck( defaultDbFinder.File() );
        if( driveCheck.Drive().Compare( _L("C:") ) != 0 )
            {
            //found default db from somewhere else than C:
            if( driveCheck.Drive().Compare( _L("Z:") ) == 0 )
                {
                //found default from Z: ==> can't be removed EPOC code
                RDebug::Print( _L("NOTE!! Default file [%S] found from ROM!!"), &defaultDbFinder.File() );
                RDebug::Print( _L("NOTE!! Default file [%S] found from ROM!!"), &defaultDbFinder.File() );
                User::Panic( _L("Def DB on ROM"), 1 );
                }

            TInt err = fileMan->Delete( defaultDbFinder.File() );
            if( err != KErrNone )
                {
                RDebug::Print( _L("IMPSSAPSettingsStoreTestUtils:: Default file to delete [%S] failed: %d"),
                               &defaultDbFinder.File(), err );
                User::LeaveIfError( err );
                }
            }

        defaultFound = defaultDbFinder.Find();
        }

    CleanupStack::PopAndDestroy( 2 ); //fileMan, fs
    }


void IMPSSAPSettingsStoreTestUtils::DoDeleteNormalFileL( const TDesC& aFile )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );

    TInt err = fileMan->Attribs( aFile,
                                 KEntryAttNormal,
                                 KEntryAttReadOnly | KEntryAttArchive | KEntryAttSystem,
                                 TTime(0),
                                 0 );

    if( (err != KErrNone) && (err != KErrNotFound) )
        {
        RDebug::Print( _L("IMPSSAPSettingsStoreTestUtils::Normal delete [%S] failed: %d"), &aFile, err );
        User::LeaveIfError( err );
        }

    err = fileMan->Delete( aFile );
    if( (err != KErrNone) && (err != KErrNotFound) )
        {
        RDebug::Print( _L("IMPSSAPSettingsStoreTestUtils::Normal delete [%S] failed: %d"), &aFile, err );
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( 2 ); //fileMan, fs
    }
*/