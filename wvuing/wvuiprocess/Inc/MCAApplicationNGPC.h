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
* Description:  PC Interface for app ui
*
*/


#ifndef MCAAPPLICATIONNGPC_H
#define MCAAPPLICATIONNGPC_H

#include "TEnumsPC.h"

//class declaration
/**
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAApplicationNGPC
    {


    public:

        /**
        * Enable or disable WV hiding
        * @param aHiding ETrue if hidden
        */
        virtual void SetWVHiding( TInt aHide ) = 0;
        /**
        * Enable or disable WV prefix hiding
        * @param aHiding ETrue if hidden
        */
        virtual void SetWVHidingPrefixOnly( TInt aHide ) = 0; // UI CR : 101-39728
        /**
        * Enable or disable Capitalization of contactlists
        * @param aCapital ETrue if capitalizing is enabled.
        */
        virtual void SetCapitalizingEnabled( TInt aCapital ) = 0; // UI CR : 101-39727

        /**
        * Identification of contact. This is resolved from known
        * identifications.
        * Alias, nickname and contactId.
        * Nickname is shown if set.
        * @param aContactId Id of contact which identification needed.
        * @return Identification of contact.
        *      Alias if nickname is not set and alias is enabled.
        *      Wvid without domain and prefix if nickname does not
               exists and alias is not enabled or not exists.
        */
        virtual const TPtrC Identification( const TDesC& aContactId ) = 0;

        /**
         * Count of pending messages
         * @return count of messages.
         */
        virtual TInt MessagesPendingCount( TInt& aCountOfChats,
                                           TEnumsPC::TUnreadFilter aUnreadFilter = TEnumsPC::EUnreadAll ) = 0;

        /**
         * Count of unread chat group messages.
         * @since v3.2
         * Count of pending messages for groups
         * @return count of messages.
         */
        virtual TInt ChatGroupMessagesPendingCount( TInt &aCountOfChats ) const = 0;

    public:
        /**
        * virtual destructor
        */
        virtual ~MCAApplicationNGPC()
            {

            };


    };

#endif      // MCAAPPLICATIONNGPC_H

// End of File
