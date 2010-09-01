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
* Description:  Attribute library object holder.
*
*/

#ifndef CPENGATTRLIBOBJECTHOLDER_H
#define CPENGATTRLIBOBJECTHOLDER_H

//  INCLUDES
#include <E32Base.h>



// FORWARD DECLARATIONS
class MPEngPresenceAttrManager;
class MPEngPresenceAttrTransactionEnv;
class MPEngTransactionFactory;
class CPEngSessionSlotId;
class CPEngSessionSlotObjectCon;


// CLASS DECLARATION

/**
 * Attribute library reference counted singleton holder.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngAttrLibObjectHolder ) : public CObject
    {
public:  // Constructors and destructor

    /**
     * Singleton instance retrieve method.
     *
     * Gets pointer to attribute library
     * singleton object holder. Object holder
     * is reference counted, so returned instance
     * is pushed on the CleanupStack and freed
     * automaticly in case of leave or
     * CleanupStack::PopAndDestroy().
     *
     * @since 3.0
     */
    static CPEngAttrLibObjectHolder* InstanceLC();


    /**
     * Helper to verify that everything is
     * cleaned properly when executing
     * thread is closing.
     *
     * @since 3.0
     */
    static void HandleThreadDetach();


private:

    /**
     * Two-phased constructor.
     * Leaves created object on the CleanupStack.
     */
    static CPEngAttrLibObjectHolder* NewLC();


    /**
     * C++ default constructor.
     */
    CPEngAttrLibObjectHolder();


    /**
     * Private destructor.
     *
     * Singleton holder is reference counted.
     * It is automaticly destroyed when last
     * holded object or reference is destroyed.
     */
    ~CPEngAttrLibObjectHolder();



    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();



public: // Singleton access


    /**
     * Returns attribute manager instance for requested
     * NWSessionSlot.
     *
     * Method increases returned object reference
     * count by one before returning it ==> client must
     * call Close() to object when no more needed.
     *
     * @since 3.0
     * @return Attribute manager instance.
     */
    MPEngPresenceAttrManager* AttributeManagerInstanceL(
        const CPEngSessionSlotId& aSlotId );


    /**
     *
     * @since 3.0
     * @return Attribute transaction environment instance.
     */
    MPEngPresenceAttrTransactionEnv* AttributeTransEnvInstanceL(
        const CPEngSessionSlotId& aSlotId );


    /**
     *
     * @since 3.0
     * @return Attribute transaction factory instance.
     */
    MPEngTransactionFactory* AttributeTransFactInstanceL(
        const CPEngSessionSlotId& aSlotId );




public: // Debug helpers

#ifdef _DEBUG
    static CPEngAttrLibObjectHolder* __DbgInstanceOrNull();
    void __DbgObjectCounts( TInt& aHolder,
                            TInt& aAttrMan,
                            TInt& aTransEnv,
                            TInt& aTransFact ) const;
    void __DbgAttrManRefCount( const CPEngSessionSlotId& aSlotId, TInt& aRefCount ) const;
    void __DbgTransEnvRefCount( const CPEngSessionSlotId& aSlotId, TInt& aRefCount ) const;
    void __DbgTransFactRefCount( const CPEngSessionSlotId& aSlotId, TInt& aRefCount ) const;
#endif // _DEBUG



private:    // Data

    //OWN: Session Slot attribute managers
    CPEngSessionSlotObjectCon*      iAttributeManagers;


    //OWN: Session Slot attribute transaction environments
    CPEngSessionSlotObjectCon*      iAttributeTransEnvs;


    //OWN: Session Slot attribute transaction factories
    CPEngSessionSlotObjectCon*      iAttributeTransFacts;

    };

#endif      // CPENGATTRLIBOBJECTHOLDER_H

// End of File
