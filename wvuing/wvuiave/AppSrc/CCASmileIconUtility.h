/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility for selecting smile icon and converting
*                between smile id and smile string.
*
*/


#ifndef CCASMILEICONUTILITY_H
#define CCASMILEICONUTILITY_H

// INCLUDES
#include <e32base.h>
#include "mcaresourcechangeobserver.h"

// FORWARD DECLARATIONS
class CGulIcon;
class CCASmileString;
class CCASmileyInformation;

class MCAAppUi;
class MCASkinVariant;

// CLASS DECLARATION

/**
* CCASmileIconUtility
*
* Utility for selecting smile icon and mapping
* between smile icons and smile strings.
*/
class CCASmileIconUtility : public CBase,
            public MCAResourceChangeObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor using default smile icon array.
        */
        static CCASmileIconUtility* NewL(
            MCASkinVariant& aSkinVariant,
            MCAAppUi& aAppUi );

        /**
        * Two-phased constructor with custom smile icon array.
        * @param aResourceId : Resource id of IM_SMILE_ARRAY.
        */
        static CCASmileIconUtility* NewL(
            MCASkinVariant& aSkinVariant,
            MCAAppUi& aAppUi,
            TInt aResourceId );

        /**
        * Destructor.
        */
        virtual ~CCASmileIconUtility();

    public:

        /**
        * Launch pop-up dialog for selecting smile icon.
        * @param aSelectedSmileIndex Returns selected smile icon index
        * @return (TInt) Return dialog value
        */
        TInt LaunchSmileIconDialogL( TInt& aSelectedSmileIndex );

        /**
        * Smile icon count.
        * @return (TInt) Smile icon count
        */
        TInt SmileIconCount() const;

        /**
        * Return needed smile icon pointer from icon array.
        * @param aSmileIndex Smile icon index
        */
        const CGulIcon* GetSmileIcon( TInt aSmileIndex ) const;

        /**
        * Return needed smile string reference from string array.
        * @param aSmileIndex Smile icon index
        */
        const TDesC& GetSmileString( TInt aSmileIndex ) const;

        /**
        *   Search smileys from buffer. Replace every found smiley with KPuaCodeSmileyIconPadding
        *   @param aStr String to search smiles from.
        *   @param aFixedStr, Target string where smileys are replaced with KPUaCodeSmileyIconPadding
        *   @param aArray Array reference to icon array indexes in right order
        */
        void SearchSmilesL( const TDesC& aStr, TDes& aFixedStr,
                            RArray<TInt>* aArray );

        /**
        *   Search smileys from buffer.
        *   @param aStr String to search smiles from.
        *   @param aSmileyInfoArray Information of found smileys.
        *   @param aFixedStr, Target string where smileys are replaced with KPUaCodeSmileyIconPadding
        *   @param aStartPos, Absolute position of index 0 in source string. Used when position of smiley is calculated.
        */
        void SearchSmilesL( const TDesC& aStr, RPointerArray< CCASmileyInformation >& aSmileyInfoArray, TDes* aFixedStr = NULL, TInt aStartPos = 0 ) const;

        /**
        * Compares two TSmileyInformation objects' position field.
        * @param aFirst Reference to the first TSmileyInformation
        * @param aSecond Reference to the second TSmileyInformation
        * @return (TInt) Positive if aFirst > aSecond, negative if opposite,
        *			     0 if aFirst == aSecond
        */
        static TInt Compare( const CCASmileyInformation& aFirst,
                             const CCASmileyInformation& aSecond );

        /**
        * Compares two CCASmileString objects' smiley length.
        * @param aFirst Reference to the first CCASmileString
        * @param aSecond Reference to the second CCASmileString
        * @return (TInt) Positive if aFirst(length) < aSecond(length),
        *                negative if opposite, 0 if aFirst == aSecond
        */
        static TInt CompareSmileyLengthReversed(
            const CCASmileString& aFirst,
            const CCASmileString& aSecond );

        /**
        * Resizes all icons.
        * @param aSize new icon size
        */
        void ResizeIcons( const TSize& aSize );

    public: // From MCAResourceChangeObserver

        /**
         * Reload all icons on resource change
         */
        void ResourceChangedL();

    private:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CCASmileIconUtility( MCASkinVariant& aSkinVariant,
                             MCAAppUi& aAppUi );

        /**
        * Second-phased constructor
        */
        void ConstructL( TInt aResourceId );

    private: // New methods

        void ConstructFromResourceL( TInt aResourceId );

        void ConstructSmileDialogIconArrayL( TInt aResourceId );

        /**
         * Seek the first smiley which points to a given icon.
         * This is slow method, and it should only be used to
         * precalculate the index table.
         * @param aIconIndex index of icon in iIconArray
         * @return index of smiley in iStringArray
         */
        TInt GetFirstSmileyIndex( TInt aIconIndex ) const;

    private: // Data

        // Own. Smiley icons
        RPointerArray<CGulIcon> iIconArray;

        // Own. Smiley strings with ref to icon array
        RPointerArray<CCASmileString> iStringArray;

        // Own. Temporary array for searching the smileys.
        // This should be empty when not searching.
        RPointerArray<CCASmileyInformation> iSmileArray;

        // Own. Icon reference to string smileys.
        // Using a precalculated array makes seeking a lot faster,
        RArray<TInt> iIconToSmileyPointers;

        // Cache length, so we don't need the seek this every time.
        TInt iLongestSmileyLength;

        // does not own
        MCASkinVariant& iSkinVariant;
        MCAAppUi& iAppUi;

        //Own. Smiley icons for SmileDialog
        RPointerArray<CGulIcon> iSmileDlgIconArray;
    };

#endif  // CCASMILEICONUTILITY_H
