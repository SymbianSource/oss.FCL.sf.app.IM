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
* Description:  General utitity services for IM UI
*
*/



#ifndef CCAUINGUTILS_H
#define CCAUINGUTILS_H

//  INCLUDES
#include    <badesca.h>
#include    "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class MCAPresence;
class CEikMenuPane;
class CGulIcon;
// CLASS DECLARATION

/**
 *  General utitity services for IM UI
 */
// This is not a C-class although it begins with a CA
// Note: CodeScanner Warning to be ignored.
class CCAUINGUtils
    {
    public:

        enum TChatContactListCreationType
            {
            EChatNoContactList = -1,
            EChatContactListExists = 1,
            EChatContactListWasCreated = 2
            };
    public:


        /**
         * Tries to create a new contact list.
         * Asks a name of the list from user and shows
         * appropriate notes if an error occurs
         * @param aIndex ,index of reference of newly added contact list
         * @return KErrNone if successfull, otherwise an error code.
         */
        static TInt NewContactListL( TInt& aIndex );

        /**
         * Verifies the we have at least one contact list
         * else tries to create one
         * @return EChatNoContactList if there was no contact lists, and
         *              creation failed.
         *         EChatContactListExists if there was at least one contact
         *              list.
         *         EChatContactListWasCreated if there was none and a new list
         *              was created
         */
        static TChatContactListCreationType VerifyContactlistL();

        /**
        * Sets title-text and updates navi-group
        * @since 2.1
        * @param aResourceId Resource id for title text
        * @param aViewId View id for updating navi-group
        */
        static void SetTitleL( TInt aResourceId, const TUid aViewId );

        /**
        * Handles given error. With partial success, errornote will be
        * displayed. Otherwise leaves with aError.
        * @since 2.1
        * @param aError Errorcode
        */
        static void HandleErrorL( TInt aError );

        /**
        * Handles partial successful errors
        * @since 2.1
        */
        static void HandlePartialSuccessL();

        /**
        * Displays error-note
        * @since 2.1
        * @param aResourceId Resource id for note text.
        */
        static void DisplayErrorNoteL( TInt aResourceId );

    };

#endif      // CCAUINGUTILS_H

// End of File
