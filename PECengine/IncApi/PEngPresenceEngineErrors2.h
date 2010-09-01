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
* Description:  Presence Engine specific error codes.
*
*/

#ifndef __PENGPRESENCEENGINEERRORS2_H
#define __PENGPRESENCEENGINEERRORS2_H



/**
 * Presence Engine specific error codes.
 *
 * Error codes returned from Presence Engine are either:
 * 1) System wide Symbian OS error codes (KErrNoMemory etc.)
 * 2) Presence Engine specific error codes defined here.
 *    These error codes have range: KPEngErrorBase ... KPEngErrorLow.
 * 3) Wireless Village CSP protocol error codes.
 *    Those are defined in PEngWVPresenceErrors.h
 *
 * @since 3.0
 */
enum	// CSI: 28 #
    {
    KPEngErrorBase = -30131,

    KPEngNWPresenceSessionTerminated = KPEngErrorBase,

    KPEngErrorLow = -30150
    };



#endif // __PENGPRESENCEENGINEERRORS2_H

//  End of File



