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
* Description:  Chat message viewer component
*
*/



#ifndef CCACHATVIEWCONTROL_H
#define CCACHATVIEWCONTROL_H

//  INCLUDES
#include "CCATextViewControl.h"
#include "CCARichTextContainer.h"


// FORWARD DECLARATIONS
class MCAMessageFlowHandlerPC;
class MCASettingsPC;
class CCAMessageExtensionsHandler;
class CGulIcon;
class MCAConversationMessage;
class MCAMessageUtils;
class MCATapEventObserver;
class MCAConversationPC;
class MCARecordedChatsPC;
class MCAGroupPC;

// CLASS DECLARATION

/**
*  Chat message viewer component
*
*  @lib chat.app
*  @since 3.2
*/


class CCAChatViewControl : public CCATextViewControl
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param Process component of conversation
         * @param aSettingsInterface Settings interface
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon
         * @param aToMe "To me" icon
         * @param aMessageUtils Message utils
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         * @return Instance of this class
         */
        static CCAChatViewControl* NewL( const TRect& aRect,
                                         const CCoeControl& aParent,
                                         MCAConversationPC& aMsgRWInterfacePC,
                                         MCASettingsPC& aSettingsInterface,
                                         CCAMessageExtensionsHandler& aExtensionsHandler,
                                         CGulIcon*& aFromMe,
                                         CGulIcon*& aToMe,
                                         CGulIcon*& aUnsupported,
                                         CGulIcon*& aCorrupted,
                                         TBool aScrollOver = ETrue,
                                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver = NULL );

        /**
         * Two-phased constructor.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param Process component of RecordedChats
         * @param aSettingsInterface Settings interface
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon
         * @param aToMe "To me" icon
         * @param aMessageUtils Message utils
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         * @return Instance of this class
         */
        static CCAChatViewControl* NewL( const TRect& aRect,
                                         const CCoeControl& aParent,
                                         MCARecordedChatsPC& aRecordedChatsPC,
                                         MCASettingsPC& aSettingsInterface,
                                         CCAMessageExtensionsHandler& aExtensionsHandler,
                                         CGulIcon*& aFromMe,
                                         CGulIcon*& aToMe,
                                         CGulIcon*& aUnsupported,
                                         CGulIcon*& aCorrupted,
                                         TBool aScrollOver = ETrue,
                                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver = NULL );

        /**
        * Two-phased constructor.
        * @param aRect Draw area
        * @param aParent Parent control
        * @param Process component of Groups
        * @param aSettingsInterface Settings interface
        * @param aExtensionsHandler Message extensions handler
        * @param aFromMe "From me" icon
        * @param aToMe "To me" icon
        * @param aMessageUtils Message utils
        * @param aScrollOver Should we scroll over the top and bottom,
        *                    i.e. disabling highlight when pressing up at top.
        * @param aAddMsgObserver - observer
        * @return Instance of this class
        */
        static CCAChatViewControl* NewL( const TRect& aRect,
                                         const CCoeControl& aParent,
                                         MCAGroupPC& aGroupPC,
                                         MCASettingsPC& aSettingsInterface,
                                         CCAMessageExtensionsHandler& aExtensionsHandler,
                                         CGulIcon*& aFromMe,
                                         CGulIcon*& aToMe,
                                         CGulIcon*& aUnsupported,
                                         CGulIcon*& aCorrupted,
                                         TBool aScrollOver = ETrue,
                                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver = NULL );
        /**
         * Destructor.
         */
        virtual ~CCAChatViewControl();

    public: // New functions

        /**
         * Should we fetch new messages or not
         * @param aFetch ETrue if this chat is in foreground and
         *               should receive messages,
         *               EFalse otherwise.
         */
        void FetchMessages( TBool aFetch );

        /**
         * @return Selected message or NULL if nothing selected
         */
        MCAConversationMessage* SelectedMessage();

        /**
         * Returns buffer containing the currently selected text
         * (or empty buffer if nothing selected)
         */
        HBufC* SelectedItemL();

        /**
         * @return Type of selected item
         */
        TInt SelectedItemType();

        /**
         * Set scrolling method
         * @param aHihghlight If ETrue, scroll by highlighting items
         * otherwise scroll by rows
         */
        void SetHighlight( TBool aHighlight );

        /**
         * Disable all highlighting
         */
        void DisableHighlightL();

        /**
         * @return ETrue if some item is highlighted
         */
        TBool Highlighted();

        /**
        * Move text's highlight up and down.
        * @param aMovement In which direction we move the higlight
        */
        void MoveHighlightL(
            CCARichTextContainer::THighlightMovement aMovement );

        /**
         * Set observer for handling tap events.
         * Supports only single observer.
         * @param aObserver observer to handle tap events
         * @param aId control id which is used by the observer to
         *        identify this object.
         */
        void SetTapObserver( MCATapEventObserver* aObserver, TUint aId );

        /**
         * Check if messages are deleted.
         * @since S60 v3.2
         * @return ETrue: chat is deleted,
         *         EFalse: chat is not deleted.
         */
        TBool IsDeleted() const;

        /**
         * @returns the total messages in the view
         */
        TInt GetMessageCount();


        /**
         * unregisters the observer added to listen to
         * message added in rich text container
         * @Param - None
         * @return - None
         */
        void UnRegisterRichTxtCtrlObserver();

    protected:  // Functions from CCATextViewControl

        /**
         * @see CCATextViewControl
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * @see CCATextViewControl
         */
        void ProcessStylusEventL( const TPointerEvent& aPointerEvent );

        /**
         * @see CCATextViewControl
         */
        void SizeChanged();

    private:

        /**
         * C++ default constructor.
         */
        CCAChatViewControl();

        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param Process component of conversation
         * @param aSettingsInterface Settings interface
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon
         * @param aFromMe "To me" icon
         * @param aMessageUtils Message utils
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         */
        void ConstructL( const TRect& aRect, const CCoeControl& aParent,
                         MCAConversationPC& aMsgRWInterfacePC,
                         MCASettingsPC& aSettingsInterface,
                         CCAMessageExtensionsHandler& aExtensionsHandler,
                         CGulIcon*& aFromMe,
                         CGulIcon*& aToMe,
                         CGulIcon*& aUnsupported,
                         CGulIcon*& aCorrupted,
                         TBool aScrollOver,
                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver );

        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param Process component of RecordedChats
         * @param aSettingsInterface Settings interface
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon
         * @param aFromMe "To me" icon
         * @param aMessageUtils Message utils
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         */
        void ConstructL( const TRect& aRect, const CCoeControl& aParent,
                         MCARecordedChatsPC& aRecordedChatsPC,
                         MCASettingsPC& aSettingsInterface,
                         CCAMessageExtensionsHandler& aExtensionsHandler,
                         CGulIcon*& aFromMe,
                         CGulIcon*& aToMe,
                         CGulIcon*& aUnsupported,
                         CGulIcon*& aCorrupted,
                         TBool aScrollOver,
                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver );
        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aRect Draw area
         * @param aParent Parent control
         * @param Process component of Groups
         * @param aSettingsInterface Settings interface
         * @param aExtensionsHandler Message extensions handler
         * @param aFromMe "From me" icon
         * @param aFromMe "To me" icon
         * @param aMessageUtils Message utils
         * @param aScrollOver Should we scroll over the top and bottom,
         *                    i.e. disabling highlight when pressing up at top.
         * @param aAddMsgObserver - observer
         */
        void ConstructL( const TRect& aRect, const CCoeControl& aParent,
                         MCAGroupPC& aGroupPC,
                         MCASettingsPC& aSettingsInterface,
                         CCAMessageExtensionsHandler& aExtensionsHandler,
                         CGulIcon*& aFromMe,
                         CGulIcon*& aToMe,
                         CGulIcon*& aUnsupported,
                         CGulIcon*& aCorrupted,
                         TBool aScrollOver,
                         MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver );


    private:    // Data

        // owns. Text container
        CCARichTextContainer* iTextContainer;

        // owns. Message flow handler PC
        MCAMessageFlowHandlerPC* iFlowHandlerPC;

        // Not owned. Pointer to observer
        MCATapEventObserver* iTapObserver;

        // ID which needs to be reported back to TapObserver
        TUint iTapControlId;

        // Tap waiting flags.
        TBool iWaitForDoubleTap;
        TBool iWaitForSingleTap;
        TBool iStylusOutsideMessage;
    };

#endif      // CCACHATVIEWCONTROL_H

// End of File
