/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for getting read access to history files
*
*/


#ifndef __MCALOGGERREADINTERFACE_H__
#define __MCALOGGERREADINTERFACE_H__

//	FORWARD CLASS DECLERATIONS

class MCALoggerMessageFactory;
class MCALoggerMessageHeader;
class MCALoggerMessage;

// CLASS DECLARATION

/**
 *  Interface for getting read access to history files
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class MCALoggerReadInterface
    {

    protected:

        /**
         * Destructor.
         */
        virtual ~MCALoggerReadInterface() {};

    public: // Interface

        /**
         *	Client gets pointer to message header array trough parameter.
         *	@since 2.1
         *	@param aHeaderArray Pointer to message header array is set to this
         *						parameter. Ownership of array does not change.
         */
        virtual void GetLoggerMessageHeaderArray(
            RPointerArray< MCALoggerMessageHeader >*& aHeaderArray ) = 0;

        /**
         *	Client asks logger to create messages to array concerning one header
         *	Leaves with KErrNotFound if history file does not exists.
         *	Leaves if history file read fails.
         *	@since 2.1
         *	@param aMessageArray Client provides array for messages read from
         *			file. Messages are appended to array.
         *	@param aKey File to be read is recognized by header. Pointer to
         *				aKey reference is used for task.
         */
        virtual void CreateLoggerMessagesL(
            RPointerArray< MCALoggerMessage >& aMessageArray,
            MCALoggerMessageHeader& aKey ) = 0;

        /**
         *	Delete one history file recognized by header.
         *	Leaves if MCALoggerMessageHeader::FileNameL fails.
         *	WARNING: header passed will be deleted and removed from header
         *	array. Array is same that client has been given to with
         *	GetLoggerMessageHeaderArray so removal will be seen directly in
         *	client side too.
         *	@since 2.1
         *	@param aKey File to be read is recognized by header. Pointer to
         *				aKey reference is used for task.
         */
        virtual void DeleteHistoryFileL( MCALoggerMessageHeader& aKey  ) = 0;

        /**
         *	Set message factory. Factory is used to create messages and message
         *	headers for logger and client use.
         *	@since 2.1
         *	@param aMessageFactory Pointer to message factory. Pass NULL to
         *							reset messageFactory.
         */
        virtual void SetLoggerMessageFactory(
            MCALoggerMessageFactory* aMessageFactory ) = 0;
    };

#endif      // __MCALOGGERREADINTERFACE_H__

// End of File