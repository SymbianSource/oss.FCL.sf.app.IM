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
* Description:  Uikon application architecture's application class
 *
*/


// INCLUDE FILES
#include "CCAApp.h"
#include "CCADocument.h"
#include "CAExternalInterface.h"

#include "CCAVariantFactory.h"	// for variation
#include "MCAOpBrandVariant.h"
#include "CAlternator.h"
#include "ChatDebugPrint.h"
#include <eikstart.h>

#include <ErrorUI.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCAApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CCAApp::AppDllUid() const
    {
    CHAT_DP_FUNC_ENTER( "AppDllUid" );
    return KUidChatClient;
    }

// destructor
CCAApp::~CCAApp()
    {
    delete iVariantFactory;

    //Release alternator instance because not needed anymore.
    CAlternator::Release();
    }

// ---------------------------------------------------------
// CCAApp::VariantFactory
// Variant factory accessor
// ---------------------------------------------------------
//
CCAVariantFactory* CCAApp::VariantFactory() const
    {
    return iVariantFactory;
    }

// ---------------------------------------------------------
// CCAApp::CreateDocumentL()
// Creates CChatClientDocument object
// ---------------------------------------------------------
//
CApaDocument* CCAApp::CreateDocumentL()
    {
    CHAT_DP_FUNC_ENTER( "CreateDocumentL" );
    iVariantFactory = CCAVariantFactory::NewL();

    // CR : 101-39724:: Error note needed when application cannot be opened because of the lack of the memory
    MCAOpBrandVariant* opBrand = NULL ;
    TRAPD ( err, opBrand = iVariantFactory->OpBrandVariantL() );
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

    opBrand->RemoveAppResourceL( iCoeEnv, iResourceFileOffset );

    CHAT_DP_TXT( "CreateDocumentL before return" );
    return CCADocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication()
// Constructs CCAApp
// Returns: CApaDocument*: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    CHAT_DP_FUNC_ENTER( "NewApplication" );
    return new CCAApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File
