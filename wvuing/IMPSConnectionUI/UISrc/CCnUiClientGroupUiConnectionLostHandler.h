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
* Description:  Client group connection lost notify handler.
*
*/



#ifndef __CCNUICLIENTGROUPUICONNECTIONLOSTHANDLER_H
#define __CCNUICLIENTGROUPUICONNECTIONLOSTHANDLER_H

// INCLUDES
#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>



//FORWARD DECLARATION
class MCnUiUiFacade;



// CLASS DECLARATION
/**
 * Client group connection lost notify handler.
 *
 * Shows proper UI notifications for
 * active connection lost events.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientGroupUiConnectionLostHandler ) : public CBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor, constructs the
     * CCnUiClientGroupUiConnectionLostHandler object.
     *
     * @since 2.1
     * @param aClient The client.
     * @param aUi The UI to use.
     * @param aInitialEvent initial event
     * @return is pointer to CCnUiClientGroupUiConnectionLostHandler object.
     */
    static CCnUiClientGroupUiConnectionLostHandler* NewL(
        TIMPSConnectionClient aClient );

    /**
     * Destructor.
     */
    virtual ~CCnUiClientGroupUiConnectionLostHandler();


private: //Constructors

    /**
     * C++ default constructor.
     */
    CCnUiClientGroupUiConnectionLostHandler( TIMPSConnectionClient aClient );

public: //New methods

    /**
     * Notifies from presence event.
     *
     * @since 2.1
     * @param aEvent The presence event.
     */
    void HandlePresenceEventL( TIMPSPresenceServiceEvent aEvent );


    /**
     * Sets the new resource file to use in Connection UI.
     *
     * @param aNewResourceFile the new resourcefile to be used
     * @since 2.6
     */
    void UseResourceFileL( const TDesC& aNewResourceFile );

private: //data

    ///<Client for which behalf working. Owned.
    const TIMPSConnectionClient             iClient;

    ///<The UI to use. Owned.
    MCnUiUiFacade*                          iUi;

    ///<Resource file name, owned
    HBufC*									iBrandedResourceFileName;
    };

#endif      //__CCNUICLIENTGROUPUICONNECTIONLOSTHANDLER_H

//  End of File

