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
* Description:  Interface for handling one message
*
*/


#ifndef __MCAMESSAGECREATOR_H__
#define __MCAMESSAGECREATOR_H__

#include <e32base.h>
#include <bamdesca.h>

#include "MCAMessage.h"
//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Interface for creating message
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageCreator
    {
    public: // Definitions

        /**
         * SMessageData
         * Struct to contain data of message
         */
        struct SMessageData
            {
            TInt iVersion;
            TInt iOpId;
            const TDesC& iSapId;
            const TDesC& iUserId;
            const TDesC& iSender;
            const TDesC& iTargetId;
            const MDesCArray* iRecipients;
            const MDesCArray* iScreenNames;
            const TDesC& iText;
            const TDesC8& iContentType;
            const TDesC8& iContentData;
            MCAMessage::TMessagerType iMessager;
            };

    public:

        /**
         * Create message.
         * @param aData, Structure to contain data for message.
         * @return New message.
         */
        virtual MCAMessage* CreateMessageL( const SMessageData& aData ) const = 0;

        /**
         * Create failed message
         * @param pSource, Source message which is used as base for new failed message
         * @return New failed message.
         */
        virtual MCAMessage* CreateFailedMessageL( const MCAMessage* pSource ) const = 0;

        /**
         * Create system message
         * @param aType, Type of system message.
         * @param aText, Textual data for message creation.
         * @return New system message
         */
        virtual MCAMessage* CreateSystemMessageL( MCAMessage::TSystemMessage aType, const TDesC& aText ) const = 0;

        /**
         * Create a clone from source message
         * @param pSource, source message
         * @return New cloned message
         */
        virtual MCAMessage* CloneMessageL( const MCAMessage* pSource ) const = 0;

        /**
         * Virtual destructor
         */
        virtual ~MCAMessageCreator() {}
    };

/// Current version of message creator messages.
const TInt KMessageDataVersion = 0;

#endif      // __MCAMESSAGE_H__

// End of File
