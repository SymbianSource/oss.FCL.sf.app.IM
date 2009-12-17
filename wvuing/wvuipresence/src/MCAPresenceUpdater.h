/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for fetching presence for contacts
*
*/


#ifndef MCAPRESENCEUPDATER_H
#define MCAPRESENCEUPDATER_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Contact presence fetching interface
*
*  @lib CAPresence.lib
*  @since 3.0
*/
class MCAPresenceUpdater
    {
    public:

        /**
         * Update presence of users
         */
        virtual void UpdatePresenceL( const TDesC& aUserId ) = 0;

        virtual void ExtractAttributeModelsForUserL(
            const TDesC& aUserId,
            RPointerArray<MPEngPresenceAttrModel2>& aAttrModels ) = 0 ;

        virtual const RPointerArray<CCAState>& PresenceStates() = 0;

        virtual void SetAttributeProcessing( TBool aEnabled ) = 0;

    protected:

        /**
         * Destruction for protection
         */
        virtual ~MCAPresenceUpdater() {}
    };


#endif      // MCAPRESENCEUPDATER_H

// End of File

