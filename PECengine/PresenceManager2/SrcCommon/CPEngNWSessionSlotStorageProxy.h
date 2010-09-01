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
* Description:  CPEngNWSessionSlotStorageProxy
*
*/

#ifndef CPENGNWSESSIONSLOTSTORAGEPROXY_H__
#define CPENGNWSESSIONSLOTSTORAGEPROXY_H__

//  INCLUDES
#include <E32Base.h>


//FORWARD DECLARATION
class CPEngNWSessionSlotID2;
class MPEngStorageManager;
class CPEngSessionSlotId;



// CLASS DECLARATION
/**
 *
 *
 * @since
 */
NONSHARABLE_CLASS( CPEngNWSessionSlotStorageProxy ) : public CBase
    {
public: // Construction


    /**
     * Instantiates CPEngNWSessionSlotStorageProxy object.
     */
    static CPEngNWSessionSlotStorageProxy* NewL(
        const CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CPEngNWSessionSlotStorageProxy();


private:  // constructors

    /**
     * C++ constructor.
     */
    CPEngNWSessionSlotStorageProxy();


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const CPEngNWSessionSlotID2& aNWSessionSlotID );




public: // Accessors


    /**
     *
     *
     * @since 3.0
     */
    MPEngStorageManager& StorageManager();


    /**
     *
     *
     * @since 3.0
     */
    const CPEngSessionSlotId& BaseId() const;


    /**
     *
     *
     * @since 3.0
     */
    const TDesC16& AppId() const;





private: // data


    //OWN: Storage Manager
    MPEngStorageManager*          iStorageManager;


    //OWN: Application ID
    HBufC*  iAppId;

    };

#endif      //  CPENGNWSESSIONSLOTSTORAGEPROXY_H__

//  End of File

