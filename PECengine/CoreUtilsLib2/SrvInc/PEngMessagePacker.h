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
* Description:  Collection of tools used to pack and unpack
*                data transferred over IPC.
*
*/

#ifndef __PENGMESSAGEPACKER_H__
#define __PENGMESSAGEPACKER_H__

//  INCLUDES
#include <E32Base.h>
#include <Badesca.h>


// CLASS DECLARATION

/**
 * Collection of tools used to pack and unpack
 * data transferred over IPC.
 *
 * @since 3.0
 */
class PEngMessagePacker
    {
    public:  //Packing and adding to descriptor array

        /**
         * Calculating descriptor array package size.
         * Overloads for array of buffer.
         */
        IMPORT_C static TInt DesArrayPackageSize( const MDesCArray& aDesArray );
        IMPORT_C static TInt DesArrayPackageSize( const TDesC& aDesAsArray );


        /**
         * Packing descriptor array to new or existing buffer.
         *
         * New buffer variant returns buffer ownership to caller.
         */
        IMPORT_C static HBufC16* PackDesArrayL( const MDesCArray& aDesArray );
        IMPORT_C static TInt PackDesArray( TDes& aBuffer,
                                           const MDesCArray& aDesArray );


        /**
         * Packing one descriptor as descriptor array
         * to new or existing buffer.
         *
         * New buffer variant returns buffer ownership to caller.
         */
        IMPORT_C static HBufC16* PackOneDesAsArrayL( const TDesC& aDes );
        IMPORT_C static TInt PackOneDesAsArray( TDes& aBuffer,
                                                const TDesC& aDes );


        /**
         * Adding a descriptor to existing descriptor array package.
         */
        IMPORT_C static TInt AppendToDesArray( TDes& aBuffer,
                                               const TDesC& aDes );


    public:  //UnPacking the descriptor array

        /**
         * Gets element count from descriptor array package.
         */
        IMPORT_C static TInt DesArrayCount( const TDesC& aBuffer );


        /**
         * Extracts descriptor array package to existing array.
         */
        IMPORT_C static TInt UnpackDesArrayL( const TDesC& aBuffer,
                                              CDesCArray& aArray );

        /**
         * Extracts descriptor array package to existing pointer array.
         */
        IMPORT_C static TInt UnpackDesArrayL( const TDesC& aBuffer,
                                              CPtrCArray& aArray );

        /**
         * Extracts descriptor array package to new array.
         *
         * Returns array ownership to caller.
         */
        IMPORT_C static CDesCArray* UnpackDesArrayLC( const TDesC& aBuffer );


        /**
         * Extracts first descriptor from descriptor
         * array package.
         * Returns buffer ownership to caller.
         */
        IMPORT_C static HBufC*  UnpackFirstDesFromArrayL( const TDesC& aBuffer );



    public:  //Integer arrays

        /**
         * Calculates TInt array package size.
         */
        IMPORT_C static TInt TIntArrayPackageSize( TInt aElementCount );


        /**
         * Packs TInt array to new or existing buffer.
         * New buffer variant returns buffer ownership to caller.
         */
        IMPORT_C static HBufC16* PackTIntArrayL( const RArray<TInt>& aTIntArray );

        /**
         * Resets the TInt array package to empty.
         */
        IMPORT_C static void ResetTIntArray( TDes& aBuffer );

        /**
         * Gets element count from TInt array package.
         */
        IMPORT_C static TInt TIntArrayCount( const TDesC& aBuffer );

        /**
         * Gets certain element from TInt array package.
         */
        IMPORT_C static TInt TIntFromArray( const TDesC& aBuffer,
                                            TInt aIndex );

    private: // private functions

        /**
         * Unpack des array
         */
        template<class T>
        static TInt UnpackDesTempArrayL( const TDesC& aBuffer,
                                         T& aArray );

    private: //Prohibited constructor / destructor
        PEngMessagePacker();
        ~PEngMessagePacker();

    };


#endif // __PENGMESSAGEPACKER_H__


//  End of File


