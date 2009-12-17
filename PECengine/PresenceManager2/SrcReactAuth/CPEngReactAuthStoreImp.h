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
* Description:  Reactive authorization store API implementation owner.
*
*/

#ifndef CPENGREACTAUTHSTOREIMP_H
#define CPENGREACTAUTHSTOREIMP_H

//  INCLUDES
#include <E32Base.h>



//FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngNWSessionSlotStorageProxy;
class MPEngAuthorizationManager;
class MPEngAuthorizationRespond;


/**
 * Reactive authorization store API implementation owner.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngReactAuthStoreImp ): public CBase

    {
public: /* Construction */


    /**
     * Instantiates CPEngReactAuthStoreImp object.
     *
     * @return New CPEngReactAuthStoreImp instance.
     */
    static CPEngReactAuthStoreImp* NewL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID,
        TInt aPriority );


    /**
     * Destructor.
     */
    ~CPEngReactAuthStoreImp();



protected:

    /**
     * C++ constructor.
     */
    CPEngReactAuthStoreImp( TInt aPriority );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );


public: //New functions

    void GetAndLockAuthorizationRespondL(
        MPEngAuthorizationRespond*& aRespond );


private: //Data

    /**
     * Interface is friend.
     * @since 3.0
     */
    friend class CPEngReactAuthStore;       // CSI: 36 #

    //OWN: The priority
    TInt    iCActivePriority;

    //OWN:
    CPEngNWSessionSlotStorageProxy*  iUsedSlot;

    //OWN:
    MPEngAuthorizationManager*  iAuthManager;

    };

#endif  //CPENGREACTAUTHSTOREIMP_H

// End of File


