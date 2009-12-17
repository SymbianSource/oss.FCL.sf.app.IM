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
* Description:  Session Slot Event container.
*
*/

#ifndef __CPENGSESSIONSLOTEVENT_H__
#define __CPENGSESSIONSLOTEVENT_H__

//  INCLUDES

#include    <e32base.h>
#include    "PEngPresenceEngineConsts2.h"


// FORWARD DECLARATIONS
class CPEngSessionSlotId;
class RWriteStream;
class RReadStream;



// CLASS DECLARATION

/**
* Presence Engine Session Slot Event container.
* It is used infernally by the PE for transportation
* of the slot event between the engine components.
*
* @since 3.0
*/
class CPEngSessionSlotEvent : public CBase
    {
    public:
        /**
         * Instantiates CPEngSessionSlotEvent object.
         *
         * @return New CPEngSessionSlotEvent instance.
         */
        IMPORT_C static CPEngSessionSlotEvent* NewL( );
        IMPORT_C static CPEngSessionSlotEvent* NewLC( );


        /**
         * Destructor.
         */
        ~CPEngSessionSlotEvent();

    protected:

        /**
         * C++ constructor.
         */
        CPEngSessionSlotEvent();



    public: // Session slot state identifications

        /**
         * Session Slot ID identification
         * Gets the Session slot ID class from where the event came
         *
         * @since 3.0
         * @return session slot ID class
         */
        IMPORT_C const CPEngSessionSlotId& SessionSlotIndentification() const;

        /**
         * Application Id
         *
         * @since 3.0
         * @return application Id
         */
        IMPORT_C const TDesC& ApplicationId() const;

        /**
         * Global event
         *
         * @since 3.0
         * @return session slot event
         */
        IMPORT_C TPEngNWSessionSlotEvent Event() const;

        /**
         * Application session slot state
         *
         * @since 3.0
         * @return session slot state
         */
        IMPORT_C TPEngNWSessionSlotState AppSessSltState() const;

        /**
         * Global session slot state
         *
         * @since 3.0
         * @return session slot state
         */
        IMPORT_C TPEngNWSessionSlotState GlobSessSltState() const;

        /**
         * Additional event integer
         *
         * @since 3.0
         * @return additional event integer
         */
        IMPORT_C TInt EventInt() const;


        /**
         * Additional event descriptor
         *
         * @since 3.0
         * @return additional event descriptor
         */
        IMPORT_C const TDesC& EventDesc() const;




    public: // Global Event management

        /**
         * Sets the session slot ID.
         *
         * @since 3.0
         * @param aSlotId New session slot ID
         * @param aOwnershipTransfered ETrue if aSlotId ownership is
         *        transfered to CPEngSessionSlotEvent instance
         */
        IMPORT_C void SetSessionSlotId( CPEngSessionSlotId* aSlotId,
                                        TBool aOwnershipTransfered = EFalse );

        /**
         * Sets application Id
         *
         * @since 3.0
         * @param aApplicationId New application id
         */
        IMPORT_C void SetAppIdL( const TDesC& aAppId );


        /**
         * Sets session slot event
         *
         * @since 3.0
         * @param aEvent New session slot event
         */
        IMPORT_C void SetSessSltEvent( TPEngNWSessionSlotEvent aEvent );


        /**
         * Sets application session slot state
         *
         * @since 3.0
         * @param aState New application session slot state
         */
        IMPORT_C void SetAppSessSltState( TPEngNWSessionSlotState aState );


        /**
         * Sets new global session slot state
         *
         * @since 3.0
         * @param aState New global session slot state
         */
        IMPORT_C void SetGlobSessSltState( TPEngNWSessionSlotState aState );


        /**
         * Sets additional event integer identification
         *
         * @since 3.0
         * @param New additional event integer
         */
        IMPORT_C void SetEventInt( TInt aEventInt );

        /**
         * Set Event descriptor identification
         *
         * @since 3.0
         * @param New additional event descriptor
         */
        IMPORT_C void SetEventDesL( const TDesC& aEventDes );




    public: // Importing the state


        /**
         * Internalize session slot event.
         *
         * @since 3.0
         * @param aStream read stream from to internalize
         * @param aAllData If ETrue, then all data including sesssion slot id
         *        is internalized. Else only event part.
         */
        IMPORT_C void InternalizeL( RReadStream& aStream,
                                    TBool aAllData = ETrue );

        /**
         * Unpacks session slot event from
         * the descriptor buffer
         *
         * @since 3.0
         * @param Descriptor containing session slot event
         * in streamed format.
         */
        IMPORT_C void UnpackEventL( const TDesC8& aDes );


        /**
         * Unpacks session slot event's event part from
         * the descriptor buffer.
         *
         * @since 3.0
         * @param Descriptor containing session slot event
         * in streamed format. Only event part is supposed
         * to be found from the descriptor.
         */
        IMPORT_C void UnpackEventPartL( const TDesC8& aDes );



    public: // Exporting the state

        /**
         * Externalize session slot event
         *
         * @since 3.0
         * @param aStream write stream to externalize the event.
         * @param aAllData If ETrue, then all data is externalized.
         *                 Else only event part.
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream,
                                    TBool aAllData = ETrue ) const;


        /**
         * Returns size of the buffer needed for
         * streaming the session slot event instance.
         *
         * @since 3.0
         * @param aAllData If ETrue, then size is calculated for all data.
         *                 Else size is calculated only for event part.
         * @return size of the buffer in bytes
         */
        IMPORT_C TInt Size( TBool aAllData = ETrue ) const;


        /**
         * Packs full event into descriptor buffer.
         *
         * @since 3.0
         * @return Descriptor buffer containing full session
         * slot event data in streamed format. Returns
         * the buffer ownership.
         */
        IMPORT_C HBufC8* PackEventLC();

        IMPORT_C void PackEventL( RBuf8& aBuf );


        /**
         * Packs only event part of the session slot event
         * container into descriptor buffer.
         *
         * @since 3.0
         * @return Descriptor buffer containing session
         * slot event's event part data in streamed format.
         * Returns the buffer ownership.
         */
        IMPORT_C HBufC8* PackEventPartLC();


    private:    //Data

        //OWN/REF: Session Slot ID
        CPEngSessionSlotId*             iSessionId;

        //OWN: Is previous session Id owned
        TBool                           iIdentificationOwned;

        //OWN: Application Id
        HBufC*                          iApplicationId;

        //OWN: Session event
        TPEngNWSessionSlotEvent         iEvent;

        //OWN: Application Session Slot state
        TPEngNWSessionSlotState         iAppSessState;

        //OWN: Global Session Slot state
        TPEngNWSessionSlotState         iGlobSessState;

        //OWN: Event integer value
        TInt                            iEventInt;

        //OWN: Event descriptor value
        HBufC*                          iEventDesc;
    };

#endif      // __CPENGSESSIONSLOTEVENT_H__

// End of File