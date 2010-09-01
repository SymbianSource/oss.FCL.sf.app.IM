/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Storage database table names
 *
*/


#ifndef __CCASTORAGEDEFS_H__
#define __CCASTORAGEDEFS_H__

// Name for database.
_LIT( KStorageName, "ccastorage.db" );

// Names for database tables
_LIT( KContactTable, "Contacts" );
_LIT( KGroupTable, "Groups" );

// Names for database columns
_LIT( KFirstName, "FirstName" );
_LIT( KLastName, "LastName" );
_LIT( KUserId, "UserId" );

_LIT( KContactItemId, "ContactItemId" );
_LIT( KContactData, "ContactData" );
_LIT( KContactDataSize, "ContactDataSize" );
_LIT( KContactVersion, "ContactVersion" );

_LIT( KGroupId, "GroupId" );
_LIT( KGroupData, "GroupData" );
_LIT( KGroupDataSize, "GroupDataSize" );

// "test character identity and accents, ignore case"
const TInt KCollationLevel = 1;

#endif // __CCASTORAGEDEFS_H__
