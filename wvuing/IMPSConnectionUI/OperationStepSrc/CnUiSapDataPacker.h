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

#ifndef __CNUISAPDATAPACKER_H
#define __CNUISAPDATAPACKER_H

// INCLUDE FILES
#include <E32std.h>
#include "impspresenceconnectionuiconstsng.h"

//FORWARD DECLARATION
class CIMPSSAPSettings;
class RWriteStream;
class RReadStream;


// CLASS DECLARATION
/**
 * SAP data packer.
 * @since 3.0
 */
NONSHARABLE_CLASS( CnUiSapDataPacker )
    {
public: // New connection related functions

    /**
     * Removes any previously packed SAP from
     * passed SAP.
     * @since 3.0
     * @param aSap The SAP to clear.
     */
    static void RemovePackedSapL( CIMPSSAPSettings& aSap );


    /**
     * Packs the client information inside
     * of the SAP.
     *
     * @since 3.0
     * @param aClients The clients to pack.
     * @param aContainer The container SAP where
     * to pack the clients.
     */
    static void PackAAClientsL( const RArray< TIMPSConnectionClient >& aClients,
                                CIMPSSAPSettings& aContainer );

    /**
     * Extracts the packed client information from
     * the containing SAP.
     *
     * @since 3.0
     * @param aContainer SAP container where to search
     * for packed clients.
     * @param aClients On the return contains unpacked clients.
     * @return ETrue if could unpack. Else EFalse.
     */
    static TBool ExtractAAClientsL( CIMPSSAPSettings& aContainer,
                                    RArray< TIMPSConnectionClient >& aClients );


private: //helpers
    /**
     * Writes the given mark to the given stream.
     *
     * @since 3.0
     * @param aWStream write stream
     * @param aMark the mark to write
     */
    static void WriteMarkL( RWriteStream& aWStream, TInt8 aMark );

    /**
     * Reads the given mark from the given stream.
     *
     * @since 3.0
     * @param aRStream read stream
     * @param aSupposedMark the mark to read
     */
    static TBool CheckMarkL( RReadStream& aRStream, TInt8 aSupposedMark );

    /**
     * Writes the given data to the given stream.
     *
     * @since 3.0
     * @param aWStream write stream
     * @param aClients the clients to write
     */
    static void WriteClientsL( RWriteStream& aWStream ,
                               const RArray< TIMPSConnectionClient >& aClients );

    /**
     * Reads the clients from the given stream.
     *
     * @since 3.0
     * @param aRStream write stream
     * @param aClients the array to read the clients into
     */
    static void ReadClientsL( RReadStream& aRStream,
                              RArray< TIMPSConnectionClient >& aClients );

private: // prohibited constructor / destructor
    CnUiSapDataPacker();
    ~CnUiSapDataPacker();

    };


#endif //__CNUISAPDATAPACKER_H

//  End of File

