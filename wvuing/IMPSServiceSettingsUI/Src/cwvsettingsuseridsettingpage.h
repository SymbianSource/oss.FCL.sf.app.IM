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



#ifndef C_CWVSETTINGSUSERIDSETTINGPAGE_H
#define C_CWVSETTINGSUSERIDSETTINGPAGE_H

#include <akntextsettingpage.h>

/**
 *  Setting page for editing user id in server settings view.
 *
 *  @lib wvservicesettingsui.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CWVSettingsUserIDSettingPage ) : public CAknTextSettingPage
    {
public:

    /**
     * Constructor. See CAknTextSettingPage header for
     * parameter descriptions.
     * @since S60 v3.1
     * @see CAknTextSettingPage
     */
    CWVSettingsUserIDSettingPage( TInt aResourceID,
    TDes& aText,
    TInt aTextSettingPageFlags );

    /**
     * Destructor.
     */
    virtual ~CWVSettingsUserIDSettingPage();

public:
// from base class CAknTextSettingPage

    /**
     * From CAknTextSettingPage.
     * @since S60 v3.1
     * @see CAknTextSettingPage
     */
    void SetSettingPageObserver( MAknSettingPageObserver* aObserver );

private:
// from base class CAknTextSettingPage

    /**
     * From CAknTextSettingPage.
     * @since S60 v3.1
     * @see CAknTextSettingPage
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // data

    /**
     * Setting page observer.
     * Not own.
     */
    MAknSettingPageObserver* iObserver;
    };

#endif // C_CWVSETTINGSUSERIDSETTINGPAGE_H
