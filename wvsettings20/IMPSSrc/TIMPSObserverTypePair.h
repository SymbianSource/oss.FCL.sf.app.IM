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
* Description:  SAP Observer type pair container.
*
*/


#ifndef TIMPSOBSERVERTYPEPAIR_H
#define TIMPSOBSERVERTYPEPAIR_H

//  INCLUDES
#include <e32base.h>
#include "impssapsettings.h"

// FORWARD DECLARATIONS

class MIMPSSAPObserver;

// CLASS DECLARATION
/**
 * Observer type pair container.
 *
 * @lib WVSAPSettingsStore.dll
 * @since 3.0
 **/
NONSHARABLE_CLASS( TIMPSObserverTypePair )
    {
public: // Constructors and destructor

    TIMPSObserverTypePair( MIMPSSAPObserver* aObs, TIMPSAccessGroup aGroup );

public: // New functions

    //Getters for memberdata
    TIMPSAccessGroup Group();
    MIMPSSAPObserver* Observer();

private: // Data

    MIMPSSAPObserver* 		iObs;	//SAP observer. Not owned.
    TIMPSAccessGroup 		iGroup;	//Access group. Not owned.
    };


#endif	// TIMPSOBSERVERTYPEPAIR_H
