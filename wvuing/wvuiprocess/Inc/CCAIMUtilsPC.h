/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: IM Utils Class on Process Component Side
*
*/
#ifndef CCAIMUTILSPC_H
#define CCAIMUTILSPC_H

#include    <badesca.h>

class CAPresenceManager;
class MCAPresence;
class MCAContactLists;

class CCAIMUtilsPC : public CBase

    {

    public:

        //symbian 2 phase construction
        static CCAIMUtilsPC* NewL();

        void ConstructL();

        //if there is a contact list returns true else returns false
        IMPORT_C TBool PresenceContactPCL();

        //check whether the ID is already present.
        //if yes returns KErrAlreadyExists else returns KErrNone
        IMPORT_C TInt IdsAlreadyExistsOrNotL( TPtr aNamePtr );

        //It actually creates the contact list
        IMPORT_C TInt CreateContactListPCL( const TDesC& aname );

        IMPORT_C TBool IsThereAnyContactsL();

        IMPORT_C MCAContactLists* GetContactList();

        ~CCAIMUtilsPC();

    private:

        //private constructor
        CCAIMUtilsPC();

        //data
        MCAPresence* iPresence;
        MCAContactLists* iContactList;

    };


#endif // CCAIMUTILSPC_H
