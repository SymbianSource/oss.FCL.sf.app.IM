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
* Description:  Rich text container
*
*/



#ifndef CCARICHTEXTCONTAINER_H
#define CCARICHTEXTCONTAINER_H

//  INCLUDES
#include "MCAMessageContainer.h"
#include "mcalayoutchangeobserver.h"
#include "MCASettings.h"
#include <e32base.h>
#include <e32std.h>
#include <frmtlay.h> // TCursorSelection


// FORWARD DECLARATIONS
class CCAMessageWrapper;
class CParaFormatLayer;
class CCharFormatLayer;
class CRichText;
class MLayDoc;
class MCATextView;
class CCAMessageExtensionsHandler;
class CGulIcon;
class MCAMessageUtils;
class MCAAppUi;
class MCASettingsPC;
class MCAMsgAddedToRichTxtCtrlObserver;

// CLASS DECLARATION

/**
*  Rich text container
*
*  @lib chat.app
*  @since Series 60 3.0
*/
class CCARichTextContainer : public CBase,
            public MCAMessageContainer,
            public MCALayoutChangeObserver
    {
    public:  // enumerations
        enum THighlightMovement
            {
            ENext = 0,
            EPrevious
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aTextView Viewer of the text
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon.
         * @param aToMe "To me" icon.
         * @param aMap Graphics device map for converting pixels to twips
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         */
        static CCARichTextContainer* NewL( MCAAppUi& aAppUi,
                                           MCASettingsPC& aSettingsPC,
                                           MCATextView& aTextView,
                                           CCAMessageExtensionsHandler& aExtensionsHandler,
                                           CGulIcon*& aFromMe,
                                           CGulIcon*& aToMe,
                                           CGulIcon*& aUnsupported,
                                           CGulIcon*& aCorrupted,
                                           MGraphicsDeviceMap& aMap,
                                           TBool aScrollOver = ETrue,
                                           MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver = NULL );

        /**
         * Destructor.
         */
        virtual ~CCARichTextContainer();

    public: // New functions

        /**
         * @return Selected message or NULL if nothing selected
         */
        MCAConversationMessage* SelectedMessage();

        /**
         * @return Buffer containing the currently selected text
         * (or empty buffer if nothing selected)
         */
        HBufC* SelectedItemL();

        /**
         * @return Type of selected item
         */
        TInt SelectedItemType();

        /**
         * @return Text layout interface for displaying the text
         */
        MLayDoc* TextLayout();

        /**
         * Moves current highlight selection
         * @return KErrNotFound If there are no items to highlight.
         *         KErrNone otherwise.
         */
        TInt MoveHighlightL( THighlightMovement aDirection );

        /**
         * Set all highlighting off
         */
        void DisableHighlightL();

        /**
         * Sets item highlight on or off
         * @param aItemHighlight ETrue if the items inside messages
         *        should be highlighted
         */
        void SetItemHighlight( TBool aItemHighlight );

        /**
         * @return ETrue if some item is highlighted
         */
        TBool Highlighted();

        /**
         * Highlights whole message or item at given position.
         * @param aPos Highlights the message or item which has
         *             the craracter at aPos
         * @return KErrNone           - if highlighted a new item
         *         KErrAlreadyExists  - if item is already highlighted
         *         KErrNotFound       - if can't find message or item at aPos
         */
        TInt HighlightItemAtPosL( TInt aPos );

    public: // From MCALayoutChangeObserver

        /**
         * From MCALayoutChangeObserver
         */
        void LayoutChangedL( TInt aType );

    public:     // Functions from MCAMessageContainer

        /**
         * Check if chat container is deleted.
         * Use this to check if the messages still exists
         * @return ETrue: chat is deleted,
         *         EFalse: chat is not deleted
         */
        TBool IsDeleted() const;

    protected:  // Functions from MCAMessageContainer

        /**
         * Add new message to container
         * @param aMessage Message
         */
        void AddMessageL( MCAConversationMessage& aMessage );

        /**
         * Inform about changed message
         * @param aIndex Index of changed message
         */
        void MessageChangedL( TInt aIndex );

        /**
         * Remove message from container
         * @param aIndex Index of message to be removed
         */
        void RemoveMessage( TInt aIndex );

        /**
         * Marks message container as deleted.
         */
        void MarkDeleted();

        /**
         * @see MCAMessageContainer
         */
        void InsertMessageL( MCAConversationMessage& aMessage );

    protected: // New functions

        /**
         * Updates all wrappers after (and including) aFirstIndex
         * Adds aChange to wrapper positions (in rich text)
         * @param aFirstIndex Index of first modified wrapper
         * @param aChange Removed or added number of characters in rich
         *                text before wrapper at aFirstIndex
         */
        void UpdateWrappers( TInt aFirstIndex, TInt aChange );

        /**
         * @return Union of two selections
         */
        TCursorSelection Union( const TCursorSelection& aSel1,
                                const TCursorSelection& aSel2 );

        /**
         * @return Currently highlighted selection, or last
         *         message if highlighting is disabled
         */
        TCursorSelection CurrentSelection() const;

        /**
         * Sets the background color of selected text
         */
        void TextBackColorL( const TCursorSelection& aSel, const TRgb& aColor );

        /**
         * Sets the color of selected text
         */
        void TextColorL( const TCursorSelection& aSel, const TRgb& aColor );

        /**
         * Sets the background color of selected paragraph
         */
        void BackColorL( const TCursorSelection& aSel, const TRgb& aColor );

        /**
         * Format the highlight item
         */
        void HighLightItemL( const TCursorSelection& aSel );

        /**
         * Sets the selection bold
         */
        void BoldL( const TCursorSelection& aSel );

        /**
         * Parses text for highlight items (phone numbers, urls) and places
         * the positions to messagewrapper.
         * @param aText Text to be parsed
         * @param aStartPos Start position of aText in rich text
         * @param aRelativePos Relative position of aText inside one message
         * @param aMessageWrapper Message wrapper where positions are stored
         */
        void ParseTextL( const TDesC& aText, TInt aStartPos, TInt aRelativePos,
                         CCAMessageWrapper& aMessageWrapper );

        /**
         * Add UserIdentity of a message to rich text
         * @param aMessageWrapper Message wrapper
         * @return Selection for UserIdentity
         */
        TCursorSelection AddUserIdentityL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Add nickname of a message to rich text
         * @param aMessageWrapper Message wrapper
         * @return Selection for nickname (for coloring)
         */
        TCursorSelection AddNickL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Add timestamp of a message to rich text
         * @param aMessageWrapper Message wrapper
         * @return Selection of timestamp
         */
        TCursorSelection AddTimeStampL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Add content of a message to rich text
         * @param aMsgStart Start position of this message in rich text
         * @param aMessageWrapper Message wrapper
         */
        void AddContentL( TInt aMsgStart, CCAMessageWrapper& aMessageWrapper );

        /**
         * Add thumbnail of a picture message to rich text
         * @param aPos Position in rich text
         * @param aMessageWrapper Message wrapper
         */
        void AddThumbL( TInt aPos, CCAMessageWrapper& aMessageWrapper );

        /**
        * Updates the color of messages according to current skin.
        * Only the theme colored part of messages are updated.
        * This method should be called when the skin is changed.
        * @return None
        */
        void UpdateSkinnedTextColourL();

        /**
        * Helper method to determine which part of the message really is
        * colored by the active skin.
        * @param aMessageWrapper  Message to inspect
        * @param aUseDefaultColors Variation for default colours
        * @param aCustomDefinedColor Has user customized the message colour
        * @return Selection of the text which follows the skin color
        */
        TCursorSelection SkinColoredSelection(
            CCAMessageWrapper& aMessageWrapper,
            TBool aUseDefaultColors,
            TBool aCustomDefinedColor );

        /**
         * Helper method for replacing all paragraph delimiters
         * inside message content with line breaks, needed for
         * keeping text alignment consistent inside one message.
         * Used in AddContentL.
         * @since S60 v3.1
         * @param aSource Source descriptor.
         * @param aTarget Target descriptor. Caller is responsible
         *                to provide a large enough descriptor,
         *                aTarget size must not be less than
         *                size of aSource.
         */
        void ReplaceParaDelimsWithLineBreaks(
            const TPtrC& aSource,
            TPtr& aTarget ) const;

        /**
         * Helper method to update selection inside rich text.
         * This will remove the highlight from previously selected item
         * and add highlight to newly selected item.
         * @since S60 v3.1
         */
        void UpdateSelectionL();

        /**
         * Insert nickname of a message to rich text start.
         * @param aMessageWrapper Message wrapper.
         * @return Selection for nickname.
         */
        TCursorSelection InsertNickL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Insert timestamp of a message to rich text.
         * @param aMessageWrapper Message wrapper.
         * @return Selection of timestamp.
         */
        TCursorSelection InsertTimeStampL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Insert User Identity of a message to rich text.
         * @param aMessageWrapper Message wrapper.
         * @return Selection of User Identity.
         */
        TCursorSelection InsertUserIdentityL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Insert content of a message to rich text.
         * @param aMessageWrapper Message wrapper.
         * @return Length of content added.
         */
        TInt InsertContentL( CCAMessageWrapper& aMessageWrapper );

        /**
         * Reads integer resource value from resources
         * @param aResourceId resource id which is read
         * @return value of wanted integer resource flag
         */
        TInt IntResourceValueL( TInt aResourceId );

    public:
        /**
         * @returns the total messages in the view
         */
        TInt GetMessageCount();

        /**
         * Changes the HighlightColor
         */
        void ChangeHighlightColorL();

        //when user navigate among tab.
        /**
         * unregisters the observer added to listen to
         * message added in rich text container
         * @Param - None
         * @return - None
         */
        void UnRegisterRichTxtCtrlObserver();

    private:

        /**
         * C++ default constructor.
         */
        CCARichTextContainer( MCAAppUi& aAppUi,
                              MCASettingsPC& aSettingsPC,
                              MCATextView& aTextView,
                              CCAMessageExtensionsHandler& aExtensionsHandler,
                              CGulIcon*& aFromMe,
                              CGulIcon*& aToMe,
                              CGulIcon*& aUnsupported,
                              CGulIcon*& aCorrupted,
                              MGraphicsDeviceMap& aMap,
                              TBool aScrollOver,
                              MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // enums

        enum THighlightState
            {
            ENoHighlight = 0,
            EItemSelected,
            EMessageTop,
            EMessageBottom
            };

    private:    // Data

        // doesn't own, interface to AppUi -class
        MCAAppUi& iAppUi;

        // doesn't own, interface to application settings
        MCASettingsPC& iSettings;

        // doesn't own, interface to text viewer
        MCATextView& iTextView;

        // Owns. Paragraph formatter
        CParaFormatLayer* iParaFormatLayer;

        // Owns. Character formatter
        CCharFormatLayer* iCharFormatLayer;

        // Owns. rich text document
        CRichText* iRichText;

        // Owns. message wrappers
        RPointerArray<CCAMessageWrapper> iMessages;

        // "From me" icon. Not owned.
        // Reference to a pointer because this can change
        CGulIcon*& iFromMe;

        // "To me" icon. Not owned.
        // Reference to a pointer because this can change
        CGulIcon*& iToMe;

        // "Unsupported" icon. Not owned.
        // Reference to a pointer because this can change
        CGulIcon*& iUnsupported;

        // "Corrupted" icon. Not owned.
        // Reference to a pointer because this can change
        CGulIcon*& iCorrupted;

        // item highlight
        TBool iItemHighlight;

        // selected message
        TInt iSelected;

        // selected item in message
        TInt iSelectedItem;

        // current highlight state
        THighlightState iHighlightState;

        // previous selection
        TCursorSelection iPrevious;

        // message extensions handler
        CCAMessageExtensionsHandler& iExtensionsHandler;

        // interface to graphics device for converting pixels to twips
        MGraphicsDeviceMap& iMap;

        // Should we scroll over the top and bottom,
        // i.e. disabling highlight when pressing up at top.
        TBool iScrollOver;

        // should whole message be colored
        TBool iColorWholeMessage;
        // alternative colors
        TBool iOwnMsgColorInUse;
        TRgb iOwnMsgColor;
        TBool iMsgColorInUse;
        TRgb iMsgColor;

        TRgb iDefaultMsgColor;

        // Show timestamp setting value
        TBool iShowTimeStamps;

        // icon in front of own messages
        TBool iOwnMsgIcon;
        // icon in front of other messages
        TBool iMsgIcon;

        TSize iThumbSize;

        // doesn't own
        const CFont* iPrimaryFont;
        const CFont* iSecondaryFont;

        // are messages deleted
        TBool iMessagesDeleted;

        // is RichTextContainer container recovering from OOM situation
        TBool iRecoveringFromOOM;

        //used to notify of message was added to the rich text control.
        MCAMsgAddedToRichTxtCtrlObserver* iAddMsgObserver;

        //Used only for recorded chats
        TInt iMsgAddedToContentProcessor;

    };

#endif      // CCARICHTEXTCONTAINER_H

// End of File
