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
* Description: Const values for WV engine private CR key.
*
*/


#ifndef WVENGINEPRIVATECRKEYS_H
#define WVENGINEPRIVATECRKEYS_H

const TUid KCRUIDWVEngine = {0x101FB0D8}; 

// Local Variation flag indicating whether WAP Push over UDP CIR method is supported or not
const TUint32 KWVEngineFeatWapPushOverUDP = 0x00000001;
// Local Variation flag indicating whether IM Application Launcher is available or not
const TUint32 KWVEngineFeatOpenAPILauncher = 0x00000002;

#endif      // WVENGINEPRIVATECRKEYS_H

// End of File
