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
* Description:  System Agent keys change observer
*
*/



#ifndef CIMPSSYSTEMNOTFIERSYSTEMAGENT_H
#define CIMPSSYSTEMNOTFIERSYSTEMAGENT_H

//  INCLUDES
#include <e32base.h>        // CActive
#include <saclient.h>       // RSystemAgent

#include "MIMPSSystemNotifier.h"

// FORWARD DECLARATIONS
class MIMPSSystemNotifierObserver;

// CLASS DECLARATION

/**
*  Implementation of the System Agent change observer
*
*  @since 2.6
*/
NONSHARABLE_CLASS( CIMPSSystemNotifierSystemAgent ) : public CActive,
        public MIMPSSystemNotifier
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aObserver Observer to be informed when observed system key changes
    */
    static CIMPSSystemNotifierSystemAgent* NewL( MIMPSSystemNotifierObserver& aObserver,
    const TUid aKey );

    /**
    * Destructor.
    */
    virtual ~CIMPSSystemNotifierSystemAgent();

protected:     // from MIMPSSystemNotifier

    /**
    * @see MIMPSSystemNotifier
    */
    TInt Subscribe();

    /**
    * @see MIMPSSystemNotifier
    */
    void UnSubscribe();

    /**
    * @see MIMPSSystemNotifier
    */
    TInt GetIntKey( TInt& aValue );

protected:  // Functions from base classes

    /**
    * From CActive
    */
    void RunL();

    /**
    * From CActive
    */
    void DoCancel();

private:

    /**
    * C++ default constructor.
    */
    CIMPSSystemNotifierSystemAgent( MIMPSSystemNotifierObserver& aObserver,
                                    const TUid aKey );

    /**
    * Symbian 2nd phase constructor..
    */
    void ConstructL();

private:    // Data

    // Owns: System Agent
    RSystemAgent iSysAgent;

    // Call event stored here
    TSysAgentEvent iAgentEvent;

    // Observer to be informed when value of system key changes
    MIMPSSystemNotifierObserver& iObserver;

    // the key this observer is attached
    TUid iKey;

    };

#endif      // CIMPSSYSTEMNOTFIERSYSTEMAGENT_H

// End of File
