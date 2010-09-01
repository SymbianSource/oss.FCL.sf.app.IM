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
* Description:  Openchats
*
*/



#ifndef CCAOPENCHATSPC_H
#define CCAOPENCHATSPC_H

//includes
#include <e32std.h>
#include <e32base.h>

#include "chatdefinitions.h"
#include "MCAOpenChatsPC.h"

//forward declarations
class MCAArrayItemPC;

// CLASS DECLARATION

/**
 *  CCAOpenChatsPC
 *
 *  @lib wvuiprocessng.dll
 *  @since 3.2
 */
class CCAOpenChatsPC : public CBase,
            public MCAOpenChatsPC

    {

    public: // From MCAOpenChatsPC


        /**
         * @see MCAOpenChatsPC
         */
        const TPtrC GetIdentification( TInt aIndex );


        /**
         * @see MCAOpenChatsPC
         */
        const TPtrC GetId( TInt aIndex );

        /**
         * @see MCAOpenChatsPC
         */
        TEnumsPC::TItem GetType( TInt aIndex );

        /**
         * @see MCAOpenChatsPC
         */
        TInt Count() const;

        /**
         * @see MCAOpenChatsPC
         */
        void SetActiveItem( const TInt aIndex );

        /**
         * @see MCAOpenChatsPC
         */
        void GetActiveItem( TDesC*& aId, TEnumsPC::TItem& aType );


        /**
         * @see MCAOpenChatsPC
         */
        void SetInvitationIdentification( TDesC* aIdentification );



    public:

        /**
        * NewL - returns the instance to CCAOpenChatsPC
        */
        static CCAOpenChatsPC* NewL( const RPointerArray<MCAArrayItemPC>&
                                     aOpenChatsArray );


        /**
        * Destructor
        */
        virtual ~CCAOpenChatsPC();


    private: //Two phased ctor

        /**
        *CCAOpenChatsPC - ctor
        */
        CCAOpenChatsPC( const RPointerArray<MCAArrayItemPC>&
                        aOpenChatsArray );



        /**
        *ConstructL
        */
        void ConstructL( );

    private: //Data


        //Doesn't own
        const RPointerArray<MCAArrayItemPC>& iOpenChatsArray;

        //holds the index of the active tab item
        TInt iActiveItem;

        //not owned
        TDesC* iInvitationIdentification;

        HBufC* iActiveItemId;

        TEnumsPC::TItem iActiveItemType;

    };

#endif      // CCAOPENCHATSPC_H

// End of File
