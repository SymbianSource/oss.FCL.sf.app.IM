/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Username and password multiline dataquery.
*
*/

#ifndef CCNOUIUSERNAMEANDPWDQUERY_H
#define CCNOUIUSERNAMEANDPWDQUERY_H


//  INCLUDES
#include <aknquerydialog.h>
#include "MCnUiUiFacade.h"


// Class declarations
class CBrowserLauncher;
class MIMPSSharedData;

/**
 * Username and password multi line data query.
 *
 * Username and password multi line data query.
 * Focuses right query line depending from original
 * username and password and also the query mode.
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnOUiUsernameAndPwdQuery ) : public CAknMultiLineDataQueryDialog
    {
private: // Enumerations

    /**
     * Enumerations for initial line focus.
     */
    enum TInitialFocus
        {
        EFirstLine,
        ESecondLine
        };


public: //Symbian OS  constructor

    /**
     * Creates username and password multi line data query.
     * Handles line focus as desired.
     *
     * @since 2.1
     * @param aUserName The username to edit.
     * @param aPwd The password to edit.
     * @param aMode How to determine initially focused line etc behaviour.
     * @param aDomainSelectionQueryVariation Is domain selection query variation on or off.
     * @param aUiFacade Ui facade to use to show error notes.
     */
    static CCnOUiUsernameAndPwdQuery* NewL( TDes& aUserName,
                                            TDes& aPwd,
                                            TCnUiLoginDataQueryMode aMode,
                                            TBool aDomainSelectionQueryVariation,
                                            MCnUiUiFacade& aUiFacade );


protected:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CCnOUiUsernameAndPwdQuery( TInitialFocus aInitialFocus,
                               MCnUiUiFacade& aUiFacade,
                               TBool aDomainSelectionQueryVariation,
                               TBool aCursorToBeginning = EFalse );

    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CCnOUiUsernameAndPwdQuery();


protected:  // Functions from CEikDialog

    /**
     * From CEikdialog.
     * This is called during initialisation and it sets initially
     * focused line by calling DoSetInitialCurrentLineL().
     * @see DoSetInitialCurrentLineL
     *
     * @since 2.1
     */
    void SetInitialCurrentLine();

    /**
     * From CEikdialog.
     *
     * This function is called by the EIKON framework
     * if the user activates a button in the button panel.
     * It is not called if the Cancel button is activated,
     * unless the EEikDialogFlagNotifyEsc flag is set.
     * @param aButtonId The ID of the button that was activated
     * @return Should return ETrue if the dialog should exit,
     * and EFalse if it should not.
     */
    TBool OkToExitL( TInt aButtonId );

    /**
     * From CAknQueryDialog.
     * If the query text is ok (retrieved form query control) then display the
     * left soft key, otherwise hide it.
     */
    void UpdateLeftSoftKeyL();

    /**
     * From CAknDialog.
     * Processes commands for menu options
     */
    void ProcessCommandL( TInt aCommandId );

    /**
     * From CAknDialog.
     * Called after the user presses Options and Before the menu is actually displayed
     */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * From CEikdialog.
     * @param aControlId the control which is to be focused.
     * @since 3.0
     */
    void TryChangeFocusToL( TInt aControlId );

    /**
     * From CEikdialog.
     * @param aType type of the event
     * @since 3.0
     */
    void HandleResourceChange( TInt aType );

    /**
     * From CAknMultiLineDataQueryDialog.
     * Called by OfferkeyEventL(), gives a change to dismiss the query even with
     * keys different than Enter of Ok.
     * @since S60 v3.1
     * @param aKeyEvent  Key event which will be checked.
     * @return           Should the query be dismissed.
     */
    TBool NeedToDismissQueryL( const TKeyEvent& aKeyEvent );

private:  // New functions

    /**
     * Detects which line focus initially.
     * This utility fuction is used by NewL()
     * @see DoSetInitialCurrentLineL
     *
     * @since 2.1
     * @param aFirstLine The first editor text.
     * @param aSecondLine The second editor text.
     * @return Enumeration of the line to initially to focus.
     */
    static TInitialFocus DetectLineToFocus( const TDesC& aFirstLine,
                                            const TDesC& aSecondLine );

    /**
     * Does the actual initial focusing.
     * Focusing is determined from iInitialFocus member.
     *
     * @since 2.1
     */
    void DoSetInitialCurrentLineL();

    /**
    * Reads integer resource value from resources
    * @param aResourceId resource id which is read
    * @return value of wanted integer resource flag
    */
    TInt IntResourceValueL( TInt aResourceId );

    /**
     * Opens correct resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.1
     */
    void OpenResourceFileLC( RResourceFile& aResourceFile );

    /**
     * Checks if the data entered by user is sufficient to login
     * @ return Is it ok to Sign on i.e. user has entered the credentials
     */
    TBool CanSignOnL();

    /**
     * Opens a web browser
     * @param a URL to be opened.
     */
    void OpenWebBrowserL( const TDesC& aUrl );

    /**
     * Shows terms and conditions query dialog
     * @return User agrees or refuses Terms and conditions
     */
    TInt ShowTAndCDlgL();

private:  // data

    ///< The initial focus, owned.
    TInitialFocus iInitialFocus;

    ///< Defines that the cursor is in the beginning of the
    ///< initially focused field. Owned.
    TBool iCursorToBeginning;

    ///< UI facade, not owned
    MCnUiUiFacade& iUiFacade;

    // Is Domain selection query variation on or off.
    TBool iDomainSelectionQueryVariation;

    // Should dialog be exited, used in OkToExitL
    TBool iForcedExit;

    // Browser launcher
    CBrowserLauncher* iBrLauncher;


    // file handle used by resource file reading routine
    RFs   iRFs;

    //owns
    MIMPSSharedData*	iSharedData;
    };

#endif      // CCNOUIUSERNAMEANDPWDQUERY_H


//  End of File

