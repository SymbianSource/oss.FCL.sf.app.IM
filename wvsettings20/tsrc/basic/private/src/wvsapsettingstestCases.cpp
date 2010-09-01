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
* Description: SAP settings test cases
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <e32debug.h>

// Classes under test include
#include "IMPSSAPSettingsStoreTestUtils.h"

#include "wvsapsettingstest.h"

#include <S32strm.h>
#include <S32mem.h>

#include <CImpssapsettingsstore.h>

#include <CImpssapsettings.h>

#include <CIMPSSAPSettingsList.h>

#include <IMPSSAPSettingsStoreDefinitions.h>
#include <IMPSSAPSettingsStoreCenRepUids.h>
#include <IMPSSAPSettingsStorePanics.h>

//Obersever class

#include "WVSAPSettingsObserverTestCases.h"

#define STIF_TRAP( err, operation ) \
    TRAP( err, operation ); \
    if( err == KErrNoMemory ) \
        { \
        User::Leave( KErrNoMemory ); \
        } \
     

//FORWARD DECLARATIONS

class CIMPSSAPSettingsStore;
class CIMPSSAPSettings;
class cimpssapsettingsstoretestobserver;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cwvsapsettingstest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
//
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo Cwvsapsettingstest::Case (
    const TInt aCaseNumber ) const
    {

    /**
    * To add new test cases, implement new test case function and add new
    * line to KCases array specify the name of the case and the function
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to
    *    wvsapsettingstest.cpp file and to wvsapsettingstest.h
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and
    * TInt value for last heap memory allocation failure.
    *
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation
    * failure and TInt value for last heap memory allocation failure.
    */

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array

        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g.
        // Cwvsapsettingstest::PrintTest. Otherwise the compiler
        // gives errors.

        //FUNCENTRY( Cwvsapsettingstest::PrintTest ),
        //ENTRY( "Loop test", Cwvsapsettingstest::LoopTest ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", Cwvsapsettingstest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( Cwvsapsettingstest::PrintTest, ETrue, 1, 3 ),

        ENTRY( "SetupEmpty SAP", Cwvsapsettingstest::SetupEmptySAPL ),
        ENTRY( "Setup Partially Filled SAP", Cwvsapsettingstest::SetupPartiallyFilledSAPL ),
        ENTRY( "Setup Complete SAP", Cwvsapsettingstest::SetupFullSAPL ),
        ENTRY( "Test Data Set and Get APIs", Cwvsapsettingstest::Test_DataSetAndGetL ),
        ENTRY( "TestBrand Protection Store", Cwvsapsettingstest::TestBrandProtection_StoreL ),
        ENTRY( "TestBrand Protection Update", Cwvsapsettingstest::TestBrandProtection_UpdateL ),
        ENTRY( "TestBrand Protection Delete", Cwvsapsettingstest::TestBrandProtection_DeleteL ),
        ENTRY( "Test Login Count", Cwvsapsettingstest::TestLoginCountHandlingL ),
        ENTRY( "Test Login Count Storage", Cwvsapsettingstest::TestLoginCountStorageL ),
        ENTRY( "Test Externalize and Internalize", Cwvsapsettingstest::TestSAPContainerExternalizeInternalizeL ),
        ENTRY( "Test Pack-Unpack", Cwvsapsettingstest::TestSAPContainerPackUnpackL ),
        ENTRY( "Test Key Value pair", Cwvsapsettingstest::TestSAPContainerKeyValuePairsL ),
        ENTRY( "Test SAP Container Copy", Cwvsapsettingstest::TestSAPContainerCopyL ),
        ENTRY( "Test PopulateSettingsList", Cwvsapsettingstest::TestPopulateSettingsListL ),
        ENTRY( "Test SettingsList Ascend", Cwvsapsettingstest::TestIterateTroughSettingsListAscendL ),
        ENTRY( "Test SettingsList Descend", Cwvsapsettingstest::TestIterateTroughSettingsListDescendL ),
        ENTRY( "Test Obs for Adding SAP", Cwvsapsettingstest::TestObsForAddingSAP ),
        ENTRY( "Test Obs for Remove SAP", Cwvsapsettingstest::TestObsForRemoveSAP ),

        };

    // Verify that case number is valid
    if ( ( TUint ) aCaseNumber >= sizeof( KCases ) /
         sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( ( const TText* ) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// Cwvsapsettingstest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt Cwvsapsettingstest::PrintTest(
    TTestResult& aResult )
    {
    /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );

    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );

    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cwvsapsettingstest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt Cwvsapsettingstest::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for ( TInt i = 0; i < 10; i++ )
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i );
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

TInt Cwvsapsettingstest::SetupEmptySAPL( TTestResult& aResult )
    {

    //Create and initialize iSAP and iTempSAP.
    iSAP = CIMPSSAPSettings::NewL();
    iTempSAP = CIMPSSAPSettings::NewL();

    _LIT ( KNullValue , "" );
    //This is the test case for empty SAP,
    //verify the values with empty strings and empty values.
    //This will also test the Get methods on null and empty values.

    if ( ( iSAP->SAPName() != KNullValue ) ||
         ( iSAP->SAPUserPassword() != KNullValue ) ||
         ( iSAP->HTTPProxyAddress() != KNullValue ) ||
         ( iSAP->HTTPProxyUserPassword() != KNullValue ) ||
         ( iSAP->AccessPoint() != 0 ) || //Check the default value of AccessPoint
         ( iSAP->AccessGroup() != EIMPSNoAccessGroup )

       )

        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }

    }


TInt Cwvsapsettingstest::SetupPartiallyFilledSAPL( TTestResult& aResult )
    {

    iSAP = CIMPSSAPSettings::NewL();
    iTempSAP = CIMPSSAPSettings::NewL();

    iSAP->SetSAPNameL( _L( "name" ) );
    iSAP->SetSAPUserPasswordL( _L( "pass" ) );
    iSAP->SetHTTPProxyAddressL( KNullDesC );
    iSAP->SetHTTPProxyUserPasswordL( KNullDesC );
    iSAP->SetAccessPoint( 123 );
    iSAP->SetAccessGroup( EIMPSIMAccessGroup );


    //Use the getter methods and check if iSAP is set to
    //the values that are passed.
    _LIT( KName , "name" );
    _LIT( KPassword, "pass" );

    if ( ( iSAP->SAPName() != KName ) ||
         ( iSAP->SAPUserPassword() != KPassword ) ||
         ( iSAP->HTTPProxyAddress() != KNullDesC ) ||
         ( iSAP->HTTPProxyUserPassword() != KNullDesC ) ||
         ( iSAP->AccessPoint() != 123 ) || //Check the default value of AccessPoint
         ( iSAP->AccessGroup() != EIMPSIMAccessGroup )

       )

        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }

    }

TInt Cwvsapsettingstest::SetupFullSAPL( TTestResult& aResult )
    {

    iSAP = CIMPSSAPSettings::NewL();
    iTempSAP = CIMPSSAPSettings::NewL();

    IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( *iSAP );
    iSAP->ProtectL( ESAPBrandProtection );

    return 0;
    }



// TEST METHODS
TInt Cwvsapsettingstest::Test_DataSetAndGetL( TTestResult& aResult )
    {

    SetupSchedulerL();

    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewL();
    CIMPSSAPSettings* sap1 = CIMPSSAPSettings::NewL();


    TInt err = KErrNone;

    _LIT( KDummy, "strng" );

    sap->ProtectL( ESAPNoProtection );
    sap->SetSAPNameL( _L( "strng" ) );
    sap->SetSAPAddressL( _L( "strng" ) ) ;
    sap->SetSAPPort( 1 );
    sap->SetSAPUserIdL( _L( "strng" ) );
    sap->SetSAPUserPasswordL( _L( "strng" ) );
    sap->SetHTTPProxyAddressL( _L( "strng" ) );
    sap->SetHTTPProxyUserIdL( _L( "strng" ) );
    sap->SetHTTPProxyUserPasswordL( _L( "strng" ) );
    sap->SetHTTPProxyPort( 1 );

    sap->SetAccessPoint( 1 );
    sap->SetClientIdL( _L( "strng" ) );
    sap->ProtectL( ESAPBrandProtection );

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 uid = 0;
    uid = iStore->StoreNewSAPL( sap, EIMPSIMAccessGroup ) ;

    iStore->GetSAPL( uid, sap1 );

    // Set APIs are alreday tested in test cases (1) and (2).
    //Use the get APIs for testing.

    if (	( sap1->SAPName() != KDummy ) ||
         ( sap1->SAPAddress() != KDummy ) ||
         ( sap1->SAPPort() != 1 ) ||
         ( sap1->SAPUserId() != KDummy ) ||
         ( sap1->SAPUserPassword() != KDummy ) ||
         ( sap1->SAPUserPassword() != KDummy ) ||
         ( sap1->HTTPProxyAddress() != KDummy ) ||
         ( sap1->HTTPProxyUserId() != KDummy ) ||
         ( sap1->HTTPProxyUserPassword() != KDummy ) ||
         ( sap1->HTTPProxyPort() != 1 ) ||
         ( sap1->AccessPoint() != 1 ) ||
         ( sap1->ClientId() != KDummy )
       )
        {
        err = KErrArgument;
        }

    CloseScheduler();

    return err;
    }


TInt Cwvsapsettingstest::TestBrandProtection_StoreL( TTestResult& aResult )
    {

    SetupSchedulerL();

    CIMPSSAPSettings* sap1 = CIMPSSAPSettings::NewLC();

    //Create the SettingsStore and delete any existing Settings
    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( *sap1 );

    //Fill the SAPContainer
    ///IMPSSAPSettingsStoreTestUtils::FillContainerL(*sap1, _L("SettingsFilled") );
    sap1->ProtectL( ESAPBrandProtection );

    //TUint32 uid = iStore->StoreNewSAPL( sap1, EIMPSIMAccessGroup );


    //_LIT(Kname, "newsap1");
    //sap1->SetSAPNameL(Kname);

    TUint32 uid = 0;
    TRAPD( err, uid = iStore->StoreNewSAPL( sap1, EIMPSIMAccessGroup ) );


    //Null SAP settings in sap2.
    CIMPSSAPSettings* sap2 = CIMPSSAPSettings::NewLC();

    if ( err == KErrNone )
        {
        //See whether sap1 is correctly stored or not using Get method.
        iStore->GetSAPL( uid, sap2 );
        }

    if ( IMPSSAPSettingsStoreTestUtils::Equals( *sap1, *sap2 ) )
        {
        err = KErrNone;
        }
    else
        {
        err = KErrArgument;
        }

    CleanupStack::PopAndDestroy( 2 ); //sap1 , sap2
    CloseScheduler();

    return err;
    }


TInt Cwvsapsettingstest::TestBrandProtection_UpdateL( TTestResult& aResult )
    {

    SetupSchedulerL();

    CIMPSSAPSettings* sap1 = CIMPSSAPSettings::NewLC();
    IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( *sap1 );
    sap1->ProtectL( ESAPBrandProtection );

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 uid = iStore->StoreNewSAPL( sap1, EIMPSIMAccessGroup );

    sap1->SetAccessPoint( 1234 );
    iStore->UpdateOldSAPL( sap1, uid );

    CIMPSSAPSettings* sap2 = CIMPSSAPSettings::NewLC();
    iStore->GetSAPL( uid, sap2 );

    TInt err;

    //Test whether the update happened correctly.
    if ( IMPSSAPSettingsStoreTestUtils::Equals( *sap1, *sap2 ) )
        {
        err = KErrNone;
        }
    else
        {
        err = KErrArgument;
        }

    CleanupStack::PopAndDestroy( 2 ); //sap1 , sap2, istore
    CloseScheduler();

    return err;

    }


TInt Cwvsapsettingstest::TestBrandProtection_DeleteL( TTestResult& aResult )
    {

    SetupSchedulerL();

    CIMPSSAPSettings* sap1 = CIMPSSAPSettings::NewLC();
    IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( *sap1 );
    sap1->ProtectL( ESAPBrandProtection );

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 uid = iStore->StoreNewSAPL( sap1, EIMPSIMAccessGroup );

    TInt err = KErrNone;

    iStore->DeleteSAPL( uid );

    //We stored one new SAP, deleted the same sap1.
    //After deletion, the SAP count on the store must be 0.
    //If not 0, return error.

    if ( iStore->SAPCountL( EIMPSIMAccessGroup ) != 0 )
        {
        err = KErrGeneral;
        }

    CleanupStack::PopAndDestroy( 1 ); //sap1

    CloseScheduler();

    return err;


    }


TInt Cwvsapsettingstest::TestLoginCountHandlingL( TTestResult& aResult )
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    TInt err = KErrNone;
    TInt err1 = KErrNone;

    //Initially LoginCount should be 0.
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        };

    sap->IncreaseLoginCount();
    if ( sap->LoginCount() != 1 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->SetSAPNameL( _L( "strng" ) );
    if ( sap->LoginCount() != 1 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    sap->IncreaseLoginCount();
    sap->SetSAPAddressL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    sap->IncreaseLoginCount();
    sap->SetSAPPort( 1 );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->IncreaseLoginCount();
    sap->SetSAPUserIdL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    sap->IncreaseLoginCount();
    sap->SetSAPUserPasswordL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->IncreaseLoginCount();
    sap->SetHTTPProxyAddressL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    sap->IncreaseLoginCount();
    sap->SetHTTPProxyUserIdL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->IncreaseLoginCount();
    sap->SetHTTPProxyUserPasswordL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    sap->IncreaseLoginCount();
    sap->SetHTTPProxyPort( 1 );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->IncreaseLoginCount();
    sap->SetAccessPoint( 1 );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;

    sap->IncreaseLoginCount();
    sap->SetClientIdL( _L( "strng" ) );
    if ( sap->LoginCount() != 0 )
        {
        err1 = KErrArgument;
        }
    err &= err1;


    CleanupStack::PopAndDestroy(); //sap

    if ( err == KErrNone )
        {
        return err;
        }
    else
        {
        return KErrArgument;
        }

    }


TInt Cwvsapsettingstest::TestLoginCountStorageL( TTestResult& aResult )
    {

    SetupSchedulerL();

    TInt err = KErrGeneral;

    CIMPSSAPSettings* sap1 = CIMPSSAPSettings::NewLC();

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    IMPSSAPSettingsStoreTestUtils::FillContainerLongDataL( *sap1 );

    //make the login count in sap1 as 2.
    sap1->IncreaseLoginCount();
    sap1->IncreaseLoginCount();

    //Login count has become 2.
    if ( sap1->LoginCount() == 2 )
        {
        err = KErrNone;
        }

    //store sap1 and get the uid of the transaction
    TUint32 uid = iStore->StoreNewSAPL( sap1, EIMPSIMAccessGroup );

    CIMPSSAPSettings* sap2 = CIMPSSAPSettings::NewLC();

    //Get the sap stored in iStore in sap2.
    iStore->GetSAPL( uid, sap2 );


    //Check the previous error code and login count of sap2 which has
    //just been read from the iStore

    if ( ( sap2->LoginCount() == 2 ) && ( err == KErrNone ) )
        {
        err = KErrNone;
        }

    CleanupStack::PopAndDestroy( 2 ); //sap1 , sap2
    CloseScheduler();

    return err;
    }


TInt Cwvsapsettingstest::TestSAPContainerExternalizeInternalizeL( TTestResult& aResult )
    {
    CBufSeg* buffer = CBufSeg::NewL( 50000 );
    CleanupStack::PushL( buffer );

    RBufWriteStream wstream;
    wstream.Open( *buffer );
    CleanupClosePushL( wstream );

    CIMPSSAPSettings* iSAP = CIMPSSAPSettings::NewLC();


    iSAP->ExternalizeL( wstream );
    CleanupStack::PopAndDestroy(); //wstream

    RBufReadStream rstream;
    rstream.Open( *buffer );
    CleanupClosePushL( rstream );

    CIMPSSAPSettings* iTempSAP = CIMPSSAPSettings::NewLC();
    iTempSAP->InternalizeL( rstream );

    TInt err = KErrNone;

    if ( IMPSSAPSettingsStoreTestUtils::Equals( *iTempSAP, *iSAP ) )
        {
        //Internalized and Externalized strings are equal,
        //Just as a check also match Connection Settings and User Settings
        if ( iTempSAP->EqualConnectionSettings( *iSAP )  &&
             iTempSAP->EqualUserSettings( *iSAP ) )
            {
            err = KErrNone;
            }

        }
    else
        {
        err = KErrGeneral;
        }


    CleanupStack::PopAndDestroy(); //rstream
    CleanupStack::PopAndDestroy(); //buffer

    CleanupStack::PopAndDestroy( 2 );//iSAP,iTempSAP

    return err;
    }

TInt Cwvsapsettingstest::TestSAPContainerPackUnpackL( TTestResult& aResult )
    {

    CIMPSSAPSettings* iSAP = CIMPSSAPSettings::NewLC();


    // Pack the SAP
    HBufC8* buff = iSAP->PackLC();

    CIMPSSAPSettings* iTempSAP = CIMPSSAPSettings::NewLC();

    // Unpack the SAP
    TPtrC8 buffPtr( buff->Des() );
    iTempSAP->UnPackL( buffPtr );

    TInt err;

    if ( IMPSSAPSettingsStoreTestUtils::Equals( *iTempSAP, *iSAP ) )
        {
        //Internalized and Externalized strings are equal,
        //Just as a check also match Connection Settings and User Settings
        if ( iTempSAP->EqualConnectionSettings( *iSAP )  &&
             iTempSAP->EqualUserSettings( *iSAP ) )
            {
            err = KErrNone;
            }
        else
            {
            err = KErrArgument;
            }
        }
    else
        {
        err = KErrGeneral;
        }

    CleanupStack::PopAndDestroy(); // buff
    CleanupStack::PopAndDestroy( 2 );//iSAP,iTempSAP

    return err;
    }

TInt Cwvsapsettingstest::TestSAPContainerKeyValuePairsL( TTestResult& aResult )
    {

    CIMPSSAPSettings* iSAP = CIMPSSAPSettings::NewLC();
    CIMPSSAPSettings* iTempSAP = CIMPSSAPSettings::NewLC();

    //Test case for SetOpaqueInt , GetOpaqueInt and DeleteOpaque
    _LIT( KKey16, "key16" );
    _LIT( KKey8, "key8" );
    _LIT( KKeyInt, "keyint" );

    _LIT( KValue16, "value16" );
    _LIT8( KValue8, "value8" );
    const TInt KValueInt = 500;

    iSAP->SetOpaqueDesC16( KKey16, KValue16 );
    iSAP->SetOpaqueDesC8( KKey8, KValue8 );
    iSAP->SetOpaqueInt( KKeyInt, KValueInt );

    TPtrC16 stored16;
    TPtrC8 stored8;
    TInt storedInt;

    iSAP->GetOpaqueDesC16( KKey16, stored16 );
    iSAP->GetOpaqueDesC8( KKey8, stored8 );
    iSAP->GetOpaqueInt( KKeyInt, storedInt );

    TInt err, err1, err2;
    if ( ( stored16.Compare( KValue16 ) == 0 ) || ( stored8.Compare( KValue8 ) == 0 )
         || ( storedInt == KValueInt ) )
        {
        err1 = KErrNone;

        }
    else
        {
        err1 = KErrArgument;
        }
    if ( ( iSAP->DeleteOpaque( KKey16 ) == KErrNone ) || ( iSAP->DeleteOpaque( KKey8 ) == KErrNone )
         || ( iSAP->DeleteOpaque( KKeyInt ) == KErrNone ) )
        {
        err2 = KErrNone;
        }
    else
        {
        err2 = KErrArgument;
        }

    err = err1 & err2;

    CleanupStack::PopAndDestroy( 2 );//iSAP,iTempSAP

    return err;

    }

TInt Cwvsapsettingstest::TestSAPContainerCopyL( TTestResult& aResult )
    {

    TInt err;

    CIMPSSAPSettings* iSAP = CIMPSSAPSettings::NewLC();
    CIMPSSAPSettings* destination = CIMPSSAPSettings::NewLC();
    destination->CopyL( *iSAP );

    if ( IMPSSAPSettingsStoreTestUtils::Equals( *destination, *iSAP ) )
        {
        if ( destination->AccessGroup() == iSAP->AccessGroup() )
            {
            err = KErrNone;
            }
        else
            {
            err = KErrArgument;
            }
        }
    else
        {
        err = KErrArgument;
        }

    CleanupStack::PopAndDestroy( 2 ); // destination , iSAP

    return err;
    }


TInt Cwvsapsettingstest::TestPopulateSettingsListL( TTestResult& aResult )
    {
    //test:
    // 1. no SAPs defined
    // 2. 1 SAP defined
    // 3. 2 SAPs defined
    // 4. 10 SAPs defined
    // 5. 30 SAPs defined

    //Install the scheduler
    SetupSchedulerL();

    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();

    //Instantiate SettingsStore with 1 entry
    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    CIMPSSAPSettings* settings = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( settings );

    IMPSSAPSettingsStoreTestUtils::FillContainerPartiallyL( *settings, _L( "PartiallyFilled" ) );
    iStore->StoreNewSAPL( settings, EIMPSIMAccessGroup );

    iStoredCount = 1;

    iStore->PopulateSAPSettingsListL( *list, EIMPSIMAccessGroup );
    //verify list count
    TUint32 count( 0 );
    count = list->MdcaCount();

    TInt err1 = KErrNone;

    if ( count == iStoredCount )
        {
        err1 = KErrNone;
        }
    else err1 = KErrArgument ;

    //verify direct store count

    TInt err2 = KErrNone;

    //istore must be equal to the number of elements on the SettingsList,
    //else there is some problem.
    if ( iStore->SAPCountL( EIMPSIMAccessGroup ) == iStoredCount )
        {
        err2 = KErrNone;
        }
    else err2 = KErrArgument;

    if ( err1 || err2 != KErrNone )
        {
        //return with a failure code since there was an error
        //since we are returning at this point itself,
        //PopandDestroy the list on the cleanupstack.

        CleanupStack::PopAndDestroy( 3 ); //list(CIMPSSAPSettingsList) ,iStore, settings

        iStoredCount = 0;

        CloseScheduler();
        return KErrArgument;
        }


    TUint32 uid;
    for ( TInt ii( 0 ); ii < iStoredCount; ii++  )
        {
        list->MdcaPoint( ii );
        uid = list->UidForIndex( ii );

        //From the uid, try to access the listitem.
        const CIMPSSAPSettingsListItem* item = list->ListItemForUid( uid  );
        item->Name();
        item->Uid();
        }

    CleanupStack::PopAndDestroy( 2 ); //list(CIMPSSAPSettingsList), settings
    CloseScheduler();

    iStoredCount = 0;

    return KErrNone;
    }

TInt Cwvsapsettingstest::TestIterateTroughSettingsListAscendL( TTestResult& aResult )
    {
    //test:
    // 1. no SAPs defined
    // 2. 1 SAP defined
    // 3. 2 SAPs defined
    // 4. 10 SAPs defined

    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();

    SetupSchedulerL();

    //Instantiate SettingsStore.

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 dummy1, dummy2;
    iStoredCount = 10;
    IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( *iStore,
                                                    iStoredCount,
                                                    dummy1,
                                                    dummy2,
                                                    EIMPSIMAccessGroup );

    iStore->PopulateSAPSettingsListL( *list, EIMPSIMAccessGroup );

    TInt err = KErrNone;

    if ( list->MdcaCount() == iStoredCount )
        {
        err = KErrNone;
        }
    else err = KErrArgument;

    if ( err == KErrArgument )
        {

        //return from the test case with an error code.
        CleanupStack::PopAndDestroy( 1 ); //list

        CloseScheduler();
        return err;
        }

    RDebug::Print( _L( "Pos:  name   [uid]" ) );
    for ( TInt ii( 0 ); ii < iStoredCount; ii++  )
        {
        const TPtrC name = list->MdcaPoint( ii );
        RDebug::Print( _L( "%d: %S [%u]" ), ii, &name, list->UidForIndex( ii ) );
        }

    CleanupStack::PopAndDestroy( 1 ); //list
    CloseScheduler();
    return KErrNone;

    }


TInt Cwvsapsettingstest::TestIterateTroughSettingsListDescendL( TTestResult& aResult )
    {
    //test:
    // 1. no SAPs defined
    // 2. 1 SAP defined
    // 3. 2 SAPs defined
    // 4. 10 SAPs defined

    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewL();
    CleanupStack::PushL( list );


    SetupSchedulerL();

    //Instantiate SettingsStore.

    iStore = CIMPSSAPSettingsStore::NewL();
    IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 dummy1, dummy2;
    iStoredCount = 10;
    IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( *iStore,
                                                    iStoredCount,
                                                    dummy1,
                                                    dummy2,
                                                    EIMPSIMAccessGroup );

    iStore->PopulateSAPSettingsListL( *list, EIMPSIMAccessGroup, EFalse );

    TInt err = KErrNone;
    if ( list->MdcaCount() == iStoredCount )
        {
        err = KErrNone;
        }
    else err = KErrArgument;

    RDebug::Print( _L( "Pos:  name   [uid]" ) );
    for ( TInt ii( 0 ); ii < iStoredCount; ii++  )
        {
        const TPtrC name = list->MdcaPoint( ii );
        RDebug::Print( _L( "%d: %S [%u]" ), ii, &name, list->UidForIndex( ii ) );
        }


    CleanupStack::PopAndDestroy( 1 ); //list

    CloseScheduler();
    return err;
    }


/* TInt Cwvsapsettingstest::TestDeleteSettingsListL(TTestResult& aResult)
    {

    //First setup the Store with 3 lists say.

    CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();

    SetupSchedulerL();

    //Instantiate SettingsStore.

    iStore = CIMPSSAPSettingsStore::NewLC();
	IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 dummy1, dummy2;
    iStoredCount = 3;
    IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( *iStore,
                                                  iStoredCount,
                                                  dummy1,
                                                  dummy2,
                                                  EIMPSIMAccessGroup );

    iStore->PopulateSAPSettingsListL( *list, EIMPSIMAccessGroup );

    TInt err = KErrNone;

    if( list->MdcaCount() == iStoredCount )
    {
    	err = KErrNone;
    }
    else err = KErrArgument;

    if (err == KErrArgument)
    {

    	//return from the test case with an error code.
    	CleanupStack::PopAndDestroy(2); //list, iStore

    	CloseScheduler();
    	return err;
    }

    RDebug::Print( _L("Pos:  name   [uid]") );
    for( TInt ii(0); ii<iStoredCount; ii++  )
        {
        const TPtrC name = list->MdcaPoint( ii );
        RDebug::Print( _L("%d: %S [%u]"), ii, &name, list->UidForIndex( ii ) );
        }


  	//test:
    // 1. delete one of the lists from the store.

    TInt startCount( list->Count() );
    iList->Delete( 3 );
    EUNIT_ASSERT( startCount == list->Count() + 1 );


    //test:
    // 1. delete many

    TInt startCount( list->Count() );
    iList->Delete( 1, 3 );
    EUNIT_ASSERT( startCount == list->Count() + 3 );

    //test:
    // 1. delete one - out of bounds

    TInt startCount( list->Count() );
    EUNIT_ASSERT_PANIC( list->Delete( startCount + 2 ), "E32USER-CBase", 21 );

    //test:
    // 1. delete one out of bounds

    TInt startCount( list->Count() );
    EUNIT_ASSERT_PANIC( list->Delete( startCount - 1, 5 ), "E32USER-CBase", 21 );

    CleanupStack::PopAndDestroy(2); //list, iStore
    CloseScheduler();
    return KErrNone;

    } */

//Utility functions for adding items to store.

/* void Cwvsapsettingstest::SetupOpenFilledStore1L()
    {

    SetupSchedulerL();

    iStore = CIMPSSAPSettingsStore::NewLC();
	IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    CIMPSSAPSettings* settings = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( settings );

    IMPSSAPSettingsStoreTestUtils::FillContainerPartiallyL( *settings, _L("PartiallyFilled") );
    iStore->StoreNewSAPL( settings, EIMPSIMAccessGroup );

    CleanupStack::PopAndDestroy(); //Settings
    CloseScheduler();

    iStoredCount = 1;
    } */


/* void Cwvsapsettingstest::SetupOpenFilledStore10L()
    {

    SetupSchedulerL();

    iStore = CIMPSSAPSettingsStore::NewL();
	IMPSSAPSettingsStoreTestUtils::DeleteAllSAPSettingsL( *iStore );

    TUint32 dummy1, dummy2;
    iStoredCount = 10;
    IMPSSAPSettingsStoreTestUtils::AddSAPSettingsL( *iStore,
                                                  iStoredCount,
                                                  dummy1,
                                                  dummy2,
                                                  EIMPSIMAccessGroup );

	CloseScheduler();
    } */

TInt Cwvsapsettingstest::TestObsForAddingSAP( TTestResult& aResult )
    {
    SetupSchedulerL();

    WVSAPSettingsObserverTestCases *obstestitnf = WVSAPSettingsObserverTestCases::NewL();
    //Call the FilledStoreL function that will register the observer

    TInt err = ( obstestitnf->TestNotify_AddSapL() );

    CloseScheduler();

    return err;

    }

TInt Cwvsapsettingstest::TestObsForRemoveSAP( TTestResult& aResult )
    {

    SetupSchedulerL();

    WVSAPSettingsObserverTestCases *obstestitnf = WVSAPSettingsObserverTestCases::NewL();
    //Call the RemoveSAPL APIs
    TInt err;

    err = obstestitnf->TestNotify_RemoveSapL();

    CloseScheduler();

    return err;

    }

//Utility functions for installing and uninstalling the scheduler

void Cwvsapsettingstest::SetupSchedulerL()
    {
    // Create a scheduler
    CActiveScheduler* scheduler = new( ELeave )CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    CleanupStack::Pop(); // scheduler

    }

void Cwvsapsettingstest::CloseScheduler()
    {
    delete CActiveScheduler::Current();
    }