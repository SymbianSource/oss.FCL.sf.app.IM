/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Query container implementation.
*
*/

#ifndef __CCNUIQUERYCONTAINER_H
#define __CCNUIQUERYCONTAINER_H

//  INCLUDES
#include <E32Base.h>
#include <AknQueryDialog.h>
#include "MCnUiUiFacade.h"

// FORWARD DECLARATIONS
class MCnUiConnQueryObserver;


/**
 * Supported query types.
 *
 * @since 2.1
 */
enum TNonBlockingQueryType
    {
    ENBConfirmationQuery = 1,
    ENBConfirmedErrorNote
    };


// CLASS DECLARATION
/**
 * Query container implementation.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiQueryContainer ) : public CBase,
        public MCnUiConnQuery
    {
    /**
     * Helper derivation from CAknDialog.
     * Offers non waiting dialog features.
     * Reports events to observer and self pointer
     * NULLing. (Self pointer NULLing needed to ease
     * MCnUiConnQuery client usage.)
     *
     * @since 2.1
     */
    NONSHARABLE_CLASS( CCnUiNonWaitingQueryDialog ) : public CAknQueryDialog
        {
public:  // Two-phased constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since 2.1
         * @param aType The query type.
         * @param aLabelText The label text to show in UI.
         * @param aObserver The observer to notify from dialog selections.
         * @param aSelfPtr Self pointer. Nulled when dialog is destroyed.
         */
        static CCnUiNonWaitingQueryDialog* NewL( TNonBlockingQueryType aType,
        const TDesC& aLabelText,
        MCnUiConnQueryObserver& aObserver,
        CCnUiNonWaitingQueryDialog** aSelfPtr );

        /**
         * Destructor.
         */
        virtual ~CCnUiNonWaitingQueryDialog();

private:
        /**
         * C++ constructor.
         */
        CCnUiNonWaitingQueryDialog( MCnUiConnQueryObserver& aObserver,
                                    CCnUiNonWaitingQueryDialog** aSelfPtr );

        /**
         * Symbian OS constructor.
         */
        void LaunchQueryLD( TNonBlockingQueryType aType, const TDesC& aLabelText );


private: //From CEikDialog.

        /**
         * From CEikDialog.
         * Overridden to report the selection to observer.
         * @since 2.1
         */
        TBool OkToExitL( TInt aButtonId );


private: // data

        ///<The observer, not owned.
        MCnUiConnQueryObserver&             iObserver;

        ///< Self pointer reference.
        ///< Used to NULL self pointer in deletion. Not owned.
        CCnUiNonWaitingQueryDialog**        iSelfPtr;
        };



public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 2.1
     * @param aType query type
     * @param aLabelText The label text to show in UI.
     * @param aObserver The observer to notify.
     */
    static CCnUiQueryContainer* NewL( TNonBlockingQueryType aType,
                                      const TDesC& aLabelText,
                                      MCnUiConnQueryObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CCnUiQueryContainer();

private:

    /**
     * C++ constructor.
     */
    CCnUiQueryContainer();


private: // data

    ///<Used query dialog. Owned.
    CCnUiNonWaitingQueryDialog*      iDlg;
    };

#endif      //  __CCNUIQUERYCONTAINER_H

//  End of File

