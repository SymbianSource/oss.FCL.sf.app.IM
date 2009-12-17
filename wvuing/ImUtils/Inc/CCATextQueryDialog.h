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
* Description:  Text query dialog that accepts empty input
*
*/



#ifndef CCATEXTQUERYDIALOG_H
#define CCATEXTQUERYDIALOG_H

//  INCLUDES
#include <AknQueryDialog.h>

// CLASS DECLARATION

/**
 *  Text query dialog that accepts empty input.
 *  Wrapper for CAknTextQueryDialog
 *
 *  @lib chat.app
 *  @since
 */
class CCATextQueryDialog : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @see CAknTextQueryDialog::NewL
         */
        IMPORT_C static CCATextQueryDialog* NewL( TDes& aDataText,
                                                  const TTone& aTone = ENoTone,
                                                  TBool aLeftSoftkeyAlwaysVisible = ETrue,
                                                  TBool aDomainSelectionQuery = EFalse );

        /**
         * Destructor.
         */
        ~CCATextQueryDialog();

    public: // Functions from base classes

        /**
         * From CAknTextQueryDialog, allows empty text
         * Overwritten method
         */
        IMPORT_C void UpdateLeftSoftKeyL();

        /**
         * Handle key events.
         * @see CAknTextQueryDialog
         * @since
         */
        IMPORT_C TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType );

        /**
         * Called before dialog is closed.
         * @see CAknTextQueryDialog
         * @since
         */
        IMPORT_C TBool OkToExitL( TInt aButtonId );

    private:

        /**
         * C++ default constructor.
         * @see CAknTextQueryDialog::CAknTextQueryDialog
         */
        CCATextQueryDialog( TDes& aDataText, const TTone& aTone = ENoTone,
                            TBool aLeftSoftkeyAlwaysVisible = ETrue,
                            TBool aDomainSelectionQuery = EFalse );

    private: // Data

        TBool iLeftSoftkeyAlwaysVisible;

        TBool iDomainSelectionQuery;
    };

#endif      // CCATEXTQUERYDIALOG_H

// End of File
