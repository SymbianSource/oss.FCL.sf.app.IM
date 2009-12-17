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
* Description: 
* classes for IMPS encoder.
*
*/


#ifndef MImpsMessageInterpreterApi_H
#define MImpsMessageInterpreterApi_H

// FORWARD DECLARATIONS
class MImpsDataAccessor;
class CBufFlat;
class MImpsCSPSession;

// CLASS DECLARATION

/**
*  Abstract Encoder class
*/
class MImpsEncoder
    {

    public:

        /**
        * Method builds (wb)xml encoded message based on data
        * included in class aImpsData
        * @param aImpsData reference to MImpsDataAccessor class.
        * @param aEncodeBuffer buffer to hold the encoded message,
        */
        virtual void EncodeMessageL(
            MImpsDataAccessor& aImpsData,
            CBufFlat& aEncodeBuffer ) = 0;

        /**
        * Destructor
        */
        virtual void Destroy() = 0;

        /**
        * MIME type supported
        * @return MIME type
        */
        virtual TPtrC8 MimeType() = 0;

    };

/**
*  Abstract Decoder class
*/
class MImpsDecoder
    {

    public: // New functions

        /**
        * Method decodes (wb)xml message to internal data structures and stores
        * it in aImpsData
        * @param aImpsData reference to MImpsDataAccessor class.
        * @param pointer to aEncodeBuffer buffer to hold the encoded message
        *        Please note that the buffer ownership is taken.
        *        It is taken in such way that the aEncodeBuffer will point to
        *        NULL if the transfer was successful.
        */
        virtual void DecodeMessageL(
            MImpsDataAccessor& aImpsData,
            HBufC8** aEncodeBuffer ) = 0;

        /**
        * Method decodes (wb)xml message to internal data structures
        * in asynchronous manner. The completion of the operation is signaled in
        * MImpsCSPSession::TransportResponseParsedL() method.
        * @param aEncodeBuffer buffer to hold the encoded message
        *        Please note that the buffer ownership is taken!!!
        * @return operation ID
        */
        virtual TInt DecodeMsgAsyncL(
            HBufC8** aBuffer ) = 0;

        /**
        * Method to cancel the asynchronous decoding
        */
        virtual void CancelDecoding() = 0;

        /**
        * Destructor
        */
        virtual void Destroy() = 0;

    };

/**
* This creates the actual message decoder instance.
* This replaces dynamic DLL loading because of
* static loading is a little bit faster.
* @return a pointer to the actual C-class instance.
*/
IMPORT_C GLDEF_C MImpsDecoder* NewDecoderL( MImpsCSPSession* aCallback );

/**
* This creates the actual message encoder instance.
* This replaces dynamic DLL loading because of
* static loading is a little bit faster.
* @return a pointer to the actual C-class instance
*/
IMPORT_C GLDEF_C MImpsEncoder* NewEncoderL( );

#endif

// End of File
