/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for search operations
*
*/


#ifndef MCASEARCHINTERFACEPC_H
#define MCASEARCHINTERFACEPC_H


// INCLUDES
#include "MCASearchObserverInterface.h"
#include "TEnumsPC.h"
#include "MCASearchInterface.h"

#include	<impsgroupprops.h>

// FORWARD DECLARATIONS
class   MCASearchObserverInterfacePC;
class   MCASearchBackgroundObserverPC;
class   MCAServerContactsArrayPC;


// CLASS DECLARATION
/**
 *  Search services interface for UI and Engine.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCASearchInterfacePC: public MCASearchObserverInterface

    {
    public:

        /**
         * Destructor.
         */
        virtual ~MCASearchInterfacePC()
            {

            };
    public:

        /**
        * To add the UI side observer for MCASearchObserverInterfacePC
        * events from the engine.
        */
        virtual void AddObserver( MCASearchObserverInterfacePC* aObserver ) = 0;

        /**
        * To remove the UI side observer for MCASearchObserverInterfacePC
        * events from the engine.
        */
        virtual void RemoveObserver() = 0;

        /**
        * Registers observer for background task
        * @param aBackgaBackgroundObserver pointer of instance of UI component
        * @param aTaskMask is an AND mask for task requested,
        * @param aEventMask is an AND mask for events requested,
        * @return KErrNone on success, system wide error code otherwise
        */
        virtual TInt RegisterBackgroundObserver(
            MCASearchBackgroundObserverPC* aBackgroundObserver,
            TInt aTaskMask,
            TInt aEventMask
        ) = 0;
        /**
        * Removes the observer
        * @param aBackgroundObserver pointer of instance to remove
        */
        virtual void UnRegisterBackgroundObserver(
            MCASearchBackgroundObserverPC* aBackgroundObserver
        ) = 0;

        /**
        * @param aTaskID is an id of a background task
        * @retrun status of the background task or KErrNotFound
        */
        virtual TInt BackgroundTaskStatus( TEnumsPC::TCABackgroundTasks aTaskID ) = 0;

        /**
        * Builds the search pairs and sets the search requestSet values.
        * This allocates memory and copies aValue to its internal data storage.
        * @param aSearchCriteriaArray specifies list of search criterias (search keys)
        * @param aSearchStringArray specifies array of strings to be searched(search values)
        */

        virtual void SetRequestL(  RArray<TEnumsPC::TSearchCriteria>&  aSearchCriteriaArray,
                                   CDesCArray& aSearchStringArray ) = 0;

        /**
         * @return ETrue if iSearchPairs is not null
         */
        virtual TBool IsSearchPairsSet() = 0;

        /**
         * @return SearchString - string to be searched
         */
        virtual HBufC*  GetSearchStringLC() = 0;

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return ETrue if contact found else EFalse
         */
        virtual TBool FindAnyContactL( const TDesC& aContactId ) = 0;

    public:

        /**
        * @see MCASearchObserverInterface::HandleError
        */
        virtual void HandleSearchError( TInt aErrorCode ) = 0;
        /**
        * @see MCASearchObserverInterface::HandleSearchFinished
        */
        virtual void HandleSearchFinished( ) = 0;


    public:

        /**
         * Start the search.
         * @param aSearchLimit defines how many results you want.
         * @param aObserver The observer for this search operation
         * @param aSearchPairs Pairs to be searched
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        virtual TInt StartSearchL( TInt aSearchLimit,
                                   MCASearchObserverInterfacePC* aObserver,
                                   CSearchPairs* aSearchPairs = NULL ) = 0;

        /**
         * Stops the current search. Must be called before starting new search.
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        virtual TInt EndSearchL() = 0;

        /**
         * Continues search. This function must only be called when more results
         * are wanted.
         * @param aIndex specifies the next index where to start search.
         * @param aObserver The observer for this search operation
         * @return Error code.
         * Leaves
         *   ERequestedResultsButNotAvailable
         *   ESearchNotSupported
         */
        virtual TInt SearchNextL( TInt aIndex,
                                  MCASearchObserverInterfacePC* aObserver ) = 0;

        /**
         * Gets the properties of group
         * @param The group id
         * @param aCommonProps Common group properties
         * @param aPrivateProps Private group properties
         * @return CSP error code
         */
        virtual TEnumsPC::TImpsCSPAllErrors GetPropertiesL( const TDesC& aGroupId,
                                                            CImpsCommonGroupProps*& aCommonProps,
                                                            CImpsPrivateGroupProps*& aPrivateProps
                                                          ) = 0;

        /**
        * To set the groupname of the group whose id is aGroupId
        * @param aGroupId: id of the group whose name has to be set
        * @param aGroupName: name of the group to be set
        */

        virtual void SetGroupNameL( const TDesC& aGroupId, const TDesC& aGroupName ) = 0;

        /**
        * To create a group with group id aGroupId
        * @param aGroupId: id of the group to be created
        */
        virtual void CreateGroupL( const TDesC& aGroupId ) = 0;

        /**
        * To save the changes made to a groups properties and make it persistent. Once this is done
        * the isFavorite property of the group will always return true.
        * @param aGroupId: id of the group which has to be saved
        */
        virtual void SaveChangesL( const TDesC& aGroupId ) = 0;

        /**
        * To populate the two arrays of group names and goup ids
        * @param aGroupNames: array of the current group names
        * @param aGroupIds: array of the current group IDs
        */
        virtual MCAServerContactsArrayPC* GetPairArrayForServerContactsArrayLC() = 0;

        /**
        * To set the groupname of the group whose id is aGroupId
        * @param aGroupId: id of the group which has to be searched for
        * @param bool : returns true if the group with aGroupId is found else returns false
        */
        virtual TBool FindGroup( const TDesC& aGroupId ) = 0;


        /**
         * Delete the given group
         * @param aGroupId The group id of the group to delete
         * @param aDeleteFromNetwork Defines whether the group should
         *        be deleted also from network server or not
         * @return The errorcode
         */
        virtual TInt DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork ) = 0;


        /**
        * To get the groupname of the group whose id is aGroupId
        * @param aGroupId: of the group whose name has to be set
        * @param aHasCommonProps: Returned by reference. True if the group has common group properties
        * else return false
        * @param aImpsError: return by reference. Return the error code if any.
        */
        virtual TPtrC GetGroupNameL( const TDesC& aWvid, TBool& aHasCommonProps,
                                     TEnumsPC::TImpsCSPAllErrors& aImpsError ) = 0;
        /*
         * Get the storage type of this group and check if it is a persistent grp.
         * If the group is marked as favourite, then it will be persistent
         * @return ETrue if favourite else EFalse
         */
        virtual TBool IsFavouriteChatGroup( const TDesC& aGroupId ) const = 0;

        /*
         * Get the total no. of groups the user has joined too'.
         */
        virtual TInt JoinedGroups() = 0;
    };

#endif      // MCASEARCHINTERFACEPC_H

// End of File
