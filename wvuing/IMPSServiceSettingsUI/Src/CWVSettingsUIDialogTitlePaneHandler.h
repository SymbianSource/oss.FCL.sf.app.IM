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
* Description:  Title pane handler
*
*/

#ifndef CWVSETTINGSUIDIALOGTITLEPANEHANDLER_H
#define CWVSETTINGSUIDIALOGTITLEPANEHANDLER_H

//  INCLUDES

// FORWARD DECLARATIONS
class CAknTitlePane;


// CLASS DECLARATION
/**
*  Contains titlepane handling features, that are needed
*  in all settingpages.
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUIDialogTitlePaneHandler ) : public CBase
    {
public:  // Constructors and destructor

    /**
    * C++ default constructor.
    */
    CWVSettingsUIDialogTitlePaneHandler();

    /**
    * Destructor.
    */
    virtual ~CWVSettingsUIDialogTitlePaneHandler();


public:  // New functions - titlepane related

    /**
     * Set title pane text from given text.
     * @since 2.1
     * @param aTitleText Text to set to title pane,
     */
    void SetTitlePaneTextL( const TDesC& aTitleText ) const;

    void SetTitlePaneDefaultTextL() const;

    /**
     * Stores current titlepanetext, so it can be later restored.
     * Can be called several times, but restoring is done to lastly
     * stored text.
     * @since 2.1
     */
    void StoreTitlePaneTextL();

    /**
     * Restores titlepanetext to previously stored one.
     * If there isn't any stored title, restores to aplication default.
     * @since 2.1
     */
    void RestoreTitlePaneText() const;

private:  // New functions

    /**
     * Gets titlepane instance from CEikonEnv
     * @since 2.1
     * @return The CAknTitlePane instance.
     */
    CAknTitlePane* GetTitlePaneInstanceL() const;

    /**
     * Does actual restoring.
     * @since 2.1
     */
    void DoRestoreTitlePaneTextL() const;

private:  //data
    HBufC* iOldTitleText;  //stored title text
    };

#endif      // CWVSettingsUIDIALOGTITLEPANEHANDLER_H


//  End of File

