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
* Description:  Imps engine wbxml message decoder
*
*
*/


// INCLUDE FILES
#include    <apparc.h>
#include    <s32mem.h>
#include    <utf.h>
#include    <e32std.h>

#include    "ImpsDecodewbxml.h"
#include    "Imps_1_1_Tokens.h"
#include    "Imps_1_2_Tokens.h"
#include    "ImpsDataAccessorApi.h"
#include    "ImpsErrors.h"
#include    "ImpsWbXmlCommon.h"
#include    "ImpsVariantAPI.h"
#include    "impsutils.h"
#include    "ImpsFields.h"
#include    "ImpsDataUtils.h"

//LITERALS
// TODO: This could be changed so that the NS form the message itself is used!
// There is no simple solution for proper attribute handling because of the callback
// interface logic!!!

_LIT8( KAttr11, " xmlns=\"http://www.wireless-village.org/TRC1.1\"");
_LIT8( KAttr12, " xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"");
_LIT8( KPresSubListAttr11, " xmlns=\"http://www.wireless-village.org/PA1.1\"");
_LIT8( KPresSubListAttr12,  " xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"");

// Maximum length of the element in DTD
const TInt KDtdElementMaxLength = 100;

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// TagStart
// Callback function for handling tag begin
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t TagStart(NW_WBXML_Parser_t* aParser, void* aContext)
    {

    if(aParser)
    {
        ((CImpsDecodeWbXml*)aContext)->SetState(EImpsDecodeStateTagStart);
    }
    return NW_STAT_SUCCESS;
    }

// ---------------------------------------------------------
// AttributeStart
// Callback function for handling attribute begin
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t AttributeStart(NW_WBXML_Parser_t* aParser, void* aContext)
    {
 
    if(aParser)
    {
        ((CImpsDecodeWbXml*)aContext)->SetState(EImpsDecodeStateAttributeStart);
    }
    return NW_STAT_SUCCESS;
    }

// ---------------------------------------------------------
// Binary
// Callback function for handling WBXML binary data
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t Binary(NW_WBXML_Parser_t* aParser,TUint32 aValue, void* aContext)
    {
    
    if(aParser)
    {
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->StoreTokenL(aValue));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    else
    {
        return NW_STAT_FAILURE;
    }

    }

// ---------------------------------------------------------
// FQTokenL
// Callback function for handling WBXML tokens
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t FQTokenL(NW_WBXML_Parser_t* aParser,TUint32 aFqtoken, void* aContext)
    {

    if(aParser)
    {
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->StoreTokenL(aFqtoken));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    return NW_STAT_FAILURE;
    }

// ---------------------------------------------------------
// InlineStringL
// Callback function for handling WBXML inline strings
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t InlineStringL(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext)
    {
    
    if(aParser)
    {
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->StoreInlineStringL(aParser,aLength));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    
    return NW_STAT_SUCCESS;
    }

// ---------------------------------------------------------
// TableStringL
// Callback function for handling WBXML string table strings
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t TableStringL(NW_WBXML_Parser_t* aParser,TUint32 aIndex, void* aContext)
    {
    
    if(aParser)
    {
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->StoreTableStringL(aIndex));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    else
    {
        return NW_STAT_FAILURE;
    }
    }

// ---------------------------------------------------------
// Extension
// Callback function for handling WBXML extension tokens
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t Extension(NW_WBXML_Parser_t* aParser,void* aContext)
    {
    
    if(aParser)
    {
        ((CImpsDecodeWbXml*)aContext)->SetState(EImpsDecodeStateExtension);
    }
    
    return NW_STAT_SUCCESS;
    }

// ---------------------------------------------------------
// Opaque
// Callback function for handling WBXML opaque data
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t Opaque(NW_WBXML_Parser_t* aParser,TUint32 aLength, void* aContext)
    {
    
    return(((CImpsDecodeWbXml*)aContext)->StoreOpaqueData(aParser,aLength));
    
    }

// ---------------------------------------------------------
// BinaryL
// Callback function for handling WBXML binary data
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t BinaryL(NW_WBXML_Parser_t* aParser,TUint32 aValue, void* aContext)
    {
    
    if(aParser)
    {
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->StoreTokenL(aValue));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    else
    {
        return NW_STAT_FAILURE;
    }
    
    }

// ---------------------------------------------------------
// StartDocument
// Callback function for handling wbxml document header information
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//

NW_Status_t StartDocument(NW_WBXML_Parser_t* aParser,
                          NW_WBXML_Document_t* aDocument, void* aContext)
    {
    
    if(aParser)
    {
        return(((CImpsDecodeWbXml*)aContext)->StoreDocument(aDocument));
    }
    return NW_STAT_SUCCESS;

    }

// ---------------------------------------------------------
// AttributeValue
// Callback function for handling attribute value
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t AttributeValue(NW_WBXML_Parser_t* aParser, void* aContext)
    {
    
    if(aParser)
    {
        ((CImpsDecodeWbXml*)aContext)->SetState(EImpsDecodeStateAttributeValue);
    }
    
    return NW_STAT_SUCCESS;
    }

// ---------------------------------------------------------
// TagEndL
// Callback function for handling tag end
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//

NW_Status_t TagEndL(NW_WBXML_Parser_t* aParser, void* aContext)
    {
    
    if(aParser)
    {   
        NW_Status_t rcode = NW_STAT_SUCCESS;
        TRAPD(error,rcode = ((CImpsDecodeWbXml*)aContext)->TagEndHandleL( ));
        if(error != KErrNone)
        {
            return NW_STAT_FAILURE;
        }
        else
        {
            return rcode;
        }
    }
    else
    {
        return NW_STAT_FAILURE;
    }
    
    }

// ---------------------------------------------------------
// Content
// Callback function for handling WBXML content
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t Content(NW_WBXML_Parser_t* aParser, void* aContext)
    {

    if(aParser)
    {
        ((CImpsDecodeWbXml*)aContext)->SetState(EImpsDecodeStateContent);
    }
    
    return NW_STAT_SUCCESS;
    
    }

// ---------------------------------------------------------
// EntityL
// Callback function for handling WBXML entity
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t EntityL(NW_WBXML_Parser_t* aParser, TUint32 aValue, void* aContext)
    {
    
    if(aParser)
    {
        if(aValue)
        {
            TRAPD(error, ((CImpsDecodeWbXml*)aContext)->DecodeEntityL(aValue));
            if(error != KErrNone)
            {
                return NW_STAT_FAILURE;
            }
        }
    }
    
    return NW_STAT_SUCCESS;
    
    }

// ---------------------------------------------------------
// Exception
// Callback function for handling WBXML parser exceptions
// Returns: NW_Status_t indicates success
// ---------------------------------------------------------
//
NW_Status_t Exception(NW_WBXML_Parser_t* aParser,NW_WBXML_Exception_t aException, void* aContext)
    {
    
    if(aParser)
    {
    
        ((CImpsDecodeWbXml*)aContext)->ExceptionHandle(aException);
    }
    
    return NW_STAT_FAILURE;
    
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsDecodeWbXml::CImpsDecodeWbXml():
    iDecodeToXml(EFalse),
    iTransactions(0),
    iTypeStatus(EFalse),
    iPureMessageType( EImpsMessageNone )
    {
    
    }

// default constructor can leave.
void CImpsDecodeWbXml::ConstructL()
    {
        // create buffers for data handling
    
    iStoreValue8   = HBufC8::NewL(KImpsWbXmlMaxStringLength);
    iElementValue  = HBufC8::NewL(KImpsWbXmlMaxStringLength);
    iStoreValue    = HBufC::NewL(KImpsWbXmlMaxStringLength);

    // create handler for wbxml data
    iWbXmlData = CImpsWbXmlData::NewL( );

    // PEC in use ?
    CImpsVariant* dpb = CImpsVariant::NewLC();
    if (dpb->IsFeatureSupportedL(EDpb))
    {
        iXmlUtils = CImpsXmlUtils::NewL( );
        iPEC = ETrue;
    }
    else
    {
        iXmlUtils = NULL;
        iPEC = EFalse;
    }
    CleanupStack::PopAndDestroy( );

    iCurrentDictionary = iDictionaryCount = 0;
    iDictionaries[iDictionaryCount++] = 
        (NW_WBXML_Dictionary_s*)&NW_Imps_1_1_WBXMLDictionary;
    iDictionaries[iDictionaryCount++] = 
        (NW_WBXML_Dictionary_s*)&NW_Imps_1_2_WBXMLDictionary;
    if(NW_WBXML_Dictionary_initialize (iDictionaryCount,iDictionaries) != NW_STAT_SUCCESS)
        {
        User::Leave(KImpsErrorEncode);
        }


    iStack = new(ELeave) CStack<NW_String_UCS2Buff_t, EFalse>();
    }

// Two-phased constructor.
CImpsDecodeWbXml* CImpsDecodeWbXml::NewL( MImpsCSPSession* /*aCallback*/ )
    {

    CImpsDecodeWbXml* self = new (ELeave) CImpsDecodeWbXml;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CImpsDecodeWbXml::~CImpsDecodeWbXml()
    {
    
    delete iWbXmlData;
    delete iStoreValue;
    delete iStoreValue8;
    delete iElementValue;
    delete iXmlUtils;
    delete iStack;

    NW_WBXML_Dictionary_destroy(); 
    }

// ---------------------------------------------------------
// CImpsDecodeXml::DecodeMsgAsyncL
// ---------------------------------------------------------
//
TInt CImpsDecodeWbXml::DecodeMsgAsyncL( HBufC8** aBuffer )
    {

    // TODO: implement it!
    __ASSERT_DEBUG(aBuffer, User::Panic(KImpsPanicCategory, KErrArgument));
    delete *aBuffer;
    *aBuffer = NULL; // indicate that the ownerships was taken
    return 0;
    }

// ---------------------------------------------------------
// CImpsDecodeXml::DecodeMessageL
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::CancelDecoding()
    {
    // TODO: implement it!
//    TTime finished;
//    finished.HomeTime();
//    TReal sec = finished.MicroSecondsFrom(started).Int64()/1000000.0;
    }


// ---------------------------------------------------------
// CImpsDecodeXml::DecodeMessageL
// IMPS engine WBXML message decoder main function and API for user
// aImpsData provides data storage for parsed out values
// aDecodeBuffer contains message to be decoded
// if error occurs method leaves with error code
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::DecodeMessageL(
    MImpsDataAccessor& aImpsData,
    HBufC8**  aDecodeBuffer )
    {

    __ASSERT_DEBUG(aDecodeBuffer, User::Panic(KImpsPanicCategory, KErrArgument));
    HBufC8* decodeBuffer = *aDecodeBuffer;
    *aDecodeBuffer = NULL; // indicate that the ownership was taken;
    CleanupStack::PushL(decodeBuffer); // take the message buffer
    iImpsData = &aImpsData;
        
    // initializing the XML decoder states
    iTransactions = 0;
    iDecodeToXml = EFalse;
    iTypeStatus = EFalse;
    iStack->Reset();
    //////////////////////////////////////
   

    // CREATE AND INITIALIZE WBXML PARSER
    NW_WBXML_Parser_t WBXML_parser;
    NW_WBXML_Parser_newInPlace(&WBXML_parser);
    WBXML_parser.ext_t_not_table_index = NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER;
    WBXML_parser.recursiveCallCnt = 0;

    // CREATE EVENT HANDLER AND SET EVENT HANDLER CALLBACK
    // FUNCTION POINTERS
    NW_WBXML_EventHandler_t WBXML_event_handler;
    WBXML_event_handler.StartDocument_CB = StartDocument;
    WBXML_event_handler.EndDocument_CB = NULL;
    WBXML_event_handler.Pi_CB = NULL;
    WBXML_event_handler.Tag_Start_CB = TagStart;
    WBXML_event_handler.Tag_End_CB = TagEndL;
    WBXML_event_handler.Attr_Start_CB = AttributeStart;
    WBXML_event_handler.Attr_Val_CB = AttributeValue;
    WBXML_event_handler.Content_CB = Content;
    WBXML_event_handler.CodePage_CB = NULL;
    WBXML_event_handler.Extension_CB = Extension;
    WBXML_event_handler.FQToken_CB = FQTokenL;
    WBXML_event_handler.InlineString_CB = InlineStringL;
    WBXML_event_handler.TableString_CB = TableStringL;
    WBXML_event_handler.Opaque_CB = Opaque;
    WBXML_event_handler.Binary_CB = BinaryL;
    WBXML_event_handler.Entity_CB = EntityL;
    WBXML_event_handler.Exception_CB = Exception;

        // REGISTER EVENT HANDLER TO PARSER
    NW_Status_t status = NW_WBXML_Parser_registerHandler(&WBXML_parser,&WBXML_event_handler,(void*)this);
    if(status != NW_STAT_SUCCESS)
    {
        User::Leave(KImpsErrorDecode);
    }

    iNameSpace = KImpsNameSpaceCspMessage;
    iPreviousNameSpace = KImpsNameSpaceCspMessage;

    // PREPARE PARSER AND START PARSING
//    iVersion.Copy(KImpsMessageVersion);
    NW_WBXML_Document_t document;
//    document.doc_type = NULL;
    NW_WBXML_Document_construct(&document,KImpsWvCsp11PublicIdentifier);
    TUint32 size = decodeBuffer->Size();
//    iDecodeBuffer->ResizeL(size+1);
    TPtr8 p = decodeBuffer->Des();
//    p.SetLength(size);
    const unsigned char *buffer = decodeBuffer->Ptr();

    // GET DATA ACCESS KEY FROM DATA STORAGE
    iAccessKey = iImpsData->NewKeyL();

    // change wbxml public document identifier for IOP purposes
    // Assume that Unknown ID is CSP 1.1
    if(p[KImpsWbXmlPublicID] == KImpsWbXmlPublicIdUnknown)
    {
        p[KImpsWbXmlPublicID] = KImpsWvCsp11PublicIdentifier;
    }

    // START PARSER
    iError = KErrNone;
    iDecodingState = EImpsDecodeStateContent;
    status = NW_WBXML_Parser_parseBuffer(&WBXML_parser,&document,(unsigned char*)buffer,size);

    NW_WBXML_Document_destruct(&document);
    iAccessKey->Destroy( );

    if(status != NW_STAT_SUCCESS)
    {
        if(iError == KErrNone)
        {
            User::Leave(KImpsErrorDecode);
        }
        else
        {
            User::Leave(iError);
        }
    }

    CleanupStack::PopAndDestroy(); // message buffer
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::SetState
// method updates current parser state
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::SetState(TImpsDecodeState aState)
    {
    

    iDecodingState = aState;
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreDocument
// method verifies document header information
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::StoreDocument(NW_WBXML_Document_t* aDocument)
    {
    
    if(aDocument->version !=  KImpsWbXmlVersion)
    {
        return NW_STAT_FAILURE;
    }
    if(aDocument->charset !=  KImpsDefaultCharset)
    {
        return NW_STAT_FAILURE;
    }

    if(aDocument->publicid==KImpsWvCsp12PublicIdentifier)
        {
        iImpsData->GetImpsFields()->SetCspVersion(EImpsCspVersion12);
        }
    else // the default CSP version is 1.1
        {
        iImpsData->GetImpsFields()->SetCspVersion(EImpsCspVersion11);
        }

    iDocument = aDocument;
    return NW_STAT_SUCCESS;

    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::ExceptionHandle
// method for parser exception handling
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::ExceptionHandle(NW_WBXML_Exception_t aException)
    {
    
    iError = (TInt)aException;

    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::TagEndHandleL
// method stores parsed out data to data accessor and removes
// token from data accessor key
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::TagEndHandleL( )
    {
    
    //////////////////////////////////////////////////////////////////////////
    if (iDecodeToXml)
        {
        if ( !iStack->IsEmpty() )
            {
            NW_String_UCS2Buff_t* nw_str = iStack->Pop();
            TBuf8<KDtdElementMaxLength> elementString;
            ConvertToDes8((TUint16*)nw_str->bytes, elementString);
            iXmlUtils->EndElementL(elementString);
            }
        else
            {
            // we are done with TransactionContent
            iDecodeToXml = EFalse;

            iXmlUtils->StorePureDataL(*iImpsData, iTransactions-1, iPureMessageType );
                   
            // reseting the XML decoder states
            // the transaction number is not reseted as there can be more in the message
            iDecodeToXml = EFalse;
            // reset the variable
            iPureMessageType = EImpsMessageNone;
            iNameSpace = KImpsNameSpaceCspMessage;
            iPreviousNameSpace = KImpsNameSpaceCspMessage;
            iStack->Reset();
            if (iTypeStatus)
                {
                iTypeStatus = EFalse;
                iAccessKey->PopL();
                }
            else
                {
                // clean the IMPS keys too
                // POP Transaction and TransactionContent
                iAccessKey->PopL(2);
                return NW_STAT_SUCCESS;
                }

            //////////////////////////////////////
            }
        if (!iTypeStatus)
            {
            return NW_STAT_SUCCESS;
            }
        }

    //////////////////////////////////////////////////////////////////////////
    

    TInt type = 0;
    
    if(iAccessKey->Count())
        {
        type = iImpsData->KeyTypeL(iAccessKey);
        }
    else
        {
        type = EImpsDataTypeNone;
        }
    
    if ( type == EImpsDataTypeContentData )
        {
        type = TImpsDataUtils::GetContentDataTypeL( iImpsData, ( iTransactions-1 ) );
        }

    if(type == EImpsDataTypeNone || type == EImpsDataTypeNotSupported)
        {
        iExtension = 0;
        iOpaque.length = 0;
        iElementValue->Des().Zero();
        }
    else if(type == EImpsDataTypeInt)
        {
        if(iExtension == NW_WBXML_EXT_T_0 || iExtension == NW_WBXML_EXT_T_1 || iExtension == NW_WBXML_EXT_T_2)
            {
            TInt value = iWbXmlData->GetImpsEngineValue(iElementToken,iExtensionValue);
            if(value != KErrNotFound) iImpsData->StoreIntegerL(iAccessKey,value);
            iExtension = 0;
            }
        else if(iOpaque.length)
            {
            TInt value = 0;
            if (iOpaque.data)
            	{
	            for(TUint8 i=0;i<iOpaque.length;i++) 
	                {
	                value = (value << 8) + iOpaque.data[i];
	                }
            	}
            iImpsData->StoreIntegerL(iAccessKey,value);
            iOpaque.length = 0;
            if(iElementToken == KImpsWbXmlContentSize)
                {
                iContentSize = value;
                }
            }
        else if(iElementValue->Des().Length())
            {
            if(iExtension == NW_WBXML_EXT_I_0 || iExtension == NW_WBXML_EXT_I_1 || iExtension == NW_WBXML_EXT_I_2)
                {
                iExtension = 0;
                }
            else
                {
                TInt value = iWbXmlData->GetImpsEngineValue(iElementToken,iElementValue->Des());
                if(value == KImpsWbXmlDataNotFound)
                    {
                    // Try to convert the string to integer
                    TLex8 tmp(iElementValue->Des());
                    if (tmp.Val(value) != KErrNone )
                        {
                        // Cannot be converted
                        return NW_STAT_FAILURE;
                        }
                    }
                iImpsData->StoreIntegerL(iAccessKey,value);
                }
            iElementValue->Des().Zero();
            }
        }
    else if(type == EImpsDataTypeBoolean)
        {
        if(iExtension == NW_WBXML_EXT_T_0 || iExtension == NW_WBXML_EXT_T_1 || iExtension == NW_WBXML_EXT_T_2)
            {
            TInt value = iWbXmlData->GetImpsEngineValue(iElementToken,iExtensionValue);
            if(value != KErrNotFound)
                {
                iImpsData->StoreBooleanL(iAccessKey,value);
                }
            iExtension = 0;
            }
        else if(iOpaque.length)
            {
            iImpsData->StoreBooleanL(iAccessKey,(TInt)*iOpaque.data);
            iOpaque.length = 0;
            }
        }
    else if(type == EImpsDataTypeDesc8)
        {
        iStoreValue8->Des().Zero();
        if(iExtension == NW_WBXML_EXT_T_0 || iExtension == NW_WBXML_EXT_T_1 || iExtension == NW_WBXML_EXT_T_2)
            {
            iStoreValue8->Des().Copy(iWbXmlData->GetImpsEngineDesc8Value(iElementToken,iExtensionValue));
            iExtension = 0;
            }
        if(iElementValue->Des().Length())
            {
            if(iExtension == NW_WBXML_EXT_I_0 || iExtension == NW_WBXML_EXT_I_1 || iExtension == NW_WBXML_EXT_I_2)
                {
                iExtension = 0;
                }
            else
                {
                if((iElementValue->Des().Length() + (iStoreValue8->Des().Length()) > iStoreValue8->Des().MaxLength()))
                    {
                    HBufC8* ptr = HBufC8::NewL(iStoreValue8->Des().Length() + iElementValue->Des().Length());
                    ptr->Des().Append(iStoreValue8->Des());
                    delete iStoreValue8;
                    iStoreValue8 = ptr;
                    }
                iStoreValue8->Des().Append(iElementValue->Des());
                }
            iElementValue->Des().Zero();
            }
        else if(iOpaque.length)
            {
            if(((TInt)iOpaque.length + iStoreValue8->Des().Length()) > iStoreValue8->Des().MaxLength())
                {
                HBufC8* ptr = HBufC8::NewL(iStoreValue8->Des().Length() + iOpaque.length + 1);
                ptr->Des().Append(iStoreValue8->Des());
                delete iStoreValue8;
                iStoreValue8 = ptr;
                }
            for(TInt i=0;i<(TInt)iOpaque.length;i++)
                {
                iStoreValue8->Des().Append(iOpaque.data[i]);
                }
            iOpaque.length = 0;
            }
        if(iStoreValue8->Des().Length())
            {
            iImpsData->StoreDesc8L(iAccessKey,iStoreValue8->Des());
            }
        if(iStoreValue8->Des().Length() > KImpsWbXmlMaxBufferSize)
            {
            delete iStoreValue8;
            iStoreValue8 = NULL;
            iStoreValue8 = HBufC8::NewL(KImpsWbXmlMaxStringLength);
            }
        }
    else if(type == EImpsDataTypeDesc)
        {
        iStoreValue->Des().Zero();
        if(iExtension == NW_WBXML_EXT_T_0 || iExtension == NW_WBXML_EXT_T_1 || iExtension == NW_WBXML_EXT_T_2)
            {
            TInt token = 0;
            if(iNameSpace == KImpsNameSpacePresenceSubList)
                {
                token = iAccessKey->GetElementL((iAccessKey->Count() - 2),iKeyType);
//                token = iWbXmlData->GetWbXmlTokenPre(token);
                }
            else
                {
                token = iElementToken;
                }
            iStoreValue->Des().Copy(iWbXmlData->GetImpsEngineDescValue(token,iExtensionValue));
            iExtension = 0;
            }
        TPtr p = iStoreValue->Des();
        if(iElementValue->Des().Length())
            {
            if(iExtension == NW_WBXML_EXT_I_0 || iExtension == NW_WBXML_EXT_I_1 || iExtension == NW_WBXML_EXT_I_2)
                {
                iExtension = 0;
                }
            else
                {
                if((iElementValue->Des().Length() + iStoreValue->Des().Length()) > iStoreValue->Des().MaxLength())
                    {
                    HBufC* ptr = HBufC::NewL(iElementValue->Des().Length() + iStoreValue->Des().Length());
                    ptr->Des().Append(iStoreValue->Des());
                    delete iStoreValue;
                    iStoreValue = ptr;
                    p.Set(iStoreValue->Des());
                    }
                if(iStoreValue->Des().Length())
                    {
                    TPtrC pp = iStoreValue->Mid(p.Length());
                    p.Set((unsigned short*)pp.Ptr(),0,(p.MaxLength() - p.Length()));
                    if(CnvUtfConverter::ConvertToUnicodeFromUtf8(p,iElementValue->Des()) != KErrNone)
                        {
                        return NW_STAT_FAILURE;
                        }
                    p.Set((unsigned short*)iStoreValue->Des().Ptr(),
                        (iStoreValue->Des().Length() + iElementValue->Des().Length()),
                        iStoreValue->Des().MaxLength());
                    }
                else
                    {
                    if(CnvUtfConverter::ConvertToUnicodeFromUtf8(p,iElementValue->Des()) != KErrNone)
                        {
                        return NW_STAT_FAILURE;
                        }
                    }
                }
            iElementValue->Des().Zero();
            }
        // Handle also the case when String is encoded as Opaque!
        if(iOpaque.length)
            {
            if ( iElementToken == (KImpsWbXmlDateTime & KImpsTokenMask) ) 
                {
                // !DateTime needs special handling as it is not string!
                // TODO: Now just ignored, needs proper date&time decoding
                }
            else
                {
                // Normal String decoding
                // Check the buffer size
                if(((TInt)iOpaque.length + iStoreValue->Des().Length()) > iStoreValue->Des().MaxLength())
                    {
                    HBufC* ptr = HBufC::NewL(iStoreValue->Des().Length() + iOpaque.length + 1);
                    ptr->Des().Append(iStoreValue->Des());
                    delete iStoreValue;
                    iStoreValue = ptr;
                    p.Set(iStoreValue->Des());
                    }
                // Copy the data from opaque to string format
                for(TInt i=0;i<(TInt)iOpaque.length;i++)
                    {
                    iElementValue->Des().Append(iOpaque.data[i]);
                    }
                // convert the UTF-8 to Unicode
                if(iStoreValue->Des().Length())
                    {
                    // if iStoreValue has already a string which was encoded as EXT, e.g. "http://"
                    TPtrC pp = iStoreValue->Mid(p.Length());
                    p.Set((unsigned short*)pp.Ptr(),0,(p.MaxLength() - p.Length()));
                    if(CnvUtfConverter::ConvertToUnicodeFromUtf8(p,iElementValue->Des()) != KErrNone)
                        {
                        return NW_STAT_FAILURE;
                        }
                    p.Set((unsigned short*)iStoreValue->Des().Ptr(),
                        (iStoreValue->Des().Length() + iElementValue->Des().Length()),
                        iStoreValue->Des().MaxLength());
                    }
                else
                    {
                    if(CnvUtfConverter::ConvertToUnicodeFromUtf8(p,iElementValue->Des()) != KErrNone)
                        {
                        return NW_STAT_FAILURE;
                        }
                    }
                }
            iOpaque.length = 0;
            iElementValue->Des().Zero();
            }
        
        // FIX: empty values are now stored too
        // if this fix causes that the TransactionContent and PresenceSubList 
        // will appear in the iImpsData with zero length descriptor value
//        if(p.Length())
//            {
       iImpsData->StoreDescL(iAccessKey,p);
//            }

       // reduce the size of the buffer if needed
       if(iStoreValue->Des().Length() > KImpsWbXmlMaxBufferSize)
            {
            delete iStoreValue;
            iStoreValue = NULL;
            iStoreValue = HBufC::NewL(KImpsWbXmlMaxStringLength);
            }
        }
        else if(type == EImpsDataTypeExt)
            {
            TPtr8 name = iElementValue->Des();
            TPtrC8 ext = iElementValue->Mid(name.Locate(NULL) + 1);
            name.SetLength(name.Locate(NULL));
            if(name.Length())
                {
                iImpsData->StoreExtL(iAccessKey,name,ext);
                }
            iElementValue->Des().Zero();
            }
        if(iAccessKey->Count())
            {
            iPreviousElement = iAccessKey->GetElementL((iAccessKey->Count() - 1),iKeyType);
            iPreviousNameSpace = iNameSpace;
            iAccessKey->PopL( );
            switch(iPreviousElement)
                {
                case EImpsKeyTransactionContent:
                    if(iNameSpace == KImpsNameSpaceTransactionContent)
                        {
                        iPreviousNameSpace = iNameSpace;
                        iNameSpace = KImpsNameSpaceCspMessage;
                        }
                    break;
                    
                case EImpsKeyPresenceSubList:
                    if(iNameSpace == KImpsNameSpacePresenceSubList)
                        {
                        iPreviousNameSpace = iNameSpace;
                        iNameSpace = KImpsNameSpaceTransactionContent;
                        }
                    break;
                    
                default:
                    break;
                }
            }
        
        return NW_STAT_SUCCESS;
        
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreTokenL
// method for handling wbxml tokens 
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::StoreTokenL(TUint32 aToken)
    {
    
    TInt impskey = 0;
    TInt index = 0;

    switch(iDecodingState)
    {
    
    case EImpsDecodeStateTagStart:
    
        if((aToken & KImpsByteMask) == NW_WBXML_LITERAL ||
            (aToken & KImpsByteMask) == NW_WBXML_LITERAL_A ||
            (aToken & KImpsByteMask) == NW_WBXML_LITERAL_AC ||
            (aToken & KImpsByteMask) == NW_WBXML_LITERAL_C)
            {
            iDecodingState = EImpsDecodeStateLiteral;
            iElementToken = aToken;
            impskey = EImpsKeyPRExt;
            iKeyType = EImpsKeyTypePre;
            }
        else
            {
            iElementToken = aToken & KImpsTokenMask;
            
            //////////////////////////////////////////////////////////////////////////
            
            // keep track of the transaction number in case of multitrans
            if ( iElementToken == KImpsWbXmlTransaction ) 
                {
                ++iTransactions;
                }
            // Decoding directly to XML

            
            // We have started to write the XML even we don't know the Transaction type yet
            // If the transaction type is not what we want we have to cancel the XML processing
            if ( iStack->Count() == 1 )
                {
                // we have only 1 element in the stack and that is  TransactionContent
                // now we can check the Transaction type
                if ( IsPureTransaction(iElementToken) ) 
                    {
#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log(_L("WBXMLdecoder: decoding to XML, TR_Key %x"), iElementToken);
#endif
                    iDecodeToXml = ETrue;
                    // store the message type to the variable
                    iPureMessageType = iElementToken;
                    }
                else
                    {
#ifndef _NO_IMPS_LOGGING_
                    CImpsClientLogger::Log(_L("WBXMLdecoder: decoding to internal, TR_Key %x"),iElementToken);
#endif
                    // Not presence transaction, let's get rid of the pushed element
                    iStack->Pop(); 
                    }
                }

            if ( iDecodeToXml )
                {

                NW_String_UCS2Buff_t* nw_str = NW_WBXML_Dictionary_getTagByFqToken(aToken);
                
                if ( nw_str == NULL ) 
                    {
                    return NW_STAT_FAILURE;
                    }

				iStack->PushL(nw_str);
				
                TBuf8<KDtdElementMaxLength> elementString;
                ConvertToDes8((TUint16*)nw_str->bytes, elementString);
//                iXmlUtils->WriteToBufferL(_L8("<"));
//                iXmlUtils->WriteToBufferL(elementString);
                if ( iElementToken == KImpsWbXmlPresenceSubList ) 
                    {
                    iXmlUtils->StartElementL(elementString, 
                        iDocument->publicid == 0x10 ? KPresSubListAttr11() : KPresSubListAttr12());
                    }
                else
                    {
                    iXmlUtils->StartElementL(elementString, KNullDesC8);
                    }
				
				// Status is decoded to internal too
                if ( !iTypeStatus ) 
                    {
                    break;
                    }
                }

            // check for TransactionContent
            // because the <TransactionConent> is present in pure data we must
            // continue also with normal decoding
            // in the next round we will get the Transaction type and can decide
            // whether we need to continue to decode normally
            if(iElementToken == KImpsWbXmlTransactionContent)
                {
#ifndef _NO_IMPS_LOGGING_
                CImpsClientLogger::Log(_L("WBXMLdecoder: Transaction content found"));
#endif

                iXmlUtils->ResetL();

                NW_String_UCS2Buff_t* nw_str = NW_WBXML_Dictionary_getTagByFqToken(aToken);
                if ( nw_str == NULL ) 
                    {
                    return NW_STAT_FAILURE;
                    }
//                TPtrC elementPtr;
//                elementPtr.Set((TUint16*)nw_str->bytes);
                // Simple UCS2 to 8bit conversion
                // it is safe as we have only 8bit characters
                TBuf8<KDtdElementMaxLength> elementString;
                ConvertToDes8((TUint16*)nw_str->bytes, elementString);

//                elementString.Copy(elementPtr);
                iXmlUtils->StartElementL(elementString, 
                		iDocument->version == 0x10? KAttr11() : KAttr12());
//                iXmlUtils->WriteToBufferL(_L8("<"));
//                iXmlUtils->WriteToBufferL(elementString);
                iStack->PushL(nw_str);
                }

            //////////////////////////////////////////////////////////////////////////
            
            
            if(iNameSpace == KImpsNameSpaceCspMessage && iElementToken == KImpsMessageRootElement)
                {
                iPreviousElement = EImpsKeyEND;
                }
            else
                {
                switch(iNameSpace)
                    {
                    case KImpsNameSpaceCspMessage:
                        //impskey = iWbXmlData->GetHeaderAccessKey(iElementToken);
                        impskey = iElementToken;
                        iKeyType = EImpsKeyTypeIM;
                        break;
                        
                    case KImpsNameSpaceTransactionContent:
                        //impskey = iWbXmlData->GetContentAccessKey(iElementToken);
                        impskey = iElementToken;
                        iKeyType = EImpsKeyTypeIM;
                        break;
                        
                    case KImpsNameSpacePresenceSubList:
                        //impskey = iWbXmlData->GetPresenceAccessKey(iElementToken);
                        impskey = iElementToken;
                        iKeyType = EImpsKeyTypePre;
                        break;
                        
                    default:
                        return NW_STAT_FAILURE;
                    }
                
                // !!!! Fix for JHAU-63PBMG !!!!
                // Check whether the same level already exists
                iAccessKey->AddL(CREATEKEY(impskey,index),iKeyType);
                CheckIfMultipleElementsL(impskey);
                ////////////////////////////////////////////////

                if(iAccessKey->Count() > 1)
                    {
                    if(iAccessKey->GetElementL((iAccessKey->Count() - 2),iKeyType) == EImpsKeyTransactionContent)
                        {
                        iImpsData->StoreEmptyL(iAccessKey);
                        }
                    }
//                if(impskey == EImpsKeyPresenceSubList && iNameSpace == KImpsNameSpaceTransactionContent)
//                    {
//                    iImpsData->StoreDescL(iAccessKey,static_cast<TPtrC>(KImpsPresenceSublist()));
//                    }
                if(!(aToken & KImpsTokenEmpty))
                    {
                    iImpsData->StoreEmptyL(iAccessKey);
                    }
                }
            }       
        switch(impskey)
        {
        
        case EImpsKeyTransactionContent:
        if(iNameSpace == KImpsNameSpaceCspMessage)
        {
            iPreviousNameSpace = iNameSpace;
            iNameSpace = KImpsNameSpaceTransactionContent;
        }
        break;
            
        case EImpsKeyPresenceSubList:
        if(iNameSpace == KImpsNameSpaceTransactionContent)
        {
            iPreviousNameSpace = iNameSpace;
            iNameSpace = KImpsNameSpacePresenceSubList;
        }
        break;

        default:
        break;
        
        }
        break;

        
        case EImpsDecodeStateAttributeStart:
        iAttribute = aToken & KImpsByteMask;
        // literal attributes are ignored
        if(iElementToken == NW_WBXML_LITERAL_A ||
            iElementToken == NW_WBXML_LITERAL_AC)
        {
            break;
        }
        if(iElementToken == KImpsWbXmlWV_CSP_Message  && 
            (iAttribute != KImpsWbXmlCsp11Xmlns) && (iAttribute != KImpsWbXmlCsp12Xmlns))
        {
            return NW_STAT_FAILURE;
        }
        if(iElementToken == KImpsWbXmlTransactionContent && 
            (iAttribute != KImpsWbXmlTrc11Xmlns) && (iAttribute != KImpsWbXmlTrc12Xmlns))
        {
            return NW_STAT_FAILURE;
        }
        if(iElementToken == KImpsWbXmlPresenceSubList && 
            (iAttribute != KImpsWbXmlPa11Xmlns) && (iAttribute != KImpsWbXmlPa12Xmlns))
        {
            return NW_STAT_FAILURE;
        }
        break;

        case EImpsDecodeStateExtension:
        if((aToken & KImpsByteMask) == NW_WBXML_EXT_T_0 
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_T_1 
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_T_2
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_I_0
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_I_1
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_I_2
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_0
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_1
            || (aToken & KImpsByteMask) == NW_WBXML_EXT_2)
        {
            iExtension = aToken & KImpsByteMask;
            iDecodingState = EImpsDecodeStateExtensionValue;
        }
        break;
        
        case EImpsDecodeStateExtensionValue:
            {
            iExtensionValue = aToken;
            //////////////////////////////////////////////////////////////////////////
            
            if (iDecodeToXml)
                {
                
                NW_String_UCS2Buff_t* nw_str = 
                    NW_WBXML_Dictionary_getAttributeNameByToken
                        (
                        iDictionaries[iCurrentDictionary], 
                        (NW_Uint16)aToken //conversion from 32 to 16 is OK since
                                          // we have masked it already
                        );

                if ( nw_str == NULL ) 
                    {
                    return NW_STAT_FAILURE; 
                    }

                TPtrC elementPtr;
                elementPtr.Set((TUint16*)nw_str->bytes);
                
                // Simple UCS2 to 8bit conversion
                // it is safe as we have only 8bit characters
                TBuf8<100> valueString;
                valueString.Copy(elementPtr);
                

                iXmlUtils->WriteToBufferL(valueString);
                if (!iTypeStatus)
                    {
                    // in case of not Status transaction we must break
                    break;
                    }

                }
            //////////////////////////////////////////////////////////////////////////
            

//////////////////////////////////////////////////////////////////////////
            // FIX: extension embedded in string. Only EImpsDataTypeDesc
            // is supported now!
            TInt type = 0;
            if( iAccessKey->Count() )
                {
                type = iImpsData->KeyTypeL(iAccessKey);
                }
            else
                {
                type = EImpsDataTypeNone;
                }

            if( type == EImpsDataTypeDesc )
                {
                // decode the EXTENSION as string
                TInt token = 0;
                if(iNameSpace == KImpsNameSpacePresenceSubList)
                    {
                    token = iAccessKey->GetElementL((iAccessKey->Count() - 2),iKeyType);
                    //token = iWbXmlData->GetWbXmlTokenPre(token);
                    }
                else
                    {
                    token = iElementToken;
                    }
                iElementValue->Des().Append(iWbXmlData->GetImpsEngineDescValue(token,iExtensionValue));
                iExtension = 0;
                }
//////////////////////////////////////////////////////////////////////////
            break;
            }

       default:
  
           break;
    }
    
    return(NW_STAT_SUCCESS);
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::DecodeEntityL
// method decodes WBXML entity value
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::DecodeEntityL(TUint32 aValue)
    {
    
    NW_String_t string;

    string.storage = (TUint8*)&aValue;
    string.length = 1;
    StoreStringL(string);

    return NW_STAT_SUCCESS;
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreString
// method stores parsed out string
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::StoreStringL(NW_String_t aString)
    {
    
    TPtrC8 p8((TUint8*)aString.storage,aString.length);
    
    //////////////////////////////////////////////////////////////////////////
    
    if ( iDecodeToXml ) 
        {
        iXmlUtils->WriteToBufferL(iXmlUtils->EscapeXmlL(p8));
        if ( !iTypeStatus ) 
            {
            return;
            }
        }
    //////////////////////////////////////////////////////////////////////////
    
    if((p8.Length() + iElementValue->Des().Length()) > iElementValue->Des().MaxLength())
    {
        HBufC8* ptr = HBufC8::NewL(p8.Length() + iElementValue->Des().Length() + 1);
        ptr->Des().Append(iElementValue->Des());
        delete iElementValue;
        iElementValue = ptr;
    }
    
    if(!iElementValue)
    {    
        iElementValue  = HBufC8::NewL(KImpsWbXmlMaxStringLength);
    }
   
	   iElementValue->Des().Append(p8);
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreTableStringL
// method stores WBXML table string value
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::StoreTableStringL(TUint32 aIndex)
    {

    
    NW_Status_t rcode = NW_STAT_SUCCESS;
    TInt maxlength = 0;
    NW_String_t string;

    if((rcode = NW_WBXML_Document_getTableString(iDocument,aIndex,&string)) == NW_STAT_SUCCESS)
    {
        switch(iDecodingState)
        {
 
        case EImpsDecodeStateAttributeValue:
            if(iElementToken == KImpsWbXmlWV_CSP_Message ||
               iElementToken == KImpsWbXmlTransactionContent ||
               iElementToken == KImpsWbXmlPresenceSubList)
               {
//                    TPtrC8 p8((TUint8*)string.storage);
//                    if((iVersion.Match(p8)))
//                    {
//                        rcode = NW_STAT_FAILURE;
//                        iError = KImpsErrorCSPVersion;
//                    }
                    string.length = 0;
               }
               break;

        
        // check string length  
        case EImpsDecodeStateContent:        
            iWbXmlData->GetWbXmlElementValue(iElementToken,0,maxlength);
            // string length checking removed !!!
            string.length--;
            StoreStringL(string);
            break;

        case EImpsDecodeStateLiteral:
        	// APIClient is literal
        	{
        	TPtrC8 element((TUint8*)string.storage,string.length-1);
        	if (element.CompareF( _L8("APIClient")/*KImpsAPIClient_8*/ ) == 0)
        		{
        		iAccessKey->AddL(CREATEKEY(KImpsKeyAPIClient,0),EImpsKeyTypeIM);
        		}
            break;
			}
            
        default:
            break;
            
        }
    }

   
    return(rcode);
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreInlineStringL
// method stores WBXML inline string value
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::StoreInlineStringL(NW_WBXML_Parser_t* aParser,TInt aLength)
    {
    
    
    NW_Status_t rcode = NW_STAT_SUCCESS;
    NW_String_t string;

    // check string length
    if(!aLength)
    {
        return NW_STAT_FAILURE;
    }
    if(iDecodingState == EImpsDecodeStateContent)
    {
        TInt maxlength = 0;
        iWbXmlData->GetWbXmlElementValue(iElementToken,0,maxlength);
        // string length checking removed !!!
    }

    if((rcode = NW_WBXML_Parser_getStringInline(aParser,iDocument,&string)) == NW_STAT_SUCCESS)
    {
        switch(iDecodingState)
        {

        case EImpsDecodeStateAttributeValue:
            if(iElementToken == KImpsWbXmlWV_CSP_Message ||
               iElementToken == KImpsWbXmlTransactionContent ||
               iElementToken == KImpsWbXmlPresenceSubList)   
            {
//                TPtrC8 p8((TUint8*)string.storage);
//                if((iVersion.Match(p8)))
//                {
//                    rcode = NW_STAT_FAILURE;
//                    iError = KImpsErrorCSPVersion;
//                }
                string.length = 0;    
            }
            break;
    
        case EImpsDecodeStateContent:
            string.length = aLength - 1;
            StoreStringL(string);
            break;
        
        default:
            break;
        }
    }

    return(rcode);
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::StoreOpaqueData
// method stores WBXML opaque data value
// ---------------------------------------------------------
//
NW_Status_t CImpsDecodeWbXml::StoreOpaqueData(NW_WBXML_Parser_t* aParser,TUint32 aLength)
    {
    
    NW_Status_t status = NW_STAT_SUCCESS;
    switch(iDecodingState)
    {
    
    case EImpsDecodeStateContent:
        if(aLength)
        {
            status = NW_WBXML_Parser_getOpaque(aParser,aLength,&iOpaque);
            
            //////////////////////////////////////////////////////////////////////////
            if ( iDecodeToXml ) 
                {

                // NOTE: Only integer encoded as opaque is supported
                // String cannot be encoded as opaque !!!
                TBuf8<100> valueString;
                // Copy the data from opaque to string format
                TInt value = 0;
                for(TUint8 i=0;i<iOpaque.length;i++) 
                    {
                    value = (value << 8) + iOpaque.data[i];
                    }
                valueString.AppendNum(value);
//                for(TInt i=0;i<(TInt)iOpaque.length;i++)
//                    {
//                    valueString.Append(iOpaque.data[i]);
//                    }

                TRAPD(err, iXmlUtils->WriteToBufferL( valueString ));
                if (err)
                    {
                    status = NW_STAT_FAILURE;
                    }
                if ( !iTypeStatus ) 
                    {
                    iOpaque.length = 0; // consume the opaque data
                    }
                }
            //////////////////////////////////////////////////////////////////////////
            
        }
        else // aLength == 0
        	{
        	// 0 must be also included in case when it was in WBXML like "C3 00"
        	// NOTE: the data is still NULL!!!
        	iOpaque.length = 1;
        	}
    break;

    case EImpsDecodeStateAttributeValue:
        break;

    default:      
        break;
    }
    
    return(status);

    }


// ---------------------------------------------------------
// CImpsDecodeWbXml::CheckIfMultipleElementsL
// Creating the index in case of multiple elements
// ---------------------------------------------------------


void CImpsDecodeWbXml::CheckIfMultipleElementsL(TInt aImpsKey)
    {
    TInt index ( 0 );
    switch ( aImpsKey )
        {
        case EImpsKeyTransaction: // for MultiTrans
        case EImpsKeyUserID:
        case EImpsKeyGroupID:
        case EImpsKeyMessageID:
        case EImpsKeyScreenName:
        case EImpsKeyUser:
        case EImpsKeyGroup: 
        case EImpsKeyProperty:
        case EImpsKeyPresence:
        case EImpsKeySearchPairList:
        case EImpsKeyDetailedResult:
        case EImpsKeyContactList:
        case EImpsKeyPresenceSubList:
        case EImpsKeyMessageInfo:
        case EImpsKeyAcceptedCharSet:
        case EImpsKeyAcceptedContentType:
        case EImpsKeyAcceptedTransferEncoding:
        case EImpsKeySupportedBearer:
        case EImpsKeySupportedCIRMethod:
        case EImpsKeyNickName:
        case EImpsKeyMapping:
            while ( iImpsData->CheckBranchExistenceL( iAccessKey ) )
                {
                iAccessKey->PopL();
                ++index;
                iAccessKey->AddL( CREATEKEY( aImpsKey, index ) );
                }
            break;
        default:
            break;
        }
    switch ( aImpsKey )
        {
        case EImpsKeyPRCommC:
        case EImpsKeyPRAddrPref:
            while ( iImpsData->CheckBranchExistenceL( iAccessKey ) )
                {
                iAccessKey->PopL();
                ++index;
                iAccessKey->AddL( CREATEKEY( aImpsKey, index ), EImpsKeyTypePre );
                }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CImpsDecodeXml::TrConToXmlL
// ---------------------------------------------------------
//void CImpsDecodeWbXml::TrConToXmlL ( MImpsDataAccessor& aImpsData)
//    {
//    
//    // pure data handle
//    if(iXmlUtils)
//    {
//        iXmlUtils->TransactionContentToXmlL(aImpsData);
//    }
//    
//    }    
//
//
// ---------------------------------------------------------
// NewDecoderL()
// function creates new wbxml decoder instance
// Returns: C-class entity of abstract MImpsDecoder class
// ---------------------------------------------------------
//

EXPORT_C GLDEF_C MImpsDecoder* NewDecoderL( MImpsCSPSession* aCallback )
    {
    
    return (MImpsDecoder*) CImpsDecodeWbXml::NewL( aCallback );
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::Destroy
// decoder destructor 
// ---------------------------------------------------------
//
void CImpsDecodeWbXml::Destroy( )
    {

    delete this;
    
    }

// ---------------------------------------------------------
// CImpsDecodeWbXml::IsPureTransaction ()
// ---------------------------------------------------------
TBool CImpsDecodeWbXml::IsPureTransaction(TInt aToken)
    {
    switch ( aToken )
        {
        case KImpsWbXmlPresenceNotificationRequest:
        case KImpsWbXmlGetPresenceResponse:
        case KImpsWbXmlGetWatcherListResponse:
        case KImpsWbXmlGetListResponse:
        case KImpsWbXmlListManageResponse:
        case KImpsWbXmlGetAttributeListResponse:
        case KImpsWbXmlPresenceAuthRequest:
        case KImpsKeyGetReactiveAuthStatus_Response:
            return ETrue;
        case KImpsWbXmlStatus:
            // the Status transaction needs special handling because
            // it has to be also in normal decoded version too
            iTypeStatus = ETrue;
            return ETrue;
        default:
            return EFalse;
        }

    }


// ---------------------------------------------------------
// CImpsDecodeWbXml::ConvertToDes8 ()
// ---------------------------------------------------------
void CImpsDecodeWbXml::ConvertToDes8( TUint16* aElement, TDes8& aConverted )
    {

    TPtrC elementPtr;
    elementPtr.Set(aElement);
    __ASSERT_DEBUG(elementPtr.Length()<aConverted.MaxLength(), User::Panic(_L("WBXML Panic"), KErrTooBig));
    // Simple UCS2 to 8bit conversion
    // it is safe as we have only 8bit characters
//    TBuf8<KDtdElementMaxLength> elementString;
    aConverted.Copy(elementPtr);
    }

//  End of File  
