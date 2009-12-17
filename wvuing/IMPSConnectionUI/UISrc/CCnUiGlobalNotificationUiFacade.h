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
* Description:  Global notification UI implementation.
*
*/

#ifndef __CCNUIGLOBALNOTIFICATIONUIFACADE_H
#define __CCNUIGLOBALNOTIFICATIONUIFACADE_H


//  INCLUDES
#include <E32Base.h>
#include <barsc.h>
#include "MCnUiGlobalNotificationUiFacade.h"



/**
 * Global notification UI implementation.
 *
 * Implements the MCnUiGlobalNotificationUiFacade interface.
 *
 * NOTE!!! This class is used in AA plug-in environment
 * ===> no CONE facilities available
 *
 * @lib IMPSConnectionUi.lib
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiGlobalNotificationUiFacade ) : public CBase,
        public MCnUiGlobalNotificationUiFacade
    {
public: //Symbian OS two phased constructor / destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiGlobalNotificationUiFacade* NewL();

    /**
     * Destructor.
     */
    virtual ~CCnUiGlobalNotificationUiFacade();



protected:  // Constructors

    /**
     * C++ default constructor.
     */
    CCnUiGlobalNotificationUiFacade();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();

public: //from MCnUiGlobalNotificationUiFacade


    /**
     * @see MCnUiGlobalNotificationUiFacade
     * @since 2.1
     */
    void ShowLoginUserDataErrorL( const TDesC& aServerName );


    /**
     * @see MCnUiGlobalNotificationUiFacade
     * @since 2.1
     */
    void ShowLoginDetailedErrorL( const TDesC& aServerName,
                                  const TDesC& aErrDetDescription );

    /**
     * @see MCnUiGlobalNotificationUiFacade
     * @since 2.6
     */
    virtual void ShowLoginDetailedErrorL( const TDesC& aServerName,
                                          TInt aNoteTextResourceId );

    /**
     * @see MCnUiGlobalNotificationUiFacade
     * @since 2.1
     */
    TBool CGQActiveConnectionScheduledCloseL(
        const TArray<TIMPSConnectionClient>& aClientsToShow );


    /**
     * @see MCnUiGlobalNotificationUiFacade
     * @since 2.1
     */
    TBool CGQActiveConnectionSuppressForScheduledConnectionL(
        const TDesC& aServerName,
        const TArray<TIMPSConnectionClient>& aClientsToShow );


private:  // Private helpers

    /**
     * Reads a specified resource into a heap descriptor,
     * allocating memory for it and pushing the descriptor
     * onto the cleanup stack.
     * @since 2.1
     * @return
     */
    HBufC* ReadResourceLC( TInt aNoteTextResourceId );


    /**
     * Reads a specified resource into a heap descriptor,
     * allocating memory for it and pushing the descriptor
     * onto the cleanup stack.
     * @since 2.1
     * @return
     */
    HBufC* ReadResourceWithDynTextLC( TInt aNoteTextResourceId,
                                      const TDesC& aInsertText );



private:  // data

    ///<File server session, owned
    RFs                     iFs;

    ///<Connection UI resource file, owned
    RResourceFile           iResFile;

    };

#endif      //__CCNUIGLOBALNOTIFICATIONUIFACADE_H


//  End of File

