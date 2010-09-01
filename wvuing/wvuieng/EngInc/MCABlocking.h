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
* Description:  Interface that provides blocking functions
 *
*/


#ifndef MCABLOCKING_H
#define MCABLOCKING_H

#include <badesca.h>
#include <e32std.h>

/**
 *  MCABlocking interface class provides functionality for the UI components
 *  that operate or retrieve engine's blocking data.
 *
 *  @lib CAEngine.lib
 *  @since 1.2
 */
class MCABlocking
    {
    public:  // Interface
    	
    	//Set the AppUi Exit Flag
    	virtual void SetAppUiExitFlag( TBool aFlag ) = 0;
    	
        /**
         * Get Blocked list from the server.
         * @return Error code.
         */
        virtual TInt GetBlockedListFromServerL() = 0;

        /**
         * Gets the retrieved blocked list. This is available after
         * GetBlockedListFromServerL.
         * @return The blocked array.
         */
        virtual const CDesCArray* BlockedList() = 0;

        /**
         * Insert users to be blocked or granted.
         * @param aBlockList blocked entity's list.
         * @param aGrantList granted entity's list.
         * @return error code.
         */
        virtual TInt InsertL(
            const CDesCArray* aBlockList,
            const CDesCArray* aGrantList,
            TBool aForceUpdate = EFalse ) = 0;

        /**
         * Remove user from blocked or granted list.
         * @param aBlockList blocked to be removed entity's list.
         * @return error code.
         */
        virtual TInt RemoveL(
            const CDesCArray* aBlockList,
            const CDesCArray* aGrantList,
            TBool aForceUpdate = EFalse ) = 0;

        /**
         * Is this contact in our block list or not?
         * @param aContact specifies the contact.
         * @return ETrue if yes EFalse if not.
         */
        virtual TBool IsContactBlocked( const TDesC& aContact,
                                        TBool aUseGrant = EFalse ) const = 0;

        /**
         * Contact list has been changed (to a different list).
         * Causes the blocked-flags to be updated to IM Presence for all the blocked
         * contacts.
         * @since 2.1
         */
        virtual void ContactListChangeL( TBool aUpdateInProgress ) = 0;

        /**
         *  Set temporary grant on/off. If contact is blocked, unblock is permanent.
         *  @since 2.1
         *  @param aContact specifies the contact
         *  @param aOn On/off. ETrue = on
         *  @return errorCode if unsuccessfull
         */
        virtual TInt SetTemporaryGrantL( const TDesC& aContact, TBool aOn ) = 0;

    protected:

        /**
         * Destructor.
         */
        virtual ~MCABlocking() {};

    };

#endif      // MCABlocking_H

// End of File
