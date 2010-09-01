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
* Description:  Container class for Invitations view
*
*/



#ifndef CCAINVITATIONCONTAINER_H
#define CCAINVITATIONCONTAINER_H

//  INCLUDES

#include "MCALayoutChangeObserver.h"

#include <eiklbo.h>
#include <coecntrl.h>
#include <aknlists.h>
#include <bldvariant.hrh>
#include <aknlongtapdetector.h>

#include "frmtlay.h"

// FORWARD DECLARATIONS
class MCAViewSwitcher;
class CCABlockingUI;
class CCAAppUi;
class CCAView;
class MCAInvitationPC;
class CCAInvitationView;
class CCATextViewControl;
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CAknsBasicBackgroundControlContext;
class MCARecipientObserver;
class CAknLongTapDetector;

// CLASS DECLARATION

/**
*  Container class for private chat list view
*
*  @lib chat.app
*  @since 1.2
*/
class CCAInvitationContainer : public CCoeControl,
            public MCALayoutChangeObserver,
            public MAknLongTapDetectorCallBack
    {
    public:  // Constructors and destructor


        /**
        * Static constructor
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @param aInvitationIndex index of the invitation to be opened
        * @param aViewSwitcher Handle to view-switcher
        * @param aEngine Handle to engine
        */
        static CCAInvitationContainer* NewL(
            const TRect& aRect,
            TInt aInvitationIndex,
            TPtr aRejectReason,
            MCAViewSwitcher& aViewSwitcher,
            CCAView& aView,
            TInt& aTabId,
            TBool aContextFlag,
            MCARecipientObserver& aMenuObserver );

        /**
        * Destructor.
        */
        virtual ~CCAInvitationContainer();

        /**
         * From CCoeControl, Handles key-events
         * @param aEvent Event that occured
         * @param aType Type of key-event (EEventKey, EEventKeyUp or EEventKeyDown)
         * @return Containers response to event (EKeyWasNotConsumed/ EKeyWasConsumed)
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );

    protected:

        /**
        * Symbian OS default constructor.
        */
        void ConstructL( const TRect& aRect );

        /*
        * @param aRect Frame rectangle for container.
        * @param aViewSwitcher Handle to view-switcher
        * @param aEngine Handle to engine
        */
        CCAInvitationContainer( MCAViewSwitcher& aViewSwitcher,
                                TInt aInvitationIndex,
                                TPtr aRejectReason,
                                CCAView& aView,
                                TInt& aTabId,
                                TBool aContextflag,
                                MCARecipientObserver& aMenuObserver );

    public: // New functions

        /**
         * @returns ETrue if the contact is blocked
         * Else return EFalse.
         */
        TBool IsBlocked( const TDesC& aUserId );

        /**
         * Displays Blocked List
         */
        void DisplayBlockedListL() const;

        /**
         * Displays unblock list query
         */
        void DisplayUnblockListQueryL() const;

        /**
         * Displays block textquery
         */
        void DisplayBlockTextQueryL() const;

        /**
         * Adds focused conversation dead end to friendslist
         */
        void AddToFriendsL();

        /**
         * Switches to the singlelistview
         */
        void SwitchToSingleListViewL( );

        /**
         * Joins to selected conversation
         * @param aGroupId Group id
         */
        void JoinToGroupL( TInt aInviteIndex );

        /**
         * Deletes currently active invitation
         */
        void DeleteInvitationL( TInt aInviteindex ) ;

        /**
         * RejectInvitationL is called when the active
         * invitation has been rejected by teh user
         */
        void RejectInvitationL( TInt aInviteindex, TPtr aRejectReason );

        /**
         * This method tells if selected item is invitation or not
         * @return ETrue if selected item is invitation,
         * EFalse if it's conversation
         */
        TBool IsInvitation() const;

        /**
         * This method switches between the specified
         * invitation
         * @param iCurrenntActiveTab, specifies to
         * which tab  to switch.
         */
        void InvitationSwitchTabL( TInt iCurrenntActiveTab );

        /**
         * Prepare the text in teh rich text format,
         * @param aInvitationindex, index of the invitation
         * whose data needs to prepared int eh richtext format.
         */
        void PrepareTextL( TInt aInvitationindex );

        /**
         * Displays the formated Data
         * @param aInvitationindex, index of the invitation
         * whose data has to be displayed.
         */
        void DisplayTextL( TInt aInvitationindex );

        /**
         * Search the LAF specified font for invite dialog
         * @since 3.0
         * @return LAF specified font
         */
        const CFont* GetLayoutFont();

        /**
         * Inserts line to rich text object
         * @since 3.0
         * @param aTextResourceId Resource for text in line
         * @param aBold Should text be bolded
         */
        void InsertLineL( TInt aTextResourceId, TBool aBold = EFalse );

        /**
         * Inserts line to rich text object
         * @since 3.0
         * @param aText Text for line
         * @param aBold Should text be bolded
         */
        void InsertLineL( const TDesC& aText, TBool aBold = EFalse );

        /**
         * Construct a CAknLongTapDetector object
         */
        CAknLongTapDetector& LongTapDetectorL();

    public: // From MAknLongTapDetectorCallBack

        /**
         * From MAknLongTapDetectorCallBack.
         * Handle long tap event.
         * @see MAknLongTapDetectorCallBack
        */
        void HandleLongTapEventL( const TPoint& aPenEventLocation,
                                  const TPoint& aPenEventScreenLocation );

    private: // From MCALayoutChangeObserver

        /**
         * Called when layout/skins change
         * @since 2.1
         */
        void LayoutChangedL( TInt aType );

    private: //From base classes

        /**
         * From CoeControl, Handles "size changed"-events..
         * @since 2.1
         */
        void SizeChanged();

        /**
         * From CoeControl, Returns the number of control contained by this class.
         * @return Number of controls contained
         * @since 2.1
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl, Returns handle to control pointed by aIndex
         * @param aIndex Wanted control's index [0..n]
         * @return Handle to wanted control
         * @since 2.1
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * Gets help context
         * @since 2.1
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
         * From CCoeControl.
         * Handle pointer event.
         * @see CCoeControl
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
         * From CCoeControl.
         * Handle focus event change.
         * @see CCoeControl
         */
        //void FocusChanged( TDrawNow aDrawNow );

    private:  // New functions

        /**
         * Sets the layout of container
         * @since 2.1
         */
        void SetLayout();

        /*
        * Error handler function. Propagates error note, if necessary.
        * @param aError Error value.
        * @since 2.1
        */
        void HandleError( TInt aError ) const;

        /**
         * Updates CBA set, shows label in MSK if there are
         * items in list.
         * @since S60 v3.2
         */
        void UpdateCbaL();

    public: // new functions

        /**
         *  Checks if invitation is open and processing.
         *  @return ETrue, if invitation is opened and processing.
         *	@since 2.1
         */
        TBool IsInvitationOpen();

    private:    // Data

        // Owns. Array of controls contained
        CArrayPtrSeg<CCoeControl>* iControlArray;

        //Doesn't own
        MCAViewSwitcher& iViewSwitcher;

        //Owns
        CCABlockingUI* iBlockingUI;

        //doesn't own
        CCAAppUi* iAppUi;

        // Flag for telling if invitation is opened.
        TBool iInvitationOpen;

        // EFalse, if numeric input for WVID fields is wanted
        TBool iTextualInputMode;

        // Doesn't own
        CCAView& iView;

        // doesn't own
        CEikButtonGroupContainer* iCba;

        //Does not Own
        MCAInvitationPC* iInvitationPC;


        TInt iInvitationIndex;

        //pointer to teh reject reason
        TPtr iRejectReason;

        // Owns. Text view control
        CCATextViewControl* iTextView;

        // Owns. Rich text for invitation view
        CRichText* iRichText;

        // Owns. Paragraph formatter
        CParaFormatLayer* iParaFormatLayer;

        // Owns. Character formatter
        CCharFormatLayer* iCharFormatLayer;

        // Owns. Date and time buffer
        HBufC* iDateTime;

        HBufC* iIdentification;

        // Skin background control context
        CAknsBasicBackgroundControlContext* iBgContext;

        //Reference variable required to delete the tab.
        TInt& iTabId;

        //Tells whether to show the contxtmenu or not
        TBool iContextFlag;

        TCursorSelection iLastSelection;


        //Doesn't own. Handle to recipient observer
        MCARecipientObserver& iMenuObserver;

        // Owns. Long tap the main pane
        CAknLongTapDetector* iLongTapDetector;
        TBool iLongTapUsed;


    };

#endif      // CCAINVITATIONCONTAINER_H

// End of File
