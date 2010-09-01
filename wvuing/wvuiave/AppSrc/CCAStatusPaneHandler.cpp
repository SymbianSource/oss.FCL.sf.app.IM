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
* Description:  Handles statuspane, context pane, navi pane
*
*/


// INCLUDE FILES
#include	"CCAStatusPaneHandler.h"
#include	"CAExternalInterface.h"
#include	"ChatDefinitions.h"
#include	"CCAAppUi.h"
#include	"CCAApp.h"				// A/H variation
#include	"CCAVariantFactory.h"
#include    "CCAIconLoader.h"
#include	"MCAAHVariant.h"
#include	"MCASkinVariant.h"
#include    "CCAUISessionManager.h"

#include    <chatNG.mbg>
#include	<eikspane.h>
#include	<akntitle.h>
#include	<aknnavi.h>
#include	<aknenv.h>
#include	<akncontext.h>
#include	<barsread.h>
#include	<aknindicatorcontainer.h>
#include	<gulicon.h>
#include	<stringloader.h>

#include	"MCAProcessManager.h"
#include	"MCAOpenChatsPC.h"
#include	"MCAMainViewArrayPC.h"
#include	"MCAInvitationPC.h"
#include	"CCAMainViewArrayPC.h"
#include	"MCAStoredGroups.h"

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"

#include	<eikdef.h>
#include    <chatNG.rsg>

const TUint16 KNewMessageIcon( 0xF818 );

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAStatusPaneHandler::CCAStatusPaneHandler()
    {
    }

// Symbian OS default constructor can leave.
void CCAStatusPaneHandler::ConstructL()
    {
    iAppUi = static_cast<CCAAppUi*>( CEikonEnv::Static()->AppUi() );

    // Get pointer to status-pane
    CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
                                 StatusPane();

    // Get pointer to navi-pane
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
                    statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // Get pointer to title-pane
    iTitlePane = static_cast<CAknTitlePane*>( statusPane->ControlL(
                                                  TUid::Uid( EEikStatusPaneUidTitle ) ) );

    iContextPane = static_cast<CAknContextPane*>( statusPane->ControlL(
                                                      TUid::Uid( EEikStatusPaneUidContext ) ) );

    iNewMsgIcon = HBufC::NewL( 1 );

    TPtr iconPtr( iNewMsgIcon->Des() );
    iconPtr.Append( TChar( KNewMessageIcon ) );

    iInvitationTabText = CEikonEnv::Static()->AllocReadResourceL( R_QTN_CHAT_INVITATION_TAB );

    //store the process components
    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    iOpenChatsPC = iAppUi->GetProcessManager().CreateOpenChatsPCL( mainViewArrayPC->GetOpenChatsArray() );

    iOpenChatsPC->SetInvitationIdentification( iInvitationTabText );

    }

// Two-phased constructor.
CCAStatusPaneHandler* CCAStatusPaneHandler::NewL()
    {
    CCAStatusPaneHandler* self = new ( ELeave ) CCAStatusPaneHandler;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAStatusPaneHandler::~CCAStatusPaneHandler()
    {
    if ( iAppUi )
        {
        MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();
        if ( mainViewArrayPC )
            {
            mainViewArrayPC->RemoveObserver( this );
            }
        }

    iObserverList.Reset();

    delete iNewMsgIcon;
    delete iInvitationTabText;
    delete iNewDecoratedTabGroup;
    delete iNaviPaneText;
    delete iOpenChatsPC;
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::SetPicture
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::SetPicture( const CFbsBitmap* aPicture ,
                                       const CFbsBitmap* aMask )
    {
    iContextPane->SetPicture( aPicture, aMask );
    }

// ---------------------------------------------------------
// CCAStatusPaneHandler::ShowTabGroupL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAStatusPaneHandler::ShowTabGroupL( const TEnumsPC::TItem aType, const TDesC& aId,
                                          const TBool noCreate /* = EFalse */ )
    {

    iPrevTabId = 0;
    iCurrentTabId = 0;

    if ( ! iProhibitTabGroup )
        {

        MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

        iCurrentTabId = mainViewArrayPC->FindIndexOfOpenChatsItem( aType, aId );

        mainViewArrayPC->AddObserverL( this );

        CreateTabsL( noCreate );

        if ( !noCreate )
            {
            TabGroup()->SetActiveTabById( iCurrentTabId );

            iOpenChatsPC->SetActiveItem( iCurrentTabId );
            }

        iNaviTabsActivated = ETrue;
        }

    return iCurrentTabId;

    }

// ---------------------------------------------------------
// CCAStatusPaneHandler::ShowTabGroupL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::ShowTabGroupL( const TUid /*aViewId*/ )
    {
    /*
        if ( ! iProhibitTabGroup )
            {
            LoadBitmapsL();
            iActiveViewId = aViewId;

            // Pushes new decorator object to top of the stack and draws it
            iNaviPane->PushL( *iNewDecoratedTabGroup );
            TabGroup()->SetActiveTabById( aViewId.iUid );
            iNaviTabsActivated = ETrue;
            }
    */
    }



// ---------------------------------------------------------
// CCAStatusPaneHandler::SynchroniseAndRemoveTabGroup
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::SynchroniseAndRemoveTabGroup( )
    {

    iOpenChatsPC->SetActiveItem( -1 );

    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    mainViewArrayPC->RemoveObserver( this );

    if ( iOpenChatsPC->Count() > 1 )
        {
        //only pop this item if it is pushed
        //if there is more than 1 item in the tab, then  it is
        //pushed. Then only pop.
        PopNaviPane();
        }

    delete iNewDecoratedTabGroup;
    iNewDecoratedTabGroup = NULL;


    }
// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::ClearNaviPaneL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::ClearNaviPaneL( TBool aAllowDuplicates )
    {
    // Pushes default decorator object (empty one)
    // to top of the stack and draws it. if aAllowDuplicates is ETrue
    // the new empty decorator is created on top of the stack, rather than
    // raising the existing one.
    iNaviPane->PushDefaultL( aAllowDuplicates );

    iNaviTabsActivated = EFalse;
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::RestoreNaviPane
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::RestoreNaviPane()
    {
    // Pushes decorator object (tabs)
    // to top of the stack and draws it
    iProhibitTabGroup = EFalse;
    iNaviTabsActivated = ETrue;
    if ( iNewDecoratedTabGroup )
        {
        TRAP_IGNORE( ShowTabL() );
        }

    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::SetTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::SetTitleL( const TDesC& aTitle )
    {
    iTitlePane->SetTextL( aTitle );
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::SetTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::SetTitleL( CEikonEnv* aEikonEnv, TInt aResourceId )
    {
    if ( !aEikonEnv )
        {
        return;
        }
    HBufC* title = aEikonEnv->AllocReadResourceLC( aResourceId );
    SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::SetTitleL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::SetTitleL( TInt aResourceId,
                                      const TDesC& aSearchString )
    {
    HBufC* title = StringLoader::LoadLC( aResourceId, aSearchString );
    SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::SetNaviLabelL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::SetNaviLabelL( const TDesC& aText )
    {
    HBufC* newTitle = HBufC::NewLC( aText.Length() + KTab().Length() );
    TPtr titlePtr( newTitle->Des() );

    titlePtr.Copy( aText.Left( titlePtr.MaxLength() ) );

    CAknNavigationDecorator* tmpNaviPaneText =
        iNaviPane->CreateMessageLabelL( titlePtr );
    CleanupStack::PushL( tmpNaviPaneText );
    iNaviPane->PushL( *tmpNaviPaneText );
    CleanupStack::Pop( tmpNaviPaneText );
    delete iNaviPaneText;
    iNaviPaneText = tmpNaviPaneText;

    iNaviTabsActivated = EFalse;

    CleanupStack::PopAndDestroy( newTitle );
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::TabGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAknTabGroup* CCAStatusPaneHandler::TabGroup() const
    {
    // Returns pointer to topmost decorator's tabgroup
	if ( iNewDecoratedTabGroup )
		{
	    return static_cast<CAknTabGroup*>( 
	                        iNewDecoratedTabGroup->DecoratedControl() );
		}
	else
		{
	    return NULL;
		}
    }


// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::ActiveTabViewId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CCAStatusPaneHandler::ActiveTabViewId()
    {
    return TUid::Uid( TabGroup()->TabIdFromIndex(
                          TabGroup()->ActiveTabIndex() ) );
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::ActivateTab
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::ActivateTab( const TInt aIndex )
    {
    if ( iNaviTabsActivated )
        {
        TabGroup()->SetActiveTabByIndex( aIndex );
        iOpenChatsPC->SetActiveItem( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::RefreshTitlePaneTextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::RefreshTitlePaneTextL()
    {
    // seems that this is the only way that the title pane text color updates..
    if ( iTitlePane->Text() )
        {
        HBufC* origText = iTitlePane->Text()->AllocLC();
        HBufC* empty = HBufC::NewLC( 1 );
        iTitlePane->SetText( empty );
        iTitlePane->SetText( origText );
        CleanupStack::Pop( 2, origText ); // empty, origText
        }
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::RefreshTitlePaneTextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC* CCAStatusPaneHandler::Title()
    {
    return iTitlePane->Text();
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::ProhibitTabGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::ProhibitTabGroup( TBool aProhibit /* ETrue */ )
    {
    iProhibitTabGroup = aProhibit;
    iNaviTabsActivated = !aProhibit;
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::CreateTabsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::CreateTabsL( const TBool noCreate )
    {

    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();
    iOpenChatArrayCount = iOpenChatsPC->Count();
    TInt openChatsCount = mainViewArrayPC->GetOpenChatsActiveIndex() + 1;

    if ( ( noCreate ) && ( iNewDecoratedTabGroup ) )
        {

        CAknTabGroup* origtabGroup =
            static_cast<CAknTabGroup*>( iNewDecoratedTabGroup->DecoratedControl() );

        if ( origtabGroup->TabCount() == ( openChatsCount - 1 ) )
            {

            for ( TInt index = 1; index < openChatsCount; index++ )
                {
                const TEnumsPC::TItem itemType = iOpenChatsPC->GetType( index );
                const TPtrC itemId = iOpenChatsPC->GetId( index );

                if ( ( mainViewArrayPC->IsOpenChatsMsgPending( itemType, itemId ) )
                     && ( index != iCurrentTabId ) )
                    {
                    origtabGroup->ReplaceTabTextL( index, iNewMsgIcon->Des() );
                    }
                else
                    {
                    origtabGroup->ReplaceTabTextL( index, iOpenChatsPC->GetIdentification( index ) );
                    }
                }

            return;

            }

        }
    CAknNavigationDecorator* tmpDecoration = iNaviPane->CreateTabGroupL();

    CleanupStack::PushL( tmpDecoration );

    CAknTabGroup* tabGroup =
        static_cast<CAknTabGroup*>( tmpDecoration->DecoratedControl() );

    tabGroup->SetTabFixedWidthL( KTabWidthWithTwoTabs );

    tabGroup->SetObserver  ( this );


    for ( TInt index = 1; index < openChatsCount; index++ )
        {
        const TEnumsPC::TItem itemType = iOpenChatsPC->GetType( index );
        const TPtrC itemId = iOpenChatsPC->GetId( index );

        if ( ( mainViewArrayPC->IsOpenChatsMsgPending( itemType, itemId ) )
             && ( index != iCurrentTabId ) )
            {
            tabGroup->AddTabL( index, iNewMsgIcon->Des() );
            }
        else
            {
            tabGroup->AddTabL( index, iOpenChatsPC->GetIdentification( index ) );
            }
        }


    CleanupStack::Pop( tmpDecoration );

    delete iNewDecoratedTabGroup;
    iNewDecoratedTabGroup = tmpDecoration;

    ShowTabL();

    RefreshTitlePaneTextL();
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::PopNaviPane
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::PopNaviPane()
    {
    // Pops the topmost decorator out of decorator stack.
    iNaviPane->Pop();

    }


// -----------------------------------------------------------------------------
// CCAStatusPaneHandler::TabChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::TabChangedL( TInt aIndex )
    {

    TInt activechatitemindex;
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(
                                 iNewDecoratedTabGroup->DecoratedControl() );
    if ( tabGroup )
        {
        if ( TEnumsPC::ERegister == iAppUi->RetForwardTo() ||
             TEnumsPC::EAlreadyRegistered == iAppUi->RetForwardTo() )
            {
            iAppUi->SetResetForwardTo( TEnumsPC::EToBeUnregistered );
            }
        tabGroup->SetActiveTabByIndex( aIndex );
        iOpenChatsPC->SetActiveItem( tabGroup->ActiveTabId() );

        iPrevTabId = iCurrentTabId;
        iCurrentTabId = tabGroup->ActiveTabId();

        if ( iOpenChatArrayCount > iOpenChatsPC->Count() )
            {
            activechatitemindex =  ( ( iPrevTabId < iCurrentTabId ) ? iPrevTabId : iCurrentTabId );
            }
        else
            {
            activechatitemindex =  iCurrentTabId ;
            }

        tabGroup->ReplaceTabTextL( iCurrentTabId, iOpenChatsPC->GetIdentification( activechatitemindex ) ); // replace of iCurrentTabId using activechatitemindex

        if ( iOpenChatsPC->GetType( iCurrentTabId ) == iOpenChatsPC->GetType( iPrevTabId ) )
            {
            for ( TInt index = 0; index < iObserverList.Count(); index++ )
                {
                iObserverList[index]->SwitchTabL( iOpenChatsPC->GetId( iCurrentTabId ), iCurrentTabId );
                }
            }
        else
            {

            switch ( iOpenChatsPC->GetType( iCurrentTabId ) )
                {
                case TEnumsPC::EConversationItem:
                    {
                    TCADnlConvView dnlConView;
                    dnlConView.iWVID = iOpenChatsPC->GetId( iCurrentTabId );
                    dnlConView.iIsForwarded = EFalse;
                    dnlConView.iSAPChanged = EFalse;
                    dnlConView.iSwitchTab = ETrue;
                    dnlConView.iTabId = iCurrentTabId;

                    TCADnlConvViewBuf convMsgBuf( dnlConView );

                    iAppUi->ActivateLocalViewL( KUidConversationsView, KUidConvViewMsgId, convMsgBuf );

                    break;
                    }
                case TEnumsPC::EInviteItem:
                    {
                    TCADnlInvView dnlInvView;

                    MCAInvitationPC& invitePC( *iAppUi->GetProcessManager().GetInvitationsInterface() );

                    dnlInvView.iInvitationIndex = invitePC.FindInvitationIndex(
                                                      iOpenChatsPC->GetId( iCurrentTabId ) );
                    dnlInvView.iInviteID = iOpenChatsPC->GetId( iCurrentTabId );
                    dnlInvView.iSwitchTab = ETrue;
                    dnlInvView.iTabId = iCurrentTabId;

                    TCADnlInvViewBuf invMsgBuf( dnlInvView );

                    iAppUi->ActivateLocalViewL( KUidInvitationView, KUidInvViewMsgId, invMsgBuf  );
                    break;
                    }
                case TEnumsPC::EGroupItem:
                    {
                    TCADnlChatView viewDnl;
                    viewDnl.iIsForwarded = EFalse;
                    viewDnl.iSwitchTab = ETrue;
                    viewDnl.iGroupId = iOpenChatsPC->GetId( iCurrentTabId );
                    viewDnl.iGroupName = iOpenChatsPC->GetIdentification( iCurrentTabId );
                    viewDnl.iInviteQuery = EFalse;
                    viewDnl.iTabId = iCurrentTabId;

                    TCADnlChatViewBuf grpMsgBuf( viewDnl );

                    iAppUi->ActivateLocalViewL( KUidChatView, KUidChatViewMsgId, grpMsgBuf  );
                    break;
                    }
                default:
                    break;
                }

            }
        }
    }

// -----------------------------------------------------------------------------
// CCAStatusPaneHandler: AddObserver
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::AddObserver(
    MCATabObserver* aObserver )
    {

    ASSERT( aObserver );
    TInt status( iObserverList.Find( aObserver ) );
    if ( status == KErrNotFound )
        {
        status = iObserverList.Append( aObserver ) ;
        if ( status != KErrNone )
            {
            CActiveScheduler::Current()->Error( status );
            }
        }

    }


// -----------------------------------------------------------------------------
// CCAStatusPaneHandler: RemoveObserver
// -----------------------------------------------------------------------------
//
void CCAStatusPaneHandler::RemoveObserver(
    MCATabObserver* aObserver )
    {

    ASSERT( aObserver );
    const TInt status( iObserverList.Find( aObserver ) );
    if ( status != KErrNotFound )
        {
        iObserverList.Remove( status );
        iObserverList.Compress();
        }


    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::ShowTabL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::ShowTabL()
    {

    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    if ( mainViewArrayPC->GetOpenChatsActiveIndex() <= 1 )
        {
        return;
        }

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(
                                 iNewDecoratedTabGroup->DecoratedControl() );

    tabGroup->SetActiveTabById( iCurrentTabId );
    iOpenChatsPC->SetActiveItem( iCurrentTabId );

    /* Pushes new decorator object to top of the stack and draws it*/

    iNaviPane->PushL( *iNewDecoratedTabGroup );

    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::HideTab
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HideTab()
    {

    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();

    if ( mainViewArrayPC->GetOpenChatsActiveIndex() <= 1 )
        {
        return;
        }

    /* Pops*/
    iNaviPane->Pop( iNewDecoratedTabGroup );

    }



// ---------------------------------------------------------
// CCAStatusPaneHandler::ResetAndShowCurrentTabL
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAStatusPaneHandler::ResetAndShowCurrentTabL( const TEnumsPC::TItem aType,
                                                    const TDesC& aWvId )
    {

    HideTab();
    return ShowTabGroupL( aType, aWvId );

    }

// ---------------------------------------------------------
// CCAStatusPaneHandler::SetActiveTabByIndex
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::SetActiveTabByIndex( const TInt aIndex )
    {

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(
                                 iNewDecoratedTabGroup->DecoratedControl() );

    if ( !tabGroup )
        {
        return ;
        }

    iCurrentTabId = aIndex;

    TRAP_IGNORE( tabGroup->ReplaceTabL( iCurrentTabId,
                                        iOpenChatsPC->GetIdentification( iCurrentTabId ) ) );

    tabGroup->SetActiveTabById( iCurrentTabId );

    iOpenChatsPC->SetActiveItem( iCurrentTabId );

    }

// ---------------------------------------------------------
// CCAStatusPaneHandler::GetTabItemIdentification
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TPtrC CCAStatusPaneHandler::GetTabItemIdentification( const TInt aIndex )
    {
    return iOpenChatsPC->GetIdentification( aIndex );
    }


//callbacks
// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleChange
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleChange( TEnumsPC::TItem aType, TInt /*aIndex*/, TEnumsPC::TChange aChangeType )
    {

    switch ( aType )
        {
            // first handle for own data if it has..

        case TEnumsPC::EOpenChatsListItem:
            {
            switch ( aChangeType )
                {
                case TEnumsPC::EChanged:
                case TEnumsPC::EAddition:
                case TEnumsPC::EDelete:
                    {
                    TDesC* iD = NULL;
                    TEnumsPC::TItem type;

                    MCAMainViewArrayPC* mainViewArrayPC = iAppUi->GetProcessManager().GetArrayInterface();



                    if ( mainViewArrayPC )
                        {
                        TRAP_IGNORE( mainViewArrayPC->PopulateGroupsListL() );
                        }

                    iOpenChatsPC->GetActiveItem( iD, type );

                    TRAP_IGNORE( ShowTabGroupL( type, *iD ) );


                    break;
                    }
                case TEnumsPC::EUnReadCountChanged:
                    {

                    TDesC* iD = NULL;
                    TEnumsPC::TItem type;

                    iOpenChatsPC->GetActiveItem( iD, type );

                    TRAP_IGNORE( ShowTabGroupL( type, *iD, ETrue ) );

                    break;
                    }
                default:
                    {
                    //should never be here.
                    break;
                    }
                }
            break;
            }

        // default throw exception
        default:
            {
            break;
            }
        }

    }



// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleDelete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleDelete( const TDesC& /*aContactId*/ , TEnumsPC::TItem /*aType*/ )
    {
    //nothing to do
    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleAddition
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleAddition( TEnumsPC::TItem /*aType*/, TInt /*aIndex*/ )
    {
    //nothing to do
    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleWatcherEvent
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleWatcherEvent( const TDesC &  /*aString*/ , TEnumsPC::TOnlineStatus /*aOnlineStatus*/ )
    {
    //nothing to do
    }



// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleInvitationEventL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleInvitationEventL( TEnumsPC::TInviteType /*aInviteType*/,
                                                   const TDesC& /*aUserId*/,
                                                   const TDesC& /*aMessage*/ )
    {
    //nothing to do
    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleInvitationResponse
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleInvitationResponse( TBool /*aAcceptance*/,
                                                     const TDesC& /*aUserId*/,
                                                     const TDesC& /*aGroupName*/,
                                                     const TDesC& /*aResponse*/ )
    {
    //nothing to do
    }


// ---------------------------------------------------------
// CCAStatusPaneHandler::HandleSettingsChangeL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAStatusPaneHandler::HandleSettingsChangeL( TInt /*aChangedSettingEnum*/ )
    {
    //nothing to do
    }



//End of file
