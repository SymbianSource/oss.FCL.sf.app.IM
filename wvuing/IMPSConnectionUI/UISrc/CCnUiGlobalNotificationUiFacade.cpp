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
* Description:  Global notification UI implementation.
*
*/

// INCLUDE FILES
#include  "CCnUiGlobalNotificationUiFacade.h"
#include  "CnUiResourceFileName.h"
#include  "CCnUiGlobalNoteContainer.h"
#include  "CCnUiClientGroupUiNotifyMediator.h"

#include <StringLoader.h>
#include <IMPSConnectionUiNG.rsg>
#include <avkon.rsg>

// ================= GLOBAL FUNCTIONS ====================
// -----------------------------------------------------------------------------
// CreateGlobalNotificationUiFacadeL()
// -----------------------------------------------------------------------------
//
GLREF_C MCnUiGlobalNotificationUiFacade* CreateGlobalNotificationUiFacadeL()
    {
    return CCnUiGlobalNotificationUiFacade::NewL();
    }


// ================= MEMBER FUNCTIONS =======================
CCnUiGlobalNotificationUiFacade* CCnUiGlobalNotificationUiFacade::NewL()
    {
    CCnUiGlobalNotificationUiFacade* self = new ( ELeave ) CCnUiGlobalNotificationUiFacade();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CCnUiGlobalNotificationUiFacade::CCnUiGlobalNotificationUiFacade()
    {
    }


// Destructor
CCnUiGlobalNotificationUiFacade::~CCnUiGlobalNotificationUiFacade()
    {
    iResFile.Close();
    iFs.Close();
    }


// Symbian OS default constructor can leave.
void CCnUiGlobalNotificationUiFacade::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );

    TFileName resourceFileName;
    CnUiResourceFileName::NearestForCurrentLanguage( iFs, resourceFileName  );
    iResFile.OpenL( iFs, resourceFileName );
    iResFile.ConfirmSignatureL();
    }



// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::ShowLoginUserDataErrorL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNotificationUiFacade::ShowLoginUserDataErrorL( const TDesC& aServerName )
    {
    //AA Error - dynamic global note, information note icon
    HBufC* promptText = ReadResourceWithDynTextLC( R_QTN_CHAT_INCOR_USERNAMES_NOCON, aServerName );

    CCnUiGlobalNoteContainer* gnc = CCnUiGlobalNoteContainer::NewLC();
    gnc->ShowWaitingNoteL( *promptText );
    CleanupStack::PopAndDestroy( 2 ); //globalNote, promptText
    }



// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::ShowLoginDetailedErrorL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNotificationUiFacade::ShowLoginDetailedErrorL( const TDesC& aServerName,
                                                               const TDesC& aDetErrDescription )
    {
    //AA Error - dynamic global note, information note icon + detailed note if needed
    HBufC* promptText = ReadResourceWithDynTextLC( R_QTN_CHAT_AUTOM_CON_FAILED, aServerName );
    CCnUiGlobalNoteContainer* gnc = CCnUiGlobalNoteContainer::NewLC();

    if ( aDetErrDescription.Length() > 0 )
        {
        gnc->ShowWaitigDetailedNoteL( *promptText, aDetErrDescription );
        }
    else
        {
        //detailed description is empty, default to system error
        //compare to CCnUiUiFacade and login / logout errors
        HBufC* defaultDetErrDesc = ReadResourceLC( R_QTN_CHAT_ERROR_GEN_ERROR );
        gnc->ShowWaitigDetailedNoteL( *promptText, *defaultDetErrDesc );
        CleanupStack::PopAndDestroy( defaultDetErrDesc ); //defaultDetErrDesc
        }

    CleanupStack::PopAndDestroy( 2 ); //gnc, promptText
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::ShowLoginDetailedErrorL()
// -----------------------------------------------------------------------------
//
void CCnUiGlobalNotificationUiFacade::ShowLoginDetailedErrorL( const TDesC& aServerName,
                                                               TInt aNoteTextResourceId )
    {
    //AA Error - dynamic global note, information note icon + detailed note if needed
    HBufC* promptText = ReadResourceWithDynTextLC( R_QTN_CHAT_AUTOM_CON_FAILED, aServerName );
    CCnUiGlobalNoteContainer* gnc = CCnUiGlobalNoteContainer::NewLC();

    if ( aNoteTextResourceId != KErrNone )
        {
        HBufC* errorText = ReadResourceLC( aNoteTextResourceId );
        gnc->ShowWaitigDetailedNoteL( *promptText, *errorText );
        CleanupStack::PopAndDestroy( errorText );
        }
    else
        {
        //detailed description is empty, default to system error
        //compare to CCnUiUiFacade and login / logout errors
        HBufC* defaultDetErrDesc = ReadResourceLC( R_QTN_CHAT_ERROR_GEN_ERROR );
        gnc->ShowWaitigDetailedNoteL( *promptText, *defaultDetErrDesc );
        CleanupStack::PopAndDestroy( defaultDetErrDesc ); //defaultDetErrDesc
        }

    CleanupStack::PopAndDestroy( 2 ); //gnc, promptText
    }

// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::CGQActiveConnectionSuppressForScheduledConnectionL()
// -----------------------------------------------------------------------------
//
TBool CCnUiGlobalNotificationUiFacade::CGQActiveConnectionScheduledCloseL(
    const TArray<TIMPSConnectionClient>& aClientsToShow )
    {
    __ASSERT_ALWAYS( aClientsToShow.Count(), User::Leave( KErrArgument ) );

    CCnUiClientGroupUiNotifyMediator* cgNM = CCnUiClientGroupUiNotifyMediator::NewLC();
    TBool ret = cgNM->CGQActiveConnectionScheduledCloseL( aClientsToShow );
    CleanupStack::PopAndDestroy( cgNM ); //gnc
    return ret;
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::CGQActiveConnectionSuppressForScheduledConnectionL()
// -----------------------------------------------------------------------------
//
TBool CCnUiGlobalNotificationUiFacade::CGQActiveConnectionSuppressForScheduledConnectionL(
    const TDesC& aServerName,
    const TArray<TIMPSConnectionClient>& aClientsToShow )

    {
    __ASSERT_ALWAYS( aClientsToShow.Count(), User::Leave( KErrArgument ) );

    CCnUiClientGroupUiNotifyMediator* cgNM = CCnUiClientGroupUiNotifyMediator::NewLC();
    TBool ret = cgNM->CGQActiveConnectionSuppressForScheduledConnectionL(
                    aServerName, aClientsToShow );
    CleanupStack::PopAndDestroy( cgNM ); //gnc
    return ret;
    }



// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::ReadResourceLC()
// -----------------------------------------------------------------------------
//
HBufC* CCnUiGlobalNotificationUiFacade::ReadResourceLC( TInt aNoteTextResourceId )
    {
    //Own resource reader for AA plug-in environment (no CONE facilities available)

    //resource texts are hold in TBUFs
    //TBUF == "A utility struct that holds one non-zero-terminated string."
    //==> no lead bytes in resource string

    TInt plainResourceId = 0x00000fff & aNoteTextResourceId; // Remove offset from id
    HBufC8* rawDataBuf = iResFile.AllocReadLC( plainResourceId );

    //raw data buffer is actually unicode text ==> treat it so
    TPtrC16 rawData( ( TUint16* ) rawDataBuf->Ptr(),
                     rawDataBuf->Length() / 2 );

    HBufC16* resourceData = rawData.AllocL();
    CleanupStack::PopAndDestroy( rawDataBuf ); //rawDataBuf

    CleanupStack::PushL( resourceData );

    return resourceData;
    }


// -----------------------------------------------------------------------------
// CCnUiGlobalNotificationUiFacade::ReadResourceWithDynTextLC()
// -----------------------------------------------------------------------------
//
HBufC* CCnUiGlobalNotificationUiFacade::ReadResourceWithDynTextLC( TInt aNoteTextResourceId,
                                                                   const TDesC& aInsertText )
    {
    //Own dynamic string loader for AA plug-in environment (no CONE facilities available)

    HBufC* resStringBuf = ReadResourceLC( aNoteTextResourceId );

    TInt reqDynLen = resStringBuf->Length() + aInsertText.Length();
    HBufC* dynStringBuf = HBufC::NewL( reqDynLen );
    TPtr dynString = dynStringBuf->Des();
    StringLoader::Format( dynString,
                          *resStringBuf,
                          -1,             //No index code in source string
                          aInsertText );

    CleanupStack::PopAndDestroy( resStringBuf ); //resStringBuf
    CleanupStack::PushL( dynStringBuf );
    return dynStringBuf;
    }



//  End of File



