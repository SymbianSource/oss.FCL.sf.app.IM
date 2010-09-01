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
* Description:  Setting page for user ID
*
*/


#include "cwvsettingsuseridsettingpage.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CWVSettingsUserIDSettingPage::CWVSettingsUserIDSettingPage(
    TInt aResourceID, TDes& aText, TInt aTextSettingPageFlags ) :
        CAknTextSettingPage( aResourceID, aText, aTextSettingPageFlags )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWVSettingsUserIDSettingPage::~CWVSettingsUserIDSettingPage()
    {
    // Implementation not needed
    }

// ---------------------------------------------------------------------------
// From class CAknTextSettingPage.
// ---------------------------------------------------------------------------
//
void CWVSettingsUserIDSettingPage::SetSettingPageObserver(
    MAknSettingPageObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// From class CAknTextSettingPage.
// ---------------------------------------------------------------------------
//
TKeyResponse CWVSettingsUserIDSettingPage::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // Get char count before key event
    TInt oldCharCount = 0;
    CCoeControl* ctrl = EditorControl();
    CEikEdwin* edwin = NULL;
    if ( ctrl )
        {
        edwin = static_cast<CEikEdwin*>( ctrl );
        oldCharCount = edwin->TextLength();
        }

    // Offer key event to base class
    TKeyResponse response = CAknTextSettingPage::OfferKeyEventL( aKeyEvent, aType );

    // Report changes only when user has cleared editor
    if ( edwin )
        {
        if ( response == EKeyWasConsumed
             && edwin->TextLength() == 0
             && oldCharCount > 0
             && iObserver )
            {
            iObserver->HandleSettingPageEventL( this,
                                                MAknSettingPageObserver::EEventSettingChanged );
            }
        }

    return response;
    }
