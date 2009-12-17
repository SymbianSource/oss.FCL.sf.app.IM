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
* Description:  Text viewer component
*
*/



// INCLUDE FILES
#include    "CCAChatViewControl.h"
#include    "MCAMessageFlowHandlerPC.h"

#include    "CCACustomDraw.h"
#include    "CCAPicture.h"
#include    "CCAAppUi.h"
#include    "mcatapeventobserver.h"
#include	"MCASettingsPC.h"

#include    <AknUtils.h>
#include    <aknsettingcache.h>
#include    <aknenv.h>
#include    <aknconsts.h>
#include    <avkon.mbg>
#include    <eikrted.h>

#include	"MCAProcessManager.h"
#include	"IMMessageUtilsPC.h"

#include	"MCAMsgAddedToRichTxtCtrlObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAChatViewControl::CCAChatViewControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAChatViewControl::CCAChatViewControl()
    {
    }
// -----------------------------------------------------------------------------
// CCAChatViewControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::ConstructL( const TRect& aRect,
                                     const CCoeControl& aParent,
                                     MCAGroupPC& aMsgRWInterfacePC,
                                     MCASettingsPC& aSettingsInterface,
                                     CCAMessageExtensionsHandler& aExtensionsHandler,
                                     CGulIcon*& aFromMe,
                                     CGulIcon*& aToMe,
                                     CGulIcon*& aUnsupported,
                                     CGulIcon*& aCorrupted,
                                     TBool aScrollOver,
                                     MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver )
    {
    // get the graphics device
    CWindowGc& gc = SystemGc();
    CBitmapDevice* device = static_cast<CBitmapDevice*>( gc.Device() );

    // text container
    iTextContainer = CCARichTextContainer::NewL(
                         static_cast<CCAAppUi&>( *iEikonEnv->AppUi() ),
                         aSettingsInterface, *this,
                         aExtensionsHandler,
                         aFromMe, aToMe, aUnsupported,
                         aCorrupted,
                         *device, aScrollOver,
                         aAddMsgObserver );

    // construct base class
    CCATextViewControl::ConstructL( aRect, aParent,
                                    iTextContainer->TextLayout() );

    // Initialise flow handler and fetch messages
    iFlowHandlerPC = IMMessageUtilsPC::CreateInstanceL( *iTextContainer,
                                                        aMsgRWInterfacePC,
                                                        !aScrollOver ) ;

    // We want to handle all the events which occur inside the text area.
    // Rest is done by the base class.
    SetPenBehaviour( EPenProcessEvent );
    }
// -----------------------------------------------------------------------------
// CCAChatViewControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::ConstructL( const TRect& aRect,
                                     const CCoeControl& aParent,
                                     MCAConversationPC& aMsgRWInterfacePC,
                                     MCASettingsPC& aSettingsInterface,
                                     CCAMessageExtensionsHandler& aExtensionsHandler,
                                     CGulIcon*& aFromMe,
                                     CGulIcon*& aToMe,
                                     CGulIcon*& aUnsupported,
                                     CGulIcon*& aCorrupted,
                                     TBool aScrollOver,
                                     MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver )
    {
    // get the graphics device
    CWindowGc& gc = SystemGc();
    CBitmapDevice* device = static_cast<CBitmapDevice*>( gc.Device() );

    // text container
    iTextContainer = CCARichTextContainer::NewL(
                         static_cast<CCAAppUi&>( *iEikonEnv->AppUi() ),
                         aSettingsInterface, *this,
                         aExtensionsHandler,
                         aFromMe, aToMe, aUnsupported,
                         aCorrupted,
                         *device, aScrollOver,
                         aAddMsgObserver );

    // construct base class
    CCATextViewControl::ConstructL( aRect, aParent,
                                    iTextContainer->TextLayout() );

    // Initialise flow handler and fetch messages
    iFlowHandlerPC = IMMessageUtilsPC::CreateInstanceL( *iTextContainer,
                                                        aMsgRWInterfacePC,
                                                        !aScrollOver ) ;

    // We want to handle all the events which occur inside the text area.
    // Rest is done by the base class.
    SetPenBehaviour( EPenProcessEvent );
    }

void CCAChatViewControl::ConstructL( const TRect& aRect,
                                     const CCoeControl& aParent,
                                     MCARecordedChatsPC& aRecordedChatsPC,
                                     MCASettingsPC& aSettingsInterface,
                                     CCAMessageExtensionsHandler& aExtensionsHandler,
                                     CGulIcon*& aFromMe,
                                     CGulIcon*& aToMe,
                                     CGulIcon*& aUnsupported,
                                     CGulIcon*& aCorrupted,
                                     TBool aScrollOver,
                                     MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver )
    {
    // get the graphics device
    CWindowGc& gc = SystemGc();
    CBitmapDevice* device = static_cast<CBitmapDevice*>( gc.Device() );

    // text container
    iTextContainer = CCARichTextContainer::NewL(
                         static_cast<CCAAppUi&>( *iEikonEnv->AppUi() ),
                         aSettingsInterface, *this,
                         aExtensionsHandler,
                         aFromMe, aToMe, aUnsupported,
                         aCorrupted,
                         *device, aScrollOver, aAddMsgObserver );

    // construct base class
    CCATextViewControl::ConstructL( aRect, aParent,
                                    iTextContainer->TextLayout() );

    // Initialise flow handler and fetch messages
    iFlowHandlerPC = IMMessageUtilsPC::CreateInstanceL( *iTextContainer,
                                                        aRecordedChatsPC,
                                                        !aScrollOver ) ;

    // We want to handle all the events which occur inside the text area.
    // Rest is done by the base class.
    SetPenBehaviour( EPenProcessEvent );
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAChatViewControl* CCAChatViewControl::NewL( const TRect& aRect,
                                              const CCoeControl& aParent,
                                              MCAGroupPC& aMsgRWInterfacePC,
                                              MCASettingsPC& aSettingsInterface,
                                              CCAMessageExtensionsHandler& aExtensionsHandler,
                                              CGulIcon*& aFromMe,
                                              CGulIcon*& aToMe,
                                              CGulIcon*& aUnsupported,
                                              CGulIcon*& aCorrupted,
                                              TBool aScrollOver /*= ETrue*/,
                                              MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver /*=NULL*/ )
    {
    CCAChatViewControl* self = new( ELeave ) CCAChatViewControl;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aMsgRWInterfacePC, aSettingsInterface,
                      aExtensionsHandler, aFromMe, aToMe,
                      aUnsupported, aCorrupted,
                      aScrollOver, aAddMsgObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAChatViewControl* CCAChatViewControl::NewL( const TRect& aRect,
                                              const CCoeControl& aParent,
                                              MCAConversationPC& aMsgRWInterfacePC,
                                              MCASettingsPC& aSettingsInterface,
                                              CCAMessageExtensionsHandler& aExtensionsHandler,
                                              CGulIcon*& aFromMe,
                                              CGulIcon*& aToMe,
                                              CGulIcon*& aUnsupported,
                                              CGulIcon*& aCorrupted,
                                              TBool aScrollOver /*= ETrue*/,
                                              MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver /*=NULL*/ )
    {
    CCAChatViewControl* self = new( ELeave ) CCAChatViewControl;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aMsgRWInterfacePC, aSettingsInterface,
                      aExtensionsHandler, aFromMe, aToMe,
                      aUnsupported, aCorrupted,
                      aScrollOver, aAddMsgObserver );
    CleanupStack::Pop( self );
    return self;
    }

CCAChatViewControl* CCAChatViewControl::NewL( const TRect& aRect,
                                              const CCoeControl& aParent,
                                              MCARecordedChatsPC& aRecordedChatsPC,
                                              MCASettingsPC& aSettingsInterface,
                                              CCAMessageExtensionsHandler& aExtensionsHandler,
                                              CGulIcon*& aFromMe,
                                              CGulIcon*& aToMe,
                                              CGulIcon*& aUnsupported,
                                              CGulIcon*& aCorrupted,
                                              TBool aScrollOver /*= ETrue*/,
                                              MCAMsgAddedToRichTxtCtrlObserver* aAddMsgObserver /*= NULL*/ )
    {
    CCAChatViewControl* self = new( ELeave ) CCAChatViewControl;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aRecordedChatsPC, aSettingsInterface,
                      aExtensionsHandler, aFromMe, aToMe,
                      aUnsupported, aCorrupted,
                      aScrollOver, aAddMsgObserver );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCAChatViewControl::~CCAChatViewControl()
    {
    delete iTextContainer;
    delete iFlowHandlerPC;
    }


// -----------------------------------------------------------------------------
// CCAChatViewControl::FetchMessages
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::FetchMessages( TBool aFetch )
    {
    iFlowHandlerPC->FetchMessages( aFetch );
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SelectedMessage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAConversationMessage* CCAChatViewControl::SelectedMessage()
    {
    return iTextContainer->SelectedMessage();
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SelectedItem
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCAChatViewControl::SelectedItemL()
    {
    return iTextContainer->SelectedItemL();
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SelectedItemType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAChatViewControl::SelectedItemType()
    {
    return iTextContainer->SelectedItemType();
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SetHighlight
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::SetHighlight( TBool aHighlight )
    {
    iTextContainer->SetItemHighlight( aHighlight );
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::DisableHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::DisableHighlightL()
    {
    iTextContainer->DisableHighlightL();
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::Highlighted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAChatViewControl::Highlighted()
    {
    return iTextContainer->Highlighted();
    }


// -----------------------------------------------------------------------------
// CCAChatViewControl::OfferKeyEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCAChatViewControl::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType )
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    CCARichTextContainer::THighlightMovement movement(
        CCARichTextContainer::ENext );

    switch ( aKeyEvent.iCode )
        {
        case EKeyUpArrow:
            {
            movement = CCARichTextContainer::EPrevious;
            // flowthrough
            }
        case EKeyDownArrow:
            {
            return iTextContainer->MoveHighlightL( movement ) == KErrNone ?
                   EKeyWasConsumed : EKeyWasNotConsumed;
            }

        default:
            {
            // No handling for other events
            break;
            }
        }

    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::ProcessStylusEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::ProcessStylusEventL(
    const TPointerEvent& aPointerEvent )
    {
    // We end up here only if the pointer event started inside
    // the text area.
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down ||
         aPointerEvent.iType == TPointerEvent::EDrag )
        {
        // Reset poiter flags
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            iWaitForDoubleTap = iWaitForSingleTap;
            iWaitForSingleTap = ETrue;
            iStylusOutsideMessage = EFalse;
            }

        TInt pos = FindDocPos( aPointerEvent.iPosition );
        if ( pos != KErrNotFound )
            {
            TInt res = iTextContainer->HighlightItemAtPosL( pos );
            if ( res == KErrNone )

                {
                // New item/message was highlighted
                if ( aPointerEvent.iType == TPointerEvent::EDrag )
                    {
                    // Single tap is not possible if highlight was changed
                    // with draging
                    iWaitForSingleTap = EFalse;
                    }
                // doubletap is not possible for now
                iWaitForDoubleTap = EFalse;
                }
            }
        else
            {
            // Stylus was moved/activated outside text elements
            iStylusOutsideMessage = ETrue;
            }
        }

    // Notify Observers
    if ( iTapObserver )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            // Start
            iTapObserver->HandleTapEventL(
                MCATapEventObserver::EStartTap, iTapControlId );
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            // Release
            iTapObserver->HandleTapEventL(
                MCATapEventObserver::EReleaseTap, iTapControlId );
            if ( iWaitForSingleTap && !iStylusOutsideMessage )
                {
                // Single tap
                iTapObserver->HandleTapEventL(
                    MCATapEventObserver::ESingleTap, iTapControlId );
                }
            if ( iWaitForDoubleTap && !iStylusOutsideMessage )
                {
                // Double Tap
                iTapObserver->HandleTapEventL(
                    MCATapEventObserver::EDoubleTap, iTapControlId );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SizeChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::SizeChanged()
    {
    SetHighlighted( Highlighted() );
    CCATextViewControl::SizeChanged();
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::MoveHighlightL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::MoveHighlightL(
    CCARichTextContainer::THighlightMovement aMovement )
    {
    iWaitForSingleTap = ETrue;
    iTextContainer->MoveHighlightL( aMovement );
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::SetTapObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::SetTapObserver( MCATapEventObserver* aObserver, TUint aId )
    {
    iTapObserver = aObserver;
    iTapControlId = aId;
    }

// -----------------------------------------------------------------------------
// CCAChatViewControl::IsDeleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAChatViewControl::IsDeleted() const
    {
    return iTextContainer->IsDeleted();
    }


// -----------------------------------------------------------------------------
// CCAChatViewControl::GetMessageCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCAChatViewControl::GetMessageCount()
    {
    return iTextContainer->GetMessageCount();
    }

//when user navigate among tab.
// -----------------------------------------------------------------------------
// CCAChatViewControl::UnRegisterRichTxtCtrlObserver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAChatViewControl::UnRegisterRichTxtCtrlObserver()
    {
    return iTextContainer->UnRegisterRichTxtCtrlObserver();
    }

//  End of File
