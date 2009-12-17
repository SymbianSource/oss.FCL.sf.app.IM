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
* Description:  Base class for applications views
*
*/



// INCLUDE FILES
#include "CCAView.h"
#include "CCAAppUi.h"
#include "MCAViewSwitcher.h"
#include "CAExternalInterface.h"
#include "CCAFadeControl.h"
#include "chatngclient.hrh"
#include "IMDialogUtils.h"

#include <bldvariant.hrh>
#include <chatNG.rsg>
#include <eikenv.h>
#include <aknenv.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akneditstateindicator.h>
#include <aknindicatorcontainer.h>
#include <featmgr.h>
#include <hlplch.h>

// The Settings have been moved to Cenrep (also retained in the Resource file),
// so the enums for keys and central repository header is added here
#include 	"VariantKeys.h"
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAView::CCAView()
    {
    }

// Symbian OS default constructor can leave.
void CCAView::BaseConstructL( TInt aAvkonViewResourceId, TUid aViewId )
    {
    iViewId = aViewId;
    CAknView::BaseConstructL( aAvkonViewResourceId );
    }

// Destructor
CCAView::~CCAView()
    {
    delete iDecoratedFadeText;
    }

// ---------------------------------------------------------
// CCAView::Id()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CCAView::Id() const
    {
    return iViewId;
    }

// ---------------------------------------------------------
// CCAView::CAAppUi()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCAAppUi* CCAView::CAAppUi() const
    {
    return static_cast<CCAAppUi*>( iEikonEnv->AppUi() );
    }

// -----------------------------------------------------------------------------
// CCAView::CAViewSwitcher
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAViewSwitcher* CCAView::CAViewSwitcher() const
    {
    return static_cast<MCAViewSwitcher*>( CAAppUi() );
    }

// -----------------------------------------------------------------------------
// CCAView::UISessionManager
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAUISessionManager& CCAView::UISessionManager() const
    {
    return CAAppUi()->UISessionManager();
    }

// ---------------------------------------------------------
// CCAView::RunFadeText()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::RunFadeText()
    {
    static_cast<CCAFadeControl*>( iDecoratedFadeText->DecoratedControl() )->Start();
    }

// -----------------------------------------------------------------------------
// CCAView::HideFadeText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAView::HideFadeText()
    {
    if ( iDecoratedFadeText )
        {
        iNaviPane->Pop( iDecoratedFadeText );
        }
    }

// -----------------------------------------------------------------------------
// CCAView::ShowFadeText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAView::ShowFadeText( const TDesC& aTextToFade )
    {
    IMessageIndicator()->SetMessage( aTextToFade );
    RunFadeText();
    }

// -----------------------------------------------------------------------------
// CCAView::ShowNavipaneIndicators
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAView::ShowNavipaneIndicators()
    {
    MAknEditingStateIndicator* editStateIndicator =
        CAknEnv::Static()->EditingStateIndicator();

    if ( editStateIndicator )
        {
        CAknIndicatorContainer* indicatorContainer = editStateIndicator->IndicatorContainer();

        if ( indicatorContainer )
            {
            indicatorContainer->SetIndicatorState(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageInfo ),
                EAknIndicatorStateOn );
            }
        }
    }

// ---------------------------------------------------------
// CCAView::HideNavipaneIndicators()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::HideNavipaneIndicators()
    {
    MAknEditingStateIndicator* editStateIndicator =
        CAknEnv::Static()->EditingStateIndicator();

    if ( editStateIndicator )
        {
        CAknIndicatorContainer* indicatorContainer = editStateIndicator->IndicatorContainer();

        if ( indicatorContainer )
            {
            indicatorContainer->SetIndicatorState(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageInfo ),
                EAknIndicatorStateOff );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCAView::IMessageIndicator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCAFadeControl* CCAView::IMessageIndicator() const
    {
    if ( iDecoratedFadeText )
        {
        return static_cast<CCAFadeControl*>( iDecoratedFadeText->DecoratedControl() );
        }
    else
        {
        return NULL;
        }

    }

// ---------------------------------------------------------
// CCAView::PushFadeTextL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::PushFadeTextL()
    {
    if ( iDecoratedFadeText )
        {
        iNaviPane->PushL( *iDecoratedFadeText );
        }
    }

// ---------------------------------------------------------
// CCAView::ResetFadeText()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::ResetFadeText()
    {
    IMessageIndicator()->Reset();
    }

// ---------------------------------------------------------
// CCAView::LaunchHelpL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::LaunchHelpL()
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                             AppUi()->AppHelpContextL() );
        }
    }

// ---------------------------------------------------------
// CCAView::DynInitMenuPaneL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_CHATCLIENT_GENERAL_MENU )
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EChatClientCmdHelp, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CCAView::IsFunctionSupported
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCAView::IsFunctionSupported( CCAUISessionManager::TSupportedFeature aSupporterFeature )
    {
    if ( UISessionManager().IsSupported( aSupporterFeature ) )
        {
        return ETrue;
        }
    else
        {
        TRAPD( err, IMDialogUtils::DisplayNoteDialogL( R_CHAT_ERROR_NOT_SUP_FUNCTIONS ) );
        if ( err != KErrNone )
            {
            CActiveScheduler::Current()->Error( err );
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCAView::InitMessageIndicatorL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::InitMessageIndicatorL()
    {
    CAknNavigationDecorator* fade = CreateIMessageIndicatorL();
    delete iDecoratedFadeText;
    iDecoratedFadeText = fade;
    }

// ---------------------------------------------------------
// CCAView::DestroyMessageIndicator()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAView::DestroyMessageIndicator()
    {
    delete iDecoratedFadeText;
    iDecoratedFadeText = NULL;
    }
// ---------------------------------------------------------
// CCAView::CreateIMessageIndicatorL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAknNavigationDecorator* CCAView::CreateIMessageIndicatorL()
    {
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
                    StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->
                                 StatusPane();

    CCAFadeControl* fadetext = CCAFadeControl::NewLC(
                                   statusPane->PaneRectL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    fadetext->SetContainerWindowL( *iNaviPane );

    CAknNavigationDecorator* decoratedFadetext =
        CAknNavigationDecorator::NewL( iNaviPane, fadetext,
                                       CAknNavigationDecorator::EHintText );

    CleanupStack::Pop( fadetext );
    CleanupStack::PushL( decoratedFadetext );
    decoratedFadetext->SetContainerWindowL( *iNaviPane );
    decoratedFadetext->SetControlContext( iNaviPane );
    decoratedFadetext->MakeScrollButtonVisible( EFalse );

    CleanupStack::Pop( decoratedFadetext );

    return decoratedFadetext;
    }
