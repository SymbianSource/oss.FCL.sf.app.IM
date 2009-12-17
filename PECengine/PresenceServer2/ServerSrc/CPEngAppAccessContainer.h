/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSP session access container
*  				 This class maintain reference counted access to the CSP session
*
*/



#ifndef __CPENGAPPACCESSCONTAINER_H__
#define __CPENGAPPACCESSCONTAINER_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


/**
 *  CSP session access container
 *  This class maintain reference counted access to the CSP session
 *
 *  @lib PEngServer2
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngAppAccessContainer ) :
        public CObject
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngAppAccessContainer* NewLC( const TDesC& aAppId );

    /**
     * Destructor.
     */
    virtual ~CPEngAppAccessContainer();

public: // Function from the base class

    /**
     *  Application Id
     *
     *  @since 3.0
     *  @return application id
     */
    const TDesC& AppId() const;

    /**
     *  Access count
     *  Return access count of the container
     *
     *  @since 3.0
     *  @return access count
     */
    TInt ActiveAccessCount();

    /**
     *	Close access
     *	Returns ETrue is this object is being destroyed
     *
     *  @since 3.0
     *  @return ETrue if object is being destroyed
     */
    TBool CloseAccess();

    /**
     *	Reserve Process Id
     *
     *  @since 3.0
     *  @param aProcessId process Id
     */
    void ReserveProcessL( const TDesC& aProcessId );

    /**
     *	Activate Process Id
     *
     *	Process Id is removed from list of reserved Ids and
     *	reference count is increased by one, to signal process
     *	is again active
     *	Function leaves with KErrNotFound if Process Id is not found
     *
     *  @since 3.0
     *  @param aProcessId process Id
     */
    void ActivateProcessIdL( const TDesC& aProcessId );

    /**
     *	Closes container and let framework deletes it
     *
     *  Sets access count to zero and resets array of registeres
     *	process ids, so container is deleted by the CObject framework
     *
     *  @since 3.0
     */
    void CloseContainer();

private:

    /**
     * C++ default constructor.
     */
    CPEngAppAccessContainer( );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC& aAppId );


private:    // Data

    /// OWN: Reserved Process Names
    CDesC16ArrayFlat	iReserved;

    /// OWN: Application Id
    HBufC* 				iAppId;
    };

#endif      // __CPENGAPPACCESSCONTAINER_H__

// End of File
