/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper class for arrays
 *
*/



#ifndef CAARRAYUTILS_H
#define CAARRAYUTILS_H

//  INCLUDES
#include    <e32base.h>
#include    <badesca.h>

// CLASS DECLARATION

/**
 *  This class gives some helper functions
 *
 *  @lib chat.app
 *  @since 1.2s
 */
// This is not a C-class although it begins with a CA
class CAArrayUtils // CSI: 51 #see above
    {
    public: // New functions

        /**
         * This method compares two arrays and if there are same data selection
         * array contains indexes that are same
         * @since
         * @param aSource1 Array that contains values that are compared to
         * aSource2
         * @param aSource2 Array that contains values that are compared to
         * aSource1
         * @return Integer array that contains indexes of selected items
         */
        IMPORT_C static CArrayFixFlat<TInt>* SetSelectionsLC( const CDesCArray& aSource1,
                                                              const CDesCArray& aSource2 );

        /**
        * Creates the CDesCArray from the source.
        * Ownership is transferred to caller!
        * @since
        * @param aSource Array to be copied
        * @return Array containing the same elements as source array
        */
        IMPORT_C static CDesCArray* CloneArrayLC( const MDesCArray& aSource );

        /**
        * Creates the CDesCArray containing the items from arrayDiff that
        * are not on arrayTest
        * Ownership is transferred to caller!
        * @since
        * @param aArrayDiff Array whose differences are copied
        * @param aArrayTest Array to be tested on
        * @return Array containing the differences
        */
        IMPORT_C static CDesCArray* CreateDiffLC( const CDesCArray& aArrayDiff,
                                                  const CDesCArray& aArrayTest );
        /**
        * Appends aSource array to aDest
        * @since
        * @param aSource Source array
        * @param aDest Destination array
        */
        IMPORT_C static void AppendArrayL( const CDesCArray& aSource, CDesCArray& aDest );

        /**
         * Appends aPtr to aDest array, if aPtr not found from aDest array
         * @since
         * @param aDest Destination array
         * @param aPtr  to be added text
         * @return ETrue if aPtr was appended ( not found ), else EFalse
         */
        IMPORT_C static TBool AppendIfNotFoundL( CDesCArray& aDest, const TDesC& aPtr );

        /**
         * Removes aPtr from aDest array, if aPtr found from aDest array
         * @since
         * @param aDest Destination array
         * @param aPtr  to be removed text
         * @return ETrue if aPtr was removed ( found ), else EFalse
         */
        IMPORT_C static TBool DeleteIfFound( CDesCArray& aDest, const TDesC& aPtr );


    };

#endif      // CAARRAYUTILS_H

// End of File
