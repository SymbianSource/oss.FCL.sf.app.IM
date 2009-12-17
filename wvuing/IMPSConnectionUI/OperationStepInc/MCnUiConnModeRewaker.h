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
* Description:  Connection Mode rewaker
*
*/

#ifndef __MCNUICONNMODEREWAKER_H
#define __MCNUICONNMODEREWAKER_H

//  INCLUDES
#include <E32Std.h>
#include <impspresenceconnectionuiconstsng.h>



//FORWARD DECLARATION
class MCnUiClientPlugin;
class CPEngNWSessionSlotID2;


//DATA TYPES
/**
 * Connection mode rewaker.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnModeRewaker )
    {
public:     //New functions

    /**
     * Adds the new client to AA rewake list.
     * - Method implementation takes care of
     *   multiple add's of same client type.
     *
     * - Only clients which current login mode
     * is automatic, are added to AA rewake list.
     *
     * @since 2.1
     * @param aClientPlugin The client to add
     * AA mode rewake list.
     */
    virtual void AddToAAModeRewakeListL( MCnUiClientPlugin& aClientPlugin,
                                         CPEngNWSessionSlotID2& aNWSessionSlotID ) = 0;


    /**
     * Gets all client types currently holded in
     * AA rewake list.
     *
     * @since 2.1
     * @param aAARewakedClients On the return contains
     * client's from current AA rewake list.
     */
    virtual void GetAARewakeListL( RArray< TIMPSConnectionClient >& aAARewakedClients ) = 0;


    /**
     * Resets the AA rewake list.
     *
     * @since 2.1
     */
    virtual void ClearAARewakeList() = 0;


    /**
     * Removes the given client from rewake list.
     *
     * @since 2.1
     * @param aClient The client to remove from rewake list.
     */
    virtual void RemoveFromAARewakeList( TIMPSConnectionClient aClient ) = 0;



protected:  //Destructor

    /**
     * Inline destructor.
     *
     * Destruction using this
     * interface isn't possible.
     */
    virtual ~MCnUiConnModeRewaker() {};
    };


#endif      //  __MCNUICONNMODEREWAKER_H

//  End of File

