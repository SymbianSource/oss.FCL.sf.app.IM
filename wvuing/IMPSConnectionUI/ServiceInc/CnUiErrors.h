/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection UI internal error codes.
*
*/

#ifndef __CNUIERRORS_H
#define __CNUIERRORS_H

// INCLUDES
#include <E32Std.h>


/**
 * Connection UI internal error codes.
 *
 * NOTE!! These errors are for Connection UI
 * internal use only, and they has to be mapped
 * genereal return values before returning the to
 * clients...
 */
const TInt KCnUiErrorBase = -19000;

///< Login operation is already in use by some other client
const TInt KCnUiErrorLoginOperationAlreadyInUse = KCnUiErrorBase - 1;


///< No proper default SAP or no SAP's at all
const TInt KCnUiErrorNoProperDefaultSap = KCnUiErrorBase - 2;


///< Selected login SAP is missing compulsory fields
const TInt KCnUiErrorSapMissingCompulsoryFields = KCnUiErrorBase - 3;


///< Network server has insufficient capabilities for client to login
const TInt KCnUiErrorInsufficientNWServerCapabilities = KCnUiErrorBase - 4;


///< NW usage not allowed (e.g. offline profile in use)
const TInt KCnUiErrorNetworkConnectionNotAllowed = KCnUiErrorBase - 5;


///< Already connected to another SAP
const TInt KCnUiErrorAlreadyConnectedToAnotherSap = KCnUiErrorBase - 6;


///< Client already connected
const TInt KCnUiErrorClientAlreadyConnected = KCnUiErrorBase - 7;


///< No clients to reconnect
const TInt KCnUiErrorNoClientsToReconnect = KCnUiErrorBase - 8;


///< No clients to disconnect
const TInt KCnUiErrorNoClientsToDisconnect = KCnUiErrorBase - 9;



#endif      //__CNUIERRORS_H

//  End of File

