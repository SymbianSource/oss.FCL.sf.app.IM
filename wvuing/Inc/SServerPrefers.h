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
* Description:  Structure used for signalling various branding-related things
*                to other DLLs.
*
*/


#ifndef SSERVERPREFERS_H
#define SSERVERPREFERS_H

/**
 *  @since 2.1
 *  iUseGrant - ETrue if grant list is used. EFalse if not
 *  iAliasUsed - ETrue if alias support enabled
 *  iLocalEchoInGroup - ETrue if locally echoing conversed texts in groups
 *  iReactiveAuthorization - ETrue if reactive authorization support is enabled
 */
struct SServerPrefers
    {
    TBool iUseGrant;
    TBool iAliasUsed;
    TBool iLocalEchoInGroup;
    TBool iReactiveAuthorization;
    };

#endif      // SSERVERPREFERS_H

// End of File
