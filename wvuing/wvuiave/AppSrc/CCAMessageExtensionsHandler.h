/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message extension handler
*
*/


#ifndef CCAMESSAGEEXTENSIONSHANDLER_H
#define CCAMESSAGEEXTENSIONSHANDLER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "MCAMessageExtension.h"

// FORWARD DECLARATIONS
class CEikRichTextEditor;
class TCursorSelection;
class MCASkinVariant;
class MCAAppUi;

// CLASS DECLARATION

/**
*  Message extensions handler
*
*  @lib chat.app
*  @since 2.1
*/
class CCAMessageExtensionsHandler : public CBase
    {
    public:

        /**
         * Two-phased constructor
         * @param aAddSmileyHandler
         * @since 2.1
         */
        static CCAMessageExtensionsHandler* NewL(  MCASkinVariant& aSkinVariant,
                                                   MCAAppUi& aAppUi, TBool aAddSmileyHandler = ETrue );

        /**
         * Destructor
         * @since 2.1
         */
        ~CCAMessageExtensionsHandler();

    private:
        /**
         * Symbian OS Constructor
         * @since 2.1
         */
        void ConstructL(  MCASkinVariant& aSkinVariant,
                          MCAAppUi& aAppUi, TBool aAddSmileyHandler );

        /**
         * Constructor
         * @since 2.1
         */
        CCAMessageExtensionsHandler();

    public:
        /**
         *	Process editor messages for sending them as characters.
         *  Resets extensions.
         *  @param aEditor Editor which contains message.
         *  @since 2.1
         *  @return Ownership of message returned is passed to caller.
         */
        HBufC* ProcessMessageLC( CEikRichTextEditor& aEditor );

        /**
         *	Convert extensions to characters.
         *  @param aEditor Editor which contains message.
         *  @since 2.1
         *  @return Ownership of message returned is passed to caller.
         */
        HBufC* ExtensionToStringLC( CEikRichTextEditor& aEditor );

        /**
         *  Insert extension to editor
         *  @param aEditor Editor which contains message.
         *  @param aType Type of extension needed.
         *  @param aCancelled ETrue after method returns if
         *                    extension insertion was cancelled,
         *                    EFalse otherwise
         *  @since 2.1
         */
        void InsertExtensionL( CEikRichTextEditor& aEditor,
                               TMessageExtensionTypes aType,
                               TBool& aCancelled );

        /**
         *  Delete extension from editor
         *  @param aEditor Editor which contains message.
         *  @since 2.1
         */
        void DeleteExtensionL( CEikRichTextEditor& aEditor,
                               TMessageExtensionTypes aType, TInt aPos );

        /**
         *  Converts all the extensions in selection area from
         *  strings to extensions.
         *  @since 2.6
         *  @param aEditor, used editor
         */
        void ConvertSelectionToExtensionL( CEikRichTextEditor& aEditor );

        /**
         *  Converts all the extensions in selection area from
         *  strings to extensions.
         *  @since 3.0
         *  @param aRichText Rich text to be converted
         *  @param aSelection Selection area
         */
        void ConvertSelectionToExtensionL( CRichText& aRichText,
                                           TCursorSelection& aSelection );

        /**
         *  Converts all the extensions in selection area from
         *  extensions to strings
         *  @since 2.6
         *  @param aEditor, used editor
         *  @param aPreserve, should we preserve original extension status.
         *                    Are they strings or extensions.
         */
        void ConvertSelectionToStringL( CEikRichTextEditor& aEditor,
                                        TBool aPreserve = EFalse );

        /**
         * Informs the extensions about changed size
         * @param aSize
         * @since 3.0
         */
        void SizeChanged( TSize& aSize );

        /**
         * Converts all the extensions in selection area from
         * extensions to strings.
         * @since S60 v3.1
         * @param aEditor Used editor.
         * @param aResultString Descriptor for converted string,
         *                      caller is responsible to provide
         *                      large enough descriptor.
         * @param aSelectionAfterConversion Cursor selection after
         *                      extensions are converted to strings.
         * @param aPreserve Should we preserve original extension status.
         */
        void ConvertSelectionToStringL(
            CEikRichTextEditor& aEditor,
            TDes& aResultString,
            TCursorSelection& aSelectionAfterConversion,
            TBool aPreserve );

    private:

        //Array of extensions
        RPointerArray<MCAMessageExtension> iExtensionArray;
    };

#endif      // CCAMESSAGEEXTENSIONSHANDLER_H

// End of File
