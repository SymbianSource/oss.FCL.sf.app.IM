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

//  INCLUDES
#include "RPEngMessage.h"
#include "PEngMessagePacker.h"


// MACROS
/*
#define __CHECK_CLIENT_PART_\
	RThread client;\
	CleanupClosePushL( client );\
	iMessage.ClientL( client );\
	TInt exitType( client.ExitType() );\
	CleanupStack::PopAndDestroy();\
	if ( exitType != EExitPending )\
	    {\
		return;\
	    }

#define __CHECK_CLIENT_PART_INT\
	RThread client;\
	CleanupClosePushL( client );\
	iMessage.ClientL( client );\
	TInt exitType( client.ExitType() );\
	CleanupStack::PopAndDestroy();\
	if ( exitType != EExitPending )\
	    {\
		return EFalse;\
	    }

#define __CHECK_CLIENT_PART_LEAVE\
	RThread client;\
	CleanupClosePushL( client );\
	iMessage.ClientL( client );\
	TInt exitType( client.ExitType() );\
	if ( exitType != EExitPending )\
	    {\
		User::Leave( KErrGeneral );\
	    }\
	CleanupStack::PopAndDestroy();
*/
#define __CHECK_CLIENT_PART_
#define __CHECK_CLIENT_PART_INT
#define __CHECK_CLIENT_PART_LEAVE


// ====================== MEMBER FUNCTIONS ======================================
// -----------------------------------------------------------------------------
// RPEngMessage::RPEngMessage()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngMessage::RPEngMessage()
        : iMessageValid( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// RPEngMessage::RPEngMessage
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngMessage::RPEngMessage( const RMessage2& aMessage )
        : iMessageValid( ETrue ),
        iMessage( aMessage )
    {
    }


// -----------------------------------------------------------------------------
// RPEngMessage::RPEngMessage()
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngMessage::RPEngMessage( const RPEngMessage& aMyMessage )
        : iMessageValid( aMyMessage.iMessageValid ),
        iMessage( aMyMessage.iMessage )
    {
    }


// -----------------------------------------------------------------------------
// RPEngMessage::RPEngMessage()
// C++ assignment operator can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPEngMessage& RPEngMessage::operator=(
    const RPEngMessage & aMessage )
    {
    iMessageValid = ETrue;
    iMessage = aMessage.iMessage;
    return( *this );
    }


// Destructor
EXPORT_C RPEngMessage::~RPEngMessage()
    {
    }



// =============================================================================
// =================== Message compete / validation ============================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngMessage::MessageValid()
// Check if client message is still valid
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RPEngMessage::MessageValid() const
    {
    if ( !iMessageValid )
        {
        return iMessageValid;
        }
    __CHECK_CLIENT_PART_INT;
    return iMessageValid;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Complete()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::Complete( TInt aReason ) const
    {
    if ( iMessageValid )
        {
        __CHECK_CLIENT_PART_;
        iMessage.Complete( aReason );
        }

    // Once message is completed,
    // Set flag to empty ==> multiple completing of message do not harm
    ( const_cast<RPEngMessage*>( this ) )->iMessageValid = EFalse;
    }




// =============================================================================
// ================== General message read/write utils =========================
// =============================================================================


// -----------------------------------------------------------------------------
// RPEngMessage::DesLength()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::DesLength( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_INT;
    return iMessage.GetDesLength( aParam );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::DesMaxLength()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::DesMaxLength( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_INT;
    return iMessage.GetDesMaxLength( aParam );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor16L()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* RPEngMessage::ReadOneDescriptor16L( TInt aParam ) const
    {
    HBufC16* desBuf = ReadOneDescriptor16LC( aParam );
    CleanupStack::Pop(); // desBuf
    return desBuf;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor16L()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* RPEngMessage::ReadOneDescriptor16LC( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    if ( length < 0 )
        {
        User::Leave( length );
        }
    HBufC16* desBuf = HBufC16::NewLC( length );
    TPtr des = desBuf->Des();
    iMessage.ReadL( aParam, des );
    return desBuf;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::ReadOneDescriptor( TInt aParam,
                                               HBufC16*& aDes  ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    if ( length < KErrNone )
        {
        return length;
        }

    aDes = HBufC16::New( length );
    if ( !aDes )
        {
        return KErrNoMemory;
        }

    TPtr des = aDes->Des();
    return iMessage.Read( aParam, des );
    }

// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::ReadOneDescriptorL( TInt aParam,
                                                RBuf& aDes ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    User::LeaveIfError( length );
    if ( length > aDes.MaxLength() )
        {
        aDes.ReAllocL( length );
        }
    aDes.Zero();
    return iMessage.ReadL( aParam, aDes );
    }

// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor8L()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RPEngMessage::ReadOneDescriptor8L( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    HBufC8* desBuf = ReadOneDescriptor8LC( aParam );
    CleanupStack::Pop( desBuf );
    return desBuf;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor8LC()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RPEngMessage::ReadOneDescriptor8LC( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    if ( length < 0 )
        {
        User::Leave( length );
        }

    HBufC8* desBuf = HBufC8::NewLC( length );
    TPtr8 des = desBuf->Des();
    iMessage.ReadL( aParam, des );

    return desBuf;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::ReadOneDescriptor( TInt aParam,
                                               HBufC8*& aDes ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    if ( length < KErrNone )
        {
        return length;
        }

    aDes = HBufC8::New( length );
    if ( !aDes )
        {
        return KErrNoMemory;
        }

    TPtr8 des = aDes->Des();
    return iMessage.Read( aParam, des );
    }

// -----------------------------------------------------------------------------
// RPEngMessage::ReadOneDescriptor()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::ReadOneDescriptorL( TInt aParam,
                                                RBuf8& aDes ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    TInt length( DesLength( aParam ) );
    User::LeaveIfError( length );
    if ( length > aDes.MaxLength() )
        {
        aDes.ReAllocL( length );
        }
    aDes.Zero();
    return iMessage.ReadL( aParam, aDes );
    }

// -----------------------------------------------------------------------------
// RPEngMessage::WriteOneDescriptorL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::WriteOneDescriptorL( TInt aParam,
                                                 const TDesC16& aDes,
                                                 TInt aOffset /* 0 */ ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    iMessage.WriteL( aParam, aDes, aOffset );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::WriteOneDescriptor()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::WriteOneDescriptor( TInt aParam,
                                                const TDesC16& aDes,
                                                TInt aOffset /* 0 */ ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    return iMessage.Write( aParam, aDes, aOffset );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::WriteOneDescriptorL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::WriteOneDescriptorL( TInt aParam,
                                                 const TDesC8& aDes,
                                                 TInt aOffset /* 0 */ ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    iMessage.WriteL( aParam, aDes, aOffset );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::WriteOneDescriptor(
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::WriteOneDescriptor( TInt aParam,
                                                const TDesC8& aDes,
                                                TInt aOffset /* 0 */ ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    return iMessage.Write( aParam, aDes, aOffset );
    }





// =============================================================================
// =========== message read/write descriptor array functions ===================
// =============================================================================


// -----------------------------------------------------------------------------
// RPEngMessage::ReadDescriptorArrayCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::ReadDescriptorArrayCountL( TInt aParam ) const
    {
    __CHECK_CLIENT_PART_LEAVE;
    // read just first element where is stored info about the count
    TBuf<1> buf;
    iMessage.ReadL( aParam, buf );
    return PEngMessagePacker::DesArrayCount( buf );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadFirstDescriptorFromArrayL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC16* RPEngMessage::ReadFirstDescriptorFromArrayL( TInt aParam ) const
    {
    HBufC16* desBuf = ReadOneDescriptor16LC( aParam );
    HBufC16* ret = PEngMessagePacker::UnpackFirstDesFromArrayL( *desBuf );
    CleanupStack::PopAndDestroy(); // desBuf
    return ret;
    }


// -----------------------------------------------------------------------------
// RPEngMessage::ReadDescriptorArrayL
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::ReadDescriptorArrayL(
    TInt aParam,
    CDesCArray& aTargetArray ) const
    {
    HBufC16* desBuf = ReadOneDescriptor16LC( aParam );
    PEngMessagePacker::UnpackDesArrayL( *desBuf, aTargetArray );
    CleanupStack::PopAndDestroy(); // desBuf
    }

// -----------------------------------------------------------------------------
// RPEngMessage::ReadDescriptorArrayL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* RPEngMessage::ReadDescriptorArrayLC(
    TInt aParam,
    CPtrCArray& aTargetArray ) const
    {
    HBufC16* desBuf = ReadOneDescriptor16LC( aParam );
    PEngMessagePacker::UnpackDesArrayL( *desBuf, aTargetArray );
    return desBuf;
    }

// -----------------------------------------------------------------------------
// RPEngMessage::WriteDescriptorArrayL(
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::WriteDescriptorArrayL(
    TInt aParam,
    const MDesCArray& aSourceArray ) const
    {
    HBufC16* bufDes = PEngMessagePacker::PackDesArrayL( aSourceArray );
    CleanupStack::PushL( bufDes );
    WriteOneDescriptorL( aParam, *bufDes );
    CleanupStack::PopAndDestroy( ); //bufDes
    }




// =============================================================================
// =========== message read/write RArray<TInt> functions =======================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngMessage::WriteTIntArrayL
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::WriteTIntArrayL(
    TInt aParam,
    RArray<TInt>& aTargetArray ) const
    {
    HBufC16* desBuf = PEngMessagePacker::PackTIntArrayL( aTargetArray );
    CleanupStack::PushL( desBuf );
    WriteOneDescriptorL( aParam, *desBuf );
    CleanupStack::PopAndDestroy( ); // desBuf
    }



// =============================================================================
// =========== RMessage2 take over functions ===================================
// =============================================================================

// -----------------------------------------------------------------------------
// RPEngMessage::Int0()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Int0() const
    {
    return iMessage.Int0();
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Int1()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Int1() const
    {
    return iMessage.Int1();
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Int2()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Int2() const
    {
    return iMessage.Int2();
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Int3()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Int3() const
    {
    return iMessage.Int3();
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Panic()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::Panic( const TDesC& aCategory,
                                   TInt aReason ) const
    {
    iMessage.Panic( aCategory, aReason );
    }

// -----------------------------------------------------------------------------
// RPEngMessage::Kill()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::Kill( TInt aReason ) const
    {
    iMessage.Kill( aReason );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Terminate()
// -----------------------------------------------------------------------------
//
EXPORT_C void RPEngMessage::Terminate( TInt aReason ) const
    {
    iMessage.Terminate( aReason );
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Function()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Function() const
    {
    return iMessage.Function();
    }


// -----------------------------------------------------------------------------
// RPEngMessage::Client()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPEngMessage::Client( RThread& aClient,
                                    TOwnerType aOwnerType ) const
    {
    return iMessage.Client( aClient, aOwnerType );
    }


// End of File


