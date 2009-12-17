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
* Description:  Concrete presence attribute manager implementation.
*
*/

#ifndef CPENGPRESENCEATTRMANAGER_H
#define CPENGPRESENCEATTRMANAGER_H

//  INCLUDES
#include "CPEngSessionSlotObject.h"
#include "MPEngPresenceAttrManager.h"
#include <E32Base.h>


// FORWARD DECLARATIONS
class MPEngStorageManager;
class CPEngAttrConstructorCon;





// CLASS DECLARATION

/**
 * Presence attribute manager implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceAttrManager ) : public CPEngSessionSlotObject,
        public MPEngPresenceAttrManager

    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngPresenceAttrManager* NewLC( const CPEngSessionSlotId& aSlotId );

    /**
     * Destructor.
     */
    ~CPEngPresenceAttrManager();


private:

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL( const CPEngSessionSlotId& aSlotId );

    /**
     * C++ default constructor.
     */
    CPEngPresenceAttrManager();



public: // Functions from MPEngPresenceAttrManager


    /**
     * From MPEngPresenceAttrManager.
     *
     * @since 3.0
     */
    void Close();

    void InstantiateAttributeLC( MPEngPresenceAttrModel2*& aModel,
                                 const TDesC& aPresenceID,
                                 TUint32 aType,
                                 TInt aInstanceOptions );


    TInt LoadAttributeLC( MPEngPresenceAttrModel2*& aModel,
                          const TDesC& aPresenceID,
                          TUint32 aType,
                          TInt aInstanceOptions );

    void StoreEditLockedAttributeL( const MPEngPresenceAttrModel2& aModel );
    TInt StopEditSupport( MPEngPresenceAttrModel2& aModel ) const;
    void ModelEditLockedFromThisSlotL( const MPEngPresenceAttrModel2& aModel ) const;

    TInt ValidNetworkAttribute( TUint32 aType ) const;
    TArray<TUint32> KnownAttributeTypes() const;

    TInt GetAttributeXmlNameAndNameSpace( TUint32 aType,
                                          TPtrC8& aAttributeName,
                                          TPtrC8& aAttributeNameSpace ) const;



    HBufC16* PackModelArrayL( const RPointerArray<MPEngPresenceAttrModel2>& aModels ) const;
    void UnpackModelArrayL( const TDesC16& aModelPkg,
                            TInt aInstanceOptions,
                            RPointerArray<MPEngPresenceAttrModel2>& aModels );


    HBufC16* PackFetchRequestL( const MDesCArray& aPresenceIDs,
                                const TArray<TUint32>& aTypes ) const;
    void UnpackFetchRequestL( const TDesC16& aFetchRequestPkg,
                              CDesCArray& aPresenceIDs,
                              RArray<TUint32>& aTypes ) const;


    HBufC* GenerateStoreIdL( TUint32 aAttributeType,
                             const TDesC& aPresenceId ) const;
    TInt ResolveStoreId( const TDesC& aStoreId,
                         TUint32& aAttributeType,
                         TPtrC& aPresenceId ) const;

    void ForceStoreBatchL( const RPointerArray<MPEngPresenceAttrModel2> aModels );
    void ForceRemoveFromStoreBatchL( RPointerArray<MPEngPresenceAttrModel2> aModels );



private:    // Data

    //OWN: Storage manager
    MPEngStorageManager*    iStorageManager;

    //OWNn: Attribute type constructors
    CPEngAttrConstructorCon*    iTypeConstructors;


    };

#endif      // CPENGPRESENCEATTRMANAGER_H

//  End of File


