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
* Description:  Abstract factory for creating command objects.
*
*/



#ifndef MCAUICONTACTEDITCMDCB_H
#define MCAUICONTACTEDITCMDCB_H

//  INCLUDES
#include <e32std.h>



// CLASS DECLARATION

class MCAUiContactEditCmdCB
    {
    public:

        class TContactEditParams
            {

            public:
                enum TContactEditOperations
                    {
                    ELoadContactDetails = 0,
                    EAddContact,
                    EUpdateContact,
                    ERemoveContact,
                    EMoveContact

                    } iContactEditOperation;



            public:
                //ctor
                TContactEditParams()
                        : iAliasEnabled( EFalse ),
                        iNicknameEnabled( EFalse ),
                        iOwnData( EFalse ),
                        iOrigId( NULL ),
                        iAlias( NULL ),
                        iNickname( NULL ),
                        iUserId( NULL ),
                        iIdentification( NULL ),
                        iNewContact( EFalse ),
                        iContactEditOperationError( KErrNone ),
                        iListId( NULL )
                    {

                    }

            public:
                TBool iAliasEnabled;
                TBool iNicknameEnabled;

                TBool  iOwnData;
                HBufC* iOrigId;
                HBufC* iAlias;
                HBufC* iNickname;
                HBufC* iUserId;
                HBufC* iIdentification;

                TBool  iNewContact;

                TInt iContactEditOperationError;

                HBufC* iListId;
            };

    public:
        /**
        * Executes the command.
        */
        virtual void GetParameters( TContactEditParams& ) = 0;

        virtual void SetParameters( TContactEditParams& ) = 0;

        virtual ~MCAUiContactEditCmdCB()
            {

            }

    };

#endif      // MCAUICONTACTEDITCMDCB_H
// End of File
