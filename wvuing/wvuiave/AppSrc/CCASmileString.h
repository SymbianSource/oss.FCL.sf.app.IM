/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for storing one smiley with index to icon array.
*
*/


#ifndef CCASMILESTRING_H
#define CCASMILESTRING_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* CCASmileString
*
* Class for storing smile string and it's index of icon array.
*/
class CCASmileString : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCASmileString* NewL();

        /**
        * Destructor.
        */
        virtual ~CCASmileString();

    public:

        /**
        * Smile icon index.
        * @return (TInt) Smile icon index
        */
        TInt SmileIconIndex() const;

        /**
        * Smile string corresponding a icon.
        * @return (TDesC&) Reference to smile string
        */
        const TDesC& SmileString() const;

        /**
        * Set smiley string and it's index to icon.
        * Takes ownership of the smiley string.
        * @param aSmileyString String of smiley
        * @param aIndex index of icon.
        */
        void Set( const HBufC* aSmileyString, TInt aIndex );

        /**
         * Sets shortest smiley string for this smileys.
         * @since S60 3.0
         * @param aSmileyString String for smiley
         */
        void SetShortestSmileStringL( const TDesC& aSmileyString );

        /**
         * Check if this smiley has shortest string set or not.
         * @since S60 3.0
         * @return ETrue if shortest string set, EFalse otherwise
         */
        TBool HasShortestString() const;

        /**
         * Returns shortest smiley string of this smiley.
         * @since S60 3.0
         * @return Reference to shortest smiley string
         */
        const TDesC& ShortestSmileString() const;

    private:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CCASmileString();

    private: // Data

        // Owns; Smiley string
        const HBufC* iSmiley;

        // Icon index of smiley
        TInt iIndex;

        // Owns; Shortest string of this smiley
        HBufC* iShortestSmileyString;
    };

#endif  // CCASMILESTRING_H
