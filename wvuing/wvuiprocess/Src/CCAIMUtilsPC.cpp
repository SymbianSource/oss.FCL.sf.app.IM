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


#include "CAUtils.h"
#include "CCAIMUtilsPC.h"
#include "CAPresenceManager.h"
#include "MCAPresence.h"
#include "MCAContactLists.h"
#include "MCAContactList.h"
#include "MCAStoredContacts.h"
#include "CCAStorageManagerFactory.h"
#include "ChatDefinitions.h"

#include    <collate.h>

CCAIMUtilsPC :: CCAIMUtilsPC()
    {
    iPresence = NULL;
    iContactList = NULL;
    }

CCAIMUtilsPC* CCAIMUtilsPC :: NewL()
    {
    CCAIMUtilsPC* self = new( ELeave ) CCAIMUtilsPC();

    CleanupStack::PushL( self );

    self->ConstructL();


    CleanupStack::Pop( self );

    return self;
    }


void CCAIMUtilsPC::ConstructL()
    {
    iPresence = CAPresenceManager::InstanceL();

    }


EXPORT_C TBool CCAIMUtilsPC :: PresenceContactPCL()
    {
    iContactList = iPresence->ContactLists();

    if ( iContactList )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

EXPORT_C TInt CCAIMUtilsPC::IdsAlreadyExistsOrNotL( TPtr aNamePtr )
    {
    // check the display id's of local lists
    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();
    TInt count( contacts->ListCount() );
    TInt err = KErrNone;

    // get the standard method
    TCollationMethod m = *Mem::CollationMethodByIndex( 0 );
    m.iFlags |= TCollationMethod::EIgnoreNone; // dont ignore punctuation and spaces

    for ( TInt i( 0 ); i < count && err == KErrNone; ++i )
        {
        if ( contacts->ListAt( i ).DisplayName().CompareC(
                 aNamePtr, KCollationLevel, &m ) == 0 )
            {
            // list with this name already exists
            err = KErrAlreadyExists;
            }
        }

    return err;

    }

EXPORT_C TInt  CCAIMUtilsPC::CreateContactListPCL( const TDesC& aname )
    {
    TInt err = iContactList->CreateContactListL( aname );
    return err;
    }

EXPORT_C TBool CCAIMUtilsPC::IsThereAnyContactsL()
    {

    MCAStoredContacts* contacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    TInt count( contacts->ListCount() );

    if ( 0 == count )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

EXPORT_C MCAContactLists* CCAIMUtilsPC::GetContactList()
    {
    return iContactList;
    }


CCAIMUtilsPC::~CCAIMUtilsPC()
    {
    //
    }
