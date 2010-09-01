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
* Description:  Interface used to check if current recipient is blocked.
*
*/


#ifndef M_BLOCKCHECKER_H
#define M_BLOCKCHECKER_H

/**
 *  Interface used to check if current recipient
 *  is blocked.
 *
 *  @lib chat.exe
 *  @since S60 v3.1
 */
class MCABlockChecker
    {

    public:

        /**
         * Checks if conversation partner is blocked
         * and unblocks if partner is blocked. Shows needed
         * notes. Used in message sending.
         * @since S60 v3.1
         */
        virtual void CheckBlockedL() = 0;

    protected:

        /**
         * Deletion not allowed through this interface.
         */
        virtual ~MCABlockChecker() {};

    };


#endif // M_BLOCKCHECKER_H
