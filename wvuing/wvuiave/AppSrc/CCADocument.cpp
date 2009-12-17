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
* Description:  Document class of chat application
*
*/


// INCLUDE FILES
#include "CCADocument.h"
#include "CCAAppUi.h"
#include "CCAEngine.h"
#include "ChatDebugPrint.h"
#include "MCAProcessManager.h"
#include "CCAProcessManagerFactory.h"

#include <bautils.h>
#include <ErrorUI.h>


// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCADocument* CCADocument::NewL(
    CEikApplication& aApp )     // CChatClientApp reference
    {
    CHAT_DP_FUNC_ENTER( "NewL" );
    CCADocument* self = new ( ELeave ) CCADocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// destructor
CCADocument::~CCADocument()
    {
    CHAT_DP_TXT( "Application engine destruction starting" );
    CCAProcessManagerFactory::ReleaseResources();
    CHAT_DP_TXT( "Application engine destruction done" );
    }

// Symbian OS default constructor can leave.
void CCADocument::ConstructL()
    {
    CHAT_DP_TXT( "Application engine construction starting" );

    TRAPD( err, iProcessManager =  CCAProcessManagerFactory::InstanceL()->GetProcessManager() );
    if ( err )
        {
        // Framework is not capable of showing the errornote if user leaves.
        // Using  CErrorUI, which matches the errorcode passed with coreesponding error string
        // and displays the note.

        CErrorUI* errorUI = CErrorUI::NewLC( );
        errorUI->ShowGlobalErrorNoteL( err );
        CleanupStack::PopAndDestroy( errorUI );
        User::Leave( err );
        }

    iEngine = iProcessManager->GetEngine();
    CHAT_DP_TXT( "Application engine construction done" );
    }

// ----------------------------------------------------
// CCADocument::CreateAppUiL()
// constructs CChatClientAppUi
// ----------------------------------------------------
//
CEikAppUi* CCADocument::CreateAppUiL()
    {
    return new ( ELeave ) CCAAppUi( *iEngine, *iProcessManager );
    }

// End of File
