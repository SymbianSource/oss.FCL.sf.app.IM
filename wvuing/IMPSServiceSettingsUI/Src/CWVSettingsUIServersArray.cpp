/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Array wrapper for server list
*
*/


// INCLUDE FILES
#include "CWVSettingsUIServersArray.h"
#include "WVSettingsUIPanics.h"
#include "CWVSettingsUIDefs.h"
#include <cimpssapsettingslist.h>
// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CWVSettingsUIServersArray* CWVSettingsUIServersArray::NewL( MDesCArray& aServerList,
                                                            RArray<TInt>& aProtectedServers )
    {
    CWVSettingsUIServersArray* self = new ( ELeave ) CWVSettingsUIServersArray(
        aServerList,
        aProtectedServers );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CWVSettingsUIServersArray::~CWVSettingsUIServersArray()
    {
    delete iData;
    delete iServerList;
    }

// ---------------------------------------------------------
// CWVSettingsUIServersArray::MdcaCount
// Returns the number of descriptor elements in a descriptor array.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CWVSettingsUIServersArray::MdcaCount() const
    {
    return iServerList->MdcaCount();
    }

// ---------------------------------------------------------
// CWVSettingsUIServersArray::MdcaCount
// Return pointer to descriptor data in given index of an array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC16 CWVSettingsUIServersArray::MdcaPoint( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iServerList->MdcaCount(),
                     User::Panic( KPanicText, EServersArrayIndexOutOfBounds ) );

    // Copy server's name to buffer
    // Altough this is const method, next line does affect it's member data,
    // because of performance -> no need to create new buffer every time...
    *iData = iServerList->MdcaPoint( aIndex ).Left( KWVSettingsServerNameMaxLength );

    // Add needed format string
    TPtr dataPtr( iData->Des() );

    dataPtr.Insert( 0, KServerNameFormatTrailer );

    if ( ( static_cast<CIMPSSAPSettingsList*>
           ( iServerList ) )->At( aIndex )->Protection() == ESAPBrandProtection )
        {
        dataPtr.Append( KFirstIconFormat );
        }

    return *iData;
    }

// ---------------------------------------------------------
// CWVSettingsUIServersArray::RawDataMdcaPoint
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC16 CWVSettingsUIServersArray::RawDataMdcaPoint( TInt aIndex ) const
    {
    return iServerList->MdcaPoint( aIndex );
    }

// ---------------------------------------------------------
// CWVSettingsUIServersArray::ReplaceModelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CWVSettingsUIServersArray::ReplaceModelL( MDesCArray* aServerList )
    {
    if ( !aServerList )
        {
        User::Leave( EServersArrayReplacelingModelWithNULL );
        }

    delete iServerList;
    iServerList = aServerList;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIServersArray::CWVSettingsUIServersArray(
    MDesCArray& aServerList,
    RArray<TInt>& aProtectedServers )
        : iServerList( &aServerList ), iProtectedServers ( &aProtectedServers )
    {
    }

// Symbian OS default constructor can leave.
void CWVSettingsUIServersArray::ConstructL()
    {

    // Reserve enough memory to handle maximum size item
    iData = HBufC::NewL( KWVSettingsServerNameMaxLength +
                         KServerNameFormatTrailer().Length() +
                         KFirstIconFormat().Length() );

    }
//  End of File
