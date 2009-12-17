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
* Description:  PC Implementation for app ui
*
*/


#ifndef CCAAPPLICATIONNGPC_H
#define CCAAPPLICATIONNGPC_H

#include <e32base.h>
// INCLUDES
#include	"MCAApplicationNGPC.h"
#include    "MCAMessagesReadInterface.h"

// FORWARD DECLARATIONS
class CCAEngine;
class MCAStoredContacts;


//class declaration
/**
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCAApplicationNGPC:  public CBase,
            public MCAApplicationNGPC

    {

    public:

        /**
        * Symbian Two Phase Constructor
        * @param aEngine : a reference to chat engine
        */
        static CCAApplicationNGPC* NewL( CCAEngine& aEngine );


        /**
        * Second phased symbian constructor.
        */
        void ConstructL();

        /**
        * virtual destructor
        */
        virtual ~CCAApplicationNGPC();

    public:

        /**
         * Enable or disable WV hiding
         * @param aHiding ETrue if hidden
         */
        virtual void SetWVHiding( TInt aHide );
        /**
         * Enable or disable WV Prefix hiding
         * @param aHiding ETrue if hidden
         */
        virtual void SetWVHidingPrefixOnly( TInt aHide ); //UI CR : 101-39728
        /**
        * Enable or disable Capitalization of contactlists
        * @param aCapital ETrue if capitalizing is enabled.
        */
        void SetCapitalizingEnabled( TInt aCapital ); //UI CR : 101-39727

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
        virtual const TPtrC Identification( const TDesC& aContactId );

        /**
         * Count of pending messages
         * @return count of messages.
         */
        virtual TInt MessagesPendingCount( TInt& aCountOfChats,
                                           TEnumsPC::TUnreadFilter aUnreadFilter =
                                               TEnumsPC::EUnreadAll );

        /**
         * Count of unread chat group messages.
         * @since v3.2
         * Count of pending messages for groups
         * @return count of messages.
         */
        TInt ChatGroupMessagesPendingCount( TInt &aCountOfChats ) const;

    private:
        MCAMessagesReadInterface::TUnreadFilter
        ConvertToEngineUnreadFilter( TEnumsPC::TUnreadFilter aUnreadFilter );


    private:

        /**
        * default constructor
        * @param aEngine : Reference to the engine component
        */
        CCAApplicationNGPC( CCAEngine& aEngine );

    private:

        //Reference to the engine component
        CCAEngine& iEngine;

        //Doesnt own, Handle to Interface for stored contacts
        MCAStoredContacts* iStoredContacts;

    };

#endif      // CCAAPPLICATIONNGPC_H

// End of File
