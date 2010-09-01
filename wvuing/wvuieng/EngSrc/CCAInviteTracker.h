/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Describes one tracked invitation
*
*/


#ifndef CCAINVITETRACKER_H
#define CCAINVITETRACKER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MCAInviteTrackerCallback;
class MCAInvitation;

// CLASS DECLARATION

/**
*  Describes one tracked invitation.
*  Wrapper to CCAInvitation class to provide expiry-handling features
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class CCAInviteTracker : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aCallback is callback interface to invite expire handling
        * @param aInvitation Invitation that is wrapped
        */
        static CCAInviteTracker* NewL( MCAInviteTrackerCallback* aCallback,
                                       MCAInvitation* aInvitation );

        /**
        * Destructor.
        */
        virtual ~CCAInviteTracker();

    public: // New methods

        /**
        * Invitation that this class capsulates
        * @return Invitation data
        */
        MCAInvitation* Invitation() const;

    private: // Methods from base classes

        /**
        * Called when active object completes
        */
        void RunL();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @see CCAInviteTracker::NewL
        */
        CCAInviteTracker( MCAInviteTrackerCallback* aCallback,
                          MCAInvitation* aInvitation );

    private: // Data
        // Doesn't own. Invitation data
        MCAInvitation* iInvitation;
        // Doesn't own. Callback interface to invite expire-handling
        MCAInviteTrackerCallback* iCallback;
        // Timeout value (in seconds)
        TInt iTimeout;
    };

#endif      // CCAINVITETRACKER_H

// End of File
