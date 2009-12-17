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
* Description:  Openchats (Tab items)
*
*/



#ifndef MCAOPENCHATSPC_H
#define MCAOPENCHATSPC_H


//includes
#include "TEnumsPC.h"


//forward declarations
class MCAArrayItemPC;


// CLASS DECLARATION

/**
 *  Interface for tab related items
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class MCAOpenChatsPC
    {

    public:

        /**
         * GetIdentification: returns the Identification
         * @param aIndex - Index of the item for which Identification
         *                 is required
         * @return TPtrC - Identification
         */
        virtual const TPtrC GetIdentification( TInt aIndex ) = 0;


        /**
         * GetWvId: returns the WV user ID
         * @param aIndex - Index of the item for which WvId
         *                 is required
         * @return TPtrC - WV user ID
         */
        virtual const TPtrC GetId( TInt aIndex ) = 0;


        /**
         * GetType: returns the type of the item
         * @param aIndex - Index of the item for which type
         *                 is required
         * @return TEnumsPC::TTabType - conversation/invitation/group
         */
        virtual TEnumsPC::TItem GetType( TInt aIndex ) = 0;




        /**
         * Count: Returns the number of items
         * @return TInt - Total number of openchats item
         */
        virtual TInt Count() const = 0;


        /**
         * SetActiveItem: Sets the item as the current active one
         * @param aIndex - Index of the item
         * @return void
         */
        virtual void SetActiveItem( const TInt aIndex ) = 0;

        virtual void GetActiveItem( TDesC*& aId, TEnumsPC::TItem& aType ) = 0;


        /**
         * SetInvitationIdentification: Sets the title for invitation items
         * @param aIdentification - Ptr to the invitaion title
         *							Ownership should not be transferred
         * @return void
         */
        virtual void SetInvitationIdentification( TDesC* aIdentification ) = 0;



        /**
         * Virtual destructor
         */
        virtual ~MCAOpenChatsPC()
            {
            };


    };

#endif      // MCAOPENCHATSPC_H

// End of File
