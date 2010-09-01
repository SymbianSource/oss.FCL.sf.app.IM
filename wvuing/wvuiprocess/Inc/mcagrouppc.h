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
* Description:  Interface for group operations
*
*/


#ifndef MCAGROUPPC_H
#define MCAGROUPPC_H


// INCLUDES

#include "TEnumsPC.h"
#include "badesca.h"

// FORWARD DECLARATIONS
class MCAGroupObserverPC;
class MCAGroupEventObserverPC;
class MCAServerContactsArrayPC;



// CLASS DECLARATION
/**
 *  Group interface for UI and Engine.
 *
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class MCAGroupPC

    {
    public:

        /**
         * Set the current group
         * @param aGroupId groupid of the active group in the local list.
         */
        virtual void SetActiveGroupChatL( const TDesC& aGroupId ) = 0;

        /**
         * To register the UI component as an observer
         * to listen to the chat related events
         * @param aGroupObserverPC - Ptr to the group observer to be added
         * for chat/message error related events
         */
        virtual void RegisterGroupObserver( MCAGroupObserverPC*
                                            aGroupObserverPC ) = 0;
        /**
         * To unregister the UI side observer
         */
        virtual void UnRegisterGroupObserver() = 0;

        /**
         * Set an observer for events of individual group (join, leave, etc.)
         * @param aGroupId The group id
         * @param aGroupEventObserverPC The observer to be added
         */
        virtual void SetGroupEventObserverL( MCAGroupEventObserverPC* aGroupEventObserverPC,
                                             const TDesC& aGroupId ) = 0;

        /**
         * Part of container identifier which contains this message
         * @return Identifier to identify container which contains this message
         */
        virtual const TDesC& ActiveGroupId()	= 0;

        /**
         * Part of identification of container
         * @return Screen name of container owner.
         */
        virtual const TDesC& ScreenName() const = 0;

        /**
         * InitialiseResourcesL - intialise iGroupId
         * and some other members like read/write interface
         * @param aGroupId - the current groupid
         */
        virtual void InitialiseResourcesL( const TDesC& aGroupId ) = 0;


        /**
         * Tests whether the logging is on or not.
         * @return ETrue if on EFalse if not.
         */
        virtual TBool IsLoggingOn() const = 0;

        /**
         * Release all resources that was initialised
         */
        virtual void ReleaseResources() = 0;

        /**
         * Name of the group
         * @return name of the group
         */
        virtual TPtrC GroupName() const = 0;


        /**
         * Ends the logging in a specified group.
         */
        virtual void EndLoggingL() = 0;


        /**
         * Is private messages allowed
         * @return EImpsPropYes if allowed
         */
        virtual TEnumsPC::TImpsPropertyBoolPC IsPrivateAllowed() = 0;


        /**
         * Is group open or closed
         * @return EImpsPropYes if open
         */
        virtual TEnumsPC::TImpsPropertyBoolPC IsOpen() = 0;



        /**
         *  Count of stored groups
         *  @return Count of groups
         */
        virtual TInt GroupCount( TBool aOnlyVisible ) const = 0;


        /**
         * Count of stored contacts
         * @param aSkipOfflineContacts ETrue skip offline contacts.
         *                             EFalse Do not skip.
         * @return Count of contacts
         */
        virtual TInt ContactCount(
            TBool aSkipOfflineContacts = EFalse ) const = 0;


        /**
         * Gets the properties of group
         * @param aIsGroupProps return ETrue if
         * Common group properties are defined
         */
        virtual TInt GetPropertiesL( TBool& aIsGroupProps ) = 0;


        /**
         * SendMessageL
         * @param aMessageBuffer: message to be sent
         * @param aRecipients: List of all recipients to whom
         * message has to be sent
         */
        virtual void SendMessageL( const TDesC& aMessageBuffer,
                                   CDesCArrayFlat& aRecipients  ) = 0;

        /**
         * To populate the two arrays of group names and goup ids
         * @return - Fills and returns groupname along with the ID
         * Ownership is transferred to the caller
         */
        virtual MCAServerContactsArrayPC* GetGroupPairArrayLC() = 0;

        /**
         * To Check whether a GroupChat Id is the current active one
         * in groupChatView
         * @param aGroupId - Group Id
         * @return ETrue - if "aGroupId" is current active one in ChatView
         *		else EFalse
         */
        virtual TBool IsActiveGroupChat( const TDesC& aGroupId ) = 0;

        /**
         * Saves the group conversation
         * @param aLogFilename - log file name in which group conversation would be saved
         */
        virtual void SaveChatL( const TDesC& aLogFilename ) = 0;


        /**
        * Gets the message count from ReadInterface
        */
        virtual TInt MessageCount() = 0;

    public:

        /**
         * Destructor.
         */
        virtual ~MCAGroupPC()
            {

            };


    };

#endif      // MCAGROUPPC_H

// End of File
