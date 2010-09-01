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
* Description:  Presence Attribute Manager.
*
*/

#ifndef __MPENGPRESENCEATTRMANAGER_H
#define __MPENGPRESENCEATTRMANAGER_H


//INCLUDES
#include <E32std.h>
#include "BaMDescA.h"
#include "BaDescA.h"


//DATA TYPES

/**
* Attribute instance options.
*
* @since 3.0
*/
enum TPEngAttributeInstanceOptions
    {
    ///
    EPEngInstanceOptionsNone  = 0x0,

    ///
    EPEngUserAttribute        = 0x00000001,

    ///
    EPEngStorableModel        = 0x00000002,

    ///
    EPEngEditLockedModel      = 0x00000004,

    ///
    EPEngCreationNotAllowed   = 0x00000008,
    };




//FORWARD DECLARATIONS
class MPEngPresenceAdvancedAttrModel2;
class MPEngPresenceAttrModel2;




// CLASS DECLARATION
/**
 * Interface for Presence Attribute Manager.
 *
 * NOTE!!! Attribute Manager is reference counted singleton, so client
 * which has retrieved a instance to it, must also free it
 * by calling MPEngPresenceAttrManager::Close(), when the manager isn't
 * needed anymore.
 *
 * @since 3.0
 */
class MPEngPresenceAttrManager
    {
    public: //Reference cleanup

        /**
         * Attribute Manager is reference counted singleton.
         * and clients must free all obtained instances by
         * calling this to each of them when the Attribute Manager
         * isn't anymore needed.
         *
         * @since 3.0
         */
        virtual void Close() = 0;



    public: //Attribute model



        /**
         * Instantiates attribute model.
         *
         * @since 3.0
         * @param aModel On the return the instantiated attribute model.
         * Returned model is pushed on the CleanupStack. Returns ownership.
         * @param aPresenceID The attribute owners presence ID.
         * @param aType Presence attribute type.
         * @param aInstanceGenre The instance genre for new attribute instance.
         */
        virtual void InstantiateAttributeLC( MPEngPresenceAttrModel2*& aModel,
                                             const TDesC& aPresenceID,
                                             TUint32 aType,
                                             TInt aInstanceOptions ) = 0;


        /**
         * Instantiates and loads attribute model.
         *
         * @since 3.0
         * @param aModel On the return the loaded attribute model.
         * Returned model is pushed on the CleanupStack. Returns ownership.
         * @param aPresenceID The attribute owners presence ID.
         * @param aType Presence attribute type.
         * @param aInstanceGenre The instance genre for new attribute instance.
         * @return Was the model found from storage?
         * KErrNotFound if attribute wasn't found from storage.
         * Else KErrNone.
         */
        virtual TInt LoadAttributeLC( MPEngPresenceAttrModel2*& aModel,
                                      const TDesC& aPresenceID,
                                      TUint32 aType,
                                      TInt aInstanceOptions ) = 0;


        /**
         * Stores attribute model to storage.
         *
         * @since 3.0
         * @param aModel The model to store.
         */
        virtual void StoreEditLockedAttributeL( const MPEngPresenceAttrModel2& aModel ) = 0;


        /**
         * Releases the edit support from the attribute model.
         *
         * @since 3.0
         * @param aModel The model.
         * @return Error code.
         */
        virtual TInt StopEditSupport( MPEngPresenceAttrModel2& aModel ) const = 0;


        /**
         * Checks is the attribute model edit locked from
         * the same slot as the Attribute Manager is connected.
         *
         * @since 3.0
         * @param aModel The model.
         */
        virtual void ModelEditLockedFromThisSlotL(
            const MPEngPresenceAttrModel2& aModel ) const = 0;




    public: //Attribute type information

        /**
         * Tests is the passed attribute type id a valid network attribute
         * or not. Returns:
         * - KErrNone if the attribute type is a valid network attribute.
         * - KErrUnknown if the attribute id is unknown.
         * - KErrNotSupported if the attribute id is known,
         *   but the attribute isn't a valid network attribute
         *   (Attribute is a local attribute.)
         *
         * Attribute type is a valid network attribute if it's originator
         * is one of following:
         *  - EPEngClientOriginated
         *  - EPEngServerOriginated
         *  - EPEngClientServerOriginated
         *
         * @since 3.0
         * @param aType the type of the attribute.
         * @return KErrNone, KErrUnknown or KErrNotSupported.
         */
        virtual TInt ValidNetworkAttribute( TUint32 aType ) const = 0;


        /**
         * Returns the generic array holding all known attribute types.
         *
         * @since 3.0
         * @return The generic array holding all known attribute types.
         */
        virtual TArray<TUint32> KnownAttributeTypes() const = 0;


        /**
         * Gets the XML name and XML namespace for certain attribute type.
         * If the attribute type isn't known, returns KErrUnknown and empty
         * string as name and namespace.
         *
         * @since 3.0
         * @param aAttributeType The attribute type for which to get name and namespace.
         * @param aAttributeName On the return contains the asked attribute's XML name.
         * @param aAttributeName On the return contains the asked attribute's XML namespace.
         * @return KErrNone if the attribute type is known, else KErrUnknown.
         */
        virtual TInt GetAttributeXmlNameAndNameSpace( TUint32 aType,
                                                      TPtrC8& aAttributeName,
                                                      TPtrC8& aAttributeNameSpace ) const = 0;




    public: // Attribute request packing


        /**
         * Packs a attribute model array to buffer.
         *
         * @since 3.0
         * @param aModels Models to pack.
         * @return Heap buffer containing models data.
         * Buffer ownership is returned to caller.
         */
        virtual HBufC16* PackModelArrayL(
            const RPointerArray<MPEngPresenceAttrModel2>& aModels ) const = 0;


        /**
         * Extracts models from package.
         *
         * @since 3.0
         * @param aModelPkg Model package to extract.
         * @param aInstanceOptions Instance options
         * for instantiated attribute models.
         * @param aModels On the return contains extracted attribute models.
         * Models are appended to array.
         */
        virtual void UnpackModelArrayL(
            const TDesC16& aModelPkg,
            TInt aInstanceOptions,
            RPointerArray<MPEngPresenceAttrModel2>& aModels ) = 0;



        /**
         * Packs a attribute fetch request.
         *
         * @since 3.0
         * @param aPresenceIDs Presence ID's to pack.
         * @param aTypes Attribute types to pack.
         * @return Heap buffer containing fetch request data.
         * Buffer ownership is returned to caller.
         */
        virtual HBufC16* PackFetchRequestL( const MDesCArray& aPresenceIDs,
                                            const TArray<TUint32>& aTypes ) const = 0;


        /**
         * Extracts a attribute fetch request from package.
         *
         * @since 3.0
         * @param aFetchRequestPkg Attribute fetch request package to extract.
         * @param aPresenceIDs On the return contains extracted Presence IDs.
         * Presence IDs are appended to the end of array.
         * @param aTypes On the return contains extracted attribute type IDs.
         * Attribute types are appended to the end of array.
         */
        virtual void UnpackFetchRequestL( const TDesC16& aFetchRequestPkg,
                                          CDesCArray& aPresenceIDs,
                                          RArray<TUint32>& aTypes ) const = 0;


    public: // Attribute store ID handling

        /**
         * Generates a attribute store id from given
         * attribute type and presence id. Returns store id
         * in HBufC. HBufC's ownership is returned to caller.
         *
         * @since 3.0
         * @param aAttributeType The type of the attribute
         * @param aPresenceID The presenceID of the attribute
         * @return Attribute StoreId.
         */
        virtual HBufC* GenerateStoreIdL( TUint32 aAttributeType,
                                         const TDesC& aPresenceId ) const = 0;


        /**
         * Resolves attribute type and owner presence id
         * from store id.
         *
         * @since 3.0
         * @param aStoreId The store id to resolve.
         * @param aAttributeType On succesful return contains resolved attribute type.
         * @param aPresenceId On successful return contains resolved presence id.
         * Returned aPresenceId is valid only for the lifetime of given source store id.
         * @return KErrNone if succesful. KErrCorrupt if given aStoreId
         * isn't valid attribute store id.
         */
        virtual TInt ResolveStoreId( const TDesC& aStoreId,
                                     TUint32& aAttributeType,
                                     TPtrC& aPresenceId ) const = 0;



    public: // Batch operations

        /**
         * Stores an array of attribute models in a batch to storage.
         * Force stores the models, so edit locks are ignored.
         *
         * @since 3.0
         * @param aModel The models to store.
         */
        virtual void ForceStoreBatchL(
            const RPointerArray<MPEngPresenceAttrModel2> aModels ) = 0;


        /**
         * Removes an array of attribute models in a batch from storage.
         * Force removes the models, so edit locks are ignored.
         *
         * @since 3.0
         * @param aModel The models to remove from the storage.
         */
        virtual void ForceRemoveFromStoreBatchL(
            RPointerArray<MPEngPresenceAttrModel2> aModels ) = 0;


    protected:

        /**
         * Protected  destructor.
         *
         * Concrete Attribute Manager can't be
         * deleted through this interface.
         */
        virtual ~MPEngPresenceAttrManager() {};

    };

#endif      //  __MPENGPRESENCEATTRMANAGER_H

//  End of File
