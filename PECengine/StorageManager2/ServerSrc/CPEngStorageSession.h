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
* Description:  See class description below.
*
*/


#ifndef __CPENGSTORAGESESSION_H__
#define __CPENGSTORAGESESSION_H__

//  INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class	MPEngStorageServer;
class	RPEngMessage;
class	CPEngStorageSubSession;
class	CPEngAdminSubSession;

/**
 *  Main session class of the storage server
 *	@since 3.0
 */
class   CPEngStorageSession
            : public CSession2
    {

    public:		// Constructors and destructors
        /**
         * Two-phased constructor.
         */
        static CPEngStorageSession* NewL( MPEngStorageServer& aServer );

        /**
         * Two-phased constructor.
         */
        static CPEngStorageSession* NewLC( MPEngStorageServer& aServer );

        /**
         * Destructor.
         */
        virtual ~CPEngStorageSession();

    protected:	// Constructors and destructors

        /**
          * C++ default constructor.
          */
        CPEngStorageSession( MPEngStorageServer& aServer );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:	// New methods

        /**
         * Handles the message
         *	@since 3.0
         * @param aMessage is received message.
         */
        void DispatchMessageL( const RPEngMessage& aMessage );

        /**
         *	Create new Folder sub-session
         *	@since 3.0
         *	@param aMessage, message to write back handle of the new Sub session
         */
        void CreateNewFolderSubSessionL( const RPEngMessage& aMessage );

        /**
         *	Create new Admin sub-session
         *	@since 3.0
         *	@param aMessage, message to write back handle of the new Sub session
         */
        void CreateNewAdminSubSessionL( const RPEngMessage& aMessage );


        /**
         *	Remove sub-session
         *	@since 3.0
         *	@param	aHandle handle of the sub-session
         *	Function leaves if there is no such a sub-session
         */
        void RemoveSubSessionL( TUint  aHandle );

        /**
         *	Find Folder Sub-session
         *	@since 3.0
         *	@param	aHandle handle of the sub-session to find
         *	@return Folder sub-session
         */
        CPEngStorageSubSession* FindFolderSubSessionL(
            TUint aHandle );

        /**
         *  Find Admin Sub-session
         *
         *  @since 3.0
         *	@param	aHandle handle of the sub-session to find
          *	@return Admin sub-session
         */
        CPEngAdminSubSession* FindAdminSubSessionL(
            TUint aHandle );

        /*
         *	Client is panicked using this method.
         *	@since 3.0
         *  @param	aMessage clients message
         *  @param	aPanic panic reason
         */
        void PanicClient( const RPEngMessage& aMessage, const TInt aPanic ) const;

    public:		// Methods derived from CSession

        /**
         *  Handles the servicing of client requests to the server.
         *  @since 3.0
         *  @param aMessage is received message.
         */
        void ServiceL( const RMessage2 &aMessage );

        /**
         *  Handles the servicing of the error case
         *  @since 3.0
         *  @param aMessage is received message.
         *  @param aError error
         */
        void ServiceError( const RMessage2& aMessage, TInt aError );

    private:	// Data

        /// OWN: Admin container indx
        CObjectIx*							iSubSessions;

        /// REF: Storage server
        MPEngStorageServer&					iStorageServer;

        /// OWN: Is procesed message completed
        TBool								iCompleteMessage;

    };

#endif      //  __CPENGSTORAGESESSION_H__
