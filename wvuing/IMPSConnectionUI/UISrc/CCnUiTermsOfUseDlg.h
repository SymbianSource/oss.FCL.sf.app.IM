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
* Description:  Dialog for showing the Terms of Use for a chat server
*
*
*/


#ifndef CCNUITERMSOFUSEDLG_H
#define CCNUITERMSOFUSEDLG_H

#include <akndialog.h>
#include <itemfinder.h>

class CEikRichTextEditor;
class CParaFormatLayer;
class CCharFormatLayer;
class CAknsBasicBackgroundControlContext;
class CFindItemMenu;
class CAknTitlePane;

/**
 *  Dialog for showing the Terms of use -text
 *
 *  @lib IMPSConnectionUi.lib
 *  @since S60 3.1u
 */
NONSHARABLE_CLASS( CCnUiTermsOfUseDlg ) : public CAknDialog,
        public MItemFinderObserver
    {
public:

    /**
     * Executes the dialog and destroys it before returning
     * @since 3.1u
     * @return EAknSoftkeyOk if user accepted.
     *         EAknSoftkeyCancel if user cancelled
     */
    TInt RunDialogLD( TInt aResourceId );

    static CCnUiTermsOfUseDlg* NewL( const TDesC& aToUFileName );

    static CCnUiTermsOfUseDlg* NewLC( const TDesC& aToUFileName );

    virtual ~CCnUiTermsOfUseDlg();

public: // from MObjectProvider

    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

protected:  // from CEikDialog

    /**
    * @see CEikDialog
    */
    void PreLayoutDynInitL();

    /**
    * @see CEikDialog
    */
    void PostLayoutDynInitL();

    /**
    * @see CEikDialog
    */
    TBool OkToExitL( TInt aButtonId );

    /**
    * @see CEikDialog
    */
    void SetSizeAndPosition( const TSize& aSize );

    /**
    * @see CEikDialog
    */
    void SizeChanged();

protected:  //from base class CCoeControl

    /**
    * @see CCoeControl
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );

    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
    * @see CCoeControl
    */
    void HandleResourceChange( TInt aType );

protected:  // From MItemFinderObserver

    /**
     * @see MItemFinderObserver
     */
    void HandleParsingComplete();

private:

    CCnUiTermsOfUseDlg();

    void ConstructL( const TDesC& aToUFileName );

private:	//from MEikCommandObserver

    /**
    * @see MEikCommandObserver
    */
    void ProcessCommandL( TInt aCommand );

private:    // new functions

    /**
     * Inserts line to rich text object
     * @since 3.1u
     * @param aTextResourceId Resource for text in line
     * @param aBold Should text be bolded
     */
    void InsertLineL( TInt aTextResourceId, TBool aBold = EFalse );

    /**
     * Inserts line to rich text object
     * @since 3.1u
     * @param aText Text for line
     * @param aBold Should text be bolded
     */
    void InsertLineL( const TDesC& aText, TBool aBold = EFalse );

    /**
    * Search the LAF specified font for invite dialog
    * @since 3.1u
    * @return LAF specified font
    */
    const CFont* GetLayoutFont();

    /**
    * Show confirmation query when user tries to cancel TOU dialog
    * @since 3.1u
    * @return Query return value
    */
    TInt AskCancelConfirmationL();

    /**
    * Show accept terms -list query
    * @since 3.1u
    * @return Query return value
    */
    TInt AcceptTermsL();

    /**
    * Gets the instance of the title pane
    * @return Title pane pointer
    * @since 3.1u
    */
    CAknTitlePane* GetTitlePaneInstanceL() const;

    /**
    * Stores the old title pane text and sets a custom one
    * @since 3.1u
    */
    void SetNewTitleTextL();

    /**
    * Retores the old title pane text
    * @since 3.1u
    */
    void RestoreTitlePaneTextL() const;

    /**
     * Updates CBA.
     * @since S60 v3.2
     */
    void UpdateCbaL();

    /**
     * Helper method to update TOU message text color according to
     * current skin.
     * Note! This will not update text control
     * @since S60 v3.1u
     */
    void UpdateTextColorL();

    /**
     * Reads Terms of Use (ToU) message from file (iToUFileName).
     * Leaves the returned descriptor in CleanupStack.
     * @since S60 v3.1
     * @return HBufC* Pointer to descriptor containing
     *                the ToU message.
     */
    HBufC* ReadToUMessageLC();

private: // data

    // own.
    CFindItemMenu* iFindItemMenu;

    // not own
    CEikRichTextEditor* iEditor;

    // own
    CItemFinder* iItemFinder;

    // Return value of this dialog
    TInt iReturnVal;

    // Not Own. Paragraph formatter
    CParaFormatLayer* iParaFormatLayer;

    // Not Own. Character formatter
    CCharFormatLayer* iCharFormatLayer;

    // Own. For controlling the background image
    CAknsBasicBackgroundControlContext* iBgContext;

    // Own.
    HBufC* iOldTitleText;

    TInt* iRetVal; // Not own, return value of the dialog,

    // Own.
    HBufC* iToUFileName;
    };

#endif // CCNUITERMSOFUSEDLG_H
