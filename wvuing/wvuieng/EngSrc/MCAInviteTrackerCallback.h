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
* Description:  Callback interface for engine's internal invite expiry-handling
*
*/


#ifndef MCAINVITETRACKERCALLBACK_H
#define MCAINVITETRACKERCALLBACK_H

// FORWARD DECLARATIONS
class 	CCAInvitation;

// CLASS DECLARATION

/**
*  Callback interface for engine's internal invite expiry-handling
*
*  @lib CAEngine.lib
*  @since 1.2
*/
class MCAInviteTrackerCallback
    {
    public:  // Interface

        /**
        * Called when some invite expires.
        * @param aInvitation Invitation that was expired
        */
        virtual void InviteExpiredL( const MCAInvitation* aInvitation ) = 0;

    protected:

        /**
        * Destructor.
        */
        virtual ~MCAInviteTrackerCallback() {};
    };

#endif      // MCAINVITETRACKERCALLBACK_H

// End of File
