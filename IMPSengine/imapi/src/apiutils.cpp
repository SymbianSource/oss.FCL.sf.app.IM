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
* Description: Utils for IM api.
*
*/



// INCLUDE FILES
#include "apiutils.h"

#include <CImpssapsettingsstore.h>
#include <CImpssapsettings.h>
#include <CImpsSAPSettingsList.h>
#include <CImpsSAPSettingsListItem.h>

// CONSTANTS
const TInt KLogBufferLength = 256;

_LIT( KLogDir,  "impsc" );
_LIT( KLogFile, "imapi.txt" );

void CImApiLogger::Log( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList( aFmt, list );

    // Write to log file
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf );

    }


const TDesC* SSapSettingsReader::ClientIdLC( const TDesC& aSapUrl )
    {
    TBool isFound( EFalse );
    HBufC* retVal( NULL );
    _LIT( httpString, "http://" );
    TChar slash( '/' );
    TPtrC16 wohttpstr;
    TPtrC16 wohttpstr2;
    wohttpstr.Set( aSapUrl );

    // parsing aSAPUrl
    if ( ( wohttpstr.Find( httpString ) == NULL ) ) // if string is found at the begining
        wohttpstr.Set( wohttpstr.Mid( httpString().Length() ) );

    // search for / at the end
    if ( ( wohttpstr.LocateReverse( slash ) ) == ( ( wohttpstr.Length() ) - 1 ) )
        wohttpstr.Set( wohttpstr.Left( ( ( wohttpstr.Length() ) - 1 ) ) );


    CIMPSSAPSettingsStore* store = CIMPSSAPSettingsStore::NewLC(); // <<<
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC(); // <<<
    store->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    for ( TInt i = 0; i < sapList->MdcaCount(); ++i )
        {
        CIMPSSAPSettings* sapSettings = CIMPSSAPSettings::NewLC();     // <<<
        store->GetSAPL( sapList->At( i )->Uid(), sapSettings );
        const TDesC& server = sapSettings->SAPAddress();
        if ( server.Length() <= 0 )
            {
            CleanupStack::PopAndDestroy( 1 ); // sapSettings
            continue;
            }
        wohttpstr2.Set( server );

        // parsing aSAPUrl
        if ( ( wohttpstr2.Find( httpString ) == NULL ) ) // if string found at the begining
            wohttpstr2.Set( wohttpstr2.Mid( httpString().Length() ) );

        // search for / at the end
        if ( ( wohttpstr2.LocateReverse( slash ) ) == ( ( wohttpstr2.Length() ) - 1 ) )
            wohttpstr2.Set( wohttpstr2.Left( ( ( wohttpstr2.Length() ) - 1 ) ) );

        if ( wohttpstr == wohttpstr2 )
            {
            isFound = ETrue;
            retVal = ( sapSettings->ClientId() ).AllocL();
            CleanupStack::PopAndDestroy( 1 ); // sapSettings
            break;
            }
        else
            {
            CleanupStack::PopAndDestroy( 1 ); // sapSettings
            }
        }
    CleanupStack::PopAndDestroy( 2 );   // >>> sapList, store

    if ( !isFound )
        {
        retVal = KImOpenAppID().AllocLC();
        }
    else
        CleanupStack::PushL( retVal );

    return retVal;
    }

//  End of File
