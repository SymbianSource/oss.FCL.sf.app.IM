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
* Description:  Describes one invitation request
*
*/


#ifndef CCAINVITATIONREQUEST_H
#define CCAINVITATIONREQUEST_H

//  INCLUDES
#include <badesca.h>

const TInt KInviteDefaultTimeout = 1800;    // default timeout value (seconds)

// CLASS DECLARATION

/**
*  Describes one invitation request.
*
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCAInvitationRequest : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aUserIDs specifies the user IDs who are going to receive
        *        this invitation.
        * @param aGroupID is the group WV ID.
        * @param aMessage is the invitation message.
        * @param aTimeout is the invitation timeout in seconds.
        */
        IMPORT_C static CCAInvitationRequest* NewL(
            const CDesCArray& aUserIDs,
            const TDesC&      aGroupID,
            const TDesC&      aMessage,
            const TInt        aTimeout = KInviteDefaultTimeout );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCAInvitationRequest();

    public: // New methods

        /**
        * Gets the invitation message.
        * @return The invitation message.
        */
        IMPORT_C const TDesC& Message() const;

        /**
        * Gets the group ID.
        * @return The group ID.
        */
        IMPORT_C const TDesC& GroupID() const;

        /**
        * Gets the list on invited user IDs.
        * @return Invite list.
        */
        IMPORT_C const CDesCArray& Invitees() const;

        /**
        * Gets the list timeout value.
        * @return Timeout value.
        */
        IMPORT_C TInt Timeout() const;

    private:

        /**
        * C++ default constructor.
        * @param aTimeout is the invitation timeout.
        */
        CCAInvitationRequest( const TInt aTimeout );

        /**
        * By default Symbian OS constructor is private.
        * @see CCAInvitationRequest::NewL.
        */
        void ConstructL( const CDesCArray& aUserIDs,
                         const TDesC&      aGroupID,
                         const TDesC&      aMessage );

    private:    // Data

        // Timeout value
        TInt iTimeout;
        // Owns: Invitation message.
        HBufC* iMessage;
        // Owns: Group ID
        HBufC* iGroupID;
        // Owns: Invitee list
        CDesCArray* iInvitees;

    };

#endif      // CCAINVITATIONREQUEST_H

// End of File
