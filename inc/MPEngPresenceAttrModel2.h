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
* Description:  Presence attribute model interface & related definitions.
*
*/

#ifndef __MPENGPRESENCEATTRMODEL2_H
#define __MPENGPRESENCEATTRMODEL2_H

//  INCLUDES
#include <E32Std.h>
#include <PEngPresenceEngineConsts2.h>



//FORWARD DECLARATIONS
class MPEngAttrModelExtension2;
class MPEngPresenceAdvancedAttrModel2;
class MPEngPresenceAttrTypeProperties2;



// DATA DECLARATION
/**
 * Attribute model data source.
 *
 * Attribute model data source identifies source
 * of the currently handled attribute model data.
 *
 * @since 3.0
 */
enum TPEngAttrDataSource
    {
    /**
     * Presence attribute data is committed to network.
     */
    EPEngAttrNetwork = 1,


    /**
     * Presence attribute data is locally edited.
     */
    EPEngAttrLocalEdited,

    /**
     * Data is local default.
     * Attribute has it attribute type specific default values.
     */
    EPEngAttrLocalDefault
    };




/**
 * Default group value for simple
 * structured presence attributes.
 *
 * @since 3.0
 */
const TInt KPEngDefaultAttrValueGroup = -1;




// CLASS DECLARATION

/**
 * Abstract interface for Presence Attribute models.
 *
 * Presence Attribute present Presence Attribute data.
 * Attribute model interface defines uniform way to
 * access and edit presence attributes.
 *
 * Each concrete attribute type supports just certain,
 * most suitable, data set and get methods.
 *
 * @since 3.0
 */
class MPEngPresenceAttrModel2
    {
    public: //Attribute type details

        /**
         * Attribute type ID.
         *
         * Returns ID identifying type of handled attribute model.
         * See attribute type documentation for information
         * about available attributes.
         *
         * Attribute type identifies the presence data hold in
         * the handled attribute model. Each concrete attribute
         * type has its own data semantics that has to be known
         * before using the attribute model.
         *
         * Presence Engine known attribute types are listed
         * in PEngWVPresenceAttributes2.h. Currently supported
         * types are:
         *  - KUidPrAttrOnlineStatus
         *  - KUidPrAttrClientInfo
         *  - KUidPrAttrCommCap
         *  - KUidPrAttrUserAvailability
         *  - KUidPrAttrStatusText
         *  - KUidPrAttrStatusContent (former KUidPrAttrMyLogoStatusContent)
         *  - KUidPrAttrMyLogoThumbnail
         *  - KUidPrAttrInfoLink
         *  - KUidPrAttrAlias
         *
         * @since 3.0
         * @return Attribute model type ID.
         */
        virtual TUint32 Type() const = 0;


        /**
         * Attribute type properties.
         *
         * Returns abstract interface which can be used to
         * query different attribute type specific
         * properties like attribute type XML name etc.
         *
         * Lifetime of the returned property interface
         * is guaranteed to be valid only as long as
         * the attribute model. Property object
         * ownership is kept on attribute model.
         *
         * @since 3.0
         * @return The attribute type properties.
         */
        virtual const MPEngPresenceAttrTypeProperties2& TypeProperties() const = 0;



        /**
         * Makes a new deep copy from attribute model object.
         *
         * Returns new object ownership to caller.
         * Returned object can be only read.
         * @since 3.0
         */
        virtual MPEngPresenceAttrModel2* CloneLC() const = 0;




    public: //Attribute instance details

        /**
         * Gets the attribute owners presence ID.
         *
         * @since 3.0
         * @return Attribute owner presence ID. For user own
         * attributes, method returns empty string.
         * (KPEngUserOwnPresenceId from PEngPresenceEngineConsts2.h)
         */
        virtual const TDesC& PresenceID() const = 0;


        /**
         * Attribute current data source.
         *
         * @since 3.0
         * @return Enumeration specifying attribute data source.
         */
        virtual TPEngAttrDataSource DataSource() const = 0;




    public: //Attribute data getters


        /**
         * Attribute qualifier.
         *
         * @since 3.0
         * @return A TBool indicating attribute qualification.
         * ETrue if the attribute qualifier is TRUE.
         * EFalse if the attribute qualifier is FALSE.
         */
        virtual TBool Qualifier() const = 0;



        /**
         * Gets integer or 8/16 bit descriptor value.
         *  - Integer values are usually attribute type
         *    specific enumerations.
         *  - 8 bit descriptor values are attribute type specific
         *    data buffers or similar objects.
         *  - 16 bit descriptor values are usually a screen texts.
         *
         *
         * Common error handling:
         * 1. Attribute type doesn't support given scope (field/group):
         *    - debug build: panic,
         *    - release build: return KErrNotSupported / empty descriptor
         *
         * 2. Attribute type doesn't support getting specified type:
         *    - debug build: panic
         *    - release build: return KErrNotSupported / empty descriptor
         *
         * @since 3.0
         * @param aField The field of the attribute value.
         * @param aGroup The group of individual attribute field.
         * @return The integer or 8/16 bit descriptor data.
         */
        virtual TInt DataInt(
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) const = 0;

        virtual const TPtrC8 DataDesC8(
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) const = 0;

        virtual const TPtrC16 DataDesC16(
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) const = 0;






    public: //Attribute data mutators


        /**
         * Sets attribute qualifier.
         *
         * Basicly most of time the attribute qualifier should
         * remain set to true.
         *
         * NOTE!! Successful setting the attribute data from
         * client side causes automaticly the qualifier to be set
         * to true.
         *
         * @since 3.0
         * @param aQualifier A TBool indicating attribute qualification.
         * If ETrue, then the attribute qualifier is set to TRUE.
         * If EFalse, then the attribute qualifier is set to FALSE.
         */
        virtual void SetQualifier( TBool aQualifier ) = 0;



        /**
         * Sets integer or 8/16 bit descriptor value.
         *  - Integer values are usually attribute type
         *    specific enumerations.
         *  - 8 bit descriptor values are attribute type specific
         *    data buffers or similar objects.
         *  - 16 bit descriptor values are usually a screen texts.
         *
         * If value set operation succeeds, sets also the
         * attribute qualifier to TRUE.
         *
         * Common error handling:
         * 1. Attribute type doesn't support given scope (field/group):
         *    - debug build: panic
         *    - release build: leave KErrNotSupported
         *
         * 2. Attribute type doesn't support given integer value,
         *    or descriptor value is out of attribute type specific bounds:
         *    - debug build: panic
         *    - release build: leave KErrArgument
         *
         * 3. Attribute type doesn't support setting specified type:
         *    - debug build: panic
         *    - release build: leave KErrNotSupported
         *
         * 4. Concrete attribute might do also other checks and
         *    conversions for the data. If these fail, data set
         *    operation fails and leaves with error code coming from
         *    concrete attribute implementation.
         *
         * @since 3.0
         * @param aData The new integer or 8/16 bit descriptor data.
         * @param aField The field of the attribute value.
         * @param aGroup The group of individual attribute field.
         */
        virtual void SetDataIntL(
            TInt aData,
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) = 0;

        virtual void SetDataDesC8L(
            const TDesC8& aData,
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) = 0;

        virtual void SetDataDesC16L(
            const TDesC16& aData,
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) = 0;


        /**
         * Sets 8/16 bit descriptor value with asynchronous processing.
         *
         * Sets 8/16 bit descriptor value and performs asynchronous
         * processing for validating or converting the value.
         *
         * If value set operation succeeds, sets also the
         * attribute qualifier to TRUE.
         *
         * If attribute has several asynchronously set data
         * fields, only one set operation is suppoted at time.
         *
         * Common error handling:
         * 1. Attribute type doesn't support given scope (field/group):
         *    - debug build: panic
         *    - release build: leave KErrNotSupported
         *
         * 2. Attribute type doesn't support setting specified data type
         *    asynchronously:
         *    - debug build: panic
         *    - release build: leave KErrNotSupported
         *
         * 3. Asynchronous processing starting fails:
         *  - leaves with error codes coming from concrete implementation.
         *
         *
         * @since 3.0
         * @param aData The 8/16 bit descriptor data.
         * @param aRequestStatus Asynchronous completion code.
         * The attribute model sets the passed request status to
         * KRequestPending state if asynchronous processing is started.
         * When asynchronous processing completes, successfully or otherwise,
         * the end status is returned in aRequestStatus.
         * - KErrNone if processing was successful. In this case
         *   attribute qualifier is set to ETrue.
         * - Else an attribute type specific error code or
         *   one of the system-wide error codes.
         *
         * @param aField The field of the attribute value.
         * @param aGroup The group of individual attribute field.
         */
        virtual void SetDataDesC8AsyncL(
            const TDesC8& aData,
            TRequestStatus& aRequestStatus,
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) = 0;

        virtual void SetDataDesC16AsyncL(
            const TDesC16& aData,
            TRequestStatus& aRequestStatus,
            TInt aField,
            TInt aGroup = KPEngDefaultAttrValueGroup ) = 0;


        /**
         * Cancels previously issued asynchronous set processing.
         *
         * Attribute value state after cancelling depends from
         * concrete attribute type.
         *
         * @since 3.0
         */
        virtual void CancelSetDataAsync() = 0;





    private: //Extensions interfaces


        /**
         * Generic attribute model extension interface.
         *
         * @since 3.0
         * @return Attribute model extension.
         */
        virtual const MPEngAttrModelExtension2* Extension() const = 0;
        virtual MPEngAttrModelExtension2* Extension() = 0;



        /**
         * Attribute model type specific extension.
         *
         * @since 3.0
         * @return Attribute type specific extension.
         */
        virtual TAny* TypeExtension( TUint32 aType ) = 0;



    public: //Presence Engine internal interface access

        /**
         * Presence Engine internal attribute model interface.
         *
         * @since 3.0
         * @return Attribute model internal interface.
         */
        virtual const MPEngPresenceAdvancedAttrModel2* Advanced() const = 0;
        virtual MPEngPresenceAdvancedAttrModel2* Advanced() = 0;



    public: //Cleanup support & destructor

        /**
         * Support for attribute model cleanup.
         *
         * When this method is called, concrete
         * attribute object is deleted and all
         * it runtime resources are freed.
         * ==> Attribute models can be pushed on the
         * CleanupStack using CleanupClosePushL().
         *
         * @since 3.0
         */
        virtual void Close() = 0;


        /**
         * Inline virtual destructor. Concrete
         * attribute objects can be deleted through
         * this interface.
         *
         * Deleting cancels possibly active
         * asynchronous processing operations.
         *
         * @since 3.0
         */
        virtual ~MPEngPresenceAttrModel2() {};

    };


#endif      // __MPENGPRESENCEATTRMODEL2_H



//  End of File


