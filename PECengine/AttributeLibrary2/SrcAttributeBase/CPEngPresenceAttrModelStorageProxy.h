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
* Description:  Presence attribute store proxy.
*
*/

#ifndef CPENGPRESENCEATTRMODELSTORAGEPROXY_H
#define CPENGPRESENCEATTRMODELSTORAGEPROXY_H

//  INCLUDES
#include "MPEngPresenceAttrManager.h"
#include <E32Base.h>


//FORWARD DECLARATIONS
class CPEngPresenceAttrModel;
class CPEngModelStoreEntryAdapter;
class MPEngStorageManager;



// CLASS DECLARATION
/**
 * Presence Storage support
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceAttrModelStorageProxy ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngPresenceAttrModelStorageProxy* NewL( CPEngPresenceAttrModel& aAttributeModel );


    /**
     * Destructor.
     */
    virtual ~CPEngPresenceAttrModelStorageProxy();


private: // Constructors.

    /**
     * C++ default constructor.
     */
    CPEngPresenceAttrModelStorageProxy( CPEngPresenceAttrModel& aAttributeModel );



public: // Presence Storage.

    void ConnectStorageL( TBool aUserModel,
                          HBufC* aStoreId,
                          MPEngStorageManager& aStorageManager );


    TInt RetrieveFromStorageL();
    void StoreToStorageL( TBool aForceStore = EFalse ) const;
    void RemoveFromStorageL();
    TBool IsFromThisStorage( const MPEngStorageManager& aStorageManager ) const;


    void GainEditLockL( TBool aCreationAllowed );
    TInt StopEditSupport();
    TBool IsEditLocked() const;
    void ForceSendAttrEventIfNeededL() const;



private:    // Data

    //REF: The attribute model to store
    CPEngPresenceAttrModel&     iAttributeModel;

    //OWN: Store entry adapter. Lazy initalized
    CPEngModelStoreEntryAdapter*    iStoreEntry;

    //OWN: Does storage proxy instance own the edit lock?
    TBool   iEditLockOwned;

    //OWN: Is force notification needed?
    TBool   iForceNotifySidChange;

    };



#endif      // CPENGPRESENCEATTRMODELSTORAGEPROXY_H

// End of File
