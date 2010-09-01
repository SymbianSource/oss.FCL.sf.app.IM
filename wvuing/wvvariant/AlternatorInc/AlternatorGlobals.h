/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header that declares dll entry point and method to create the
*				  object needed to use the dll.
*
*/

#ifndef ALTERNATOR_H
#define ALTERNATOR_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAlternator;

// FUNCTION PROTOTYPES

/*
* Method to create alternator singleton.
* @return, pointer to alternator singleton instance.
* @since 1.2s
*/
IMPORT_C CAlternator* CreateAlternatorL( const TDesC& aBrandRoot );

/**
 * Method to release alternator singleton
 * @since 2.6
 */
void ReleaseAlternatorL();


#endif      // ALTERNATOR_H

