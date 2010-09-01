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
* Description:  Message wrapper for MCAUiMessage
*
*/



#ifndef CCAMESSAGEWRAPPER_H
#define CCAMESSAGEWRAPPER_H

//  INCLUDES
#include <e32base.h>
#include <frmtlay.h> // TCursorSelection

// FORWARD DECLARATIONS
class MCAConversationMessage;
class MCAMessageContainer;

// CLASS DECLARATION

/**
*  Message wrapper for MCAConversationMessage
*  stores information about highlight items inside this message etc
*
*  @lib chat.app
*  @since Series 60 3.0
*/
class CCAMessageWrapper : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aWrappedMessage Wrapped message
         * @param aContainer Message container interface
         *                   (for informing about changed message)
         */
        static CCAMessageWrapper* NewL( MCAConversationMessage& aWrappedMessage,
                                        MCAMessageContainer& aContainer );

        /**
         * Destructor.
         */
        virtual ~CCAMessageWrapper();

    public: // New functions

        /**
         * @return Modifiable position for thumbnail
         *         of this message in rich text
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        TCursorSelection& ThumbPos();

        /**
         * @return Modifiable selection for this message in rich text
         *         (for highlighting)
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        TCursorSelection& Selection();

        /**
         * @return Modifiable whole selection for this message in rich text.
         * use this to remove message from rich text
         * as this contains the whole text.
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        TCursorSelection& WholeSelection();

        /**
        * @return Modifiable message selection for this message in rich text.
        * use this to identify the message part from the nick part
        *
        * Note that these are not used within this class,
        * so it's safe to return non-const reference.
        */
        TCursorSelection& MessageSelection();

        /**
         * @return Modifiable array of highlights for this message
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        RArray<TCursorSelection>& Highlights();

        /**
         * @return Modifiable array of highlight types for this message
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        RArray<TInt>& HighlightTypes();

        /**
         * @return Wrapped message
         *
         * Note that these are not used within this class,
         * so it's safe to return non-const reference.
         */
        MCAConversationMessage& Message();

        /**
         * Sets wrapper to be inserted or added.
         * @since S60 v3.2
         * @param aInserted ETrue to set wrapper inserted,
         *                  EFalse to set added.
         */
        void SetInserted( TBool aInserted );

        /**
         * Is wrapper inserted or added.
         * @since S60 v3.2
         * @return ETrue if this wrapper is inserted,
         *         EFalse otherwise.
         */
        TBool IsInserted() const;

    protected:  // Functions from MCAImageLoaderObserver

        /**
         * @see MCAImageLoaderObserver
         */
        //void Handleplaaplaplaa

    private:

        /**
         * C++ default constructor.
         */
        CCAMessageWrapper( MCAConversationMessage& aWrappedMessage,
                           MCAMessageContainer& aContainer );

    private:    // Data

        // Note that these shouldn't be used within this class,
        // these are only contained in this wrapper class

        // Position of thumbnail in rich text
        TCursorSelection iThumbPosition;

        //owns. wrapped message
        MCAConversationMessage* iMessage;

        // Highlight selection for this text
        TCursorSelection iSelection;

        // Whole selection for this text
        TCursorSelection iWholeSelection;

        // Message part selection for this text
        TCursorSelection iMessageSelection;

        // Selections inside this message
        RArray<TCursorSelection> iHighlights;

        // Selection types inside this message
        RArray<TInt> iTypes;

        // doesn't own, reference to message container interface
        MCAMessageContainer& iContainer;

        // Is this wrapper inserted (ETrue) or added (EFalse)
        TBool iInserted;
    };

#endif      // CCAMESSAGEWRAPPER_H

// End of File
