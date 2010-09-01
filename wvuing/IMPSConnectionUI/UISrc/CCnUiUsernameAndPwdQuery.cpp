/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Username and password multiline data query
*
*/

// INCLUDE FILES
#include  "CCnUiUsernameAndPwdQuery.h"
#include  "CnUiIMPSIdValidator.h"
#include  "MCnUiUiFacade.h"

#include <barsread.h>
#include <eikcapc.h>
#include <LayoutMetaData.cdl.h>

#include <IMPSConnectionUiNG.rsg>

#include 	"VariantKeys.h"
#include 	<centralrepository.h>
#include 	"CnUiResourceFileName.h"
#include "CIMPSSharedDataFactory.h"
#include "MIMPSSharedData.h"
#include "IMPSPresenceConnectionUiConstsNG.h"

// ================= MEMBER FUNCTIONS =======================
CCnUiUsernameAndPwdQuery* CCnUiUsernameAndPwdQuery::NewL( TDes&  aUserName,
                                                          TDes&  aPwd,
                                                          TCnUiLoginDataQueryMode aMode,
                                                          TBool aDomainSelectionQueryVariation,
                                                          MCnUiUiFacade& aUiFacade )
    {
    TInitialFocus focus;
    TBool cursorToBeginning = EFalse;

    switch ( aMode )
        {
        case ECnUiReEdit:
            {
            //in re-edit focus always the user name field
            focus = EFirstLine;
            break;
            }
        case ECnUiNormal:
            {
            // detect which line to focus
            focus = DetectLineToFocus( aUserName, aPwd );
            break;
            }
        case ECnUiEditPrefilledUser:
            {
            // detect which line to focus
            focus = EFirstLine;
            cursorToBeginning = ETrue;
            break;
            }
        default:
            {
            focus = EFirstLine;
            break;
            }
        }


    CCnUiUsernameAndPwdQuery* self =
        new ( ELeave ) CCnUiUsernameAndPwdQuery( focus,
                                                 aUiFacade,
                                                 aDomainSelectionQueryVariation,
                                                 cursorToBeginning );
    CleanupStack::PushL( self );
    self->SetDataL( aUserName, aPwd );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiUsernameAndPwdQuery::CCnUiUsernameAndPwdQuery( TInitialFocus aInitialFocus,
                                                    MCnUiUiFacade& aUiFacade,
                                                    TBool aDomainSelectionQueryVariation,
                                                    TBool aCursorToBeginning /* = EFalse */ )
        : CAknMultiLineDataQueryDialog( ENoTone ),
        iInitialFocus( aInitialFocus ),
        iCursorToBeginning( aCursorToBeginning ),
        iUiFacade( aUiFacade ),
        iDomainSelectionQueryVariation( aDomainSelectionQueryVariation ),
        iForcedExit( EFalse )
    {
    }


//ConstructL
void CCnUiUsernameAndPwdQuery::ConstructL()
    {
    iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( NULL, KBrandingUid );
    }

// Destructor
CCnUiUsernameAndPwdQuery::~CCnUiUsernameAndPwdQuery()
    {
    iRFs.Close();
    delete iSharedData;
    }

// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::SetInitialCurrentLine()
//
// From CEikDialog
// -----------------------------------------------------------------------------
//
void CCnUiUsernameAndPwdQuery::SetInitialCurrentLine()
    {

    //if initial line can't be set,
    //the error can be only ignored
    TInt ignore;
    TRAP( ignore, DoSetInitialCurrentLineL() );

    iRFs.Close();
    }


// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::OkToExitL()
//
// From CEikDialog
// -----------------------------------------------------------------------------
//
TBool CCnUiUsernameAndPwdQuery::OkToExitL( TInt aButtonId )
    {
    if ( iForcedExit )
        {
        // If dialog needs to be exited because user
        // emptied user id field, allow exit and don't do any checks
        return ETrue;
        }

    HBufC* userId = HBufC::NewLC( FirstControl()->GetTextLength() );
    TPtr IdPtr( userId->Des() );
    FirstControl()->GetText( IdPtr );

    // Check userID before changing to next page
    if ( CnUiIMPSIdValidator::ValidLoginIdL( IdPtr ) )
        {
        CleanupStack::PopAndDestroy( userId );
        TBool okValue = CAknMultiLineDataQueryDialog::OkToExitL( aButtonId );
        UpdateLeftSoftKeyL();
        return okValue;
        }
    else
        {
        //WV id faulty, show note
        iUiFacade.ShowNoteL( ECnUiImproperPresenceId, IdPtr );
        CleanupStack::PopAndDestroy( userId );

        //try to focus whole editor line
        CEikEdwin* presIdEdwin = ( CEikEdwin* )
                                 FirstControl()->ControlByLayoutOrNull( EMultiDataFirstEdwin );
        if ( presIdEdwin )
            {
            TryChangeFocusToL( EMultilineFirstLine );
            presIdEdwin->SelectAllL();
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::TryChangeFocusToL()
// From CEikDialog
// -----------------------------------------------------------------------------
//
void CCnUiUsernameAndPwdQuery::TryChangeFocusToL( TInt aControlId )
    {
    // This special focusing is only used to revert the focus
    // back from second line to first line ( landscape mode ).
    if ( Layout_Meta_Data::IsLandscapeOrientation() &&
         aControlId == EMultilineFirstLine )
        {
        // CAknMultiLineDataQueryDialog does not support changing the query
        // backwards, so we have to do it "manually".
        // This is copied from CAknMultiLineDataQueryDialog
        TBool firstLineEnabled = ETrue;
        TBool secondLineEnabled = EFalse;
        CAknMultilineQueryControl* firstControl = FirstControl();
        CAknMultilineQueryControl* secondControl = SecondControl();
        TInt line1 = FindLineIndex( *firstControl );
        TInt line2 = FindLineIndex( *secondControl );
        CEikCaptionedControl *ctrl1 = GetLineByLineAndPageIndex( line1, 0 );
        CEikCaptionedControl *ctrl2 = GetLineByLineAndPageIndex( line2, 0 );
        ctrl1->SetLatent( !firstLineEnabled );
        ctrl2->SetLatent( !secondLineEnabled );
        ctrl1->SetLatentGroupLineFollows( !firstLineEnabled );
        ctrl2->SetLatentGroupLineFollows( !secondLineEnabled );
        ctrl1->SetFocusing( firstLineEnabled );
        ctrl2->SetFocusing( secondLineEnabled );
        }

    CAknMultiLineDataQueryDialog::TryChangeFocusToL( aControlId );
    }

// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::HandleResourceChange()
// From CEikDialog
// -----------------------------------------------------------------------------
//
void CCnUiUsernameAndPwdQuery::HandleResourceChange( TInt aType )
    {
    CAknMultiLineDataQueryDialog::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRAP_IGNORE( UpdateLeftSoftKeyL( ) );
        }
    }

// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::NeedToDismissQueryL
// From CAknMultiLineDataQueryDialog
// -----------------------------------------------------------------------------
//
TBool CCnUiUsernameAndPwdQuery::NeedToDismissQueryL( const TKeyEvent& aKeyEvent )
    {
    // Check if user has emptied user id field
    // and exit if domain selection query is variated on
    if ( iDomainSelectionQueryVariation &&
         ( aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete ) )
        {
        CAknMultilineQueryControl* ctrl = FirstControl(); // user id field
        CCoeControl* coeCtrl = ctrl->ControlByLayoutOrNull( EMultiDataFirstEdwin );
        if ( coeCtrl )
            {
            CEikEdwin* firstEdwin = static_cast<CEikEdwin*>( coeCtrl );
            TInt textLen = firstEdwin->TextLength();
            TInt cursorPos = firstEdwin->CursorPos();
            if ( ( firstEdwin->Selection().Length() == textLen )
                 || ( textLen == 1 && cursorPos == 1 && aKeyEvent.iCode == EKeyBackspace )
                 || ( textLen == 1 && cursorPos == 0 && aKeyEvent.iCode == EKeyDelete ) )
                {
                // NeedToDismissQueryL is called first thing in OfferKeyEventL
                // so if editor length is 1 or whole text is selected at this point
                // it would be 0 after OfferKeyEventL, clear editor and dismiss dialog
                firstEdwin->SelectAllL();
                firstEdwin->ClearSelectionL();
                iForcedExit = ETrue;
                TryExitL( EAknSoftkeyExit );
                return ETrue;
                }
            else
                {
                // Text in editor, don't dismiss
                return EFalse;
                }
            }
        }

    // Else just return EFalse and don't dismiss dialog
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::DetectLineToFocus()
// Private helper.
// -----------------------------------------------------------------------------
//
CCnUiUsernameAndPwdQuery::TInitialFocus
CCnUiUsernameAndPwdQuery::DetectLineToFocus(
    const TDesC& aFirstLine,
    const TDesC& aSecondLine )
    {
    //focus the second line just if the first one has something in it
    //and the second one is empty
    if ( ( aFirstLine.Length() > 0 ) && ( aSecondLine.Length() == 0 ) )
        {
        return ESecondLine;
        }

    //else
    return EFirstLine;
    }



// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::DoSetInitialCurrentLineL()
// Private helper.
// -----------------------------------------------------------------------------
//
void CCnUiUsernameAndPwdQuery::DoSetInitialCurrentLineL()
    {
    ActivateFirstPageL();

    TBool textualInput = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_USERID_TEXTUAL_INPUTMODE );

    if ( ! textualInput )
        {
        // default is textual mode, but now we must switch to numeric mode
        CAknMultilineQueryControl* ctrl = FirstControl(); // user id field
        CCoeControl* coeCtrl = ctrl->ControlByLayoutOrNull( EMultiDataFirstEdwin );
        if ( coeCtrl )
            {
            CEikEdwin* edwin = static_cast<CEikEdwin*>( coeCtrl );
            edwin->SetAknEditorInputMode( EAknEditorNumericInputMode );
            }
        }

    if ( iCursorToBeginning )
        {
        CAknMultilineQueryControl* ctrl = FirstControl(); // user id field
        CCoeControl* coeCtrl = ctrl->ControlByLayoutOrNull( EMultiDataFirstEdwin );
        if ( coeCtrl )
            {
            CEikEdwin* firstEdwin = static_cast<CEikEdwin*>( coeCtrl );
            firstEdwin->AddFlagToUserFlags( CEikEdwin::ENoAutoSelection );
            }
        }

    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        // Show the OK-softkey also when password field is empty.
        UpdateLeftSoftKeyL();
        }
    else if ( iInitialFocus == ESecondLine )
        {
        TryChangeFocusToL( EMultilineSecondLine );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::OpenResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//
void CCnUiUsernameAndPwdQuery::OpenResourceFileLC( RResourceFile& aResourceFile )
    {
    TFileName resourceFileName;
    iRFs.Close();
    User::LeaveIfError( iRFs.Connect() );

    TInt err = iSharedData->GetStringKey( ( TIMPSSharedKeys )KBrandingResourceKey, resourceFileName );
    if ( err || !resourceFileName.Length() )
        {
        CnUiResourceFileName::NearestVariationForCurrentLanguage( iRFs, resourceFileName  );
        }
    aResourceFile.OpenL( iRFs, resourceFileName );
    CleanupClosePushL( aResourceFile );
    aResourceFile.ConfirmSignatureL();
    }



// -----------------------------------------------------------------------------
// CCnUiUsernameAndPwdQuery::IntResourceValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUsernameAndPwdQuery::IntResourceValueL( TInt aResourceId )
    {

    TInt val( 0 );
    TInt err ( KErrNone );

    CRepository* rep = 0;

    TRAP( err, rep = CRepository::NewL( KCRUidIMNG ) );

    if ( err == KErrNone )
        {
        TInt key = aResourceId + KIMCUStartVariationID;

        err = rep->Get( key, val );

        delete rep;
        }

    if ( err != KErrNone )
        {

        RResourceFile resFile;
        OpenResourceFileLC( resFile ); // Two items in cleanup stack.

        aResourceId = aResourceId + RSC_CRRSS_CHAT_VARIATION_IMPSCU_START_ID;

        // read the data to a buffer
        TInt plainResourceId = 0x00000fff & aResourceId; // Remove offset from id
        HBufC8* rawDataBuf = resFile.AllocReadLC( plainResourceId );

        // it's now as ascii code: \x00 for 0, \x01 for 1, etc.
        TUint value = ( *rawDataBuf )[ 0 ];

        CleanupStack::PopAndDestroy( 2 ); // rawDataBuf, resFile

        val = value;
        }

    return val;

    }


//  End of File
