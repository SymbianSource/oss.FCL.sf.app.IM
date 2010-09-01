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
* Description:  RMessage2 wrapper holding general message utils.
*
*/


#ifndef __RPENGMESSAGE_H__
#define __RPENGMESSAGE_H__

//  INCLUDES
#include <E32Base.h>
#include <badesca.h>
#include "PEngInternalGlobalConsts.h"



// CLASS DECLARATION

/**
* RMessage wrapper
*
* @since 3.0
*/
class RPEngMessage
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        IMPORT_C RPEngMessage();

        /**
         * C++ constructor.
         */
        IMPORT_C RPEngMessage( const RMessage2& aMessage );

        /**
         * C++ copy constructor.
         */
        IMPORT_C RPEngMessage( const RPEngMessage& aMyMessage );

        /**
         * C++ asigment operator.
         */
        IMPORT_C RPEngMessage& operator=( const RPEngMessage& aMessage );

        /**
         * Destructor.
         */
        IMPORT_C ~RPEngMessage();




    public: // Mesage valid and completion utils

        /**
         * Tells if message is already completed or not
         *
         * @return ETrue if there is valid RMessage
         */
        IMPORT_C TBool MessageValid( void ) const ;


        /**
         * Completes the client message with the reason
         * if the message is still valid. If message isn't
         * valid anymore (it is already completed),
         * does nothing.
         *
         * @param aReason The completion reason.
         */
        IMPORT_C void Complete( TInt aReason ) const ;



    public: // Derscriptor read/write utils


        /**
         * Gets descriptor current or maximum length
         * from the client.
         */
        IMPORT_C TInt DesLength( TInt aParam ) const;
        IMPORT_C TInt DesMaxLength( TInt aParam ) const;


        /**
         * Reads one descriptor from the client.
         * 16 bit variant.
         */
        IMPORT_C HBufC16* ReadOneDescriptor16L( TInt aParam ) const;
        IMPORT_C HBufC16* ReadOneDescriptor16LC( TInt aParam ) const;
        IMPORT_C TInt ReadOneDescriptor( TInt aParam, HBufC16*& aDes  ) const;
        IMPORT_C void ReadOneDescriptorL( TInt aParam, RBuf& aDes ) const;

        /**
         * Reads one descriptor from the client.
         * 8 bit variant.
         */
        IMPORT_C HBufC8* ReadOneDescriptor8L( TInt aParam ) const;
        IMPORT_C HBufC8* ReadOneDescriptor8LC( TInt aParam ) const;
        IMPORT_C TInt ReadOneDescriptor( TInt aParam, HBufC8*& aDes  ) const;
        IMPORT_C void ReadOneDescriptorL( TInt aParam, RBuf8& aDes ) const;


        /**
         * Writes one descriptor to client side.
         * 16 bit variant.
         */
        IMPORT_C void WriteOneDescriptorL( TInt aParam,
                                           const TDesC16& aDes,
                                           TInt aOffset = 0 ) const;

        IMPORT_C TInt WriteOneDescriptor( TInt aParam,
                                          const TDesC16& aDes,
                                          TInt aOffset = 0 ) const;

        /**
         * Writes one descriptor to client side.
         * 8 bit variant.
         */
        IMPORT_C void WriteOneDescriptorL( TInt aParam,
                                           const TDesC8& aDes,
                                           TInt aOffset = 0 ) const;

        IMPORT_C TInt WriteOneDescriptor( TInt aParam,
                                          const TDesC8& aDes,
                                          TInt aOffset = 0 ) const;



    public: // Descriptor array read/write utils

        /**
         * Gets descriptor array element count
         * from the client.
         */
        IMPORT_C TInt ReadDescriptorArrayCountL( TInt aParam ) const;


        /**
         * Gets first descriptor element
         * from the client.
         */
        IMPORT_C HBufC16* ReadFirstDescriptorFromArrayL( TInt aParam ) const;


        /**
         * Reads whole descriptor array from client.
         */
        IMPORT_C void ReadDescriptorArrayL( TInt aParam,
                                            CDesCArray& aTargetArray ) const;


        /**
         * Reads whole descriptor array from client.
         */
        IMPORT_C HBufC* ReadDescriptorArrayLC(
            TInt aParam,
            CPtrCArray& aTargetArray ) const;


        /**
         * Writes whole descriptor array to client.
         */
        IMPORT_C void WriteDescriptorArrayL( TInt aParam,
                                             const MDesCArray& aSourceArray ) const;



    public: // RArray<TInt> read/write utils

        /**
         * Writes RArray<TInt> array to client.
         */
        IMPORT_C void WriteTIntArrayL( TInt aParam,
                                       RArray<TInt>& aTargetArray ) const;




    public: // Masked RMessage2 functions


        /**
         * Functions to access request parameters.
         */
        IMPORT_C TInt Int0() const;
        IMPORT_C TInt Int1() const;
        IMPORT_C TInt Int2() const;
        IMPORT_C TInt Int3() const;


        /**
         * Functions to terminate the client.
         */
        IMPORT_C void Panic( const TDesC& aCategory, TInt aReason ) const;
        IMPORT_C void Kill( TInt aReason ) const;
        IMPORT_C void Terminate( TInt aReason ) const;


        /**
         * Functions to access function and client information.
         */
        IMPORT_C TInt Function() const;
        IMPORT_C TInt Client( RThread& aClient,
                              TOwnerType aOwnerType = EOwnerProcess ) const;



    private:     // Data

        //OWN: Is message valid
        TBool           iMessageValid;

        //OWN: Real RMessage2
        RMessage2       iMessage;
    };


#endif      // __RPENGMESSAGE_H__

// End of File


