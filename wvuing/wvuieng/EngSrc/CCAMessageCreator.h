/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Creator class for messages.
*
*/


#ifndef CCAMESSAGECREATOR_H
#define CCAMESSAGECREATOR_H

#include "MCAMessageCreator.h"

//	FORWARD CLASS DECLERATIONS
class MCABufferMemoryHandler;

// CLASS DECLARATION

/**
 *  Creator class for messages.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */

class CCAMessageCreator : public CBase, public MCAMessageCreator
    {
    public: // Construction

        /**
         * Construction
         * @param aMemoryHandler Handles memory
         * @return Pointer to new created instance of CCAMessageCreator
         */
        static CCAMessageCreator* NewL();

        /**
         * Construction
         * @param aMemoryHandler Handles memory
         * @return Pointer to new created instance of CCAMessageCreator
         */
        static CCAMessageCreator* NewLC();

        /**
         * Destruction
         */
        ~CCAMessageCreator();

    private: // Construction

        /**
         * Constructor
         */
        CCAMessageCreator();

    private: // From MCAMessageCreator

        /**
         * @see MCAMessageCreator
         */
        MCAMessage* CreateMessageL( const SMessageData& aData ) const;

        /**
         * @see MCAMessageCreator
         */
        MCAMessage* CreateFailedMessageL( const MCAMessage* pSource ) const;

        /**
         * @see MCAMessageCreator
         */
        MCAMessage* CreateSystemMessageL( MCAMessage::TSystemMessage aType,
                                          const TDesC& aText ) const;

        /**
         * @see MCAMessageCreator
         */
        MCAMessage* CloneMessageL( const MCAMessage* pSource ) const;

    private: // New helper methods

        /**
         * Create text message.
         * @param aData Data for message.
         * @return new MCAMessage. Ownership is passed to caller
         */
        MCAMessage* CreateTextMessageL( const SMessageData& aData ) const;

        /**
         * Create content message.
         * @param aData Data for message.
         * @return new MCAMessage. Ownership is passed to caller
         */
        MCAMessage* CreateContentMessageL( const SMessageData& aData ) const;

        /**
         * Create failed text message.
         * @param pSource. Message base for failed message
         * @return new MCAMessage. Ownership is passed to caller
         */
        MCAMessage* CreateFailedTextMessageL( const MCAMessage* pSource ) const;

        /**
         * Resolve right type for message
         */
        void ResolveMessageType( MCAMessage* aMessage ) const;

    private: // Member data

    };

#endif      // CCAMESSAGECREATOR_H

// End of File
