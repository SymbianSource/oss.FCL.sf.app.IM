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
* Description:  Implementation for PC Search Interface
*
*/


#ifndef CCASEARCHINTERFACEPC_H
#define CCASEARCHINTERFACEPC_H


// INCLUDES
#include	"MCASearchInterfacePC.h"
#include    "MCABackgroundObserver.h"


// FORWARD DECLARATIONS
class MCASearchBackgroundObserverPC;
class MCASearchInterface;
class MCASearchObserverInterfacePC;
class MCAGroupManagerInterface;
class MCAStoredGroups;


//class declaration
/**
 *  Implementation class for Search Interface for the UI.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCASearchInterfacePC: public CBase,
            public MCASearchInterfacePC,
            public MCABackgroundObserver

    {

    public:

        /**
        * Symbian Two Phase Constructor
        * @param aSearchInterface : a reference to search interface in the chat engine
        * @param aBackgroundInterface : a ref. to background interface in the chat engine
        * @param groupMgrInterface : a ref. to group manager interface in the chat engine
        */
        static CCASearchInterfacePC* NewL( MCASearchInterface* aSearchInterface ,
                                           MCABackgroundInterface& aBackgroundInterface,
                                           MCAGroupManagerInterface& groupMgrInterface );


        /**
         * virtual destructor
         */
        virtual ~CCASearchInterfacePC();

    private:
        /**
        * default constructor
        * @param aSearchInterface : a reference to search interface in the chat engine
        * @param aBackgroundInterface : a ref. to background interface in the chat engine
        */


        CCASearchInterfacePC( MCASearchInterface* aSearchInterface,
                              MCABackgroundInterface& aBackgroundInterface,
                              MCAGroupManagerInterface& groupMgrInterface );

        /**
        * constructor
        * @since 3.2
        */
        void ConstructL();

    public:

        /**
         * Find any contact which id is aContactId
         * @param aContactId Contactid to recognize contact.
         * @return ETrue if contact found else EFalse
         */
        TBool FindAnyContactL( const TDesC& aContactId );

        /**
        * To convert the engine side TCABackgroundTasks enum to PC side TEnumsPC
        * @param aEventSource: enum type in engine MCABackgroundInterface::TCABackgroundTasks
        * @return TEnumsPC::TCABackgroundTasks
        */
        TEnumsPC::TCABackgroundTasks ConvertTCABackgroundTasksToTEnumsPC
        ( MCABackgroundInterface::TCABackgroundTasks aEventSource );

        /**
        * To convert the engine side TCABackgroundStatus enum to PC side TEnumsPC
        * @param aEventSource: enum type in engine MCABackgroundInterface::TCABackgroundStatus
        * @return TEnumsPC::TCABackgroundStatus
        */
        TEnumsPC::TCABackgroundStatus ConvertTCABackgroundStatus
        ( MCABackgroundInterface::TCABackgroundStatus aStatus );

        /**
        * To convert the PC Side TEnumsPC::TCABackgroundTasks enum to Engine side TCABackgroundStatus
        * @param aSource: enum type in process TEnumsPC::TCABackgroundStatus
        * @return enum type in engine MCABackgroundInterface::TCABackgroundStatus
        */
        MCABackgroundInterface::TCABackgroundTasks ConvertToTCABackgroundTasks
        ( TEnumsPC::TCABackgroundTasks aSource );

        /**
        * To convert the PC Side TSearchCriteria enum to IMEngine side TImpsSearchElement
        * @param aElement: enum type in process TEnumsPC::TSearchCriteria
        * @return enum type in IMEngine TImpsSearchElement
        */
        TImpsSearchElement CCASearchInterfacePC::
        ConvertToTImpsSearchElement( TEnumsPC::TSearchCriteria aElement );

        /**
         * @return a CSearchPairs pointer
         */
        CSearchPairs* SearchPairs();

        /**
         * set search pairs
         * @param aCount: specifies the number of elements
         * in the array iSearchPairs that has to be constructed
         */
        void SetSearchPairsL( TInt aCount );

        /**
         * @return ETrue if iSearchPairs is not null
         */
        TBool IsSearchPairsSet();

        /**
         * @return SearchString -string to be searched
         */
        HBufC*  GetSearchStringLC();

        /**
        * Builds the search pairs and sets the search requestSet values.
        * This allocates memory and copies aValue to its internal data storage.
        * @param aSearchCriteriaArray specifies list of search criterias (search keys)
        * @param aSearchStringArray specifies array of strings to be searched(search values)
        */

        void SetRequestL(  RArray<TEnumsPC::TSearchCriteria>&  aSearchCriteriaArray,
                           CDesCArray& aSearchStringArray );
    public://From MCABackgroundObserver

        /**
        * Call if an event happend that match registered task and event mask.
        * @param aEventSource is the source of event
        * @param aStatus is the status of the background task
        * @param aSubTaskNumber is the number of subtask
        * @param aLeaveCode is the leave code of the task/subtask
        */
        void HandleBackgroundEvent(
            MCABackgroundInterface::TCABackgroundTasks aEventSource,
            MCABackgroundInterface::TCABackgroundStatus aStatus,
            TInt aSubTaskNumber,
            TInt aLeaveCode
        );

    public://From MCASearchInterfacePC

        /**
        * @see MCASearchInterfacePC
        * To add the UI side observer for MCASearchObserverInterfacePC
        * events from the engine.
        */
        void AddObserver( MCASearchObserverInterfacePC* aObserver );

        /**
        * @see MCASearchInterfacePC
        * To remove the UI side observer for MCASearchObserverInterfacePC
        * events from the engine.
        */
        void RemoveObserver();

        /**
        * Registers observer for background task
        * @param aBackgaBackgroundObserver pointer of instance of UI component
        * @param aTaskMask is an AND mask for task requested,
        * @param aEventMask is an AND mask for events requested,
        * @return KErrNone on success, system wide error code otherwise
        */
        TInt RegisterBackgroundObserver(
            MCASearchBackgroundObserverPC* aBackgroundObserver,
            TInt aTaskMask,
            TInt aEventMask );

        /**
        * Removes observer from the queue
        * @param aBackgroundObserver pointer of instance to remove
        */
        void UnRegisterBackgroundObserver(
            MCASearchBackgroundObserverPC* aBackgroundObserver );

        /**
        * @param aTaskID is an id of a background task
        * @retrun status of the background task or KErrNotFound
        */
        TInt BackgroundTaskStatus( TEnumsPC::TCABackgroundTasks aTaskID );

    public:

        /**
        * @see MCASearchObserverInterface::HandleSearchError
        */
        void HandleSearchError( TInt aErrorCode );
        /**
        * @see MCASearchObserverInterface::HandleSearchFinished
        */
        void HandleSearchFinished( );


    public:

        /**
         * @see MCASearchInterfacePC
         * Start the search.
         * @param aSearchLimit defines how many results you want.
         * @param aObserver The observer for this search operation
         * @param aSearchPairs Pairs to be searched
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        TInt StartSearchL( TInt aSearchLimit,
                           MCASearchObserverInterfacePC* aObserver,
                           CSearchPairs* aSearchPairs = NULL ) ;

        /**
          * @see MCASearchInterfacePC
          * Stops the current search. Must be called before starting new search.
         * @return Error code.
         * Leaves
         *   ESearchNotSupported
         */
        TInt EndSearchL() ;

        /**
         * @see MCASearchInterfacePC
         * Continues search. This function must only be called when more results
         * are wanted.
         * @param aIndex specifies the next index where to start search.
         * @param aObserver The observer for this search operation
         * @return Error code.
         * Leaves
         *   ERequestedResultsButNotAvailable
         *   ESearchNotSupported
         */
        TInt SearchNextL( TInt aIndex, MCASearchObserverInterfacePC* aObserver ) ;

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
                                                          );

        /**
        * To set the groupname of the group whose id is aGroupId
        * @param aGroupId: id of the group whose name has to be set
        * @param aGroupName: name of the group to be set
        */
        virtual void SetGroupNameL( const TDesC& aGroupId, const TDesC& aGroupName );

        /**
        * To create a group with group id aGroupId
        * @param aGroupId: id of the group to be created
        */
        virtual void CreateGroupL( const TDesC& aGroupId );

        /**
        * To save the changes made to a groups properties and make it persistent. Once this is done
        * the isFavorite property of the group will always return true.
        * @param aGroupId: id of the group which has to be saved
        */
        virtual void SaveChangesL( const TDesC& aGroupId );

        /**
        * To populate the two arrays of group names and goup ids
        * @param aGroupNames: array of the current group names
        * @param aGroupIds: array of the current group IDs
        */
        virtual MCAServerContactsArrayPC* GetPairArrayForServerContactsArrayLC() ;

        /**
        * To set the groupname of the group whose id is aGroupId
        * @param aGroupId: id of the group which has to be searched for
        * @param bool : returns true if the group with aGroupId is found else returns false
        */
        virtual TBool FindGroup( const TDesC& aGroupId );

        /**
         * Delete the given group
         * @param aGroupId The group id of the group to delete
         * @param aDeleteFromNetwork Defines whether the group should
         *        be deleted also from network server or not
         * @return The errorcode
         */
        virtual TInt DeleteGroupL( const TDesC& aGroupId, TBool aDeleteFromNetwork );

        /**
        * To get the groupname of the group whose id is aGroupId
        * @param aGroupId: of the group whose name has to be set
        * @param aHasCommonProps: Returned by reference. True if the group has common group properties
        * else return false
        * @param aImpsError: return by reference. Return the error code if any.
        */
        virtual TPtrC GetGroupNameL( const TDesC& aGroupId, TBool& aHasCommonProps,
                                     TEnumsPC::TImpsCSPAllErrors& aImpsError );
        /*
         * Get the storage type of this group and check if it is a persistent grp.
         * If the group is marked as favourite, then it will be persistent
         * @return ETrue if favourite else EFalse
         */
        virtual TBool IsFavouriteChatGroup( const TDesC& aGroupId ) const;


        /*
         * Get the total no. of groups the user has joined too'.
         */
        virtual TInt JoinedGroups();

    private:

        /*
         * To convert the imps error code to the TEnumsPC type to pass in to the UI.
         * @param integer imps error code
         * @return TImpsCSPAllErrors specified in TEnumsPC
         */
        TEnumsPC::TImpsCSPAllErrors ConvertImpsErrorToUiEnums( TInt aImpsError );

    private:

        /**
        * iDataInterface: a reference to searchdata interface in the chat engine
        */
        MCASearchInterface* iSearchInterface;

        /**
        * Does not own, do not delete
        * iSearchObserverPC: a reference to the observer for events
        * from MCASearchInterfacePC. MCASearchInterfacePC inturn receives
        * the actual engine events from MCASearchObserverInterface.
        */
        MCASearchObserverInterfacePC* iSearchObserverPC;

        /**
        * Pointer to BackgroundObserverPC that has to be given to UI
        */
        MCASearchBackgroundObserverPC* iBackgroundObserver;

        /**
        * Reference to the engine's contactlist model
        */
        MCABackgroundInterface& iBackgroundInterface;

        // SearchPairs, Owns.
        CSearchPairs* iSearchPairs;

        //Reference to the group manager interface in the engine component. Doesnt own
        MCAGroupManagerInterface& iGroupMgrInterface;

        //Pointer to stored groups interface in the storage component. Doesnt own
        MCAStoredGroups* iStoredGroupsInterface;
    };

#endif      // CCASEARCHINTERFACEPC_H

// End of File
