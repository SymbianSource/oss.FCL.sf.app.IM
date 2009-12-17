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
* Description:  Storage Id change observer container
*
*/



#ifndef __CPENGOBSERVERCONTAINER_H__
#define __CPENGOBSERVERCONTAINER_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class MPEngSIDChangeObserver;
class MPEngSIdContainer;

// CLASS DECLARATION

/**
 *  Observer container
 *
 *  Container of one Sids observer.
 *  It holds reference to the
 *
 *  @lib PEngStorMan2.dll
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngObserverContainer ) :
        public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngObserverContainer* NewLC(
        MPEngSIDChangeObserver& aChangeObserver,
        TInt aPriority );

    /**
     * Destructor.
     */
    virtual ~CPEngObserverContainer();

public: // New functions

    /**
     *  Mark changed Storage Id
     *
     *  @since 3.0
     *  @param aChangedSId changed Storahe Id
     */
    void SIdHasChangedL( const TDesC& aChangedSId );

    /**
     *  Remove SId container
     *
     *  @since 3.0
     *  @param aSIdContainer Storage Id container to remove
     */
    void RemoveSIdContainer( MPEngSIdContainer* aSIdContainer );

    /**
     *  Notify changes of the storage IDs to observer
     *
     *  Leave error is trapped inside and informed to the observer
     *  in the notify error method
     *
     *  @since 3.0
     *	@param aPriority priority of the notification
     *		if priority does not match to the own one, notification
     *		is ignored
     */
    void NotifySIdsChanges( TInt aPriority );

    /**
     *  Notify SId listen error
     *
     *  @since 3.0
     *  @param aError error code
     */
    void NotifySIdListenError( TInt aError );

    /**
     *	Add SIDs containers
     *
     *	@since 3.0
     *	@param aSIdContainers new storage Id containers
     *	@param aOldSIdContainers array used for rollback in case
     *						error will occur while appending
     *	@param aKeepOldIds if ETrue, then engaged SIDs in
     *									the existing listener are kept
     *	@return KErrNone if operation went fine
     *						or system wide error code
     */
    TInt UpdateSIdsContainers(
        const RPointerArray<MPEngSIdContainer>& aSIdContainers,
        RPointerArray<MPEngSIdContainer>& aOldSIdContainers,
        TBool aKeepOldIds );

    /**
     *	Add new SIds containers
     *
     *	@since 3.0
     *	@param aSIdContaners new storage Id containers to be added
     *	@param aKeepOldIds if ETrue, then engaged SIDs in
     *									the existing listener are kept
     *	@return KErrNone if operation went fine
     *						or system wide error code
     */
    TInt UpdateSIdsContainers(
        const RPointerArray<MPEngSIdContainer>& aSIdContaners,
        TBool aKeepOldIds );


    /**
     *	Remove engaged Storage Ids from existing Storage IDs listener
     *
     *	@since 3.0
     *	@param aSIDsToRemove Storage Ids to be removed
     */
    void RemoveSIdsFromSIdsObserver( const MDesCArray& aSIdsToRemove );


    /**
     *	Remove all engaged Storage Ids from existing listener
     *
     *	@since 3.0
     */
    void RestartObserver();

    /**
     *  Count of Engaged SIDs
     *
     *  @since 3.0
       *  @return number of SIds observer is still listening
     */
    TInt EngagedSIdsCount();

    /**
     *  Observer pointer
     *
     *  @since 3.0
       *  @return Observer pointer
     */
    const MPEngSIDChangeObserver* Observer() const;

    /**
     * Set suspend mode of the Observer
     *
     * @since 3.0
     * @param aSuspendMode suspend mode
     */
    void SetSuspendMode( TBool aSuspendMode );

private:  // New functions

    /**
     *	Reset Engaged SIds array,
     *
     *  @since 3.0
     */
    void ResetEngagedIdsArray();

private:

    /**
     * C++ default constructor.
     */
    CPEngObserverContainer( MPEngSIDChangeObserver& aChangeObserver,
                            TInt aPriority );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


private:    // Data
    /// REF: Storage ID change observer
    MPEngSIDChangeObserver&				iObserver;

    /// REF: SID containers observer is interested in
    RPointerArray<MPEngSIdContainer>	iEngagedSIds;

    /// Priority of the observer
    TInt								iObserverPriority;

    /// Flag is observer is suspended
    TBool								iSuspended;

    /// OWN: Buffer of changed SIDs
    CPtrCArray							iChangedSIds;

    /// Changes notification is in progress
    TBool								iProcessing;

    /// REF: Flag if object is alive
    TBool*								iAliveFlag;
    };

#endif      // __CPENGOBSERVERCONTAINER_H__

// End of File
