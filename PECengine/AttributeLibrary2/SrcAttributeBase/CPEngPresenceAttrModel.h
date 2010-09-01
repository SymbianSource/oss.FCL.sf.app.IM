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
* Description:  Attribute model implementation.
*
*/

#ifndef CPENGPRESENCEATTRMODEL_H
#define CPENGPRESENCEATTRMODEL_H

//INCLUDES
#include "MPEngPresenceAdvancedAttrModel2.h"
#include "MPEngPresenceAttrModel2.h"
#include "MPEngPresenceAttrManager.h"
#include <E32Base.h>



// FORWARD DECLARATIONS
class MPEngPresenceAttrModelTypeImp;
class CPEngPresenceAttrConstructor;
class CPEngAttrModelAsyncDataSetHook;
class CPEngPresenceAttrModelStorageProxy;



/**
 * Attribute model implementation.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngPresenceAttrModel ) :  public CBase,
        public MPEngPresenceAdvancedAttrModel2
    {
public: // Construction

    /**
     * Instantiates CPEngPresenceAttrModel object.
     *
     * @param aTypeConstruct The attribute type specific construct.
     * @return New CPEngPresenceAttrModel instance.
     */
    static CPEngPresenceAttrModel* NewL(
        CPEngPresenceAttrConstructor& aTypeConstructor,
        const TDesC& aPresenceID,
        MPEngPresenceAttrModelTypeImp* aTypeImp );


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngPresenceAttrModel();



private:

    /**
     * C++ default constructor.
     */
    CPEngPresenceAttrModel( CPEngPresenceAttrConstructor& aTypeConstructor );


    /**
     * Symbian OS constructor.
     */
    void ConstructL( const TDesC& aPresenceID );



public: // Functions from MPEngPresenceAttrModel2


    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute type details.
     * @since 3.0
     */
    TUint32 Type() const;
    const MPEngPresenceAttrTypeProperties2& TypeProperties() const;


    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute instance details.
     * @since 3.0
     */
    const TDesC& PresenceID() const;
    TPEngAttrDataSource DataSource() const;


    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute data getters.
     * @since 3.0
     */
    TBool Qualifier() const;
    TInt DataInt( TInt aField,
                  TInt aGroup ) const;

    const TPtrC8 DataDesC8( TInt aField,
                            TInt aGroup ) const;

    const TPtrC16 DataDesC16( TInt aField,
                              TInt aGroup ) const;



    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute data setters.
     * @since 3.0
     */
    void SetQualifier( TBool aQualifier );
    void SetDataIntL( TInt aData,
                      TInt aField,
                      TInt aGroup );

    void SetDataDesC8L( const TDesC8& aData,
                        TInt aField,
                        TInt aGroup );

    void SetDataDesC16L( const TDesC16& aData,
                         TInt aField,
                         TInt aGroup );


    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute async data setters.
     * @since 3.0
     */
    void SetDataDesC8AsyncL( const TDesC8& aData,
                             TRequestStatus& aRequestStatus,
                             TInt aField,
                             TInt aGroup );

    void SetDataDesC16AsyncL( const TDesC16& aData,
                              TRequestStatus& aRequestStatus,
                              TInt aField,
                              TInt aGroup );


    /**
     * From MPEngPresenceAttrModel2.
     *
     * Attribute async data set cancel.
     * @since 3.0
     */
    void CancelSetDataAsync();



    /**
     * From MPEngPresenceAttrModel2.
     *
     * Extensions interfaces.
     * @since 3.0
     */
    const MPEngAttrModelExtension2* Extension() const;
    MPEngAttrModelExtension2* Extension();
    TAny* TypeExtension( TUint32 aType );



    /**
     * From MPEngPresenceAttrModel2.
     *
     * Presence Engine internal interface.
     * @since 3.0
     */
    const MPEngPresenceAdvancedAttrModel2* Advanced() const;
    MPEngPresenceAdvancedAttrModel2* Advanced();



    /**
     * From MPEngPresenceAttrModel2.
     *
     * Clone support .
     * @since 3.0
     */
    MPEngPresenceAttrModel2* CloneLC() const;



    /**
     * From MPEngPresenceAttrModel2.
     *
     * Cleanup support .
     * @since 3.0
     */
    void Close();



public: //From MPEngPresenceAdvancedAttrModel2


    /**
     * From MPEngPresenceAdvancedAttrModel2.
     *
     * @since 3.0
     */
    void EncodeToXmlL( MPEngXMLSerializer& aSerializer ) const;
    void DecodeFromXmlL( MPEngXMLParser& aParser,
                         const TDesC8& aDataBlock );

    const CPEngPresenceAttrModelStorageProxy& Storage() const;
    CPEngPresenceAttrModelStorageProxy& Storage();


    TInt ExternalizeSize() const;
    void ExternalizeL( RWriteStream& aStream ) const;
    void InternalizeL( RReadStream& aStream );


    void HandleAttributeUpdatedToNetwork();
    void HandleNetworkSessionStateChangeL( TBool aSessionOpen );


private: //Helpers


    /**
     * Lazy initializes the async set hook.
     * @since 3.0
     */
    void LazyInitAsyncSetHookL();



private:    // Base model maintained attribute data


    //REF: Model type constructor
    CPEngPresenceAttrConstructor& iTypeConstructor;

    //OWN: Async set hook. Lazy initialized when needed.
    CPEngAttrModelAsyncDataSetHook* iAsyncSetHook;

    //OWN: Model storage proxy
    CPEngPresenceAttrModelStorageProxy*   iStorage;

    //OWN: Attribute instances presence Id
    HBufC*  iPresenceId;

    //OWN: Attribute data source
    TPEngAttrDataSource     iDataSource;

    //OWN: Attribute qualifier value
    TBool   iQualifier;

    //OWN: Attribute model type instance
    MPEngPresenceAttrModelTypeImp*    iTypeImp;

    };

#endif      // CPENGPRESENCEATTRMODEL_H

//  End of File

