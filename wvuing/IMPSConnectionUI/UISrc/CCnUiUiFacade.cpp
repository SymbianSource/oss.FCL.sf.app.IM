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
* Description:  UI facade.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <aknnotewrappers.h>
#include <aknquerydialog.h>
#include <aknlistquerydialog.h>
#include <aknwaitdialog.h>
#include <TextResolver.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <impserrors.h>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <gulicon.h>
#include <PathInfo.h>
#include <avkon.mbg>
#include <bldvariant.hrh>
#include <aknsskininstance.h>
#include <aknsutils.h>
#include <bautils.h>
#include <fbs.h>

#include <PEngWVPresenceErrors2.h>
#include <NetConError.h>
#include <es_sock.h>
#include 	"VariantKeys.h"

#include <IMPSConnectionUiNG.rsg>
#include <CAVariationNG.rsg>

#include <e32property.h>    // RProperty

#include <AknMessageQueryDialog.h>
#include <eikbtgpc.h>
#include <browserlauncher.h>

#include "CnUiErrors.h"
#include "CCnUiUiFacade.h"
#include "CnUiResourceFileName.h"
#include "CCnUiUsernameAndPwdQuery.h"
#include "CCommandAbsorbingControl.h"
#include "CCnUiWaitNoteContainer.h"
#include "CCnUiQueryContainer.h"
#include "ccnuidomainselectionquerydialog.h"
#include "CCnUiTermsOfUseDlg.h"

#include "CnUiPanics.h"
#include "CIMPSSharedDataFactory.h"
#include "MIMPSSharedData.h"
#include "impspresenceconnectionuiconstsng.h"

#include "CCnOUiUsernameAndPwdQuery.h"
#include "impswapreguiddefs.h"
#include 	<centralrepository.h>
#include    "CCnUiConnOpener.h"


//CONSTANTS
//Max CSP error code
//It is assumed that max CSP error id would be 999
//==>thus the top CSP error code is CSP error base - 999
const TInt KPEngErrorWVServerResponseTop = KPEngErrorWVServerResponseBase - 999;
const TInt KCnUiErrGPRSConnectionNotAvailable = -4167; //stated explicitly in UI spec.

// Backslash unicode
const TUint KBackSlash = 0x005C;

_LIT( KTab, "\t" );
_LIT( KDirSeparator, "\\" );
_LIT( KRelativeBrandPath, "\\system\\data\\branding\\" );
_LIT( KModuleId, "IMApp" );
_LIT( KBitmapFile, "logo.mbm" );
_LIT( KCUITOUFile, "TOU.rsc" );
//Max length of WAP registration URL
#define KMaxRegUrlLength 20

// ================= GLOBAL FUNCTIONS ====================
// -----------------------------------------------------------------------------
// CreateUiFacadeL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiUiFacade* CreateUiFacadeL()
    {
    return CCnUiUiFacade::NewL();
    }

GLREF_C MCnUiUiFacade* CreateUiFacadeLC()
    {
    CCnUiUiFacade* self = CCnUiUiFacade::NewL();
    CleanupStack::PushL( self );
    return self;
    }


// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
CCnUiUiFacade* CCnUiUiFacade::NewL()
    {
    CCnUiUiFacade* self = new ( ELeave ) CCnUiUiFacade();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCnUiUiFacade::~CCnUiUiFacade()
    {
    RemoveUsedResourceFile();
    delete iDefaultRssFileName;
    delete iCommandAbsorber;
    delete iIconArray;
    delete iSharedData;
    delete iBrLauncher;
    iRFs.Close();
    if ( iControl )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iControl );
        delete iControl;
        iControl = NULL;
        }
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiUiFacade::CCnUiUiFacade()
        : iResourceFileAdded( EFalse )
    {
    }


// Symbian OS default constructor can leave.
void CCnUiUiFacade::ConstructL()
    {
    iCoeEnv = CCoeEnv::Static();
    __ASSERT_ALWAYS( iCoeEnv,
                     CnUiPanicOrLeaveL( EIMPSConnUi_NoCoeEnv,
                                        KErrNotSupported ) );

    iCommandAbsorber = CCommandAbsorbingControl::NewL();

    iSharedData = CIMPSSharedDataFactory::CreateTemporaryKeyHandlerL( NULL, KBrandingUid );
    RProperty::Define( KBrandingUid,
                       KBrandingResourceKey,
                       RProperty::EByteArray,
                       KIMPSPubSubReadPolicy, KIMPSPubSubWritePolicy );

    RProperty::Define( KBrandingUid,
                       KBrandingConnUIResourceKey,
                       RProperty::EByteArray,
                       KIMPSPubSubReadPolicy, KIMPSPubSubWritePolicy );

    TFileName resourceFileName;
    CnUiResourceFileName::NearestForCurrentLanguage( iCoeEnv->FsSession(),
                                                     resourceFileName  );
    iDefaultRssFileName = resourceFileName.AllocL();

    // Check for branding
    resourceFileName.Zero();
    TInt err = iSharedData->GetStringKey(
                   ( TIMPSSharedKeys )KBrandingConnUIResourceKey, resourceFileName );
    if ( err != KErrNone || resourceFileName.Length() == 0 )
        {
        resourceFileName = *iDefaultRssFileName;
        }

    UseResourceFileL( resourceFileName );
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::ConnProcessUi()
// -----------------------------------------------------------------------------
//
MIMPSConnProcessUi& CCnUiUiFacade::ConnProcessUi()
    {
    return *this;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::CommandAbsorbOnLC()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::CommandAbsorbOnLC()
    {
    iCommandAbsorber->AbsorbCommandsLC();
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::FlushCommandBuffer()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::FlushCommandBuffer()
    {
    iCommandAbsorber->FlushCommands();
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::WaitNoteL()
// -----------------------------------------------------------------------------
//
MCnUiConnWaitNote* CCnUiUiFacade::WaitNoteL( TCnUiConnectionWaitNotes aNoteId,
                                             MCnUiWaitNoteObserver* aObserver )
    {
    TInt promptTextResourceId( 0 );
    TInt noteResourceId( 0 );
    switch ( aNoteId )
        {
        case ECnUiWaitNoteDisconnecting:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_WAIT_DISCONNECT;
            noteResourceId = R_WAIT_NOTE_BLANK;
            break;
            }

        case ECnUiWaitNotePlugin:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_FETCHING_CONTACT_LISTS;
            noteResourceId = R_WAIT_NOTE_BLANK;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( promptTextResourceId, iCoeEnv );

    CCnUiWaitNoteContainer* wNote = CCnUiWaitNoteContainer::NewL( noteResourceId,
                                                                  *prompt,
                                                                  aObserver );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return wNote;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::WaitNoteL()
// -----------------------------------------------------------------------------
//
MCnUiConnWaitNote* CCnUiUiFacade::WaitNoteL( TCnUiConnectionWaitNotesDynText aNoteId,
                                             const TDesC& aInsertText,
                                             MCnUiWaitNoteObserver* aObserver )
    {
    TInt promptTextResourceId( 0 );
    TInt noteResourceId( 0 );
    switch ( aNoteId )
        {
        case ECnUiWaitNoteConnectingPEC:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_PEC_CON_TO_SERVER;
            noteResourceId = R_WAIT_NOTE_WITH_CANCEL;
            break;
            }

        case ECnUiWaitNoteConnectingChat:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_CON_TO_SERVER;
            noteResourceId = R_WAIT_NOTE_WITH_CANCEL;
            break;
            }

        case ECnUiWaitNoteReConnectingChat:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_SETTING_CON_TO_PEC2;
            noteResourceId = R_WAIT_NOTE_WITH_CANCEL;
            break;
            }

        case ECnUiWaitNoteReConnectingPEC:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_SETTING_CON_TO_CHAT2;
            noteResourceId = R_WAIT_NOTE_WITH_CANCEL;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( promptTextResourceId, aInsertText, iCoeEnv );
    CCnUiWaitNoteContainer* wNote = NULL;
    TBool requireSignOnAnim = IntResourceValueL( RSC_CHAT_VARIATION_SHOW_SIGN_IN_ANIMATION );
    if ( aNoteId == ECnUiWaitNoteConnectingChat && requireSignOnAnim )
        {
        iControl = CCnUiSignInControl::NewL( *prompt, aObserver );
        CCoeEnv::Static()->AppUi()->AddToStackL( iControl );
        }
    else
        {
        wNote = CCnUiWaitNoteContainer::NewL( noteResourceId,
                                              *prompt,
                                              aObserver );
        }
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return wNote;
    }




// -----------------------------------------------------------------------------
// CCnUiUiFacade::QueryL()
// -----------------------------------------------------------------------------
//
MCnUiConnQuery* CCnUiUiFacade::QueryL( TCnUiConnAsyncQuery aQueryId,
                                       MCnUiConnQueryObserver& aObserver )
    {
    TInt promptTextResourceId( 0 );

    switch ( aQueryId )
        {
        case ECnUiQueryActiveConnectionScheduledClose:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_SCHEDULED_TO_BE_CLOSED;
            break;
            }


        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( promptTextResourceId, iCoeEnv );
    CCnUiQueryContainer* query = CCnUiQueryContainer::NewL( ENBConfirmationQuery,
                                                            *prompt,
                                                            aObserver );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return query;
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::QueryL()
// -----------------------------------------------------------------------------
//
MCnUiConnQuery* CCnUiUiFacade::QueryL( TCnUiConnAsyncQueryDynText aQueryId,
                                       const TDesC& aInsertText,
                                       MCnUiConnQueryObserver& aObserver )
    {
    TInt promptTextResourceId( 0 );
    switch ( aQueryId )
        {
        case ECnUiQueryActiveConnectionSuppressForScheduledConnection:
            {
            //res & type res ok
            promptTextResourceId = R_QTN_CHAT_DISCON_OTHER_BECAUSE_SCHEDULING;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( promptTextResourceId, aInsertText, iCoeEnv );
    CCnUiQueryContainer* query = CCnUiQueryContainer::NewL( ENBConfirmationQuery,
                                                            *prompt,
                                                            aObserver );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return query;
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::GetIntResourceValueL
// -----------------------------------------------------------------------------
//
TInt CCnUiUiFacade::GetIntResourceValueL( TInt aResourceId )
    {
    // Show user Identity, if variated so
    TResourceReader reader;
    TInt value( KErrNone );
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );
    value = ResourceUtils::ReadTInt32L( reader );
    CleanupStack::PopAndDestroy(); // reader

    return value;
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::ShowNoteL( TCnUiConnectionNotes aNoteId )
    {
    TInt resourceId( 0 );
    TCnUiConnUiNoteType noteType = EInfoNote;
    switch ( aNoteId )
        {
        case ECnUiConnectionOK:
            {
            //res & type ok
            resourceId = R_QTN_CHAT_LOGIN_OK;
            noteType = EInfoNote;
            break;
            }

        case ECnUiDisconnectedIM:
            {
            //res & type ok
            resourceId = R_QTN_CHAT_DISCONNECTED_CONF;
            noteType = EConfirmationNote;
            break;
            }

        case ECnUiDisconnectedPEC:
            {
            //res & type ok
            resourceId = R_QTN_DYC_DISCONNECTED_CONF;
            noteType = EConfirmationNote;
            break;
            }

        case ECnUiDisconnectedAutomaticConnection:
            {
            //res & type ok
            resourceId = R_QTN_CHAT_AUTOMATIC_CONN_ON;
            noteType = EInfoNote;
            break;
            }


        case ECnUiConnCanceled:
            {
            //res & type ok
            resourceId = R_QTN_CHAT_LOGING_CANCELED;
            noteType = EConfirmationNote;
            break;
            }


        case ECnUiConnForceLogout:
            {
            //res & type ok
            resourceId = R_QTN_CHAT_SERVER_DISCONNECTED_CLIENT;
            noteType = EInfoNote;
            break;
            }


        case ECnUiGPRSConnectionLost:
            {
            //Special case because here is used the system error text
            HBufC* systemErrorText = SystemErrorTextOrNullForCodeL( KErrNetConNoGPRSNetwork );

            //because supported error text are varied between releases, protect against
            //unknown text ==> note is shown with best effort
            if ( systemErrorText )
                {
                CleanupStack::PushL( systemErrorText );
                //new code/////
                if ( iControl )
                    {
                    CCoeEnv::Static()->AppUi()->RemoveFromStack( iControl );
                    delete iControl;
                    iControl = NULL;
                    }
                ///////////////
                DoShowNoteL( *systemErrorText, EErrorNote );
                CleanupStack::PopAndDestroy( systemErrorText ); //systemErrorText
                }
            return; //note is shown completely ==> return
            // no break needed because of return
            }


        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }
    TBool requireSignOnAnim = IntResourceValueL( RSC_CHAT_VARIATION_SHOW_SIGN_IN_ANIMATION );
    if ( aNoteId == ECnUiConnectionOK && requireSignOnAnim )
        {
        iControl->PrepareForFocusGainL();
        iControl->SetFocus( ETrue );
        HBufC* prompt = iCoeEnv->AllocReadResourceLC( resourceId );
        //CEikLabel* aSignInLabel = static_cast<CEikLabel*>(iControl->ComponentControl(0));
        //aSignInLabel->SetTextL(*prompt);
        iControl->AssignSignInText( *prompt );
        iControl->HideStatus();
        iControl->StopAnimation();
        iControl->DrawNow();
        CleanupStack::PopAndDestroy( prompt ); //prompt
        }
    else
        {
        //new code/////
        if ( iControl )
            {
            CCoeEnv::Static()->AppUi()->RemoveFromStack( iControl );
            delete iControl;
            iControl = NULL;
            }
        ///////////////
        DoShowNoteL( resourceId, noteType );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::ShowNoteL( TCnUiConnectionNotesDynText aNoteId,
                               const TDesC& aInsertText )
    {
    TInt promptTextResourceId( 0 );
    TCnUiConnUiNoteType noteType = EErrorNote;

    switch ( aNoteId )
        {
        case ECnUiImproperPresenceId:
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_ERROR_WRONG_WVID;
            noteType = EErrorNote;
            break;
            }

        case ECnUiConnOperationAllreadyRunning:
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_LOGIN_ONGOING_NOTE;
            noteType = EInfoNote;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( promptTextResourceId, aInsertText, iCoeEnv );
    DoShowNoteL( *prompt, noteType );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowLoginErrorNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::ShowLoginErrorNoteL( TInt aErrorCode )
    {
    //do first error code pre mapping
    if ( aErrorCode == KErrCommsLineFail )
        {
        aErrorCode = KCnUiErrGPRSConnectionNotAvailable; //GPRS connection not available
        }


    //now map the error to note
    TInt promptTextResourceId( KErrNotFound );
    TCnUiConnUiNoteType noteType = EErrorNote;
    switch ( aErrorCode )
        {
        case KErrNone:
            {
            //no error to show for KErrNone
            return;
            // no break needed because of return
            }

        case KErrCancel:
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_LOGING_CANCELED;
            noteType = EConfirmationNote;
            break;
            }

        case KCnUiErrorNetworkConnectionNotAllowed: //LoginFailedToOffLineMode
            {
            //res & type ok
            promptTextResourceId = R_QTN_OFFLINE_NOT_POSSIBLE;
            noteType = EErrorNote;
            break;
            }

        //flow trough
        case KPEngNwErrInvalidPassword:   //CSP - 409
        case KPEngNwErrUnknownUser:       //CSP - 531
        case KPEngNwErrInvalidOrUnSupportedUserProperties: //CSP - 702
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_LOGIN_PASSER_NOTE;
            noteType = EErrorNote;
            break;
            }


        case KPEngNwErrForbidden: //CSP - 403
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_LOGIN_ACCOUNT_LOST;
            noteType = EErrorNote;
            break;
            }

        //flow trough
        case KPEngNwErrServiceNotSupported:   //CSP - 405
        case KPEngNwErrServiceNotAgreed:      //CSP - 506
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_ERROR_SERVICE_ERR;
            noteType = EErrorNote;
            break;
            }

        //flow trough
        case KPEngNwErrServiceUnavailable:   //CSP - 503
        case KPEngNwErrMessageQueueIsFull:   //CSP - 507
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_ERROR_SERV_BUSY;
            noteType = EErrorNote;
            break;
            }

        //flow trough
        case KPEngNwErrSessionExpired:        //CSP - 600
        case KErrConnectionTerminated:        //login terminated by "red key" or from ConMan app.
        case KErrDisconnected:
        case KErrTimedOut:
        case KErrCouldNotConnect:
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_LOGIN_CONERROR_NOTE;
            noteType = EErrorNote;
            break;
            }


        case KPEngNwErrForcedLogout:          //CSP - 601
            {
            //res ok & type other
            promptTextResourceId = R_QTN_CHAT_SERVER_DISCONNECTED_CLIENT;
            noteType = EConfirmedErrorNote;
            break;
            }

        case KCnUiErrorInsufficientNWServerCapabilities:
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_INFO_NOT_SUP_CON;
            noteType = EConfirmedErrorNoteMSKSelectIcon;
            break;
            }


        case KPEngNwErrVersionNotSupported:   //CSP - 505
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_ERROR_GEN_ERROR;
            noteType = EErrorNote;
            break;
            }


        case KCnUiErrorSapMissingCompulsoryFields: //LoginMissingCompSettings
            {
            //res & type ok
            promptTextResourceId = R_QTN_CHAT_SETT_COMP_MISS;
            noteType = EErrorNote;
            break;
            }


        default:
            {
            //no any specific note mapped to error code

            //if error is unhandled CSP error ==> show default server error note
            if ( ( KPEngErrorWVServerResponseTop <= aErrorCode ) &&
                 ( aErrorCode <= KPEngErrorWVServerResponseBase ) )
                {
                promptTextResourceId = R_QTN_CHAT_ERROR_GEN_ERROR; //"Server error."
                noteType = EErrorNote;
                break;
                }

            // error from impsengine that has not mapped into presence server error
            if ( ( KImpsGeneralError <= aErrorCode ) && ( aErrorCode <= KImpsErrorKeyIndexInvalid ) )
                {
                // map these to "No connection to service. Try again later."
                promptTextResourceId = R_QTN_CHAT_LOGIN_CONERROR_NOTE;
                noteType = EErrorNote;
                break;
                }

            //If error code is mapped to some system
            //error text, show it. Else default to own no connection error note.
            HBufC* systemErrorText = SystemErrorTextOrNullForCodeL( aErrorCode );
            if ( systemErrorText )
                {
                CleanupStack::PushL( systemErrorText );
                //new code/////
                if ( iControl )
                    {
                    CCoeEnv::Static()->AppUi()->RemoveFromStack( iControl );
                    delete iControl;
                    iControl = NULL;
                    }
                ////////////////
                DoShowNoteL( *systemErrorText, EErrorNote );
                CleanupStack::PopAndDestroy( systemErrorText ); //systemErrorText
                return;
                }
            else
                {
                promptTextResourceId = R_QTN_CHAT_LOGIN_CONERROR_NOTE;  //"No connection to service. Try again later."
                noteType = EErrorNote;
                break;
                }
            }
        }


    if ( promptTextResourceId != KErrNotFound )
        {
        //error mapped to some internal error string ==> show it
        // read variation flag values
        TBool requireSignOnAnim = IntResourceValueL( RSC_CHAT_VARIATION_SHOW_SIGN_IN_ANIMATION );
        if ( promptTextResourceId == R_QTN_CHAT_LOGING_CANCELED && requireSignOnAnim )
            {
            iControl->PrepareForFocusGainL();
            iControl->SetFocus( ETrue );
            HBufC* prompt = iCoeEnv->AllocReadResourceLC( promptTextResourceId );
            //CEikLabel* aSignInLabel = static_cast<CEikLabel*>(iControl->ComponentControl(0));
            //aSignInLabel->SetTextL(*prompt);
            iControl->AssignSignInText( *prompt );
            iControl->HideStatus();
            iControl->StopAnimation();
            iControl->DrawNow();
            CleanupStack::PopAndDestroy( prompt ); //prompt
            }
        else
            {
            //new code/////
            if ( iControl )
                {
                CCoeEnv::Static()->AppUi()->RemoveFromStack( iControl );
                delete iControl;
                iControl = NULL;
                }
            ////////////////
            DoShowNoteL( promptTextResourceId, noteType );
            }
        }
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowLogoutErrorNoteL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::ShowLogoutErrorNoteL( TInt aErrorCode )
    {
    //do first error code pre mapping
    if ( aErrorCode == KErrCommsLineFail )
        {
        aErrorCode = KCnUiErrGPRSConnectionNotAvailable; //GPRS connection not available
        }

    //now map the error to note
    TInt promptTextResourceId( KErrNotFound );
    TCnUiConnUiNoteType noteType = EErrorNote;
    switch ( aErrorCode )
        {
        case KErrNone:
            {
            //no error to show for KErrNone
            return;
            // no break needed because of return
            }

        //flow trough
        case KErrDisconnected:
        case KErrTimedOut:
        case KErrCouldNotConnect:
            {
            promptTextResourceId = R_QTN_CHAT_ERROR_GEN_ERROR;
            noteType = EErrorNote;
            break;
            }

        //flow trough
        case KPEngNwErrServiceUnavailable:   //CSP - 503
        case KPEngNwErrMessageQueueIsFull:   //CSP - 507
            {
            promptTextResourceId = R_QTN_CHAT_ERROR_SERV_BUSY;
            noteType = EErrorNote;
            break;
            }

        default:
            {
            //no any specific note mapped to error code
            //If error code is mapped to some system
            //error text, show it. Else default to
            //own default error note.
            HBufC* systemErrorText = SystemErrorTextOrNullForCodeL( aErrorCode );
            if ( systemErrorText )
                {
                CleanupStack::PushL( systemErrorText );
                DoShowNoteL( *systemErrorText, EErrorNote );
                CleanupStack::PopAndDestroy( systemErrorText ); //systemErrorText
                return;
                }
            else
                {
                promptTextResourceId = R_QTN_CHAT_ERROR_GEN_ERROR;
                noteType = EErrorNote;
                }
            break;
            }
        }

    if ( promptTextResourceId != KErrNotFound )
        {
        //error mapped to some internal error string ==> show it
        DoShowNoteL( promptTextResourceId, noteType );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::ConfirmationQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::ConfirmationQueryL( TCnUiConnectionQueries aQueryId )
    {
    TInt resourceId( 0 );
    switch ( aQueryId )
        {
        case ECnUiCloseConnectionToNetwork:
            {
            //res & type ok
            resourceId = R_QTN_DYC_QUERY_EXIT_CONN_CLOSE;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }

    HBufC* prompt = StringLoader::LoadLC( resourceId, iCoeEnv );
    TBool selection = DoShowConfirmationQueryL( *prompt );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return selection;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::ConfirmationQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::ConfirmationQueryL( TCnUiConnectionQueriesDynText aQueryId,
                                         const TDesC& aInsertText )
    {
    TInt resourceId( 0 );
    switch ( aQueryId )
        {
        case ECnUiDropPECLogin:
            {
            resourceId = R_QTN_CHAT_SETTING_CON_TO_CHAT;
            break;
            }

        case ECnUiDropIMLogin:
            {
            resourceId = R_QTN_CHAT_SETTING_CON_TO_PEC;
            break;
            }

        default:
            {
            CnUiPanicOrLeaveL( EIMPSConnUi_UnknownNoteID, KErrNotSupported );
            break;
            }
        }


    HBufC* prompt = StringLoader::LoadLC( resourceId, aInsertText, iCoeEnv );
    TBool selection = DoShowConfirmationQueryL( *prompt );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    return selection;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::LoginQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::LoginQueryL( TDes& aUsername,
                                  TDes& aPassword,
                                  CIMPSSAPSettings& aSap,
                                  TCnUiLoginDataQueryMode aMode )
    {


    //Check for compulsory settings
    TInt compulsoryStatus = CCnUiConnOpener::CheckConnOpenCompulsorySettingsL( aSap );
    if ( compulsoryStatus != KErrNone )
        {
        ShowLoginErrorNoteL( KCnUiErrorSapMissingCompulsoryFields );
        return FALSE;
        }
    //

    // Show WAP registration query, if variated so
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, RSC_CHAT_VARIATION_IMPSCU_SHOW_WAP_REGISTRATION_QUERY );

    TBool showWapRegQuery  = ResourceUtils::ReadTInt32L( reader );
    CleanupStack::PopAndDestroy(); // reader


    if ( showWapRegQuery )
        {
        // Get pointer to WAP registration settings
        CRepository* cenRep = CRepository::NewLC( KCRUidIMPSConnUI );

        TBool  isWAPQueryShown( EFalse );
        User::LeaveIfError( cenRep->Get( KFlagWapRegistrationDlgShown, isWAPQueryShown ) );

        if ( !isWAPQueryShown )
            {
            isWAPQueryShown = ETrue;
            User::LeaveIfError( cenRep->Set( KFlagWapRegistrationDlgShown, isWAPQueryShown ) );

            if ( ShowWAPQueryL() == EAknSoftkeyOk )
                {
                if ( ShowTAndCDlgL() == EAknSoftkeyOk )
                    {
                    // Open browser with the specified URL
                    HBufC16 *url = HBufC::NewLC( KMaxRegUrlLength );
                    TPtr ptr( url->Des() );
                    TInt urlLength;

                    if ( cenRep->Get( KWAPRegistrationURL, ptr, urlLength ) == KErrOverflow )
                        {
                        //because there could be an overflow
                        CleanupStack::PopAndDestroy( url );

                        url = HBufC::NewLC( urlLength );
                        ptr.Set( url->Des() );
                        User::LeaveIfError( cenRep->Get( KWAPRegistrationURL, ptr, urlLength ) );
                        }

                    OpenWebBrowserL( *url );
                    CleanupStack::PopAndDestroy( url );
                    }
                }
            }

        CleanupStack::PopAndDestroy( cenRep );
        }



    //check for branding
    TInt showLoginQuery = KErrNone;

    showLoginQuery = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_SHOW_UID_PW_QUERY ) ;

    // if username or password is empty we show the query overriding the variation
    if ( ( aUsername.Length() == 0 ) || ( aPassword.Length() == 0 ) )
        {
        showLoginQuery = ETrue;
        }

    if ( ! showLoginQuery )
        {
        // hide login query, assume as accepted
        return ETrue;
        }

    // Show domain selection query, if variated so
    TBool domainQuery = IntResourceValueL( RSC_CHAT_VARIATION_IMPSCU_DOMAIN_SELECTION );
    if ( aUsername.Length() == 0 && domainQuery )
        {
        if ( DisplayDomainSelectionQueryL( aUsername, &aSap ) == 0 )
            {
            // User cancelled the query
            return EFalse;
            }
        if ( aUsername.Length() > 0 )
            {
            // User selected a domain
            aMode = ECnUiEditPrefilledUser;
            }
        }

    TInt retVal( EAknSoftkeyCancel );

    if ( showWapRegQuery )
        {
        //  For
        CCnOUiUsernameAndPwdQuery* query;
        query = CCnOUiUsernameAndPwdQuery::NewL( aUsername,
                                                 aPassword,
                                                 aMode,
                                                 domainQuery,
                                                 *this );

        query->SetPredictiveTextInputPermitted( ETrue ); // T9

        //executeLD returns CEikBidCancel if user cancels the query,
        //else the return value maps to the button ID.
        //do here conversion from button ID's to plain TBools...

        query->PrepareLC( R_LOGIN_DATA_QUERY );

        query->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_CANCEL );
        query->ButtonGroupContainer().DrawDeferred();

        retVal = query->RunLD();
        }
    else
        {
        CCnUiUsernameAndPwdQuery* query;
        query = CCnUiUsernameAndPwdQuery::NewL( aUsername,
                                                aPassword,
                                                aMode,
                                                domainQuery,
                                                *this );
        query->SetPredictiveTextInputPermitted( ETrue ); // T9

        //executeLD returns CEikBidCancel if user cancels the query,
        //else the return value maps to the button ID.
        //do here conversion from button ID's to plain TBools...
        retVal = query->ExecuteLD( R_LOGIN_DATA_QUERY );
        }
    if ( retVal == EAknSoftkeyOk )
        {
        // Query accepted
        return ETrue;
        }
    else if ( retVal == EAknSoftkeyExit && domainQuery )
        {
        // User cleared user id field, show domain selection query again
        aUsername.Delete( 0, aUsername.Length() );
        return LoginQueryL( aUsername, aPassword, aSap, aMode );
        }


    return EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowWAPQueryL
// -----------------------------------------------------------------------------
//
TInt CCnUiUiFacade::ShowWAPQueryL()
    {
    CAknMessageQueryDialog* msgQueryDlg = new( ELeave )CAknMessageQueryDialog( CAknMessageQueryDialog::ENoTone );
    CleanupStack::PushL( msgQueryDlg );

    msgQueryDlg->PrepareLC( R_MESSAGE_QUERY );

    HBufC* resourceText = NULL;
    resourceText = StringLoader::LoadLC( R_QTN_LSK_WAP_REG_ORANGE );
    msgQueryDlg->ButtonGroupContainer().SetCommandL( 0, EAknSoftkeyOk, *resourceText );
    CleanupStack::PopAndDestroy( resourceText );

    resourceText = StringLoader::LoadLC( R_QTN_RSK_WAP_IGNORE_ORANGE );
    msgQueryDlg->ButtonGroupContainer().SetCommandL( 2, EAknSoftkeyCancel, *resourceText );
    CleanupStack::PopAndDestroy( resourceText );

    resourceText = StringLoader::LoadL( R_QTN_NOTE_WAP_ORANGE );
    msgQueryDlg->SetMessageText( *resourceText );

    resourceText = StringLoader::LoadL( R_QTN_NOTE_TITLE_WAP_ORANGE );
    msgQueryDlg->SetHeaderText( *resourceText );

    CleanupStack::Pop( msgQueryDlg );

    TInt command = msgQueryDlg->RunLD();
    return command;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::ShowTAndCDlgL
// -----------------------------------------------------------------------------
//
TInt CCnUiUiFacade::ShowTAndCDlgL()
    {
    CAknMessageQueryDialog* msgQueryDlg = new( ELeave )CAknMessageQueryDialog( CAknMessageQueryDialog::ENoTone );
    CleanupStack::PushL( msgQueryDlg );

    msgQueryDlg->PrepareLC( R_MESSAGE_QUERY );

    HBufC* resourceText = NULL;
    resourceText = StringLoader::LoadLC( R_QTN_IM_AGREE_LSK_REGISTER );
    msgQueryDlg->ButtonGroupContainer().SetCommandL( 0, EAknSoftkeyOk, *resourceText );
    CleanupStack::PopAndDestroy( resourceText );

    resourceText = StringLoader::LoadLC( R_QTN_IM_REFUSE_RSK_REGISTER );
    msgQueryDlg->ButtonGroupContainer().SetCommandL( 2, EAknSoftkeyCancel, *resourceText );
    CleanupStack::PopAndDestroy( resourceText );

    resourceText = StringLoader::LoadL( R_QTN_NOTE_TERMS_WAP_ORANGE );
    msgQueryDlg->SetMessageText( *resourceText );

    resourceText = StringLoader::LoadL( R_QTN_NOTE_TITLE_TERMS_WAP_ORANGE );
    msgQueryDlg->SetHeaderText( *resourceText );

    CleanupStack::Pop( msgQueryDlg );

    TInt command = msgQueryDlg->RunLD();
    return command;
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::OpenWebBrowserL
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::OpenWebBrowserL( const TDesC& aUrl )
    {
    if ( !iBrLauncher )
        {
        iBrLauncher = CBrowserLauncher::NewL();
        }

    //Asynchronous operation to launch the browser with aUrl
    iBrLauncher->LaunchBrowserEmbeddedL( aUrl );
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::MMCDrive
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::MMCDrive( TDes& aDrive ) const
    {
    TParsePtrC mmc( PathInfo::MemoryCardRootPath() );
    aDrive.Zero();
    aDrive.Append( mmc.Drive() );
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::ConstructCompletePathAndCheck
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::ConstructCompletePathAndCheck( TDes& aResult,
                                                    const TDesC& aModuleId,
                                                    const TDesC& aAlternative,
                                                    const TDesC& aFileName )
    {
    // Construct full path for wanted resource
    // MMC drive letter must be here to make sure that TFindFile searches first
    // MMC for the correct brand
    MMCDrive( aResult );
    aResult.Append( KRelativeBrandPath() );
    aResult.Append( aModuleId );
    aResult.Append( KDirSeparator );
    aResult.Append( aAlternative );
    aResult.Append( KDirSeparator );
    aResult.Append( aFileName );

    // Find the actual file
    TFindFile fileFinder( CEikonEnv::Static()->FsSession() );
    TInt err( fileFinder.FindByDir( aFileName, aResult ) );

    // If file was found, then take the first appearance of it
    // (order is correct aready)
    if ( err == KErrNone )
        {
        aResult.Zero();
        aResult.Append( fileFinder.File() );
        }

#ifdef _DEBUG
    RDebug::Print( _L( "ConstructCompletePathAndCheck, TFindFile returned %d for %S" ), err, &aResult );
#endif

    // return was the file found or not
    return err == KErrNone;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::LoadServerIconsL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::LoadServerIconsL( MDesCArray& aServers )
    {
    TInt count( aServers.MdcaCount() );
    __ASSERT_DEBUG( count > 0, User::Panic( KIMPSConnUi_PanicCat, KErrGeneral ) );
    iIconArray = new( ELeave )CAknIconArray( count );

    // load icons
    TBool allEmpty( ETrue );
    HBufC* pathBuf = HBufC::NewLC( KMaxFileName );
    TPtr path( pathBuf->Des() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( !ConstructCompletePathAndCheck( path, KModuleId,
                                             aServers.MdcaPoint( i ), KBitmapFile ) )
            {
            // not found, use empty
            AknIconUtils::CreateIconLC( bitmap, mask, AknIconUtils::AvkonIconFileName(),
                                        EMbmAvkonQgn_prop_empty, EMbmAvkonQgn_prop_empty_mask );
            CleanupStack::Pop( 2 ); // bitmap, mask
            // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
            // CAknIconArray::AppendL takes ownership of the icon
            CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask ); // CSI: 35 # See above
            iIconArray->AppendL( gulIcon );
            }
        else
            {
            // load the icon that was found
            AknIconUtils::CreateIconLC( bitmap, mask, path, 0, 1 );
            CleanupStack::Pop( 2 ); // bitmap, mask
            // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
            // CAknIconArray::AppendL takes ownership of the icon
            CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask ); // CSI: 35 # See above
            iIconArray->AppendL( gulIcon );
            allEmpty = EFalse;
            }
        }
    CleanupStack::PopAndDestroy( pathBuf );

    if ( allEmpty )
        {
        // every icon is empty, delete whole array and show list without icons
        delete iIconArray;
        iIconArray = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::OpenResourceFileLC()
// !!!Notice!!!. Two variables in cleanupstack after call of this method.
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::OpenResourceFileLC( RResourceFile& aResourceFile )
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
// CCnUiUiFacade::IntResourceValueL()
// -----------------------------------------------------------------------------
//
TInt CCnUiUiFacade::IntResourceValueL( TInt aResourceId )
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

// -----------------------------------------------------------------------------
// CCnUiUiFacade::ServerToUseQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::ServerToUseQueryL( MDesCArray& aServers,
                                        TInt aIndexToHighlight,
                                        TInt& aSelectedIndex )
    {
    if ( aServers.MdcaCount() == 0 || aServers.MdcaCount() == 1 )
        {
        User::Leave( KErrNotSupported );
        }

    TRAPD( err, LoadServerIconsL( aServers ) );
    if ( err != KErrNone )
        {
        // didn't manage to load bitmaps, try to show the list without them
        delete iIconArray;
        iIconArray = NULL;
        }

    TInt selection( -1 );
    TInt retVal( -1 );
    // Code scanner warning neglected to put variable on cleanup stack (Id: 35)
    // PrepareLC pushes the dialog into cleanupstack
    CAknListQueryDialog* listQuery = new ( ELeave ) CAknListQueryDialog( &selection ); // CSI: 35 # See above

    if ( iIconArray )
        {
        // icon array exists, construct the query with icons
        listQuery->PrepareLC( R_SERVER_TO_LOGIN_LISTQUERY_GRAPHIC );
        listQuery->SetIconArrayL( iIconArray );
        iIconArray = NULL; // listbox has the ownership of icon array

        // construct formatted text array
        TInt count( aServers.MdcaCount() );
        CDesCArray* items = new( ELeave )CDesCArrayFlat( count );
        CleanupStack::PushL( items );
        for ( TInt i( 0 ); i < count; i++ )
            {
            TPtrC item( aServers.MdcaPoint( i ) );

            // 5 is more than enough for formatted number in front of server list
            HBufC* temp = HBufC::NewLC( item.Length() + KTab().Length() + 5 );
            TPtr tempPtr( temp->Des() );
            tempPtr.Num( i );
            tempPtr.Append( KTab );
            tempPtr.Append( item );

            items->AppendL( tempPtr );
            CleanupStack::PopAndDestroy( temp );
            }

        listQuery->SetItemTextArray( items );
        listQuery->SetOwnershipType( ELbmOwnsItemArray );
        CleanupStack::Pop( items ); // listbox model owns the item array
        }
    else
        {
        // icon array doesn't exist, construct the query without icons
        listQuery->PrepareLC( R_SERVER_TO_LOGIN_LISTQUERY );

        listQuery->SetItemTextArray( &aServers );
        listQuery->SetOwnershipType( ELbmDoesNotOwnItemArray ); // listbox model doesn't own the item array
        }

    //check is the index to highlight on the range...
    if ( ( aIndexToHighlight > 0 ) && ( aIndexToHighlight < aServers.MdcaCount() ) )
        {
        listQuery->ListBox()->SetCurrentItemIndex( aIndexToHighlight );
        }

    retVal = listQuery->RunLD(); //execute the selection


    // if selection made and not cancelled
    if ( ( selection >= 0 ) && ( retVal != 0 ) )
        {
        aSelectedIndex = selection;
        return ETrue;
        }


    return EFalse;
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::HandleIfError()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::HandleIfError( TInt aErr )
    {
    if ( aErr != KErrNone )
        {
        CEikonEnv::Static()->HandleError( aErr );
        }
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::SwitchResourceFileL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::SwitchResourceFileL( const TDesC& aNewResourceFile )
    {
    UseResourceFileL( aNewResourceFile );
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::DisplayDomainSelectionQueryL
// -----------------------------------------------------------------------------
//
TInt CCnUiUiFacade::DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                                  CIMPSSAPSettings* aSap /*= NULL*/ )
    {
    TInt selection( -1 );
    TInt result( 0 );

    CDesCArray* listTexts = NULL;

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader,
                                     R_CHAT_VARIATION_IMPSCU_DOMAIN_SELECTION_LIST );

    // Check if there are any predefined domains
    // Check is needed because ReadDesC16ArrayResourceL
    // panics if there aren't any items defined in resource array
    if ( reader.ReadInt16() > 0 )
        {
        listTexts = iCoeEnv->ReadDesC16ArrayResourceL(
                        R_CHAT_VARIATION_IMPSCU_DOMAIN_SELECTION_LIST );
        }
    else
        {
        listTexts = new ( ELeave ) CDesCArrayFlat( 1 ); // 1 for granularity
        }

    CleanupStack::PopAndDestroy(); // reader
    CleanupStack::PushL( listTexts );
    HBufC* lastItem = NULL;
    lastItem = StringLoader::LoadLC( R_CHAT_LOGIN_SELECT_DOMAIN_OTHER );
    listTexts->AppendL( lastItem->Des() );
    CleanupStack::PopAndDestroy( ); // lastItem

    CCnUiDomainSelectionQueryDialog* dlg =
        CCnUiDomainSelectionQueryDialog::NewL( &selection, aSap );
    dlg->PrepareLC( R_CHAT_VARIATION_IMPSCU_DOMAIN_SELECTION_QUERY );
    dlg->SetItemTextArray( listTexts );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
    result = dlg->RunLD();

    // if selection made and it's not the last item,
    // and query is not cancelled
    if ( ( selection >= 0 ) && ( selection < ( listTexts->Count() - 1 ) )
         && ( result != EAknSoftkeyCancel ) )
        {
        aSelectedDomain = listTexts->MdcaPoint( selection );
        }
    CleanupStack::PopAndDestroy( listTexts );

    return result;
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::DisplayTermsOfUseAndQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::DisplayTermsOfUseAndQueryL()
    {
    TInt retVal = -1;

    // Get branded resource file name
    TFileName resourceFileName;
    TInt err = iSharedData->GetStringKey(
                   ( TIMPSSharedKeys )KBrandingResourceKey, resourceFileName );
    if ( err != KErrNone || resourceFileName.Length() == 0 )
        {
        // No branded resources, return
        return ETrue;
        }

    // Get current servers branding root
    TChar backSlash( KBackSlash );
    // Locate first backslash starting from the end of
    // the path
    TFileName touFileName;
    TInt pos = resourceFileName.LocateReverse( backSlash );
    if ( pos != KErrNotFound )
        {
        touFileName.Append( resourceFileName.Left( pos ) );
        touFileName.Append( KDirSeparator );
        touFileName.Append( KCUITOUFile );
        BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), touFileName );
        }
    // Check that file exists
    if ( !BaflUtils::FileExists( iCoeEnv->FsSession(), touFileName ) )
        {
        // No file for ToU, return
        return ETrue;
        }

    CCnUiTermsOfUseDlg* dlg = CCnUiTermsOfUseDlg::NewLC( touFileName );
    retVal = dlg->RunDialogLD( R_CNUI_TERMS_OF_USE_DIALOG );
    CleanupStack::Pop( dlg );

    if ( retVal == EAknSoftkeyCancel )
        {
        ShowNoteL( ECnUiConnCanceled );
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CCnUiUiFacade::UseResourceFileL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::UseResourceFileL( const TDesC& aNewResourceFile )
    {
    RemoveUsedResourceFile();

    if ( aNewResourceFile.Length() == 0 )
        {
        //empty resource file name ==> use the default resource
        iResourceFileOffset = iCoeEnv->AddResourceFileL( *iDefaultRssFileName );
        }
    else
        {
        // Code scanner warning not using BaflUtils::NearestLanguageFile()
        // when loading a resource file (Id: 53) ignored because
        // the resource file given here is from branding and NearestLanguageFile
        // is already used before it is given here
        iResourceFileOffset = iCoeEnv->AddResourceFileL( aNewResourceFile ); // CSI: 53 # See above
        }

    iResourceFileAdded = ETrue;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::ResourceFileNameL()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::ResourceFileNameL( TDes& aResourceFileName )
    {
    if ( aResourceFileName.MaxLength() < iDefaultRssFileName->Length() )
        {
        User::Leave( KErrOverflow );
        }

    aResourceFileName.Copy( *iDefaultRssFileName );
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::RemoveUsedResourceFile()
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::RemoveUsedResourceFile()
    {
    if ( iCoeEnv && iResourceFileAdded )
        {
        iCoeEnv->DeleteResourceFile( iResourceFileOffset );
        }

    iResourceFileAdded = EFalse;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::SystemErrorTextOrNullForCodeL()
// Private helper.
// -----------------------------------------------------------------------------
//
HBufC* CCnUiUiFacade::SystemErrorTextOrNullForCodeL( TInt aErrorCode )
    {
    HBufC* systemErrorText = NULL;

    TUint flags( 0 );
    TInt id( 0 );

    // Resolve the error
    // Automatic context causes there to come the "default"
    // context title for error note, like:
    //"Internet: Could not resolve host address."
    CTextResolver* textResolver = CTextResolver::NewLC();
    TPtrC systemText( textResolver->ResolveErrorString( aErrorCode, id, flags,
                                                        CTextResolver::ECtxAutomatic ) );
    // textresolver seems to put OOM flag on in some cases when it does not find
    // the error text
    // if there is OOM flag on, but NO blank or unknown error flag, then
    // we probably are running out of memory
    if ( ( flags & EErrorResOOMFlag ) &&
         !( flags & ETextResolverBlankErrorFlag ) &&
         !( flags & ETextResolverUnknownErrorFlag ) )
        {
        User::Leave( KErrNoMemory );
        }

    if ( ( flags & ETextResolverBlankErrorFlag ) ||
         ( flags & ETextResolverUnknownErrorFlag ) ||
         ( systemText.Length() == 0 ) )
        {
        //Actual error is mapped to blank
        //or it is a unknown error
        systemErrorText = NULL;
        }
    else
        {
        //There should be some error text, use it
        systemErrorText = systemText.AllocL();
        }

    CleanupStack::PopAndDestroy( textResolver ); //textResolver
    return systemErrorText;
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::DoShowNoteL()
// Private helper.
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::DoShowNoteL( TInt aNoteTextResourceId, TCnUiConnUiNoteType aNoteType )
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( aNoteTextResourceId );
    DoShowNoteL( *prompt, aNoteType );
    CleanupStack::PopAndDestroy( prompt ); //prompt
    }


// -----------------------------------------------------------------------------
// CCnUiUiFacade::DoShowNoteL()
// Private helper.
// -----------------------------------------------------------------------------
//
void CCnUiUiFacade::DoShowNoteL( const TDesC& aNotePrompt, TCnUiConnUiNoteType aNoteType )
    {
    if ( aNoteType == EConfirmedErrorNote )
        {
        CAknQueryDialog* query = CAknQueryDialog::NewL();
        CleanupStack::PushL( query );
        query->SetPromptL( aNotePrompt );
        CleanupStack::Pop( query ); //query
        query->ExecuteLD( R_CONFIRMED_ERROR_NOTE );
        }
    else if ( aNoteType == EConfirmedErrorNoteMSKSelectIcon )
        {
        CAknQueryDialog* query = CAknQueryDialog::NewL();
        CleanupStack::PushL( query );
        query->SetPromptL( aNotePrompt );
        CleanupStack::Pop( query ); //query
        query->ExecuteLD( R_CONFIRMED_ERROR_NOTE_MSK );
        }
    else
        {
        CAknResourceNoteDialog* note = NULL;
        switch ( aNoteType )
            {
            case EInfoNote:
                {
                note = new ( ELeave ) CAknInformationNote( ETrue );
                break;
                }

            case EConfirmationNote:
                {
                note = new ( ELeave ) CAknConfirmationNote( ETrue );
                break;
                }

            //flow through
            case EErrorNote:
            default:
                {
                note = new ( ELeave ) CAknErrorNote( ETrue );
                break;
                }
            }

        note->ExecuteLD( aNotePrompt );
        }
    }



// -----------------------------------------------------------------------------
// CCnUiUiFacade::DoShowConfirmationQueryL()
// -----------------------------------------------------------------------------
//
TBool CCnUiUiFacade::DoShowConfirmationQueryL( const TDesC& aPromptText )
    {
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    CleanupStack::PushL( query );
    query->SetPromptL( aPromptText );
    CleanupStack::Pop( query ); //query
    return query->ExecuteLD( R_GENERAL_QUERY );
    }

//  End of File
