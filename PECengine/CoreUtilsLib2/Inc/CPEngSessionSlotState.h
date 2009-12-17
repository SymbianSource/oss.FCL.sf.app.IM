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
* Description:  Presence Engine Session Slot State container.
*
*/

#ifndef __CPENGSESSIONSLOTSTATE_H__
#define __CPENGSESSIONSLOTSTATE_H__

//  INCLUDES
#include    <E32Base.h>
#include    <BaDesca.h>
#include    "PEngPresenceEngineConsts2.h"




// DATA TYPES

/**
 * Data selection enmumeration to select
 * which parts of the CPEngSessionSlotState
 * to stream.
 *
 * @since 3.0
 */
enum TPEngDataSelection
    {
    /**
     * Session Slot Id part.
     */
    EIdentification  = 0x00000001,

    /**
     * Registered application IDs part.
     */
    ERegistredIDs    = 0x00000010,

    /**
     * State specific data.
     * Active Application Ids and the state part.
     */
    EStateSpecific   = 0x00000100,



    //Mixed fields for real storage purposes.

    /**
     * Only Session Slot Id and Registered application IDs.
     */
    EPermanentData   = EIdentification | ERegistredIDs,


    /**
     * Whole state container data without the Slot ID.
     */
    EPureState       =  ERegistredIDs | EStateSpecific,


    /**
     * Whole state container data.
     */
    EWholeState      = EIdentification | ERegistredIDs | EStateSpecific,

    };



// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CPEngSessionSlotId;
class CPEngSessionSlotEvent;




// CLASS DECLARATION


/**
* Presence Engine Session Slot State container.
* It is used infernally by the PE for transportation
* of the slot state between the engine components
* and also to maintain the slot state both
* in permanent and cached memory.
*
* @since 3.0
*/
class CPEngSessionSlotState : public CBase
    {
    public: // Construction

        /**
         * Instantiates CPEngSessionSlotState object.
         *
         * @return New CPEngSessionSlotState instance.
         */
        IMPORT_C static CPEngSessionSlotState* NewL( );
        IMPORT_C static CPEngSessionSlotState* NewLC( );


        /**
         * Destructor.
         */
        ~CPEngSessionSlotState();

    protected:

        /**
         * C++ constructor.
         */
        CPEngSessionSlotState();



    public: // Getters

        /**
         * Session Slot ID identification
         *
         * @since 3.0
         * @return session slot ID identification class
         */
        IMPORT_C CPEngSessionSlotId& SessionSlotId( ) const;


        /**
         * Session slot Online state
         *
         * @since 3.0
         * @return session slot state
         */
        IMPORT_C TPEngNWSessionSlotState SessionSlotState() const;


        /**
         * Gets list of Application IDs which are registered
         * to this Session Slot.
         *
         * @since 3.0
         * @return descriptor array with application IDs
         */
        IMPORT_C const MDesC16Array& RegisteredApplications() const;


        /**
         * Gets list of Application IDs which are active with
         * this Session Slot.
         *
         * @since 3.0
         * @return descriptor array with application IDs
         */
        IMPORT_C const MDesC16Array& ActiveApplications() const;


        /**
         * Gets certain application state from
         * the session slot state.
         *
         * @since 3.0
         * @param aAppId Application Id for which to get the state.
         * @return State of the requested application Id.
         */
        IMPORT_C TPEngNWSessionSlotState AppState( const TDesC& aAppId ) const;


        /**
         * Checks if application is registered to this session slot.
         * If application is not registered, leaves with KErrNotFound
         *
         * @since 3.0
         * @param aAppId Application Id to check.
         */
        IMPORT_C void ApplicationRegisteredL( const TDesC& aAppId ) const;




    public: // Setters

        /**
         * Set session slot ID.
         *
         * @since 3.0
         * @param aSlotId New session slot Id.
         * @param aOwnershipTransfered ETrue if aSlotId ownership is
         *        transfered to CPEngSessionSlotState instance
         */
        IMPORT_C void SetSessionSlotId( CPEngSessionSlotId& aSlotId,
                                        TBool aOwnershipTransfered = EFalse );


        /**
         * Sets Session Slot State as closed.
         *
         * @since 3.0
         */
        IMPORT_C void SetSessionSlotStateClosed();


        /**
         * Update Session Slot State based on the event.
         *
         * @since 3.0
         * @param aSessEvent Session Slot event.
         * @return ETrue if state was updated because of event and
         *         state needs to be stored to permanent storage.
         *         Else EFalse.
         */
        IMPORT_C TBool UpdateStateL( const CPEngSessionSlotEvent& aSessEvent );


        /**
         * Updates and commits the Session Slot State
         * based on the event.
         *
         * @since 3.0
         * @param aSessEvent Session Slot event.
         * @return ETrue if state was updated because of event and
         *         state needs to be stored to permanent storage.
         *         Else EFalse.
         */
        IMPORT_C TBool UpdateAndCommitStateL( const CPEngSessionSlotEvent& aSessEvent );


        /**
         * Rollback last change
         *
         * @since 3.0
         */
        IMPORT_C void RollBackLastUpdate();


        /**
         * Commit last change
         *
         * @since 3.0
         */
        IMPORT_C void CommitLastUpdate();



    public: // Export import of the state

        /**
         * Internalize Session Slot state from stream.
         *
         * @since 3.0
         * @param aStream stream to internalize from
         * @param aSelection selection flag describing what to internalize
         */
        IMPORT_C void InternalizeL( RReadStream& aStream,
                                    TPEngDataSelection aSelection );


        /**
         * Externalize Session Slot state to stream
         *
         * @since 3.0
         * @param aStream stream to externalize
         * @param aSelection selection flag describing what to externalize
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream,
                                    TPEngDataSelection aSelection ) const;


        /**
         * Packs Session Slot State to the buffer.
         *
         * @since 3.0
         * @param aSelection selection flag describing what to pack
         * @return Buffer with streamed session slot state data.
         * Buffer ownership is returned to caller.
         */
        IMPORT_C HBufC8* PackDataLC( TPEngDataSelection aSelection ) const;
        IMPORT_C void PackDataL( TPEngDataSelection aSelection, RBuf8& aBuf ) const;


        /**
         * Unpacks Session Slot State from the passed descriptor
         *
         * @since 3.0
         * @param aDes Descriptor to unpack from.
         * @param aSelection selection flag describing what to unpack
         */
        IMPORT_C void UnpackDataL( const TDesC8& aDes,
                                   TPEngDataSelection aSelection );

        /**
         * Size of needed stream to externalizate the container.
         *
         * @since 3.0
         * @param aSelection selection flag describing what
         *        to calculate to the size.
         * @return size of the buffer in the bytes
         */
        IMPORT_C TInt Size( TPEngDataSelection aSelection ) const;



    private:    //Helpers

        /**
         * Add registered Application Id
         *
         * @since 3.0
         * @param aAppId application Id to be added
         */
        void AddRegisteredAppIdL( const TDesC& aAppId );


        /**
         * Remove registered Application Id
         *
         * @since 3.0
         * @param aAppId application Id to be removed
         */
        void RemoveRegisteredAppIdL( const TDesC& aAppId );


        /**
         * Add Active Application Id
         *
         * @since 3.0
         * @param aAppId application Id to be added
         */
        void AddActiveAppIdL( const TDesC& aAppId );


        /**
         * Remove Active Application Id
         *
         * @since 3.0
         * @param aAppId application Id to be removed
         */
        void RemoveActiveAppId( const TDesC& aAppId );

        /**
         * Checks is the Application Id registered or not
         *
         * @since 3.0
         * @param aAppId application Id to be check.
         * @return ETrue if app id is registered. Else EFalse.
         */
        TBool ApplicationRegistered( const TDesC& aAppId ) const;



    private: // data

        //OWN/REF: Session Slot ID
        CPEngSessionSlotId*             iSessionId;

        //OWN: Is previous session Slot ID owned or not
        TBool                           iSessionIdOwned;

        //OWN: Session Slot State
        TPEngNWSessionSlotState         iState;

        //OWN: Active Application's Application IDs
        CDesCArrayFlat                  iActiveApps;

        //OWN: Registered Application's Application IDs
        CDesCArrayFlat                  iRegisteredApps;



        // Roll back support for app id changes

        //OWN: Index of added active Id
        TInt                            iActiveAdded;

        //OWN: Index of removed active Id
        TInt                            iActiveRemoved;

        //OWN: Index of added Registered Id
        TInt                            iRegAdded;

        //OWN: Index of removed Registered Id
        TInt                            iRegRemoved;

    };

#endif      // __CPENGNWSESSIONSLOTEVENT_H__
// End of File