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
* Description: Issues WV requests belonging to the Fundamental API
*
*/


// -----------------------------------------------------------------------------
// RImpsFundClient2::SearchHandler
// -----------------------------------------------------------------------------
inline MImpsSearchHandler2* RImpsFundClient2::SearchHandler() const
    {
    return iSearchCallBack;
    }

// -----------------------------------------------------------------------------
// RImpsFundClient2::InviteHandler
// -----------------------------------------------------------------------------
inline MImpsInviteHandler2* RImpsFundClient2::InviteHandler() const
    {
    return iInviteCallBack;
    }
