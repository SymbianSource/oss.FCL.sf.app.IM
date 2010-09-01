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
* Description:  Used when inviting to chats.
*
*/


#ifndef CCAINVITATION_H
#define CCAINVITATION_H

//  INCLUDES
#include <e32base.h>
#include "MCAInvitation.h"

// CLASS DECLARATION

/**
*  Describes one invitation.
*  Used when inviting to chats.
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCAInvitation : public CBase, public MCAInvitation
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aIviteID specifies one invitation.
        * @param aUserID is the WVID.
        * @param aGroupID specifies the wv group id.
        * @param aScreenName is the screen name in a particular group.
        * @param aGroupName specifies the group by name.
        * @param aMessage is the invitation message.
        * @param aValidityPeriod specifies the validity period of this
        *        invitation.
        */
        static CCAInvitation* NewL(
            const TDesC& aInviteID,
            const TDesC& aUserID,
            const TDesC& aGroupID,
            const TDesC& aScreenName,
            const TDesC& aGroupName,
            const TDesC& aMessage,
            TInt   aValidityPeriod = 0 );

        /**
        * Destructor.
        */
        virtual ~CCAInvitation();

    public: // New methods

        /**
         * Sets observer for invitation read.
         * @param aObserver Invitation read observer
         */
        void AddReadObserver( MCAInviteReadObserver* aObserver );

    private: // Methods from MCAInvitation

        /**
        * Gets the invitation ID.
        * @return Invite ID.
        */
        const TDesC& InviteID() const;

        /**
        * Gets the user ID.
        * @return User ID ( WV ID ).
        */
        const TDesC& UserID() const;

        /**
        * Gets the screen name.
        * @return Screen name.
        */
        const TDesC& ScreenName() const;

        /**
        * Gets the group name.
        * @return Group name.
        */
        const TDesC& GroupName() const;

        /**
        * Gets the group ID.
        * @return Group name.
        */
        const TDesC& GroupId() const;

        /**
        * Gets the invitation message.
        * @return Message.
        */
        const TDesC& Message() const;

        /**
        * Gets the timeout value.
        * @return Timeout value.
        */
        TInt Timeout() const;

        /**
         * Gets the time this invite was received (created)
         * @since 2.1
         * @return Time and date of creation
         */
        TTime ReceivedAt() const;

        /**
         * Set invitation as read.
         * @since 3.0
         */
        void SetInvitationAsReadL();

        /**
         * Set invitation as read.
         * @since 3.0
         */
        TBool IsRead() const;

    private: // New methods

        /**
        * C++ default constructor.
        * @param aValidityPeriod describes how long this invitation is valid.
        */
        CCAInvitation( TInt aValidityPeriod );

        /**
        * By default Symbian OS constructor is private.
        * @see CCAInvitation::NewL.
        */
        void ConstructL( const TDesC& aInviteID,
                         const TDesC& aUserID,
                         const TDesC& aGroupId,
                         const TDesC& aScreenName,
                         const TDesC& aGroupName,
                         const TDesC& aMessage );

    private:

        // Data
        TInt iTimeout;
        TTime iCreated;

        // Owns////////////////////////////////////////////////////////////////
        HBufC* iInviteID;
        HBufC* iUserID;
        HBufC* iGroupID;
        HBufC* iScreenName;
        HBufC* iGroupName;
        HBufC* iMessage;
        // ////////////////////////////////////////////////////////////////////

        TBool iRead;

        // Does not own
        MCAInviteReadObserver* iObserver;
    };

#endif      // CCAINVITATION_H

// End of File
