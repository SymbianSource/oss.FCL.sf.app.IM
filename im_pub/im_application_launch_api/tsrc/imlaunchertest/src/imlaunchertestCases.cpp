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
* Description:  STIF Test Cases for imlauncher.
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <ecom/ecom.h>


#include "imlaunchertestclass.h"
#include "imlaunchertest.h"

#include "imlaunchertestplugin.h"
#include <flogger.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// CONSTANTS
_LIT( KDebugOutputDir, "DummyApp" );
_LIT( KDebugOutputFileName, "DummyApp.log" );
const TInt KMaxLogLineLength = 250;


// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestPlugin;
class CImLaunchertestclass;



// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cimlaunchertest::Case
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
const TCaseInfo Cimlaunchertest::Case (
    const TInt aCaseNumber ) const
    {

    /**
    * To add new test cases, implement new test case function and add new
    * line to KCases array specify the name of the case and the function
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to
    *    imlaunchertest.cpp file and to imlaunchertest.h
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
        // Cimlaunchertest::PrintTest. Otherwise the compiler
        // gives errors.

        //FUNCENTRY( Cimlaunchertest::LoadPluginTestL ),
        ENTRY( "StartApplicationL API test", Cimlaunchertest::LoadPluginTestL ),
        ENTRY ( "CancelApplication API test", Cimlaunchertest::CancelPluginTestL ),

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
// Cimlaunchertest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
/* TInt Cimlaunchertest::PrintTest(
    TTestResult& aResult )
    {*/
/* Simple print test */
/* _LIT( KPrintTest, "PrintTest" );
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
*/

// -----------------------------------------------------------------------------
// Cimlaunchertest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt Cimlaunchertest::LoopTest( TTestResult& aResult )
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

// -----------------------------------------------------------------------------
// Cimlaunchertest::LoadPluginTestL
// This test case is the client application plugin loader
// This will later load the ECom Plugin and then call StartApplicationL
// API implemented in the ECom Plugin.
//
// -----------------------------------------------------------------------------
//


TInt Cimlaunchertest::LoadPluginTestL( TTestResult&  )
    {



    SetupSchedulerL();
    //This is the client loader application.

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "LoadPluginTestL called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );

    launcher = NULL;
    TRAPD( err,  launcher = CImLaunchertestclass::NewL() );

    //This is not Cancel test case
    launcher->iCanceled = EFalse;


    if ( err == KErrNone )
        {
        launcher->ThreadFunction();
        }

    delete launcher;

    CloseScheduler();

    return 0;

    }


TInt Cimlaunchertest::CancelPluginTestL( TTestResult& )
    {

    SetupSchedulerL();
    //This is the client loader application.
    //return CImLaunchertestclass::ThreadFunction();

    TBuf< KMaxLogLineLength > buffer;
    buffer.Append( _L( "[" ) );             // CSI: 78 #
    buffer.Append( _L( "CancelPluginTestL called" ) );
    buffer.Append( _L( "] " ) );

    RFileLogger::Write( KDebugOutputDir, KDebugOutputFileName, EFileLoggingModeAppend, buffer );


    launcher = NULL;
    TRAPD( err,  launcher = CImLaunchertestclass::NewL() );

    //This is the Cancel test case
    launcher->iCanceled = ETrue;

    if ( err == KErrNone )
        {
        launcher->StartPluginL();
        }

    launcher->CancelApplication();

    delete launcher;

    CloseScheduler();

    return 0;

    }

void Cimlaunchertest::SetupSchedulerL()
    {
    // Create a scheduler
    CActiveScheduler* scheduler = new( ELeave )CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    CleanupStack::Pop(); // scheduler

    }

void Cimlaunchertest::CloseScheduler()
    {
    delete CActiveScheduler::Current();
    }

//  End of File
