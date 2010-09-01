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
* Description:  This header contains panic constants that are used in
*		 various places in IMPS Always Online Plugin
*
*/


#ifndef IMPSAOPLUGINPANICS_H
#define IMPSAOPLUGINPANICS_H

// PANICS
_LIT( KIMPSAOPluginPanicCategory, "IMPS Always Online Plugin" );

enum TIMPSAlwaysOnlinePluginPanics
    {
    ETimeParsingFailed = 0,
    ETimeIntervalCalculationFailed
    };

#endif // IMPSAOPLUGINPANICS_H
