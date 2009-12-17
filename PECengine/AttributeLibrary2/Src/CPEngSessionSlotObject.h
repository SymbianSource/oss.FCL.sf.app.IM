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
* Description:  Base class for session slot, reference counted objects.
*
*/

#ifndef CPENGSESSIONSLOTOBJECT_H
#define CPENGSESSIONSLOTOBJECT_H

//  INCLUDES
#include <E32Base.h>



//FORWARD DECLARATION
class CPEngSessionSlotObjectCon;
class CPEngSessionSlotId;



// CLASS DECLARATION
/**
 * Base class for session slot, reference counted objects.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSessionSlotObject ) : public CBase
    {
public:  // Access count &  Destructor

    /**
     * Destructor.
     */
    virtual ~CPEngSessionSlotObject();


    /**
     * Opens the reference to object.
     *
     * @since 3.0
     */
    virtual void Open();


    /**
     * Closes the reference to object.
     *
     * @since 3.0
     */
    virtual void Close();



protected:  // Constructors

    /**
     * C++ default constructor.
     */
    CPEngSessionSlotObject();


    /**
     * Symbian OS constructor.
     * Derived class must call this to provide
     * the session slot id.
     */
    void ConstructL( const CPEngSessionSlotId& aSlotId );



public:  //New functions


    /**
     * Reference to session slot ID where
     * the object is connected.
     *
     * @since 3.0
     * @return The session slot ID.
     */
    const CPEngSessionSlotId& SlotId() const;


    /**
     * Sets the object owner.
     * Owner can be set only once.
     *
     * @since 3.0
     * @param aOwner The object owner.
     */
    void SetOwner( CObject& aOwner );


    /**
     * Gets the object's current
     * reference count.
     *
     * @since 3.0
     * @return The object current reference count.
     */
    TInt RefCount() const;



private:    // Friend declaration

    /**
     * Friend declaration.
     *
     * @since 3.0
     */
    friend class CPEngSessionSlotObjectCon;     // CSI: 36 #



private:    // Data

    //OWN: Reference access count
    TInt    iAccessCount;

    //OWN: Session slot ID.
    CPEngSessionSlotId*     iSlotId;

    //REF: The container.
    CPEngSessionSlotObjectCon*  iContainer;

    //REF: The owner.
    CObject*    iOwner;

    };


#endif      // CPENGSESSIONSLOTOBJECT_H

// End of File
