/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message container interface
*
*/



#ifndef MCAMESSAGECONTAINER_H
#define MCAMESSAGECONTAINER_H

//  INCLUDES
#include <E32STD.H>

// FORWARD DECLARATIONS
class MCAConversationMessage;
class TCursorSelection;

// CLASS DECLARATION

/**
*  UI side message container interface
*
*  @lib chat.app
*  @since Series 60 3.0
*/
class MCAMessageContainer
    {
    public: // New functions

        /**
         * Add new message to container
         * @param aMessage Message
         */
        virtual void AddMessageL( MCAConversationMessage& aMessage ) = 0;

        /**
         * Inform about changed message
         * @param aIndex Index of changed message
         */
        virtual void MessageChangedL( TInt aIndex ) = 0;

        /**
         * Remove message from container
         * @param aIndex Index of message to be removed
         */
        virtual void RemoveMessage( TInt aIndex ) = 0;

        /**
         * Check if messages are deleted.
         * @return ETrue: chat is deleted,
         *         EFalse: chat is not deleted
         */
        virtual TBool IsDeleted() const = 0;

        /**
         * Mark messages as deleted.
         */
        virtual void MarkDeleted() = 0;

        /**
         * Insert message to container start.
         * @since S60 v3.2
         * @param aMessage Message to be inserted.
         */
        virtual void InsertMessageL( MCAConversationMessage& aMessage ) = 0;

    protected:

        /**
         * Destructor.
         */
        virtual ~MCAMessageContainer() {};

    };

#endif      // MCAMESSAGECONTAINER_H

// End of File
