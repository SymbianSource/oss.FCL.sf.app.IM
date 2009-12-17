/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute model implementation.
*
*/

// INCLUDE FILES

#include "CPEngPresenceAttrModel.h"
#include "CPEngPresenceAttrConstructor.h"
#include "CPEngAttrModelAsyncDataSetHook.h"
#include "CPEngPresenceAttrModelStorageProxy.h"
#include "PEngAttributeRequestCodec.h"


#include "MPEngXMLSerializer.h"
#include "MPEngXMLParser.h"

#include "MPEngPresenceAttrModelTypeImp.h"


#include "PresenceDebugPrint.h"


#include <E32Base.h>
#include <S32Strm.h>




//LOCAL constants
namespace
    {
#if defined(_DEBUG)
    _LIT( KPEngAttributeModelPanicCategory, "PEngAttrModel" );
#endif

    enum TPEngAttributeModelPanics
        {
        EPEngDataIntNotSupported,
        EPEngDataDesC8NotSupported,
        EPEngDataDesC16NotSupported,
        EPEngSetDataIntNotSupported,
        EPEngSetDataDesC8NotSupported,
        EPEngSetDataDesC16NotSupported,
        EPEngSetDataDesC8AsyncNotSupported,
        EPEngSetDataDesC16AsyncNotSupported,
        EPEngInternalizeMismatch,
        };


    void PanicAttrModel( TPEngAttributeModelPanics aReason )
        {
#if defined(_DEBUG)
        User::Panic( KPEngAttributeModelPanicCategory, aReason );
#endif

        // suppress the unreferenced variable warning
        ( void ) aReason;
        }


    void PanicAttrModelL( TPEngAttributeModelPanics aReason, TInt aLeaveCode )
        {
#if defined(_DEBUG)
        PanicAttrModel( aReason );
#else
        User::Leave( aLeaveCode );
#endif

        // suppress the unreferenced variable warnings
        ( void ) aReason;
        ( void ) aLeaveCode;
        }
    }




// ============================= MEMBER FUNCTIONS =============================
// -----------------------------------------------------------------------------
// CPEngAttributeConstruct::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEngPresenceAttrModel* CPEngPresenceAttrModel::NewL(
    CPEngPresenceAttrConstructor& aTypeConstructor,
    const TDesC& aPresenceID,
    MPEngPresenceAttrModelTypeImp* aTypeImp )
    {
    CPEngPresenceAttrModel* self = new ( ELeave ) CPEngPresenceAttrModel(
        aTypeConstructor );
    CleanupStack::PushL( self );
    self->ConstructL( aPresenceID );
    CleanupStack::Pop( self );

    //TypeImp ownership is taken only in full success
    self->iTypeImp = aTypeImp;
    return self;
    }



// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEngPresenceAttrModel::CPEngPresenceAttrModel(
    CPEngPresenceAttrConstructor& aTypeConstructor )
        : iTypeConstructor( aTypeConstructor ),
        iDataSource( EPEngAttrLocalDefault ),
        iQualifier( EFalse )
    {
    iTypeConstructor.OpenRef();
    }


// Destructor
CPEngPresenceAttrModel::~CPEngPresenceAttrModel()
    {
    delete iStorage;
    delete iTypeImp;
    delete iPresenceId;
    delete iAsyncSetHook;

    iTypeConstructor.CloseRef();
    }




// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::ConstructL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::ConstructL( const TDesC& aPresenceID )
    {
    iPresenceId = aPresenceID.AllocL();
    iStorage = CPEngPresenceAttrModelStorageProxy::NewL( *this );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Type()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
TUint32 CPEngPresenceAttrModel::Type() const
    {
    return iTypeConstructor.iType;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::TypeProperties()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
const MPEngPresenceAttrTypeProperties2& CPEngPresenceAttrModel::TypeProperties() const
    {
    return iTypeConstructor;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::PresenceID()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
const TDesC& CPEngPresenceAttrModel::PresenceID() const
    {
    return *iPresenceId;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::DataSource()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
TPEngAttrDataSource CPEngPresenceAttrModel::DataSource() const
    {
    return iDataSource;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Qualifier()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
TBool CPEngPresenceAttrModel::Qualifier() const
    {
    return iQualifier;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::DataInt()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrModel::DataInt( TInt aField,
                                      TInt aGroup ) const
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::DataDesC8(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );

    TInt data = 0;
    if ( !iTypeImp->GetData( &data, NULL, NULL, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type not supported" ) );
        PanicAttrModel( EPEngDataIntNotSupported );
        return KErrNotSupported;
        }

    return data;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::DataDesC8()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
const TPtrC8 CPEngPresenceAttrModel::DataDesC8( TInt aField,
                                                TInt aGroup ) const
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::DataDesC8(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );


    TPtrC8 data( NULL, 0 );
    if ( !iTypeImp->GetData( NULL, &data, NULL, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type not supported" ) );
        PanicAttrModel( EPEngDataDesC8NotSupported );
        return KNullDesC8();
        }

    return data;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::DataDesC16()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
const TPtrC16 CPEngPresenceAttrModel::DataDesC16( TInt aField,
                                                  TInt aGroup ) const
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::DataDesC16(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );


    TPtrC16 data( NULL, 0 );
    if ( !iTypeImp->GetData( NULL, NULL, &data, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type not supported" ) );
        PanicAttrModel( EPEngDataDesC16NotSupported );
        return KNullDesC16();
        }

    return data;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetQualifier()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetQualifier( TBool aQualifier )
    {
    iQualifier = aQualifier;

    //data has been locally edited
    iDataSource = EPEngAttrLocalEdited;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetDataIntL()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetDataIntL( TInt aData,
                                          TInt aField,
                                          TInt aGroup )
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::SetDataIntL(%d, %d, %d) [%08x]" ),
             aData, aField, aGroup, iTypeConstructor.iType );

    if ( !iTypeImp->SetDataL( &aData, NULL, NULL, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type / value not supported" ) );
        PanicAttrModelL( EPEngSetDataIntNotSupported, KErrNotSupported );
        }

    iQualifier = ETrue;
    iDataSource = EPEngAttrLocalEdited;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetDataDesC8L()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetDataDesC8L( const TDesC8& aData,
                                            TInt aField,
                                            TInt aGroup )
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::SetDataDesC8L(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );

    if ( !iTypeImp->SetDataL( NULL, &aData, NULL, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type / value itself not supported" ) );
        PanicAttrModelL( EPEngSetDataDesC8NotSupported, KErrNotSupported );
        }

    iQualifier = ETrue;
    iDataSource = EPEngAttrLocalEdited;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetDataDesC16L()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetDataDesC16L( const TDesC16& aData,
                                             TInt aField,
                                             TInt aGroup )
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::SetDataDesC16L(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );

    if ( !iTypeImp->SetDataL( NULL, NULL, &aData, aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type / value not supported" ) );
        PanicAttrModelL( EPEngSetDataDesC16NotSupported, KErrNotSupported );
        }

    iQualifier = ETrue;
    iDataSource = EPEngAttrLocalEdited;
    }

// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetDataDesC8AsyncL()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetDataDesC8AsyncL( const TDesC8& aData,
                                                 TRequestStatus& aRequestStatus,
                                                 TInt aField,
                                                 TInt aGroup )
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::SetDataDesC8AsyncL(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );

    LazyInitAsyncSetHookL();

    iAsyncSetHook->InitializeLC( aRequestStatus,
                                 *this,        //Self is updated when processing done
                                 *iTypeImp );  //Real model is canceled

    if ( !iTypeImp->SetDataAsyncL( &aData, NULL, iAsyncSetHook->Status(), aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type / value not supported" ) );
        PanicAttrModelL( EPEngSetDataDesC8AsyncNotSupported, KErrNotSupported );
        }

    CleanupStack::Pop();
    iAsyncSetHook->Activate();
    }




// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::SetDataDesC16AsyncL()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::SetDataDesC16AsyncL( const TDesC16& aData,
                                                  TRequestStatus& aRequestStatus,
                                                  TInt aField,
                                                  TInt aGroup )
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::SetDataDesC16AsyncL(%d, %d) [%08x]" ),
             aField, aGroup, iTypeConstructor.iType );

    LazyInitAsyncSetHookL();

    iAsyncSetHook->InitializeLC( aRequestStatus,
                                 *this,        //Self is updated when processing done
                                 *iTypeImp );  //Real model is canceled


    if ( !iTypeImp->SetDataAsyncL( NULL, &aData, iAsyncSetHook->Status(), aField, aGroup ) )
        {
        PENG_DP( D_PENG_LIT( "Scope / value type / value not supported" ) );
        PanicAttrModelL( EPEngSetDataDesC16AsyncNotSupported, KErrNotSupported );
        }

    CleanupStack::Pop();
    iAsyncSetHook->Activate();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::CancelSetDataAsync()
// From MPEngPresenceAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::CancelSetDataAsync()
    {
    PENG_DP( D_PENG_LIT( "MPEngPresenceAttrModel2::CancelSetDataAsync() [%08x]" ),
             iTypeConstructor.iType );

    if ( iAsyncSetHook )
        {
        iAsyncSetHook->Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Extension()
// -----------------------------------------------------------------------------
//
const MPEngAttrModelExtension2* CPEngPresenceAttrModel::Extension() const
    {
    return NULL;
    }


MPEngAttrModelExtension2* CPEngPresenceAttrModel::Extension()
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::TypeExtension()
// -----------------------------------------------------------------------------
//
TAny* CPEngPresenceAttrModel::TypeExtension( TUint32 aType )
    {
    return iTypeImp->TypeExtension( aType );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Advanced()
// -----------------------------------------------------------------------------
//
const MPEngPresenceAdvancedAttrModel2* CPEngPresenceAttrModel::Advanced() const
    {
    return this;
    }

MPEngPresenceAdvancedAttrModel2* CPEngPresenceAttrModel::Advanced()
    {
    return this;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::CloneLC()
// -----------------------------------------------------------------------------
//
MPEngPresenceAttrModel2* CPEngPresenceAttrModel::CloneLC() const
    {
    MPEngPresenceAdvancedAttrModel2* clone =
        iTypeConstructor.NewAttributeInstanceLC( EFalse, PresenceID() );

    PEngAttributeRequestCodec::CopyModelDataL( *this, *clone );

    return clone;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Close()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::EncodeToXmlL()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::EncodeToXmlL( MPEngXMLSerializer& aSerializer ) const
    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrModel::EncodeValueToXmlL() [%08x]" ),
             iTypeConstructor.iType );

    TPtrC8 xmlName;
    TPtrC8 xmlNameSpace;

    User::LeaveIfError( iTypeConstructor.GetCurrentXmlNames( xmlName, xmlNameSpace ) );
    aSerializer.StartTagL( xmlName );
        {
        aSerializer.StartTagL( KQualifierXMLTag );
            {
            if ( iQualifier )
                {
                aSerializer.RawValueL( KXMLValueTrue );
                }
            else
                {
                aSerializer.RawValueL( KXMLValueFalse );
                }
            }
        aSerializer.EndTagL( KQualifierXMLTag );

        if ( iQualifier )
            {
            //Attribute qualifier is TRUE
            //==>let the derived model to serialize its data
            iTypeImp->EncodeDataToXmlL( aSerializer );
            }
        }


    aSerializer.EndTagL( xmlName );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::DecodeAttributeFromXmlL()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::DecodeFromXmlL( MPEngXMLParser& aParser,
                                             const TDesC8& aDataBlock )
    {
    PENG_DP( D_PENG_LIT( "CPEngPresenceAttrModel::DecodeValueFromXmlL() [%08x]" ),
             iTypeConstructor.iType );

    iQualifier = ETrue;
    if ( aParser.DecodeL( aDataBlock, KQualifierXMLTag, EFalse ) )
        {
        if ( aParser.ResultL().CompareF( KXMLValueFalse ) == 0 )
            {
            //Attribute qualifier is FALSE only if it is explicitly
            //stated so in the XML
            iQualifier = EFalse;
            }
        }

    if ( iQualifier )
        {
        //Attribute data is updated only if the qualifier was TRUE
        iTypeImp->DecodeDataFromXmlL( aParser, aDataBlock );
        }
    else
        {
        iTypeImp->ResetDataL();
        }

    HandleAttributeUpdatedToNetwork();
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::Storage()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
const CPEngPresenceAttrModelStorageProxy& CPEngPresenceAttrModel::Storage() const
    {
    return *iStorage;
    }

CPEngPresenceAttrModelStorageProxy& CPEngPresenceAttrModel::Storage()
    {
    return *iStorage;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::ExternalizeSize()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
TInt CPEngPresenceAttrModel::ExternalizeSize() const
    {
    TInt sizeInBytes = 14;
    // 4 bytes: extension1
    // 4 bytes: TypeId
    // 1 byte: DataSource
    // 1 byte: Qualifier
    // 4 bytes: extension2

    //And get externalize size from real model..
    sizeInBytes = sizeInBytes + iTypeImp->DataExternalizeSize();

    return sizeInBytes;
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::ExternalizeL()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( 0 ); //Write 32 bit for future extension
    aStream.WriteUint32L( iTypeConstructor.iType );
    aStream.WriteInt8L( iDataSource );
    aStream.WriteInt8L( iQualifier );
    aStream.WriteInt32L( 0 ); //Write 32 bit for future extension

    //let model implementation externalize its data..
    iTypeImp->ExternalizeDataL( aStream );
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::InternalizeL()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::InternalizeL( RReadStream& aStream )
    {
    aStream.ReadUint32L(); //future extension
    TUint32 interType = aStream.ReadUint32L();
    iDataSource = static_cast<TPEngAttrDataSource> ( aStream.ReadInt8L() );
    iQualifier = aStream.ReadInt8L();
    aStream.ReadUint32L(); //future extension

    // Check from attribute type that correct type is internalized.
    __ASSERT_ALWAYS( interType == iTypeConstructor.iType,
                     PanicAttrModelL( EPEngInternalizeMismatch, KErrCorrupt ) );

    //Try internalize type specific data.
    //Reset contents if corrupted
    TRAPD( err, iTypeImp->InternalizeDataL( aStream ) );
    if ( ( err == KErrCorrupt ) ||
         ( err == KErrEof ) )
        {
        iTypeImp->ResetDataL();
        }
    else
        {
        User::LeaveIfError( err );
        }
    }


// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::HandleAttributeUpdatedToNetwork()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::HandleAttributeUpdatedToNetwork()
    {
    iDataSource = EPEngAttrNetwork;
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::HandleNetworkSessionStateChangeL()
// from MPEngPresenceAdvancedAttrModel
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::HandleNetworkSessionStateChangeL( TBool aSessionOpen )
    {
    iTypeImp->HandleNetworkSessionStateChangeL( aSessionOpen );
    }



// -----------------------------------------------------------------------------
// CPEngPresenceAttrModel::LazyInitAsyncSetHookL()
// -----------------------------------------------------------------------------
//
void CPEngPresenceAttrModel::LazyInitAsyncSetHookL()
    {
    if ( !iAsyncSetHook )
        {
        iAsyncSetHook = new ( ELeave ) CPEngAttrModelAsyncDataSetHook();
        }
    }




//  End of File


