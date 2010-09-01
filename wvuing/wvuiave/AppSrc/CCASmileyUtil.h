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


#ifndef CCASMILEYUTIL_H
#define CCASMILEYUTIL_H

// INCLUDE

#include "MCAMessageExtension.h"
#include "CCASmileIconUtility.h"

#include <e32base.h>
#include <gdi.h> // TPictureHeader

// FORWARD DECLARATIONS

class CCASmileIconUtility;
class MCASkinVariant;
class MCAAppUi;

// CLASS DECLARATION


/**
* CCASmileyUtil
*
* Utility for dealing with smileys in editor
*/
class CCASmileyUtil : public CBase, public MCAMessageExtension
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCASmileyUtil* NewL(  MCASkinVariant& aSkinVariant,
                                     MCAAppUi& aAppUi, TMessageExtensionTypes aType );

        /**
        * Destructor.
        */
        virtual ~CCASmileyUtil();

    private: // From MCAMessageExtension

        /**
        * Inserts icon to editor
        * @param anEditor Reference to editor
        */
        void InsertExtensionL( CEikRichTextEditor& anEditor,
                               TBool& aCancelled );

        /**
        * Deletes icon from editor
        * @param aEditor Reference to editor
        * @param aPos Position where icon is located
        */
        void DeleteExtensionL( CEikRichTextEditor& aEditor, TInt aPos );

        /**
        * Converts smiley icons to string smileys
        * @param anEditor Reference to editor
        * @param aDes Text from editor
        */
        void ExtensionToStringL( CEikRichTextEditor& anEditor, TDes& aDes );

        /**
         *  @see MCAMessageExtension
         */
        void ConvertSelectionToExtensionL( CEikRichTextEditor& aEditor );

        /**
         *  Converts all the extensions in selection area from strings to extensions.
         *  @since 3.0
         *  @param aRichText Rich text to be converted
         *  @param aSelection Selection area
         */
        void ConvertSelectionToExtensionL( CRichText& aRichText, TCursorSelection& aSelection );

        /**
         *  @see MCAMessageExtension
         */
        void ConvertSelectionToStringL( CEikRichTextEditor& aEditor, TBool aPreserve );

        /**
         *	@see MCAMessageExtension
         */
        void Reset();

        /**
         * Informs the extensions about changed size
         * @param aSize
         * @since 3.0
         */
        void SizeChanged( TSize& aSize );

        /**
         *	@see MCAMessageExtension
         */
        void ConvertSelectionToStringL(
            CEikRichTextEditor& aEditor,
            TDes& aResultString,
            TCursorSelection& aSelectionAfterConversion,
            TBool aPreserve );

    private:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CCASmileyUtil( TMessageExtensionTypes aType );

        /**
        * Second-phased constructor
        */
        void ConstructL( MCASkinVariant& aSkinVariant,
                         MCAAppUi& aAppUi );

    private: // New methods

    private: // Data

        //util for icon/string convertings
        CCASmileIconUtility* iSmileUtil;

        // Doesn't own
        CEikRichTextEditor* iEditor;

        // Count of current smiley icons in editor
        TInt iCurrentCount;

        // Is smiley converted or not
        RPointerArray< CCASmileyInformation > iConvertArray;
    };

#endif  // CCASMILEYUTIL_H
