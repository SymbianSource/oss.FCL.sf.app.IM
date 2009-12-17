/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Storage to save smiley information
*
*/


#ifndef CCASMILEYINFORMATION_H
#define CCASMILEYINFORMATION_H

// INCLUDES
#include <e32base.h>


/**
 *  Smiley storage for saving information of one smiley and it's
 *  position inside a text.
 *
 *  @lib Chat.exe
 *  @since S60 v3.0
 */
class CCASmileyInformation : public CBase
    {

    public:

        /**
         * Symbian constructor
         */
        static CCASmileyInformation* NewL();

        /**
         * Destructor
         */
        virtual ~CCASmileyInformation();

        /**
         * Get position of smiley inside a text
         *
         * @since S60 v3.0
         * @return position of smiley
         */
        TInt Position() const;

        /**
         * Set position of smiley inside a text
         *
         * @since S60 v3.0
         * @param aPosition Position of smiley
         */
        void SetPosition( TInt aPosition );

        /**
         * Get the icon index of this smiley.
         *
         * @since S60 v3.0
         * @return Index to a correct icon.
         */
        TInt Index() const;

        /**
         * Set icon index of this smiley,
         *
         * @since S60 v3.0
         * @param aIndex index to an icon
         */
        void SetIndex( TInt aIndex );

        /**
         * Get smiley string
         *
         * @since S60 v3.0
         * @return smiley string
         */
        const TDesC& SmileyString() const;

        /**
         * Set smiley string.
         *
         * @since S60 v3.0
         * @param aSmiley smiley string
         */
        void SetSmileyStringL( const TDesC& aSmiley );

        /**
         * Flag which tells that this smiley is either in icon format (ETrue) or in
         * text format (EFalse)
         *
         * @since S60 v3.0
         * @return Smiley is icon(ETrue) or text(EFalse)
         */
        TBool IsIcon() const;

        /**
         * Set format for this smiley.
         *
         * @since S60 v3.0
         * @param aIsIcon ETrue if this is icon, EFalse if this is text
         */
        void SetIcon( TBool aIsIcon );


    private:

        /**
         * C++ constructor
         */
        CCASmileyInformation();


    private: // data

        /**
         * Position of this smiley in text.
         */
        TInt iPosition;

        /**
         * Index of smiley in icon array
         */
        TInt iIndex;

        /**
         * Text that contains one smiley e.g. ":-)"
         * Own
         */
        HBufC* iSmileyString;

        /**
         * Is smiley in icon(ETrue) or string(EFalse) format.
         */
        TBool iIsIcon;

    };

#endif // CCASMILEYINFORMATION_H
