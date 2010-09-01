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
* Description:  Multiline data query dialog
*
*/


// INCLUDE FILES
#include    "CCAMultiLineDataQueryDialog.h"
#include    <aknquerydialog.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAMultiLineDataQueryDialog::CCAMultiLineDataQueryDialog()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAMultiLineDataQueryDialog::CCAMultiLineDataQueryDialog()
        : CAknMultiLineDataQueryDialog( CAknQueryDialog::ENoTone )
    {
    }

// -----------------------------------------------------------------------------
// CCAMultiLineDataQueryDialog::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAMultiLineDataQueryDialog* CCAMultiLineDataQueryDialog::NewL( TDes&  aText1,
                                                                TDes&  aText2 )
    {
    CCAMultiLineDataQueryDialog* self =
        new ( ELeave ) CCAMultiLineDataQueryDialog();
    CleanupStack::PushL( self );
    self->SetDataL( aText1, aText2 );

    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CCAMultiLineDataQueryDialog::~CCAMultiLineDataQueryDialog()
    {
    }


// -----------------------------------------------------------------------------
// CCAMultiLineDataQueryDialog::UpdateLeftSoftKeyL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMultiLineDataQueryDialog::UpdateLeftSoftKeyL()
    {
    CAknMultilineQueryControl* first =
        CAknMultiLineDataQueryDialog::FirstControl();
    CAknMultilineQueryControl* second =
        CAknMultiLineDataQueryDialog::SecondControl();

    // +1 so that it works although there's no text
    HBufC* firstText = HBufC::NewLC( first->GetTextLength() + 1 );
    HBufC* secondText = HBufC::NewLC( second->GetTextLength() + 1 );

    TPtr firstPtr( firstText->Des() );
    TPtr secondPtr( secondText->Des() );

    first->GetText( firstPtr );
    firstPtr.TrimAll();

    second->GetText( secondPtr );
    secondPtr.TrimAll();

    if ( firstPtr.Length() > 0 || secondPtr.Length() > 0 )
        {
        CAknMultiLineDataQueryDialog::MakeLeftSoftkeyVisible( ETrue );
        }
    else
        {
        CAknMultiLineDataQueryDialog::MakeLeftSoftkeyVisible( EFalse );
        }

    CleanupStack::PopAndDestroy( 2, firstText );
    }

//  End of File
