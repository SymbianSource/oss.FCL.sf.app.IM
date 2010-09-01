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


#ifndef __IMPSSAPSettingsStoreTestUtils_H__
#define __IMPSSAPSettingsStoreTestUtils_H__

#include <E32Std.h>
#include <CIMPSSAPSettingsStore.h> //for KIMPSDefaultSAPSettings


//Forwards
class CIMPSSAPSettings;


// CONSTANTS AND MACROS
//Store db files

/*_LIT( KDbFilesFolder,"C:\\System\\Data\\");
_LIT( KDbDefaultFilesFolder,"D:\\System\\Data\\");

_LIT( KDbFileNameHead,"IMPSSapDb");
_LIT( KDbFileNameTail,".dat");

_LIT( KDbIniFileNameHead,"IMPSSapIni");
_LIT( KDbIniFileNameTail,".dat");
*/

//TEST source db:s
/*
_LIT( KSapDb_TestFileFolder, "c:\\SapDbs\\" );
_LIT( KSapDb0_V1_Filled, "IMPSSapDb0.f1" );
_LIT( KSapDb0_V1_Empty, "IMPSSapDb0.e1" );

_LIT( KSapDb0_V2_Filled, "IMPSSapDb0.f2" );
_LIT( KSapDb0_V2_Empty, "IMPSSapDb0.e2" );

_LIT( KSapDb0_V3_Filled, "IMPSSapDb0.f3" );
_LIT( KSapIni0_V3_Filled, "IMPSSapIni0.f3" );
_LIT( KSapIni0_V3_Filled_Corrupted, "IMPSSapIni0.f3.c1" );
_LIT( KSapIni0_V3_Filled_Corrupted_Slightly, "IMPSSapIni0.f3.c2" );

_LIT( KSapDb0_V3_Empty, "IMPSSapDb0.e3" );
_LIT( KSapIni0_V3_Empty, "IMPSSapIni0.e3" );

*/

//  CLASS DEFINITION

/*
-----------------------------------------------------------------------------

    DESCRIPTION

    TestUtils for SAP settings store module tests.

-----------------------------------------------------------------------------
*/
class IMPSSAPSettingsStoreTestUtils
    {
    public: // New methods

        //store access & deletion
        //static void DeleteStoresL( TUint aStoreId = KIMPSDefaultSAPSettings );
        //static void DeleteNormalStoreL( TUint aStoreId = KIMPSDefaultSAPSettings );
        //static void DeleteDefaultStoresL( TUint aStoreId = KIMPSDefaultSAPSettings );

        //static void UseDbL( TUint aStoreId, const TDesC* aSapDbToUse, const TDesC* aIniFileToUse = NULL );
        //static void UseDefaultDbL( TUint aStoreId, const TDesC& aSapDbToUse, const TDesC* aIniFileToUse = NULL );

        //static void PrintStoreSizesL( TUint aStoreId = KIMPSDefaultSAPSettings );


        //SAP managing
        static void UnigueSAPNameL( TUint& aNameIndex,
                                    CIMPSSAPSettings& aSAP );

        static void AddSAPSettingsL( CIMPSSAPSettingsStore& aStoreToAdd,
                                     TInt aHowMany,
                                     TIMPSAccessGroup aGroup );

        static void AddSAPSettingsL( CIMPSSAPSettingsStore& aStoreToAdd,
                                     TInt aHowMany,
                                     TUint32& aFirstUid,
                                     TUint32& aLastUid,
                                     TIMPSAccessGroup aGroup );


        static TInt AddSAPSettingsWithLiteralNamesL( CIMPSSAPSettingsStore& aStoreToAdd,
                                                     TUint32& aFirstUid,
                                                     TUint32& aLastUid,
                                                     TIMPSAccessGroup aGroup );


        static TInt DeleteAllSAPSettingsL( CIMPSSAPSettingsStore& aStoreToClear );


        //container fills
        static void RandomFillKey( TDes& aBuffer, TInt64& aSeed );
        static void FillContainerL( CIMPSSAPSettings& aSettings,
                                    const TDesC& aName );
        static void FillContainerPartiallyL( CIMPSSAPSettings& aSettings,
                                             const TDesC& aName );
        static void FillContainerLongDataL( CIMPSSAPSettings& aSettings );
        static void FillContainerEvenLongerDataL( CIMPSSAPSettings& aSettings );


        //container checks
        static TBool Equals( CIMPSSAPSettings& aFirst,
                             CIMPSSAPSettings& aSecond );

        //static void AppendDbName( TFileName& aFileName, TUint aStoreId, TBool aFullPath = ETrue );
        //static void AppendIniName( TFileName& aFileName, TUint aStoreId, TBool aFullPath = ETrue );

    private:        // Helpers
        //static void CopyFileToNameL( const TDesC& aSourceFile, const TDesC& aDestinationFile );
        //static void DoDeleteDefaultFilesL( const TDesC& aDefFileSearchName );
        //static void DoDeleteNormalFileL( const TDesC& aFile );


    private:        // Constructors and destructors

        IMPSSAPSettingsStoreTestUtils();
        ~IMPSSAPSettingsStoreTestUtils();



    };

#endif      //  __IMPSSAPSettingsStoreTestUtils_H__

// end of file

