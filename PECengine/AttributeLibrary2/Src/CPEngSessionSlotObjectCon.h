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
* Description:  NWSessionSlot object container.
*
*/

#ifndef CPENGSESSIONSLOTOBJECTCON_H
#define CPENGSESSIONSLOTOBJECTCON_H

//  INCLUDES
#include <E32Base.h>



//FORWARD DECLARATION
class CPEngSessionSlotObject;
class CPEngSessionSlotId;



// CLASS DECLARATION
/**
 * Object container for session slot,
 * reference counted objects.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngSessionSlotObjectCon ) : public CBase
    {
public: /* Construction */


    /**
     * Instantiates CPEngSessionSlotObjectCon object.
     *
     * @return New CPEngSessionSlotObjectCon instance.
     */
    static CPEngSessionSlotObjectCon* NewL();


    /**
     * Destructor.
     */
    ~CPEngSessionSlotObjectCon();


private:

    /**
     * C++ constructor.
     */
    CPEngSessionSlotObjectCon();



public: // Object management

    /**
     * Locates session slot object by session slot id.
     *
     * @param aSlotId The slot id for which to locate object.
     * @return Located session slot object or NULL.
     */
    CPEngSessionSlotObject* FindById( const CPEngSessionSlotId& aSlotId );
    const CPEngSessionSlotObject* FindById( const CPEngSessionSlotId& aSlotId ) const;


    /**
     * Adds a new session slot object to container.
     *
     * @param aObject The object to add. Object may not be NULL,
     * and it may not be previously added to any other container.
     */
    void AddL( CPEngSessionSlotObject* aObject );


    /**
     * Removes a session slot object from container.
     *
     * @param aObject The object to remove.
     * Object may not be NULL.
     */
    void Remove( CPEngSessionSlotObject* aObject );


    /**
     * Gets the count of hold session slot objects.
     *
     * @return The count of hold session slot objects.
     */
    TInt Count() const;


    /**
     * Gets a reference to session slot object.
     *
     * @param aIndex The index of object to get.
     * @return The session slot object.
     */
    CPEngSessionSlotObject& At( TInt aIndex );


private: // Object management

    CPEngSessionSlotObject* DoFindById( const CPEngSessionSlotId& aSlotId );



private:    // Data

    //OWN: The session slot objects
    RPointerArray< CPEngSessionSlotObject > iObjects;

    };



#endif      // CPENGSESSIONSLOTOBJECTCON_H

// End of File
