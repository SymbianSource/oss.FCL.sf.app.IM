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
* Description:  Shows global confirmation query and
*                follows user answering
*
*/



#ifndef CCAGLOBALNOTEHANDLER_H
#define CCAGLOBALNOTEHANDLER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAknGlobalConfirmationQuery;

// CLASS DECLARATION

/**
*  class for showing global confirmation query
*  @lib chat.app
*  @since 2.6
*/
class CCAGlobalNoteHandler : public CActive
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCAGlobalNoteHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CCAGlobalNoteHandler();

    public: // New functions

        /**
         * Shows global note
         * @param aResourceId resource id of the note
         * @return user selection
         */
        TInt ShowNoteL( TInt aResourceId );

        /**
         * Stops global note showing,
         */
        void CancelNoteShowing();

    protected:  // From CActive

        /**
        * @see CActive.
        */
        void DoCancel();

        /**
        * @see CActive.
        */
        void RunL();

        /**
        * @see CActive.
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CCAGlobalNoteHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        /// Global confirmation query, owned
        CAknGlobalConfirmationQuery* iGlobalConfirmationQuery;

        // activeschedulerwait
        CActiveSchedulerWait iWait;

        // the answer
        TInt iAnswer;

    };

#endif      // CCAGLOBALNOTEHANDLER_H

// End of File

