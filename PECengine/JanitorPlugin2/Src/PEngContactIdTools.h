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
* Description:  Contact Ids handling tools
*
*/

#ifndef __PENGCONTACTIDTOOLS_H__
#define __PENGCONTACTIDTOOLS_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

/**
  *	Find Contact Id in the aray
  *
  * @since 3.0
  *	@param aArray Contact ids array
  * @param aId Contact id to look for
  * @return KErrNotFound if it is not found otherwise index in array
  */
GLREF_C TInt FindContactIdInArray(
    const MDesCArray& aArray,
    const TDesC& aId );

/**
  *	Add contact Id to the array of the contact Ids
  *
  * @since 3.0
  *	@param aArray array of contact ids
  * @param aId Contact id to look for
  * @return index of the added element in array, or KErrAlreadyExists
  */
GLREF_C TInt AddContactIdToArrayL( CDesCArray& aArray,
                                   const TDesC& aId );

/**
  *	Compare two contact Ids
  *
  * @since 3.0
  * @param aId1 presence Id1
  * @param aId2 presence Id2
  * @param aUserDomain user domain
  * @return KErrNone if they are same, other non zero number if not
  */
GLREF_C TInt CompareContactIds( const TDesC& aId1,
                                const TDesC& aId2 );


#endif // __PENGCONTACTIDTOOLS_H__

//  End of File

