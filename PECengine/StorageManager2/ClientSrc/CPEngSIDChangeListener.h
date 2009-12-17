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
* Description:  Storage Id listener manager
*
*/



#ifndef __CPENGSIDCHANGELISTENER_H__
#define __CPENGSIDCHANGELISTENER_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>


// DATA TYPES
typedef TPckgBuf<TInt> TIntPckgBuf;


// FORWARD DECLARATIONS
class MPEngSIDChangeObserver;
class MPEngSIdContainer;
class CPEngSIdContainer;
class CPEngObserverContainer;
class RPEngStorageSubFolderClient;

/**
 *  Storage Ids listener manager
 *  Listens all storage ids changes in which
 *	all registered observers are interested and informs them
 *	about changes
 *
 *  @lib PEngStoreman2.dll
 *  @since 3.0
 */
NONSHARABLE_CLASS( CPEngSIdChangeListener )
        : public CActive
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPEngSIdChangeListener* NewL(
        RPEngStorageSubFolderClient& aNotifyClient );

    /**
     * Two-phased constructor.
     */
    static CPEngSIdChangeListener* NewLC(
        RPEngStorageSubFolderClient& aNotifyClient );

    /**
     * Destructor.
     */
    virtual ~CPEngSIdChangeListener();

public: // New functions

    /**
     *	Register Storage Ids observer
     *  If it was already registered, it's set of observed
     *  Storage Ids is updated.
     *
     *
     *	@since 3.0
     *
     *	@param aNewSIDs new Storage IDs listener is interested in
     *	@param aSIdChangeObserver storage Id observer
      *	@param aPriority priority of the observer
     *	@param aKeepOldIDs if ETrue, there current engaged SIDs
     *						 in the existing listener are kept
        *	@return ETrue if SId listener was existing already
     */
    TBool RegisterObserverL(
        const MDesCArray& aNewSIds,
        MPEngSIDChangeObserver& aSIdChangeObserver,
        TInt aPriority,
        TBool aKeepOldIds );

    /**
     *	Remove engaged SIDs from existing Storage Ids listener
     *
     *	@since 3.0
     *	@param aSIdsToRemove Storage IDs to be removed from the listener
     *	@param aSIdChangeObserver storage Id observer
     *	@return ETrue if SID listener was after removing empty and was deleted
     */
    void RemoveSIdsFromObserver(
        const MDesCArray& aSIdsToRemove,
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *	Remove all SIDs from existing Storage Ids listener
     *
     *	@since 3.0
     *	@param aSIdChangeObserver storage Id observer
     *	@return ETrue if SID listener was after removing empty and was deleted
     */
    void RestartObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     * Remove storage Ids observer from the listener
     * return ETrue if there are still some active observers
     *
     * @since 3.0
     * @param aSIdChangeObserver storage Id observer
     * @return ETrue if there is still some active listener,
     *			otherwise EFalse
     */
    TBool UnRegisterObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver );

    /**
     *	Update suspend state of existing SIDs listener
     *	If listener is not found, function will return KErrNotFound
     *
     *	@since 3.0
     *	@param aSIdChangeObserver storage Id observer
     *  @param aSuspendState new suspend state
     *	@return KErrNone or KErrNotFound
      */
    TInt UpdateSuspendStateOfObserver(
        const MPEngSIDChangeObserver& aSIdChangeObserver,
        const TBool aSuspendState );

    /**
     *  Check if there is still any active listener
      *
     *  @since 3.0
     *  @return ETrue if there is any active listener
     */
    TBool AnyActiveListener();

public:		// Function from the CActive
    /**
     *  Handles an active object’s request completion event.
     *  @since 3.0
     */
    void RunL();

    /**
     *  Handles a leave occurring in the RunL() handling
     *
     *  @since 3.0
     *  @param aError The leave code
     *  @return implementation should always return KErrNone,
     */
    TInt RunError( TInt aError );

    /**
     *  Implements cancellation of an outstanding request.
     *  This function is called as part of the active object’s Cancel().
     *
     *  @since 3.0
     */
    void DoCancel();

private:  // New functions

    /**
     *  Add new storage IDs observer to the listener
     *
     *  @since 3.0
     *  @param aSIds array of storage IDs observer is interested in
     *  @param aChangeObserver observer
     *	@param aPriority priority of the observer
     *  @return ETrue if SId listener was existing already
     */
    TBool AddNewObserverL(
        const MDesCArray& aSIds,
        MPEngSIDChangeObserver& aChangeObserver,
        TInt aPriority );

    /**
     *  Update Engaged SIds array
     *
     *  @since 3.0
     *  @param aNewSIDs SIDs to be added to the array
     *  @param aSIDContainers SIDs containers array
     *  @return ETrue if there was new element in array
     */
    TBool UpdateEngagedSIdsArrayL(
        const MDesCArray& aNewSIDs,
        RPointerArray<MPEngSIdContainer>& aSIDContainers );

    /**
     *  Rebuild transfer array to the presence server,
     *  @since 3.0
     */
    TInt RebuildTransferArray();

    /**
     *	Update Set of SIDs to the Presence Server
     *
     *	@since 3.0
     *	@return KErrNone if it went OK,
     *			 otherwise some system wide error code
     */
    TInt UpdateListenerToServer();

    /**
     *	Commit array of SIds
     *
     *	@since 3.0
     */
    void CommitSIdArray();

    /*
     *	Roll back array of SIds
     *
     *	@since 3.0
     */
    void RollBackSIdArray();

    /**
     *	Search in Array in array of Engaged SIds
     *
     *	@since 3.0
     *	@param aSId Store Id to look for
     *  @param aIndex last checked index in search
     *	@return KErrNone if found or KErrNotFound if not
     */
    TInt FindSIdContainer( const TDesC& aSId,
                           TInt& aIndex ) const;

    /**
     *	Search in Array of Observers
     *
     *	@since 3.0
     *	@param aObserver reference to observer
     *	@return index in array or KErrNotFound
     */
    TInt FindObserver( const MPEngSIDChangeObserver& aObserver,
                       TInt& aIndex ) const;

    /**
     *	Check array update requirement
     *
     *	@since 3.0
     *	@return ETrue if update is required
     */
    TBool UpdateRequired();

    /**
     * Call all observers and inform about SID change if needed
     *
     * @since 3.0
     */
    void InformObserverAboutChange();

    /**
     * Call all observers and inform about error in listening
     *
     * @since 3.0
     */
    void InformObserverAboutError( TInt aErroCode );

    /**
     * Allocate one slot for the observer container in the array
     *
     * @since 3.0
     * @return KErrNone if it went OK
     */
    TInt AllocateSlotForObserverContainer();


    /**
     * Execute sheduled deletion
     * @since 3.0
     */
    void ExecuteScheduledDeletions();

private:

    /**
     * C++ default constructor.
     */
    CPEngSIdChangeListener(
        RPEngStorageSubFolderClient& aNotifyClient );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


private:    // Data
    /// REF: Notify client
    RPEngStorageSubFolderClient&				iNotifyClient;

    /// OWN: Transfer array of SIds to server
    RBuf										iTransBuff;

    /// OWN: Size of the transaction buffer
    TInt										iTransBuffSize;


    /// OWN: Server answer buffer
    RBuf										iAnswerBuff;

    /// OWN: Array of observers
    RPointerArray<CPEngObserverContainer>		iObservers;

    /// OWN: Array of SId containers
    RPointerArray<CPEngSIdContainer>			iSIdContainers;

    /// OWN: Flag if listener is active
    TBool										iListenerRunning;

    /// OWN: Flag is object is alive
    TBool*										iAliveFlag;
    };

#endif      // __CPENGSIDCHANGELISTENER_H__

// End of File
