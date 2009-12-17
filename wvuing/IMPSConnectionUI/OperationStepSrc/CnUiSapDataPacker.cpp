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
* Description:  SAP data packer for Connection UI.
*
*/

// INCLUDE FILES
#include <E32std.h>
#include <s32mem.h>
#include <CIMPSSAPSettings.h>
#include "CnUiSapDataPacker.h"


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::RemovePackedSapL()
// -----------------------------------------------------------------------------
//
void CnUiSapDataPacker::RemovePackedSapL( CIMPSSAPSettings& aSap )
    {
    aSap.SetOpaqueInt( KIMPSPackAAClientsKey, EFalse );
    }


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::PackAAClientsL()
// -----------------------------------------------------------------------------
//
void CnUiSapDataPacker::PackAAClientsL( const RArray< TIMPSConnectionClient >& aClients,
                                        CIMPSSAPSettings& aContainer )
    {
    if ( aClients.Count() )
        {
        // there is an ao client
        aContainer.SetOpaqueInt( KIMPSPackAAClientsKey, ETrue );
        }
    }



// -----------------------------------------------------------------------------
// CnUiSapDataPacker::ExtractAAClientsL()
// -----------------------------------------------------------------------------
//
TBool CnUiSapDataPacker::ExtractAAClientsL( CIMPSSAPSettings& aContainer,
                                            RArray< TIMPSConnectionClient >& /* aClients */ )
    {
    TInt returnValue( 0 );
    TInt err = aContainer.GetOpaqueInt( KIMPSPackAAClientsKey, returnValue );
    //ignore not found, since there cannot be clients if the key is not set at all
    if ( ( err != KErrNotFound ) && ( err != KErrNone ) )
        {
        User::Leave( err );
        }
    aContainer.SetOpaqueInt( KIMPSPackAAClientsKey, EFalse );
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::WriteMarkL()
// -----------------------------------------------------------------------------
//
void CnUiSapDataPacker::WriteMarkL( RWriteStream& aWStream, TInt8 aMark )
    {
    aWStream.WriteInt8L( aMark );
    }


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::CheckMarkL()
// -----------------------------------------------------------------------------
//
TBool CnUiSapDataPacker::CheckMarkL( RReadStream& aRStream, TInt8 aSupposedMark )
    {
    TInt8 streamMark = aRStream.ReadInt8L();
    return ( streamMark == aSupposedMark );
    }


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::WriteClientsL()
// -----------------------------------------------------------------------------
//
void CnUiSapDataPacker::WriteClientsL( RWriteStream& aWStream ,
                                       const RArray< TIMPSConnectionClient >& aClients )
    {
    const TInt clientCount = aClients.Count();
    aWStream.WriteInt32L( clientCount );
    for ( TInt index = 0; index < clientCount ; ++index )
        {
        aWStream.WriteInt32L( aClients[ index ] );
        }
    }


// -----------------------------------------------------------------------------
// CnUiSapDataPacker::ReadClientsL()
// -----------------------------------------------------------------------------
//
void CnUiSapDataPacker::ReadClientsL( RReadStream& aRStream,
                                      RArray< TIMPSConnectionClient >& aClients )
    {
    const TInt clientCount = aRStream.ReadInt32L();
    for ( TInt index = 0; index < clientCount ; ++index )
        {
        TInt client = aRStream.ReadInt32L();
        User::LeaveIfError( aClients.Append( static_cast< TIMPSConnectionClient >( client ) ) );
        }
    }

//  End of File

