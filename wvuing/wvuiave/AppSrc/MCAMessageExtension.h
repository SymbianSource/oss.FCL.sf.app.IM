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
* Description:  Message extension interface
*
*/


#ifndef MCAMESSAGEEXTENSION_H
#define MCAMESSAGEEXTENSION_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CEikRichTextEditor;
class TCursorSelection;
class CRichText;

// Type definitions for different smiley types
enum TMessageExtensionTypes
    {
    EMessageExtensionSmiley = 0
    };

// CLASS DECLARATION

/**
*  Interface class for message extension classes
*
*  @lib chat.app
*  @since 2.1
*/
class MCAMessageExtension
    {
    public:
        /**
         * Return type of Extension. Inherited message extension does not need
         * information of type. It is only needed by handler of extensions and
         * user of extension. That is why implementation is created in here.
         * @since 2.1
         * @return Type of extension
         */
        virtual TMessageExtensionTypes Type() const
            {
            return iType;
            }

        /**
         * Destructor
         * @since 2.1
         */
        virtual ~MCAMessageExtension() {};

    protected:

        /**
         *	Constructor Inherited message extension does not need
         *  information of type. It is only needed by handler of extensions and
         *  user of extension. That is why implementation is created in here.
         *  @since 2.1
         *  @param aType Type of extension
         */
        MCAMessageExtension( TMessageExtensionTypes aType ) : iType( aType ) {}

    public: //Interface methods
        /**
        * Adds extension to current position in Editor
        * @param aEditor used editor
        * @param aCancelled
        * @since 2.1
        */
        virtual void InsertExtensionL( CEikRichTextEditor& aEditor,
                                       TBool& aCancelled ) = 0;

        /**
        * Removes extension from given position in Editor
        * @param aEditor used editor
        * @param aPos    extension position in Editor
        * @since 2.1
        */
        virtual void DeleteExtensionL( CEikRichTextEditor& aEditor,
                                       TInt aPos ) = 0;

        /**
        * Converts smiley icons to string smileys
        * @since 2.1
        * @param aEditor    used editor
        * @param aDes       Text from editor
        */
        virtual void ExtensionToStringL( CEikRichTextEditor& aEditor,
                                         TDes& aDes ) = 0;

        /**
         *  Converts extension in selection area from strings to extensions.
         *  Used for example for cut, copy, paste features.
         *  @since 2.6
         *  @param aEditor, Used editor
         */
        virtual void ConvertSelectionToExtensionL(
            CEikRichTextEditor& aEditor ) = 0;

        /**
         *  Converts all the extensions in selection area from
         *  strings to extensions.
         *  @since 3.0
         *  @param aRichText Rich text to be converted
         *  @param aSelection Selection area
         */
        virtual void ConvertSelectionToExtensionL( CRichText& aRichText,
                                                   TCursorSelection& aSelection ) = 0;

        /**
         *  Converts extension in selection area from extensions to strings.
         *  Used for example for cut, copy, paste features.
         *  @since 2.6
         *  @param aEditor, Used editor
         *  @param aPreserve, Preserve original status of extension
         */
        virtual void ConvertSelectionToStringL( CEikRichTextEditor& aEditor,
                                                TBool aPreserve ) = 0;

        /**
         *  Reset extension counters.
         *  @sinze 2.6
         */
        virtual void Reset() = 0;

        /**
         * Informs the extensions about changed size
         * @param aSize
         * @since 3.0
         */
        virtual void SizeChanged( TSize& aSize ) = 0;

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
        virtual void ConvertSelectionToStringL(
            CEikRichTextEditor& aEditor,
            TDes& aResultString,
            TCursorSelection& aSelectionAfterConversion,
            TBool aPreserve ) = 0;

    private:

        // Type of extension
        TMessageExtensionTypes iType;
    };

#endif      // MCAMESSAGEEXTENSION_H

// End of File
