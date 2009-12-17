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
* Description:  Implementation for PC Group Interface
*
*/


#ifndef CCAGROUPPC_H
#define CCAGROUPPC_H


// INCLUDES
#include	"mcagrouppc.h"
#include	"MCAChatObserver.h"
#include	"MCAGroupEventObserver.h"
#include	"MCAMessageErrorObserver.h"
#include	"MCAMessageRWInterfacePC.h"

#include    <impsgroupprops.h>


// FORWARD DECLARATIONS
class CCAEngine;
class MCAGroupObserverPC;
class MCAMessagesReadInterface;
class MCAMessagesWriteInterface;
class MCAChatInterface;
class MCAStoredGroups;
class MCAStoredGroup;
class MCAGroupEventObserverPC;
class MCAGroupManagerInterface;
class MCAGroupOperations;
class MCAStoredContacts;
class MCAServerContactsArrayPC;
class MCAMessageUtils;


//class declaration
/**
 *  Implementation class for Group Interface for the UI.
 *  @lib wvuiprocessng.lib
 *  @since 3.2
 */
class CCAGroupPC:  public CBase,
            public MCAGroupPC,
            public MCAChatObserver,
            public MCAGroupEventObserver,
            public MCAMessageErrorObserver,
            public MCAMessageRWInterfacePC

    {

    public:

        /**
         * Symbian Two Phase Constructor
         * @param aGroupInterface : a pointer to group interface
         * @param aChatInterface : a reference to chat interface
         * @param aMessageUtils : a reference to Message Utils
         */
        static CCAGroupPC* NewL( MCAGroupManagerInterface* aGroupInterface,
                                 MCAChatInterface& aChatInterface,
                                 const MCAMessageUtils& aMessageUtils );


        /**
         * Second phased symbian constructor.
         */
        void ConstructL();

        /**
         * virtual destructor
         */
        virtual ~CCAGroupPC();

    private:

        /**
         * default constructor
         * @param aGroupInterface : a pointer to group interface
         * @param aChatInterface : a reference to chat interface
         * @param aMessageUtils : a reference to Message Utils
         */

        CCAGroupPC( MCAGroupManagerInterface* aGroupInterface,
                    MCAChatInterface& aChatInterface,
                    const MCAMessageUtils& aMessageUtils );

    public: //From MCAGroupPC

        /**
         * Set the current group
         * @param aGroupId groupid of the active group in the local list.
         */
        void SetActiveGroupChatL( const TDesC& aGroupId );

        /**
         * To register the UI component as an observer
         * to listen to the chat/message error related events
         * @param aGroupObserverPC - Ptr to the group pc observer to be added
         */
        void RegisterGroupObserver( MCAGroupObserverPC*  aGroupObserverPC );

        /**
         * To unregister the observer
         */
        void UnRegisterGroupObserver();


        /**
         * Set an observer for events of individual group (join, leave, etc.)
         * @param aGroupEventObserverPC The observer to be added
         * @param aGroupId The group id for which group events to be received
         */
        void SetGroupEventObserverL( MCAGroupEventObserverPC* aGroupEventObserverPC,
                                     const TDesC& aGroupId );


        /**
         * Part of container identifier which contains this message
         * @return Identifier to identify container which contains this message
         */
        const TDesC& ActiveGroupId();

        /**
         * Part of identification of container
         * @return Screen name of container owner.
         */
        const TDesC& ScreenName() const;


        /**
         * InitialiseResourcesL - intialise iGroupId
         * and some other members like read/write interface
         * @param aGroupId - the current groupid
         */
        void InitialiseResourcesL( const TDesC& aGroupId );

        /**
         * Tests whether the logging is on or not.
         * @return ETrue if on EFalse if not.
         */
        TBool IsLoggingOn() const ;

        /**
        * Release all resources that was initialised
        */
        void ReleaseResources() ;


        /**
         * Gets the properties of group
         * @param aIsGroupProps return ETrue if
         * Common group properties are defined
         */
        TInt GetPropertiesL( TBool& aIsGroupProps );

        /**
         * Name of the group
         * @return name of the group
         */
        TPtrC GroupName() const;

        /**
         * Is private messages allowed
         * @return EImpsPropYes if allowed
         */
        TEnumsPC::TImpsPropertyBoolPC IsPrivateAllowed();


        /**
         * Is group open or closed
         * @return EImpsPropYes if open
         */
        TEnumsPC::TImpsPropertyBoolPC IsOpen();

        /**
         * @see MCAGroupPC
         */
        void EndLoggingL() ;


        /**
         * @see MCAGroupPC
         */
        void SaveChatL( const TDesC& aLogFilename );

        /**
         * @see MCAGroupPC
         */
        TInt GroupCount( TBool aOnlyVisible ) const ;


        /**
         * @see MCAGroupPC
         */
        TInt ContactCount(
            TBool aSkipOfflineContacts = EFalse ) const ;



        /**
         * @see MCAGroupPC
         */
        void SendMessageL( const TDesC& aMessageBuffer,
                           CDesCArrayFlat& aRecipients ) ;

        /**
         * @see MCAGroupPC
         */

        MCAServerContactsArrayPC* GetGroupPairArrayLC();

        /**
        * @see MCAGroupPC
        */
        TBool IsActiveGroupChat( const TDesC& aGroupId );


        /**
        * @see MCAGroupPC
        */
        TInt MessageCount();

    public:

        /**
         * To convert the impsengine side TImpsPropertyBool enum to PC side TImpsPropertyBoolPC
         * @param aEventSource: enum type in impsengine TImpsPropertyBool
         * @return TEnumsPC::TImpsPropertyBoolPC
         */
        TEnumsPC::TImpsPropertyBoolPC ConvertTImpsPropertyBoolToTEnumsPC
        ( TImpsPropertyBool aEventSource );



    public: //From MCAMessageRWInterfacePC
        /**
         * @see MCAMessageRWInterfacePC
         */
        MCAMessagesReadInterface& ReadInterfaceL() const;

        /**
         * @see MCAMessageRWInterfacePC
         */
        MCAMessagesWriteInterface& WriteInterfaceL() const;

    private: //From MCAGroupEventObserver

        /**
           * @see MCAGroupEventObserver
         */
        void HandleGroupPropertiesChanged() ;

        /**
           * @see MCAGroupEventObserver
         */
        void HandleGroupLeft( TInt aReason ) ;


    private: //From MCAChatObserver

        /**
        * Handle chat events.
        * @param aEvent Event to handle.
        * @param aMessage, Event occurred because of message
        */
        void HandleChatEvent( TChatEventType aEvent,
                              MCAMessage* aMessage = NULL ) ;

    private: //From MCAMessageErrorObserver

        /**
         * Handle message error related events.
         * @param aMessage, Event occurred because of message
         */
        void HandleMessageError( TInt aError, MCAMessage* aMessage ) ;

    private:

        //Doesn't own. Group interface
        MCAGroupManagerInterface* iGroupInterface;

        //Chat interface,doesn't own
        MCAChatInterface& iChatInterface;

        //Ref to MessageUtils on engine side,Doesn't own
        const MCAMessageUtils& iMessageUtils;


        //Doesn't own, pointer to PC group observer
        MCAGroupObserverPC* iGroupObserverPC;

        //Doesn't own, pointer to PC group event observer
        MCAGroupEventObserverPC* iGroupEventObserverPC;


        //Doesnt own,Handle to stored groups interface
        MCAStoredGroups* iStoredGroups;


        // Interface for reading messages
        MCAMessagesReadInterface* iReadInterface;

        // Interface for writing messages
        MCAMessagesWriteInterface* iWriteInterface;

        //Owns. Current Group id
        HBufC* iGroupId;

        // Doesn't own. Pointer to this stored chat-group's
        // data in engine
        MCAStoredGroup* iStoredGroup;

        // Doesn't own. Pointer to group operations
        MCAGroupOperations* iGrOps;

        //common group properties
        CImpsCommonGroupProps* iGroupProps;

        //private group props
        CImpsPrivateGroupProps* iPrivateProps;

        //Doesnt own, Handle to Interface for stored contacts
        MCAStoredContacts* iStoredContacts;

    };

#endif      // CCAGROUPPC_H

// End of File
