/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog for setting server settings
*
*/


// INCLUDE FILES
#include	"CWVSettingsUIDialogImp.h"
#include	"CWVSettingsUINGDialog.h"

#include	<cimpssapsettingsstore.h>


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWVSettingsUIDialog::CWVSettingsUIDialog( )
    {
    }

// -----------------------------------------------------------------------------
// CWVSettingsUIDialog::FormUnikServerL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CWVSettingsUIDialog* CWVSettingsUIDialog::NewL ( )
    {
    CWVSettingsUIDialog* self = CWVSettingsUIDialogImp::NewL( );
    return self;
    }

// Destructor
CWVSettingsUIDialog::~CWVSettingsUIDialog()
    {
    }


//  End of File
